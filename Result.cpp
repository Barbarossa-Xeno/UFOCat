# include "Result.hpp"

namespace UFOCat
{
	const Score &Result::m_getScore(size_t index) const
	{
		return getData().scores[index];
	}

	const Score &Result::m_currentScore() const
	{
		return getData().scores[getData().levelIndex];
	}

	Result::Result(const InitData &init)
		: IScene{ init }
	{
		// 現在のレベルの総合得点を計算しておく
		getData().scores[getData().levelIndex].calculateTotal();
	}


	void Result::update()
	{
		if (m_gui.toTitle.isPressed())
		{
			changeScene(UFOCat::State::Title);
		}
	}

	void Result::draw() const
	{
		if (m_currentScore().isCaught)
		{
			FontAsset(U"Test")(U"猫を捕まえた！").draw(Arg::leftCenter = Vec2{ 30.0, Scene::Center().y - 120.0 });
			FontAsset(U"Test")(U"+22").draw(Arg::rightCenter = Vec2{ Scene::Width() - 30.0, Scene::Center().y - 120.0 });

			if (m_currentScore().isCorrect)
			{
				FontAsset(U"Test")(U"正解ボーナス！").draw(Arg::leftCenter = Vec2{ 30.0, Scene::Center().y - 60.0 });
				FontAsset(U"Test")(U"×2.2").draw(Arg::rightCenter = Vec2{ Scene::Width() - 30.0, Scene::Center().y - 60.0 });

				{
					FontAsset(U"Test")(U"{:.2}秒で捕まえた！"_fmt(m_currentScore().response)).draw(Arg::leftCenter = Vec2{ 30.0, Scene::Center().y });
					double factor = 2.2 + 1 / (2.2 * m_currentScore().response);
					FontAsset(U"Test")(U"×{:.2}"_fmt(factor)).draw(Arg::rightCenter = Vec2{ Scene::Width() - 30.0, Scene::Center().y });
				}

				{
					FontAsset(U"Test")(U"レベルボーナス！").draw(Arg::leftCenter = Vec2{ 30.0, Scene::Center().y + 60.0 });
					double factor = Math::Exp(2.2 * (m_currentScore().level) / 10.0);
					FontAsset(U"Test")(U"×{:.2}"_fmt(factor)).draw(Arg::rightCenter = Vec2{ Scene::Width() - 30.0, Scene::Center().y + 60.0 });
				}

				if (m_currentScore().consecutiveCorrect > 0)
				{
					FontAsset(U"Test")(U"連続正解ボーナス！").draw(Arg::leftCenter = Vec2{ 30.0, Scene::Center().y + 120.0 });
					FontAsset(U"Test")(U"+{}"_fmt(222 * m_currentScore().consecutiveCorrect)).draw(Arg::rightCenter = Vec2{ Scene::Width() - 30.0, Scene::Center().y + 120.0 });
				}
			}

		}
		else
		{
			FontAsset(U"Test")(U"捕まえられなかった……").draw(Arg::leftCenter = Vec2{ 30.0, Scene::Center().y - 60.0 });
			FontAsset(U"Test")(U"+0").draw(Arg::rightCenter = Vec2{ Scene::Width() - 30.0, Scene::Center().y - 60.0 });
		}

		FontAsset(U"Test")(U"{}"_fmt(m_currentScore().total)).draw(40, Arg::bottomCenter = Vec2{ Scene::CenterF().x, Scene::Height() - 100.0 });

		m_gui.toTitle.set({ 10.0, Scene::Height() - 70.0 }, FontAsset(U"Test"), U"タイトルへ").draw();
	}
}
