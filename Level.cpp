# include "Level.hpp"

namespace UFOCat
{
	LevelData& Level::m_currentLevel() const
	{
		return getData().levels[getData().levelIndex];
	}

	Array<ScoreData> &Level::m_currentScoreDatas() const
	{
		return getData().scores.back().scoreDataList;
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
		// テクスチャ取得
		{
			m_gui.timer = Texture{ U"texture/timer.png", TextureDesc::Mipped };
			m_background = getData().backgrounds.choice();
		}

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
			// 類似条件を下回るのを「その他」としてカウント
			else if (m_currentLevel().similarity > cat.getCatData().getSameDataCount(m_target->getCatData()))
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
			similars.shuffle().resize(similarCount);
		}
		// 少なすぎる場合
		else if (comp < 0)
		{
			// 条件を緩和して絞り込んだ猫を入れる配列
			Array<std::shared_ptr<CatObject>> looses;

			// 類似度を1つ下げて絞り込んだものを looses に入れる
			for (const auto& cat : others)
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
					const auto& instead = others.shuffle().choice();
					similars << instead;
					others.remove(instead);
				}
			}

		}

		// others は既定の数から溢れている可能性が高いので、ランダムに削る
		// 既定の数に達しない場合は、それでもよしとする
		others.shuffle().resize(otherCount);

		// レベル中に行うアクションリストの中から、それぞれの発生確率だけを抜き取ったリストで確率分布をつくる
		m_actionProbabilities = DiscreteDistribution{ m_currentLevel().actionDataList.map([](const LevelData::ActionData& data) { return data.probability; }) };

		// 最終的に catsInPhase にまとめて
		m_selections.append(similars).append(others);

		// 現在のレベルに合わせてターゲットの出現時刻を設定
		m_setTargetSpawnTime(getData().levelIndex + 1);

		// 3、2、1、GO! のカウントダウンを入れるための待機時間をセット
		// シーンのフェードインアウト時間を考慮して少し長めに取る
		getData().timer.pause();
		getData().timer.set(4s);
	}

	void Level::update()
	{
		// # ステート依存処理
		switch (m_state)
		{
			case Level::State::Before:
			{
				// タイマー稼働してない
				if (not getData().timer.isRunning())
				{
					// 停止していて、残り時間が0でない場合は、
					// 初期化時にタイマーをセットしていたということなので
					if (not getData().timer.reachedZero())
					{
						getData().timer.start();
					}
					// 次にもう一回 0 になったら
					else
					{
						// ステートをプレイ中に変更する
						m_state = Level::State::Playing;

						// 制限時間を決めて、タイマー開始
						// 0.5s 猶予を持たせて、気持ち長めにすることで間に入る処理の影響を減らす
						getData().timer.restart(m_currentLevel().timeLimit + 0.5s);
					}
					
				}
			}
			break;

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
				
				// ### 制限時間内

				if (getData().timer.isRunning())
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

							for (size_t i = 0; i < m_currentScoreDatas().size() - 1; i++)
							{
								// 次のスコアデータが存在しない場合は終了
								if (m_currentScoreDatas()[i + 1].level == InvalidIndex)
								{
									break;
								}

								// 今のレベルと次のレベルの両方で正解していたら増やす
								if (m_currentScoreDatas()[i].isCorrect and m_currentScoreDatas()[i + 1].isCorrect)
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
				// 時間外
				else
				{
					// タイマーがセットだけされている状態 -> 前のステートからの遷移直後
					if (not getData().timer.reachedZero())
					{
						getData().timer.start();
					}
					else
					{
						// 制限時間が終わったら、終了表示を出しに行く
						m_state = Level::State::Finish;
					}
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
				// # GUI 処理
				{
					// 次のレベルへ進めるかは、今回が合っていて かつ 次のレベルが存在する必要がある
					bool canContinue = (m_score.isCorrect and m_isAvailableNextLevel());

					// 次のレベルへ進むボタン
					if (m_gui.toNextLevel.set(32, U"次のレベルへ", canContinue)
										  .setPosition(Arg::bottomRight = (Scene::Size() - Vec2{ 10.0, 10.0 })).isPressed())
					{
						// 次に進む場合は、レベルデータにもクリア情報を反映
						// これにより、Wanted シーンの初期化で自動的に次のレベルへ進む
						m_currentLevel().isCleared = m_score.isCorrect;

						// スコアを格納する
						m_currentScoreDatas()[getData().levelIndex] = m_score;

						// 次のレベル初期化へ
						changeScene(UFOCat::State::Wanted);
					}

					// タイトルへ戻るボタン
					if (m_gui.toResult.set(32, U"結果 / タイトルへ")
									  .setPosition(Arg::bottomLeft = Vec2{ 10.0, Scene::Height() - 10.0 })
									  .isPressed())
					{
						if (canContinue)
						{
							// 次へ行けるのにやめようとしてる人には、ダイアログを出す
							m_gui.dialog.set(22, U"本当に戻りますか？\nここまでのデータは失われます").open();
						}
						else
						{
							// 次のレベルが存在しないためにタイトルへ戻らなければならない場合もあるので、
							// レベルデータにもクリア情報を反映
							// （はなから間違えていれば false になるだけ）
							m_currentLevel().isCleared = m_score.isCorrect;

							// スコアを格納する
							m_currentScoreDatas()[getData().levelIndex] = m_score;

							// 結果シーンへ
							changeScene(UFOCat::State::Result, 1s);
						}
					}

					// ダイアログ タイトルへ戻るボタンを押して、ダイアログが開かれた場合にボタン判定が始まる
					if (m_gui.dialog.isPressedOK())
					{
						// スコアを格納する
						m_currentScoreDatas()[getData().levelIndex] = m_score;

						// 結果シーンへ
						changeScene(UFOCat::State::Result, 1s);
					}

					m_gui.dialog.isPressedCancel();
				}
			
			}
			break;
			default:
				break;
		}

# if _DEBUG    // デバッグ機能：
		if (KeyControl.pressed() and KeyShift.pressed())
		{
			// Ctrl + Shift + S でスキップ
			if (KeyS.pressed())
			{
				m_currentLevel().isCleared = true;
				m_currentScoreDatas()[getData().levelIndex] = ScoreData{ getData().levelIndex + 1, true, true, 0.3, getData().levelIndex };
			
				getData().timer.reset();

				if (getData().levelIndex + 1 >= getData().levels.size())
				{
					// レベルがもう存在しない場合は、結果シーンへ
					changeScene(UFOCat::State::Result);
				}
				else
				{
					changeScene(UFOCat::State::Wanted);
				}
			}

			// Ctrl + Shift + R で全スキップ
			if (KeyR.pressed())
			{
				// 全てのレベルをクリアしたことにして結果シーンへ
				// 一気に移るので、クリアフラグを上げる必要もない
				m_currentScoreDatas().each_index([this](size_t i, ScoreData& score) { score = ScoreData{ i + 1, true, true, 0.5, i }; });
				getData().timer.reset();
				changeScene(UFOCat::State::Result);
			}

			// Ctrl + Shift + Q でタイマー一時停止/再開
			if (KeyQ.pressed())
			{
				if (getData().timer.isRunning())
				{
					getData().timer.pause();
				}
				else
				{
					getData().timer.resume();
				}
			}
		}
# endif
	}

	void Level::draw() const
	{
		m_background.fitted(Scene::Size()).draw();

		// # 共通処理（背面）
		{

			for (const auto& cat : getData().spawns)
			{
				if (not cat)
				{
					continue;
				}

				cat->draw();
			}
		}

		// # ステート依存処理
		switch (m_state)
		{
			// ## 開始前（カウントダウン処理など）
			case UFOCat::Level::State::Before:	
			{
				// 秒数表示
				String text = U"";

				// 線形補間のパラメータ
				// sF() は小数点以下も含めた秒数、s() は切り捨ての整数秒数であることを利用して
				// 1.0 -> 0.0 に向かう値を得る
				double t = Clamp(getData().timer.sF() - getData().timer.s(), 0.0, 1.0);

				double textSize = 200.0;

				// GO! のほう
				if (getData().timer.s() == 0)
				{
					text = U"GO!";
				}
				// 3、2、1 のほう
				else
				{
					text = U"{}"_fmt(getData().timer.s());

					// テキストサイズをイージングで小さいほうに変化させる
					textSize = std::lerp(40.0, 200.0, EaseOutQuart(t));
				}

				FontAsset(FontName::KoharuiroSunray)(text).drawAt(textSize, Scene::Center(), ColorF{ 1.0, EaseOutExpo(t) });
			}
			break;

			// ## プレイ中（タイマー表示など）
			case UFOCat::Level::State::Playing:
			{
				{
					// 中心 (60, 60) としてストップウォッチのテクスチャを最大 60px で描画
					RectF swRegion = m_gui.timer.resized(60).drawAt(Point{ 60, 60 });

					// 針の角度
					double angle = 2 * Math::Pi * getData().timer.sF() / m_currentLevel().timeLimit.count();

					// 針を描画 ストップウォッチの中心からちょっとずらした位置
					// 角度は反時計回りだったので、更に反転させておいた
					Line{ Vec2{ swRegion.centerX(), swRegion.centerY() + 4.0 }, Arg::angle = -angle, 16.0 }.draw(LineStyle::RoundCap, 4.0, Palette::Salmon);

					FontAsset(FontName::YuseiMagic)(U"のこり").draw(20, swRegion.tr().x + 10, swRegion.tr().y - 10);
					RectF tRegion = FontAsset(FontName::YuseiMagic)(U"{}"_fmt(getData().timer.s())).drawBase(36, Vec2{ swRegion.br().x + 10, swRegion.br().y - 5 });
					FontAsset(FontName::YuseiMagic)(U"秒").drawBase(24, Vec2{ tRegion.br().x + 10, swRegion.br().y - 5 });
				}
			}
			break;

			// L335より、終了表示は 3s 間
			case UFOCat::Level::State::Finish:
			{
				// 移動補間のパラメータ
				double t = 1.0;

				// 1.7s までの間
				if (m_watch.now() <= 1.7)
				{
					t = Clamp(m_watch.now() / 2.0, 0.0, 1.0);
				}
				// それ以外は t = 1.0 として処理

				const DrawableText &view = FontAsset(U"KoharuiroSunray")(U"Finish!!");

				const RectF &region = view.region();

				const Vec2 begin{ -region.w, Scene::Center().y };

				view.drawAt(150.0, begin.lerp(Scene::CenterF(), EaseOutBounce(t)));
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

						FontAsset(FontName::YuseiMagic)(U"キミが捕まえた猫").drawAt(Scene::CenterF() + SizeF(-image.size.x, -150));
					}
				}
				// 画面右側 ターゲットを表示
				{
					auto &&image = m_target->getTexture().scaled(m_CatImageScale);
					image.drawAt(Scene::CenterF() + SizeF(image.size.x, 0));
					FontAsset(FontName::YuseiMagic)(U"ターゲット").drawAt(Scene::CenterF() + SizeF(image.size.x, -150));
				}
				// 画面中央下部 結果表示
				{
					// 捕まえたかどうかで分岐
					if (m_score.isCaught)
					{
						// 合っていたかどうかでも分岐
						if (m_score.isCorrect)
						{
							FontAsset(FontName::YuseiMagic)(U"正解！").drawAt(Scene::CenterF().x, Scene::CenterF().y + 150);
						}
						else
						{
							FontAsset(FontName::YuseiMagic)(U"不正解...").drawAt(Scene::CenterF().x, Scene::CenterF().y + 150);
						}
					}
					else
					{
						FontAsset(FontName::YuseiMagic)(U"時間切れ！").drawAt(Scene::CenterF().x, Scene::CenterF().y + 150);
					}

					m_gui.toResult.draw();

					// このレベルをクリアできていて、次のレベルが存在する場合のみ表示される update 参照
					m_gui.toNextLevel.draw();

					m_gui.dialog.draw();
				}
			}
				break;
			default:
				break;
		}

		BrightenCursor();
	}
}
