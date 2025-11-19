# include "Result.hpp"

namespace UFOCat
{

	Score::GeneralScoreData::ScoreData &Result::m_currentScoreData() const
	{
		return getData().scores.back().scoreDataList[getData().levelIndex];
	}

	Array<Score::GeneralScoreData::ScoreData> &Result::m_currentScoreDatas() const
	{
		return getData().scores.back().scoreDataList;
	}

	Result::Result(const InitData &init)
		: IScene{ init }
	{
		// 現在のプレイの総合得点を計算しておく
		const uint32 total = getData().scores.back().calculateTotal();

		// 閾値の割合がデカいほうから順に走査
		for (size_t i = Score::GeneralScoreData::Titles.size(); --i > 0;)
		{
			// 閾値よりも総合得点が大きければ、その称号を与える
			if (total >= Score::GeneralScoreData::Titles[i].threshold * Score::GeneralScoreData::ScoreData::GetMaxTheoretical())
			{
				getData().scores.back().titleData = Score::GeneralScoreData::Titles[i];
				break;
			}
			// 最後まで当てはまらなかったら、もう、一番下の称号を与える
			else if (i == 0)
			{
				getData().scores.back().titleData = Score::GeneralScoreData::Titles[0];
			}
		}
	}


	void Result::update()
	{
		// # スコア表示更新処理
		{
			// スコアと称号ゲージを増やす
			if (const size_t total = getData().scores.back().total;
				m_ScoreCount < total)
			{
				// インターバルは 2.0s を目指すが、引き算の結果がデルタタイムより小さくなった場合は、デルタタイムを使用する
				double interval = Max(2.0 / total - Scene::DeltaTime(), Scene::DeltaTime());

				// インターバルごとに繰り返す
				m_scoreCountUpWatch.setInterval([&]()
					{
						// 実際の総得点とカウントとの差が10を切るまで、スピードを上げてカウントアップ
						if (total - m_ScoreCount >= 10)
						{
							// 速度（デルタタイムの足し上げ）の切り上げを変化にする
							m_ScoreCount += static_cast<size_t>(Ceil(m_countUpAcceleration));
						}
						// 最後は1つずつ足し上げて誤差がないように終了
						else
						{
							++m_ScoreCount;
						}
						

						// 前回の速度 x デルタタイムの足し上げ -> 加速度的
						m_countUpAcceleration += m_countUpAcceleration * Scene::DeltaTime();

						// 閾値の割合がデカいほうから順に走査
						for (auto itr = Score::GeneralScoreData::Titles.begin(); itr != Score::GeneralScoreData::Titles.end(); ++itr)
						{
							// 現在のスコアのカウントが、比較対象の称号の閾値以下
							// （カウント / 理論値 が 1.0 以下になる状況）のとき、
							// ゲージを加算する
							if (double realThreshold = itr->threshold * Score::GeneralScoreData::ScoreData::GetMaxTheoretical();
								m_ScoreCount <= realThreshold)
							{
								// パラメータ
								double t = 0.0;

								// 一番下の称号の場合、そのまま進捗を設定
								if (itr == Score::GeneralScoreData::Titles.begin())
								{
									t = static_cast<double>(m_ScoreCount) / realThreshold;
								}
								// それ以外の場合、前の称号の閾値を引いた分だけ進捗を設定
								else
								{
									// 一個下の閾値をフィードバック
									const double prev = std::prev(itr)->threshold * Score::GeneralScoreData::ScoreData::GetMaxTheoretical();
									const double num = static_cast<double>(m_ScoreCount) - prev;
									const double den = realThreshold - prev;
									t = num / den;
								}

								// 称号を更新
								m_currentTitle = *itr;

								// ゲージを更新
								m_gui.scoreTitleGauge.setProgress(Easing::Sine(t));

								// 更新したら走査やめる
								break;
							}
						}
					}, Duration(interval));
			}

			
		}

		// # GUI 更新処理
		{
			m_gui.scoreTitleGauge.set({ 0.65 * Scene::Width(), 15.0 }, Util::Palette::LightBrown)
				.setPosition(Arg::topCenter = Vec2{ Scene::Center().x, Scene::Center().y + 110 });
			if (m_gui.toTitle.set(32, U"タイトルへ")
							  .setPosition(Arg::bottomLeft = Vec2{ 10.0, Scene::Height() - 10.0 })
							  .isPressed())
			{
				// リセット処理は、タイトル側で行う
				changeScene(State::Title, 1.5s);
			}
		}

# if _DEBUG
		// デバッグ機能：Ctrl + Shift + S でスキップ
		if (KeyControl.pressed() and KeyShift.pressed() and KeyR.pressed())
		{
			changeScene(State::Result, 1s);
		}
# endif
	}

	void Result::draw() const
	{
		Scene::Rect().draw(Util::Palette::Brown);
		DrawPolkaDotBackground(30, 0.3, Util::Palette::LightBrownAlt);
		Scene::Rect().draw(ColorF{ 0.0, 0.5 });

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

		// # スコア表示
		{
			// TODO: ほんとはこういうサイズもレスポンシブにすべきなんだろうな

			// 点数表示
			FontAsset(Util::FontFamily::KoharuiroSunray)(U"{}"_fmt(m_ScoreCount)).draw(120, Arg::bottomCenter = Scene::Center());

			const RectF& maxRegion = FontAsset(Util::FontFamily::KoharuiroSunray)(U"{}"_fmt(Score::GeneralScoreData::ScoreData::GetMaxTheoretical())).region(120, Arg::bottomCenter = Scene::Center());

			// 回転座標系
			{
				const Transformer2D tr{ Mat3x2::Rotate(-15_deg, maxRegion.left().end) };
				FontAsset(Util::FontFamily::YuseiMagic)(U"今回の評価").draw(40, Arg::bottomCenter = Vec2{ maxRegion.x, maxRegion.y });
			}

			{
				// RoundRect{ Vec2{ }, SizeF(0.65 * Scene::Width(), 150.0), 12.0};

				const RectF &region1 = FontAsset(Util::FontFamily::YuseiMagic)(U"キミは").draw(26, Arg::bottomLeft = (m_gui.scoreTitleGauge.getRegion().tl() - Point{ 0, 20 }));

				const RectF &region2 = FontAsset(Util::FontFamily::KoharuiroSunray)(U"{}"_fmt(m_currentTitle.kanjiName)).drawBase(60, (region1.br() + Point{ 10, 5 }));

				FontAsset(Util::FontFamily::YuseiMagic)(U"UFO猫ハンターだ！！").draw(26, Arg::bottomLeft = Vec2{ (region2.br().x + 10), region1.br().y });

				// 称号ゲージ更新
				m_gui.scoreTitleGauge.draw();
			}
			
		}

		m_gui.toTitle.draw();

		BrightenCursor();
	}
}
