# include "Level.hpp"

namespace UFOCat
{
	LevelData& Level::m_currentPhase()
	{
		return getData().levels[getData().levelIndex];
	}

	void Level::m_setTargetAppearTime(size_t level)
	{
		// 定式の詳細は仕様書参照
		const double&& term1 = m_currentPhase().timeLimit.count() / Random(2, 4);
		const double&& term2 = level * (m_currentPhase().breedData.total() * m_currentPhase().intervalData.period.count()) / (m_currentPhase().actionDataList.size() * m_currentPhase().breedData.similar);
		m_targetAppearTime = Duration{ Min((term1 + term2), 0.75 * m_currentPhase().timeLimit.count()) };
	}

	Level::Level(const InitData& init)
		: IScene{ init }
	{
		// 前回シーンで決めたターゲットを取得
		m_target = std::make_shared<CatObject>(getData().cats[getData().targetIndex]);

		// レベルデータのうち、登場する猫の数に関するデータを取得する
		const auto& similarCount = m_currentPhase().breedData.similar;
		const auto& otherCount = m_currentPhase().breedData.other;

		// それぞれ、似ている猫とそうじゃない猫を絞り込む用の配列を用意する
		Array<std::shared_ptr<CatObject>> similars;
		Array<std::shared_ptr<CatObject>> others;

		// 前回レベルで使った猫を吹っ飛ばし、shared_ptr も解放する
		m_selections.release();

		// 全種類の猫の中から、ターゲットと似ている猫とそうでない猫を振り分ける
		for (const auto& cat : getData().cats)
		{
			// それと、ターゲットと同じのを参照しないように保障する
			if (m_currentPhase().similarity == cat.getCatData().getSameDataCount(m_target->getCatData())
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
				if ((m_currentPhase().similarity - 1) == cat->getCatData().getSameDataCount(m_target->getCatData())
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
		m_actionProbabilities = DiscreteDistribution{ m_currentPhase().actionDataList.map([](const LevelData::ActionData &data) { return data.probability; }) };

		// 最終的に catsInPhase にまとめて
		m_selections.append(similars).append(others);

		// ステートをプレイ中に変更する
		m_state = Level::State::Playing;

		// 制限時間を決めて、タイマー開始
		// 1s 猶予を持たせて、気持ち長めにすることで間に入る処理の影響を減らす
		getData().timer.restart(m_currentPhase().timeLimit + 1s);

		// 現在のレベルに合わせてターゲットの出現時刻を設定
		m_setTargetAppearTime(getData().levelIndex + 1);

		// ターゲットの出現情報をリセット
		m_appearedTarget = false;
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
						if (getData().timer.remaining() <= m_targetAppearTime and (not m_appearedTarget))
						{
							// ターゲットを（コピーして）湧かせる
							getData().spawns << std::make_unique<CatObject>(CatObject(*m_target));
							// ターゲットにも同様にアクションと速度の設定を行う
							getData().spawns.back()->setAction(DiscreteSample(m_currentPhase().actionDataList, m_actionProbabilities)).setRandomVelocity(getData().levelIndex + 1);
							// ターゲットの出現フラグをあげる
							m_appearedTarget = true;
						}
						else
						{
							// ターゲット以外の猫をランダムに指定個選んで（コピーして）追加
							for (int32 i = 0; i < m_currentPhase().intervalData.count; i++)
							{
								getData().spawns << std::make_unique<CatObject>
									(
										// アクションを抽選してセットし、現在のレベルに合わせて速度もランダムに決める
										CatObject(*(m_selections.choice()))
											.setAction(DiscreteSample(m_currentPhase().actionDataList, m_actionProbabilities))
												.setRandomVelocity(getData().levelIndex + 1)
									);
							}
						}
					}, m_currentPhase().intervalData.period);

				// ## 制限時間内と時間超過後での処理
				if (not getData().timer.reachedZero())
				{
					for (const auto& cat : getData().spawns)
					{
						// 猫をタッチしたら、その正誤を代入
						if (cat->act().checkCatchable(m_target->getCatData(), &m_score.isCorrect))
						{
							// 捕まえた猫を記録
							m_caught = &cat;

							// ターゲットとの正誤にかかわらず、触ったことにはしておく
							m_score.isCaught = true;
							
							// 反応時間を記録
							m_score.response = (m_targetAppearTime - getData().timer.remaining()).count();

							// レベル終了へ
							m_state = Level::State::Finish;
							m_watch.reset();
							break;
						}
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
				if (m_gui.toResult.isPressed())
				{
					changeScene(UFOCat::State::Result);
				}
			}
				break;
			default:
				break;
		}
	}

	void Level::draw() const
	{
		// # 共通処理（背面）
		{
			for (const auto& cat : getData().spawns)
			{
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

					m_gui.toResult.set({ 10.0, Scene::Height() - 70.0 }, FontAsset(U"Test"), U"結果へ").draw();
				}
			}
				break;
			default:
				break;
		}
	}
}
