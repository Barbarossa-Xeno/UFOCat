# include "Level.hpp"

namespace UFOCat
{
	LevelData& Level::m_currentLevel() const
	{
		return getData().levels[getData().levelIndex];
	}

	void Level::m_setTargetSpawnTime(size_t level)
	{
		// 定式の詳細は仕様書参照
		const double&& term1 = m_currentLevel().timeLimit.count() / Random(2, 4);
		const double&& term2 = level * (m_currentLevel().breedData.total() * m_currentLevel().intervalData.period.count()) / (m_currentLevel().actionDataList.size() * m_currentLevel().breedData.similar);

		// 一度、ターゲットスポーン時間を設定
		m_targetAppearTime = Duration{ Min((term1 + term2), 0.75 * m_currentLevel().timeLimit.count()) };
	}

	bool Level::m_hasAppearedTarget() const
	{
		if (not getData().spawns.isEmpty())
		{
			// ターゲットは 0番目 に入れるよう保証しているので、0番目が nullptr でなければ出現済み
			return getData().spawns[0] != nullptr;
		}
		else
		{
			return false;
		}
	}

	bool Level::m_isAvailableNextLevel() const
	{
		// 次のレベルのインデックスがレベルの数未満であれば進める
		return (getData().levelIndex + 1) < getData().levels.size();
	}

	Level::Level(const InitData& init)
		: IScene{ init }
	{
		// 前回レベルでスポーンした猫を吹っ飛ばし、unique_ptr も解放する
		getData().spawns.release();

		// 0 番目に空のポインタを入れて置き、
		// ターゲットをスポーンさせるときはここを上書きする形にする
		// こうすることで、0 番目が nullptr かどうかでターゲットの出現判定ができるようになるうえに、
		// `catchable()` で判定をする際に、ターゲットを最も初めにチェックするので、
		// 当たり判定を優遇することができる（ミスタップを起こしにくい）
		// しかも一番初めに描画されるので、ターゲットが他の猫に隠れて見えづらいパターンが発生することもあり、難易度がちょっと上がる
		getData().spawns << nullptr;

		// 前回レベルで選んだ猫を吹っ飛ばし、shared_ptr も解放する
		m_selections.release();

		// 先にスコアのレベル情報を設定
		m_score.level = getData().levelIndex + 1;

		// 前回シーンで決めたターゲットを取得（注：コピーで取得すること！）
		m_target = std::make_shared<CatObject>(getData().cats[getData().targetIndex].clone());

		// レベルデータのうち、登場する猫の数に関するデータを取得する
		const auto& similarCount = m_currentLevel().breedData.similar;
		const auto& otherCount = m_currentLevel().breedData.other;

		// それぞれ、似ている猫とそうじゃない猫を絞り込む用の配列を用意する
		Array<std::shared_ptr<CatObject>> similars;
		Array<std::shared_ptr<CatObject>> others;

		// 全種類の猫の中から、ターゲットと似ている猫とそうでない猫を振り分ける
		for (const auto& cat : getData().cats)
		{
			// それと、ターゲットと同じのを参照しないように保障する
			if (m_currentLevel().similarity == cat.getCatData().getSameDataCount(m_target->getCatData())
				and m_target->getCatData() != cat.getCatData())
			{
				similars << std::make_shared<CatObject>(cat);
			}
			else
			{
				others << std::make_shared<CatObject>(cat);
			}
		}

		// 宇宙船演算子が使いたかっただけ！！！
		// 上で絞り込んだ結果、similars の数が既定 (similarCount) より多すぎるか、少なすぎる場合に調整する
		// まずは多すぎる場合
		if (auto comp = similars.size() <=> similarCount;
			comp > 0)
		{
			// 多すぎる場合はランダムに頭数を削る
			while (similars.size() > similarCount)
			{
				similars.shuffle().pop_back();
			}
		}
		// 少なすぎる場合
		else if (comp < 0)
		{
			// 条件を緩和して絞り込んだ猫を入れる配列
			Array<std::shared_ptr<CatObject>> looses;

			// 類似度を1つ下げて絞り込んだものを looses に入れる
			for (const auto &cat : others)
			{
				if ((m_currentLevel().similarity - 1) == cat->getCatData().getSameDataCount(m_target->getCatData())
					and m_target->getCatData() != cat->getCatData())
				{
					looses << cat;
				}
			}

			// 足りなかった分を looses か others から補う
			while (similars.size() < similarCount)
			{
				if (not looses.isEmpty())
				{
					similars << looses.shuffle().choice();
				}
				else
				{
					// others から補うことになった場合は補ったやつを元のリストから消す
					// others はもともと多く絞り込まれるはずなので、枯渇することはない
					const auto &instead = others.shuffle().choice();
					similars << instead;
					others.remove(instead);
				}
			}

		}

		// others は既定の数から溢れている可能性が高いので、ランダムに削る
		others.shuffle().resize(otherCount);

		// レベル中に行うアクションリストの中から、それぞれの発生確率だけを抜き取ったリストで確率分布をつくる
		m_actionProbabilities = DiscreteDistribution{ m_currentLevel().actionDataList.map([](const LevelData::ActionData &data) { return data.probability; }) };

		// 最終的に catsInPhase にまとめて
		m_selections.append(similars).append(others);

		// ステートをプレイ中に変更する
		m_state = Level::State::Playing;

		// 制限時間を決めて、タイマー開始
		// 1s 猶予を持たせて、気持ち長めにすることで間に入る処理の影響を減らす
		getData().timer.restart(m_currentLevel().timeLimit + 1s);

		// 現在のレベルに合わせてターゲットの出現時刻を設定
		m_setTargetSpawnTime(getData().levelIndex + 1);
	}

	void Level::update()
	{
		// # ステート依存処理
		switch (m_state)
		{
			case Level::State::Playing:
			{
				// ## スポーン処理
				m_watch.setInterval([this]()
					{
						// ターゲットの出現時刻を超えていて、ターゲットがまだ出現していなかったら
						if (getData().timer.remaining() <= m_targetAppearTime and (not m_hasAppearedTarget()))
						{
							// ターゲットを（コピーして）湧かせる
							getData().spawns[0] = std::make_unique<CatObject>(m_target->clone());
							// ターゲットにも同様にアクションと速度の設定を行う
							getData().spawns[0]->setAction(DiscreteSample(m_currentLevel().actionDataList, m_actionProbabilities)).setRandomVelocity(getData().levelIndex + 1);
						}
						else
						{
							// ターゲット以外の猫をランダムに指定個選んで（コピーして）追加
							for (uint32 i = 0; i < m_currentLevel().intervalData.count; i++)
							{
								getData().spawns << std::make_unique<CatObject>
									(
										// アクションを抽選してセットし、現在のレベルに合わせて速度もランダムに決める
										m_selections.choice()->clone()
											.setAction(DiscreteSample(m_currentLevel().actionDataList, m_actionProbabilities))
												.setRandomVelocity(getData().levelIndex + 1)
									);
							}
						}
					}, m_currentLevel().intervalData.period);

				// ## 制限時間内と時間超過後での処理
				if (not getData().timer.reachedZero())
				{
					// ターゲットから順に捜査する
					for (const auto& cat : getData().spawns)
					{
						if (not cat)
						{
							continue;
						}

						// 猫をタッチしたら、その正誤を代入
						if (cat->act().checkCatchable(m_target->getCatData(), &m_score.isCorrect))
						{
							// 捕まえた猫を記録
							m_caught = &cat;

							// ターゲットとの正誤にかかわらず、触ったことにはしておく
							m_score.isCaught = true;
							
							// 反応時間を記録
							m_score.response = (m_targetAppearTime - getData().timer.remaining()).count();

							// 連続正解数を記録
							
							// 仮変数
							uint32 temp_consecutive = 0;

							for (size_t i = 0; i < getData().scores.size() - 1; i++)
							{
								// 次のスコアデータが存在しない場合は終了
								if (getData().scores[i + 1].level == InvalidIndex)
								{
									break;
								}

								// 今のレベルと次のレベルの両方で正解していたら増やす
								if (getData().scores[i].isCorrect and getData().scores[i + 1].isCorrect)
								{
									++temp_consecutive;
								}
								else
								{
									temp_consecutive = 0;
								}
							}

							// 記録
							m_score.consecutiveCorrect = temp_consecutive;

							// プレイ終了へ
							m_state = Level::State::Finish;
							m_watch.reset();
							break;
						}
					}

					// ターゲットが初めて画面上に見えたかどうかを記録する
					if (getData().spawns[0] and (not m_targetFirstVisible))
					{
						m_targetFirstVisible = getData().spawns[0]->isVisible();

						// 初めて見えた時点での残り時間に変更しておく
						m_targetAppearTime = getData().timer.remaining();
					}
				}
				else
				{
					// 制限時間が終わったら、終了表示を出しに行く
					m_state = Level::State::Finish;
				}
			}
			break;

			case Level::State::Finish:
			{
				// 3s 経ったらレベル終わり画面を出しに行く
				m_watch.setTimeout([this]()
					{
						m_state = Level::State::After;
					}, 3s);
			}
			break;

			case Level::State::After:
			{
				// TODO: 「次へ」ボタンを作るとき、クリアマークのリセットなど、レベルデータのリセットを行うようにする
				// ボタン内の利用可能状態によって押せるかどうか決まることが保証されるので
				// この条件式はボタンが利用不可能なときには通過しない
				if (m_gui.toNextLevel.isPressed())
				{
					// 次に進む場合は、レベルデータにもクリア情報を反映
					// これにより、Wanted シーンの初期化で自動的に次のレベルへ進む
					m_currentLevel().isCleared = m_score.isCorrect;

					// スコアを格納する
					getData().scores[getData().levelIndex] = m_score;

					// 次のレベル初期化へ
					changeScene(UFOCat::State::Wanted);
				}

				if (m_gui.toResult.isPressed())
				{
					if (not m_isAvailableNextLevel())
					{
						// 次のレベルが存在しないためにタイトルへ戻らなければならない場合は、
						// レベルデータにもクリア情報を反映
						m_currentLevel().isCleared = m_score.isCorrect;

						// スコアを格納する
						getData().scores[getData().levelIndex] = m_score;

						// 結果シーンへ
						changeScene(UFOCat::State::Result);
					}
					else
					{
						// TODO: ダイアログで本当に戻るか確認をとるように

						// それでOKだったら、
						{
							// スコアを格納する
							getData().scores[getData().levelIndex] = m_score;

							// 結果シーンへ
							changeScene(UFOCat::State::Result);
						}
						
					}
				}
			}
			break;
			default:
				break;
		}

# if _DEBUG    // デバッグ機能：Ctrl + Shift + S でスキップ
		if (KeyControl.pressed() and KeyShift.pressed() and KeyS.pressed())
		{
			getData().scores[getData().levelIndex] = Score{ getData().levelIndex + 1, true, true, 0.5, getData().levelIndex };
			getData().timer.reset();
			changeScene(UFOCat::State::Result);
		}
# endif
	}

	void Level::draw() const
	{
		// # 共通処理（背面）
		{
			for (const auto& cat : getData().spawns)
			{
				if (not cat)
				{
					continue;
				}

				cat->draw().drawHitArea();
			}
		}

		// # ステート依存処理
		switch (m_state)
		{
			case UFOCat::Level::State::Playing:
			{
				FontAsset(U"Test")(U"のこり {} 秒"_fmt(getData().timer.s())).draw(10, 10);
			}
			break;

			case UFOCat::Level::State::Finish:
			{
				FontAsset(U"Test")(U"Finish!!").drawAt(Scene::Center());
			}
			break;

			case UFOCat::Level::State::After:
			{
				// 背景 ちょっと暗くする
				{
					Rect{ Scene::Size() }.draw(ColorF{ 0.0, 0.75 });
				}
				// 画面左側 捕まえた猫を表示
				{
					if (m_score.isCaught)
					{
						auto &&image = m_caught->get()->getTexture().scaled(m_CatImageScale);

						image.drawAt(Scene::CenterF() - SizeF(image.size.x, 0));

						FontAsset(U"Test")(U"キミが捕まえた猫").drawAt(Scene::CenterF() + SizeF(-image.size.x, -150));
					}
				}
				// 画面右側 ターゲットを表示
				{
					auto &&image = m_target->getTexture().scaled(m_CatImageScale);
					image.drawAt(Scene::CenterF() + SizeF(image.size.x, 0));
					FontAsset(U"Test")(U"ターゲット").drawAt(Scene::CenterF() + SizeF(image.size.x, -150));
				}
				// 画面中央下部 結果表示
				{
					// 捕まえたかどうかで分岐
					if (m_score.isCaught)
					{
						// 合っていたかどうかでも分岐
						if (m_score.isCorrect)
						{
							FontAsset(U"Test")(U"正解！").drawAt(Scene::CenterF().x, Scene::CenterF().y + 150);
						}
						else
						{
							FontAsset(U"Test")(U"不正解...").drawAt(Scene::CenterF().x, Scene::CenterF().y + 150);
						}
					}
					else
					{
						FontAsset(U"Test")(U"時間切れ！").drawAt(Scene::CenterF().x, Scene::CenterF().y + 150);
					}

					m_gui.toResult.set(FontAsset(U"Test"), U"結果・タイトルへ")
								  .draw(Arg::bottomLeft = Vec2{ 10.0, Scene::Height() - 10.0 });

					// このレベルをクリアできていて、次のレベルが存在する場合のみ表示
					m_gui.toNextLevel.set(FontAsset(U"Test"), U"次のレベルへ", (m_score.isCorrect and m_isAvailableNextLevel()))
									 .draw(Arg::bottomRight = (Scene::Size() - Vec2{ 10.0, 10.0 }));
				}
			}
				break;
			default:
				break;
		}
	}
}
