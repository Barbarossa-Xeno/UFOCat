# include "Result.hpp"

namespace UFOCat
{

	Score::Generic::ByLevel &Result::m_currentScoreData() const
	{
		return getData().scores.back().records[getData().levelIndex];
	}

	Array<Score::Generic::ByLevel> &Result::m_currentScoreDatas() const
	{
		return getData().scores.back().records;
	}

	Result::Result(const InitData &init)
		: IScene{ init }
	{
		// 現在のプレイの総合得点を計算しておく
		const uint32 total = getData().scores.back().calculateTotal();

		// 閾値の割合がデカいほうから順に走査
		for (size_t i = Score::Titles.size(); --i > 0;)
		{
			// 閾値よりも総合得点が大きければ、その称号を与える
			if (total >= Score::Titles[i].threshold * Score::Generic::ByLevel::GetMaxTheoretical())
			{
				getData().scores.back().title = Score::Titles[i];
				break;
			}
			// 最後まで当てはまらなかったら、もう、一番下の称号を与える
			else if (i == 0)
			{
				getData().scores.back().title = Score::Titles[0];
			}
		}

		// GUI 要素初期化
		{
			m_gui.scoreDetails.addContents
			(
				GUI::TextBox
				{
					FontAsset(Util::FontFamily::KoharuiroSunray)
						(U"くわしいスコア"),
					48, Util::Palette::Brown, GUI::PositionType::Relative
				}.setMargin({ 0, 5 }),
				GUI::TextBox
				{
					FontAsset(Util::FontFamily::YuseiMagic)
						(U"小数点はとちゅうでしょうりゃくしています"),
					18, Util::Palette::Brown, GUI::PositionType::Relative
				}.setMargin({ 0, 10 })
			);

			for (auto &&data : m_currentScoreDatas())
			{
				// スコアの条件に応じてうまくインデントを下げる
				m_gui.scoreDetails.addContents
				(
					GUI::TextBox
					{
						FontAsset(Util::FontFamily::YuseiMagic)
							(U"★ レベル{}"_fmt(data.level)),
						28, Util::Palette::Brown, GUI::PositionType::Relative
					}.setMargin({ 5, 5 }),
					GUI::TextBox
					{
						FontAsset(Util::FontFamily::YuseiMagic)
							(data.isCaught ? (data.isCorrect ? U"正解!!" : U"不正解…") : U"時間切れ…"),
						22, Util::Palette::Brown, GUI::PositionType::Relative
					}.setMargin({ 0, 5 }).setIndent(20)
				);

				// 以下、小数点以下のけたが大きい場合小数第3位で四捨五入
				if (data.isCaught)
				{
					m_gui.scoreDetails.addContents
					(
						GUI::TextBox
						{
							FontAsset(Util::FontFamily::YuseiMagic)
								(U"猫をつかまえた！ ── +22"),
							18, Util::Palette::Brown, GUI::PositionType::Relative
						}.setIndent(40)
					);

					if (data.isCorrect)
					{
						m_gui.scoreDetails.addContents
						(
							GUI::TextBox
							{
								FontAsset(Util::FontFamily::YuseiMagic)
									(U"タイムボーナス！ ── ×{}"_fmt(Round(100 * 2.2 * (2.2 + 1 / (2.2 * data.response))) / 100)),
								18, Util::Palette::Brown, GUI::PositionType::Relative
							}.setIndent(60)
						);
					}

					m_gui.scoreDetails.addContents
					(
						GUI::TextBox
						{
							FontAsset(Util::FontFamily::YuseiMagic)
								(U"レベルとうたつボーナス！ ── ×{}"_fmt(Round(100 * Math::Exp(2.2 * data.level / 10.0)) / 100)),
							18, Util::Palette::Brown, GUI::PositionType::Relative
						}.setIndent(40)
					);

					if (data.consecutiveCorrect > 0)
					{
						m_gui.scoreDetails.addContents
						(
							GUI::TextBox
							{
								FontAsset(Util::FontFamily::YuseiMagic)
									(U"{} 回れんぞく正解！ ── +{}"_fmt(data.consecutiveCorrect, 222 * data.consecutiveCorrect)),
								18, Util::Palette::Brown, GUI::PositionType::Relative
							}.setIndent(40)
						);
					}
				}
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
				AudioAsset(Util::AudioSource::SE::CountUpScore).play();

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
						for (auto itr = Score::Titles.begin(); itr != Score::Titles.end(); ++itr)
						{
							// 現在のスコアのカウントが、比較対象の称号の閾値以下
							// （カウント / 理論値 が 1.0 以下になる状況）のとき、
							// ゲージを加算する
							if (double realThreshold = itr->threshold * Score::Generic::ByLevel::GetMaxTheoretical();
								m_ScoreCount <= realThreshold)
							{
								// パラメータ
								double t = 0.0;

								// 一番下の称号の場合、そのまま進捗を設定
								if (itr == Score::Titles.begin())
								{
									t = static_cast<double>(m_ScoreCount) / realThreshold;
								}
								// それ以外の場合、前の称号の閾値を引いた分だけ進捗を設定
								else
								{
									// 一個下の閾値をフィードバック
									const double prev = std::prev(itr)->threshold * Score::Generic::ByLevel::GetMaxTheoretical();
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
			else
			{
				if (not m_isFinishedCountUp)
				{
					AudioAsset(Util::AudioSource::SE::CountUpScore).stop();
					AudioAsset(Util::AudioSource::SE::FinishScore).play();

					m_isFinishedCountUp = true;
				}
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
				// まだスコアのカウントアップが途中だったら、シンバルは鳴らしておく
				if (not m_isFinishedCountUp)
				{
					AudioAsset(Util::AudioSource::SE::CountUpScore).stop();
					AudioAsset(Util::AudioSource::SE::FinishScore).play();
				}

				// リセット処理は、タイトル側で行う
				changeScene(SceneState::Title, 1.5s);
			}

			if (m_gui.scoreDetailsButton.set(32, U"もっとくわしく")
										.setPosition(Arg::bottomRight = (Scene::Size() - Size{ 10, 10 }))
										.isPressed())
			{
				m_gui.scoreDetails.setSize({ 400, 450 }).open();
			}

			m_gui.scoreDetails.isPressedOK();
		}

# if _DEBUG
		// デバッグ機能：Ctrl + Shift + S でスキップ
		if (KeyControl.pressed() and KeyShift.pressed() and KeyR.pressed())
		{
			changeScene(SceneState::Result, 1s);
		}
# endif
	}

	void Result::draw() const
	{
		Scene::Rect().draw(Util::Palette::Brown);
		DrawPolkaDotBackground(30, 0.3, Util::Palette::LightBrownAlt);
		Scene::Rect().draw(ColorF{ 0.0, 0.5 });

		// # スコア表示
		{
			// TODO: ほんとはこういうサイズもレスポンシブにすべきなんだろうな

			// 点数表示
			FontAsset(Util::FontFamily::KoharuiroSunray)(U"{}"_fmt(m_ScoreCount)).draw(120, Arg::bottomCenter = Scene::Center());

			const RectF& maxRegion = FontAsset(Util::FontFamily::KoharuiroSunray)(U"{}"_fmt(Score::Generic::ByLevel::GetMaxTheoretical())).region(120, Arg::bottomCenter = Scene::Center());

			// 回転座標系
			{
				const Transformer2D tr{ Mat3x2::Rotate(-15_deg, maxRegion.left().end) };
				FontAsset(Util::FontFamily::YuseiMagic)(U"今回の評価").draw(40, Arg::bottomCenter = Vec2{ maxRegion.x, maxRegion.y });
			}

			{
				const RectF &region1 = FontAsset(Util::FontFamily::YuseiMagic)(U"キミは").draw(26, Arg::bottomLeft = (m_gui.scoreTitleGauge.getRegion().tl() - Point{ 0, 20 }));

				const RectF &region2 = FontAsset(Util::FontFamily::KoharuiroSunray)(U"{}"_fmt(m_currentTitle.kanjiName)).drawBase(60, (region1.br() + Point{ 10, 5 }));

				FontAsset(Util::FontFamily::YuseiMagic)(U"UFO猫ハンターだ！！").draw(26, Arg::bottomLeft = Vec2{ (region2.br().x + 10), region1.br().y });

				// 称号ゲージ更新
				m_gui.scoreTitleGauge.draw();
			}
			
		}

		// # GUI 描画
		{
			m_gui.toTitle.draw();
			m_gui.scoreDetails.draw();
			m_gui.scoreDetailsButton.draw();
		}

		BrightenCursor();
	}

	Result::~Result()
	{
		// 一旦リソース解放
		TextureAsset::ReleaseAll();
	}
}
