# include "Wanted.hpp"

namespace UFOCat
{
	void Wanted::m_showColor()
	{

	}

	Wanted::Wanted(const InitData& init)
		: IScene{ init }
	{
		// ターゲットを選ぶ
		getData().targetIndex = Random(getData().cats.size() - 1);
		m_target = std::make_shared<CatObject>(getData().cats[getData().targetIndex]);

		// 現在行っているレベルのインデックスは、クリアしているレベルの数と同じなのを利用する
		// （そのレベルが終わり次第、isCleared のフラグを上げるため）
		getData().levelIndex = getData().levels.filter([](const LevelData &level) { return level.isCleared; }).size();

		// TODO: レベルが進むごとにちょっと時間を短くしたら面白いかも
		// ターゲット情報の表示時間
		getData().timer.set(3s);

		// # GUI 初期化
		{
			m_gui.levelBar.set(SizeF{ 0.85 * 180, 10 }, Colors::Brown)
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
		// # デバッグ機能
		if (KeyControl.pressed() and KeyShift.pressed())
		{
			// Ctrl + Shift + S でスキップ
			if (KeyS.pressed())
			{
				getData().timer.reset();
				changeScene(State::Level, 0.7s);
			}

			// Ctrl + Shift + Q でタイマーの一時停止・再開
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

	void Wanted::draw() const
	{
		// # 左上のレベル表示
		{
			RoundRect back{ 5, 5, 180, 100, 6 };
			back.drawShadow(Point{ 2, 2 }, 4).draw(ColorF{ 0.95, 0.9, 0.8 });
			FontAsset(FontName::YuseiMagic)(U"★ {}"_fmt(getData().levelIndex + 1)).draw(26, back.rect.tl() + Point{ 10, 5 }, ColorF{ 0.4, 0.3, 0.2 });
			m_gui.levelBar.draw();
		}

		FontAsset(FontName::YuseiMagic)(U"見つけるUFOネコは...").drawAt(36, Scene::Center().x, 40);

		// # チラシ部分
		{
			// チラシの描画とその範囲
			const RectF &backRegion = m_gui.flyer.resized(Scene::Height() - 150).drawAt(Scene::Center());

			// ターゲットのテクスチャ描画位置（中央基準）
			Vec2 targetOrigin{ Scene::Center().x, Scene::Center().y - 20 };

			// シャドウ
			m_target->getTexture().scaled(0.45).drawAt(targetOrigin + Point{ 5, 5 }, ColorF{ 0.4, 0.3, 0.2 });

			// 実際の
			m_target->getTexture().scaled(0.45).drawAt(targetOrigin);

			const RectF textBoxRegion{ 0.3 * backRegion.x, 0.4 * backRegion.y };

			// ## 猫種の表示領域
			// 「猫種」というタイトルとそのバックを塗りつぶすための RectF を計算
			const RectF &box1 = textBoxRegion.movedBy(backRegion.x + 20, targetOrigin.y + 110).draw(Colors::Brown);
			FontAsset(FontName::YuseiMagic)(U"猫種").drawAt(20, box1.center(), Colors::LightBrownAlt);

			// 猫種名を表示するエリア、マージン (20) 分調整する
			const RectF &breedRegion = box1.movedBy(box1.w + 20, 0).setSize(backRegion.w - box1.w - 20 - 20 - 20, box1.h);

			// 名前は短いのから長いのもあるので、
			// エリアから溢れない範囲でフォントサイズを可変にする
			{
				double fontSize = 40;
				// このメソッドは矩形内にすべての文字列が収まらなかったら false を返すので
				while (not FontAsset(FontName::YuseiMagic)(U"{}"_fmt(m_target->getCatData().breed))
					.draw(fontSize--, breedRegion, Colors::Brown))
				{
					// そのとき、描画されてしまった文字列を上から塗りつぶして隠す
					breedRegion.draw(Colors::LightBrownAlt);
				}
			}
			
			// ## 毛色の表示領域

			const RectF &box2 = box1.movedBy(0, 40).draw(Colors::Brown);
			FontAsset(FontName::YuseiMagic)(U"毛色").drawAt(20, box2.center(), Colors::LightBrownAlt);
			

		}		

		BrightenCursor();
	}
}
