# include "Result.hpp"

namespace UFOCat
{

	ScoreData &Result::m_currentScoreData() const
	{
		return getData().scores.back().scoreDataList[getData().levelIndex];
	}

	Array<ScoreData> &Result::m_currentScoreDatas() const
	{
		return getData().scores.back().scoreDataList;
	}

	Result::Result(const InitData &init)
		: IScene{ init }
	{
		// 総合得点を計算しておく
		const uint32 total = getData().scores.back().calculateTotal();

		// 閾値の割合がデカいほうから順に走査
		for (size_t i = Score::Titles.size(); --i > 0;)
		{
			// 閾値よりも総合得点が大きければ、その称号を与える
			if (total >= Score::Titles[i].threshold * ScoreData::GetMaxTheoretical())
			{
				getData().scores.back().titleData = Score::Titles[i];
				break;
			}
			// 最後まで当てはまらなかったら、もう、一番下の称号を与える
			else if (i == 0)
			{
				getData().scores.back().titleData = Score::Titles[0];
			}
		}
	}


	void Result::update()
	{
		if (m_gui.toTitle.isPressed())
		{
			// TODO: リセット処理しましょう

			changeScene(UFOCat::State::Title);
		}

		{
			if (const size_t total = getData().scores.back().total;
				tempScore < total)
			{
				// インターバルは 2.0s を目指すが、引き算の結果がデルタタイムより小さくなった場合は、デルタタイムを使用する
				double interval = Max(2.0 / total - Scene::DeltaTime(), Scene::DeltaTime());

				// 最終表示にもっていくのに 5.0s 以上かかる場合は、
				// tempScore が大体 5.0s かけて増やした時の値以上になっていることを確認して
				// 途中でぶちぎる
				if (interval * total >= 5.0
					and tempScore >= 5.0 / interval)
				{
					// 直接代入
					tempScore = total;

					// 既に記録された称号での閾値計算に切り替えて、プログレスバーに反映
					double t = static_cast<double>(total) / (getData().scores.back().titleData.threshold * ScoreData::GetMaxTheoretical());
					m_gui.scoreBar.setProgress(t);
				}
				else
				{
					m_scoreCountUpWatch.setInterval([&]()
						{
							++tempScore;

							// 閾値の割合がデカいほうから順に走査
							for (auto itr = Score::Titles.begin(); itr != Score::Titles.end(); ++itr)
							{
								if (double realThreshold = itr->threshold * ScoreData::GetMaxTheoretical();
									tempScore <= realThreshold)
								{
									// 一番下の称号の場合、そのまま進捗を設定
									if (itr == Score::Titles.begin())
									{
										double t = static_cast<double>(tempScore) / realThreshold;
										m_gui.scoreBar.setProgress(t);
									}
									// それ以外の場合、前の称号の閾値を引いた分だけ進捗を設定
									else
									{
										// 一個下の閾値をフィードバック
										double feedbacked = static_cast<double>(tempScore) - std::prev(itr)->threshold * ScoreData::GetMaxTheoretical();

										double t = feedbacked / realThreshold;
										m_gui.scoreBar.setProgress(t);
									}
									break;
								}
							}
						}, Duration());
				}

				
			}
		}
	}

	void Result::draw() const
	{
		// TODO: 進んだレベル分だけ表示、アニメーションスクロール、終わった後自分でスクロール可

		/*if (m_currentScoreData().isCaught)
		{
			FontAsset(U"Test")(U"猫を捕まえた！").draw(Arg::leftCenter = Vec2{ 30.0, Scene::Center().y - 120.0 });
			FontAsset(U"Test")(U"+22").draw(Arg::rightCenter = Vec2{ Scene::Width() - 30.0, Scene::Center().y - 120.0 });

			if (m_currentScoreData().isCorrect)
			{
				FontAsset(U"Test")(U"正解ボーナス！").draw(Arg::leftCenter = Vec2{ 30.0, Scene::Center().y - 60.0 });
				FontAsset(U"Test")(U"×2.2").draw(Arg::rightCenter = Vec2{ Scene::Width() - 30.0, Scene::Center().y - 60.0 });

				{
					FontAsset(U"Test")(U"{:.2}秒で捕まえた！"_fmt(m_currentScoreData().response)).draw(Arg::leftCenter = Vec2{ 30.0, Scene::Center().y });
					double factor = 2.2 + 1 / (2.2 * m_currentScoreData().response);
					FontAsset(U"Test")(U"×{:.2}"_fmt(factor)).draw(Arg::rightCenter = Vec2{ Scene::Width() - 30.0, Scene::Center().y });
				}

				{
					FontAsset(U"Test")(U"レベルボーナス！").draw(Arg::leftCenter = Vec2{ 30.0, Scene::Center().y + 60.0 });
					double factor = Math::Exp(2.2 * (m_currentScoreData().level) / 10.0);
					FontAsset(U"Test")(U"×{:.2}"_fmt(factor)).draw(Arg::rightCenter = Vec2{ Scene::Width() - 30.0, Scene::Center().y + 60.0 });
				}

				if (m_currentScoreData().consecutiveCorrect > 0)
				{
					FontAsset(U"Test")(U"連続正解ボーナス！").draw(Arg::leftCenter = Vec2{ 30.0, Scene::Center().y + 120.0 });
					FontAsset(U"Test")(U"+{}"_fmt(222 * m_currentScoreData().consecutiveCorrect)).draw(Arg::rightCenter = Vec2{ Scene::Width() - 30.0, Scene::Center().y + 120.0 });
				}
			}

		}
		else
		{
			FontAsset(U"Test")(U"捕まえられなかった……").draw(Arg::leftCenter = Vec2{ 30.0, Scene::Center().y - 60.0 });
			FontAsset(U"Test")(U"+0").draw(Arg::rightCenter = Vec2{ Scene::Width() - 30.0, Scene::Center().y - 60.0 });
		}*/

		{
			// TODO: ほんとはこういうサイズもレスポンシブにすべきなんだろうな
			const RectF &region = FontAsset(U"Test")(U"{}"_fmt(tempScore)).draw(120, Arg::bottomCenter = Scene::Center());

			const RectF& maxRegion = FontAsset(U"Test")(U"{}"_fmt(ScoreData::GetMaxTheoretical())).region(120, Arg::bottomCenter = Scene::Center());
			maxRegion.drawFrame();

			// 回転座標
			{
				const Transformer2D tr{ Mat3x2::Rotate(-15_deg, maxRegion.left().end) };
				FontAsset(U"Test")(U"今回の評価").draw(40, Arg::bottomCenter = Vec2{ maxRegion.x, maxRegion.y });
			}
		}

		{
			m_gui.scoreBar.setSize({ 0.7 * Scene::Width(), 150.0 })
						  .draw(Arg::topCenter = Vec2{ Scene::Center().x, Scene::Center().y + 40 }, SizeF{ 0.9, 0.1 });
		}

		/*{
			RectF back{ Arg::topCenter(Scene::Center().x, Scene::Center().y + 40), 0.7 * Scene::Width(), 150.0 };
			back.rounded(12).draw();

			RoundRect bar = back.scaled(0.9, 0.1).rounded(9);
			bar.drawFrame(2, Palette::Dimgray);
			bar.stretched(-2).draw(Palette::Bisque);
		}*/

		m_gui.toTitle.set(FontAsset(U"Test"), U"タイトルへ").draw(Arg::bottomLeft = Vec2{ 10.0, Scene::Height() - 10.0 });
	}
}
