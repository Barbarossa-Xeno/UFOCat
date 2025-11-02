# include "Wanted.hpp"

namespace UFOCat
{
	Wanted::Wanted(const InitData& init)
		: IScene{ init }
	{
		getData().targetIndex = Random(getData().cats.size() - 1);
		m_target = std::make_unique<CatObject>(getData().cats[getData().targetIndex].clone());

		// 現在行っているレベルのインデックスは、クリアしているレベルの数と同じなのを利用する
		// （そのレベルが終わり次第、isCleared のフラグを上げるため）
		getData().levelIndex = getData().levels.filter([](const LevelData &level) { return level.isCleared; }).size();

		// TODO: レベルが進むごとにちょっと時間を短くしたら面白いかも
		// ターゲット情報の表示時間
		getData().timer.set(3s);

		// # GUI 初期化
		{
			m_gui.levelBar.set({ 0.85 * 180, 10 }, Colors::Brown)
						  .setPosition(Arg::bottomCenter = Vec2{ 180.0 / 2 + 5, 100 - 5 - 10 })
						  .setProgress((getData().levelIndex + 1) / 10.0);

			m_gui.flyer = Texture{ U"texture/flyer.png", TextureDesc::Mipped };
		}
	}

	void Wanted::update()
	{
		if (not getData().timer.isStarted())
		{
			getData().timer.start();
		}
		else if (getData().timer.reachedZero())
		{
			changeScene(State::Level, 0.7s);
		}
		
# if _DEBUG    
		if (KeyControl.pressed() and KeyShift.pressed())
		{
			// デバッグ機能：Ctrl + Shift + S でスキップ
			if (KeyS.pressed())
			{
				getData().timer.reset();
				changeScene(State::Level, 0.7s);
			}

			if (KeyQ.pressed())
			{
				if (getData().timer.isRunning())
				{
					getData().timer.pause();
				}
				else
				{
					getData().timer.start();
				}
			}
		}
# endif
	}

	void Wanted::draw() const
	{
		// # 左上のレベル表示
		{
			RoundRect back{ 5, 5, 180, 100, 6 };
			back.drawShadow({ 2, 2 }, 4).draw(ColorF{ 0.95, 0.9, 0.8 });
			FontAsset(FontName::YuseiMagic)(U"★ {}"_fmt(getData().levelIndex + 1)).draw(26, back.rect.tl() + Vec2{ 10, 5 }, ColorF{ 0.4, 0.3, 0.2 });
			m_gui.levelBar.draw();
		}

		FontAsset(FontName::YuseiMagic)(U"見つけるUFOネコは...").drawAt(36, Scene::Center().x, 40);

		{
			const RectF &backRegion = m_gui.flyer.resized(Scene::Height() - 150).drawAt(Scene::Center());

			Vec2 targetOrigin{ Scene::Center().x, Scene::Center().y - 20 };

			// シャドウ
			m_target->getTexture().scaled(0.45).drawAt(targetOrigin + Point(10, 10), ColorF{ 0.4, 0.3, 0.2 });

			// 実際の
			m_target->getTexture().scaled(0.45).drawAt(targetOrigin);


		}		

		BrightenCursor();
	}
}
