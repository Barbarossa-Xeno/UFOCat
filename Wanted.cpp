# include "Wanted.hpp"

namespace UFOCat
{
	RectF Wanted::m_showColorData(String name, Color color, Vec2 leftCenter, double size) const
	{
		// 色のアイコン (〇 <- こういう感じ)のをつくる
		const auto &icon = Circle{ leftCenter + Vec2{ size, 0 }, size }.draw(color).drawFrame(1.5, Util::Palette::LightBrown);

		// アイコンと色名表示の間の間隔
		const double margin = size / 2;

		// そのアイコンの右下隣りくらいに、少し小さく色名を表示する
		const RectF &nameRegion = FontAsset(Util::FontFamily::YuseiMagic)(U"{}"_fmt(name)).draw(1.2 * size, Arg::bottomLeft = icon.boundingRect().br() + Vec2{ margin, 0 }, Util::Palette::Brown);

		// アイコンとマージン、色名の表示領域全てを足した範囲を返す
		return icon.boundingRect().stretched(Arg::right = margin + nameRegion.w);
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
		getData().timer.set(5s);

		// # GUI 初期化
		{
			m_gui.levelBar.set(SizeF{ 0.85 * 180, 10 }, Util::Palette::Brown)
						  .setPosition(Arg::bottomCenter = Vec2{ 180.0 / 2 + 5, 100 - 5 - 10 })
						  .setProgress((getData().levelIndex + 1) / 10.0);

			m_gui.flyer = Texture{ U"texture/flyer.png", TextureDesc::Mipped };
		}

		AudioAsset(getData().bgmName).stop();
	}

	void Wanted::update()
	{
		if (not getData().timer.isStarted())
		{
			getData().timer.start();
		}
		else if (getData().timer.reachedZero())
		{
			changeScene(State::Level, 2s);
		}
		else if (2 < getData().timer.sF() and getData().timer.sF() < 4.75)
		{
			AudioAsset(Util::AudioSource::SE::Announce).play();
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
			FontAsset(Util::FontFamily::YuseiMagic)(U"★ {}"_fmt(getData().levelIndex + 1)).draw(26, back.rect.tl() + Point{ 10, 5 }, ColorF{ 0.4, 0.3, 0.2 });
			m_gui.levelBar.draw();
		}

		FontAsset(Util::FontFamily::YuseiMagic)(U"見つけるUFOネコは...").drawAt(36, Scene::Center().x, 40);

		// # チラシ部分
		{
			// チラシの描画とその範囲
			const RectF &flyerRegion = m_gui.flyer.resized(Scene::Height() - 150).drawAt(Scene::Center());

			// ターゲットのテクスチャ描画位置（中央基準）
			Vec2 targetOrigin{ Scene::Center().x, Scene::Center().y - 20 };

			// ターゲット猫の表示
			{
				// シャドウ
				m_target->getTexture().scaled(0.45).drawAt(targetOrigin + Point{ 5, 5 }, ColorF{ 0.4, 0.3, 0.2 });

				// 実際の
				m_target->getTexture().scaled(0.45).drawAt(targetOrigin);
			}			

			// ## ターゲット猫の各種情報を表示する部分
			{
				// 各データのタイトルを表示するためのテキストボックス範囲
				// チラシ全体の 30% x 40% の大きさにして、この大きさを基準とする
				const RectF textBox{ 0.3 * flyerRegion.x, 0.4 * flyerRegion.y };

				RectF breedBox{}, colorBox{}, patternBox{};

				// ### 猫種の表示領域
				{
					// 「猫種」というタイトルとそのバックを塗りつぶすための RectF を計算
					// チラシの端から 20 だけ横に
					breedBox = textBox.movedBy(flyerRegion.x + 20, targetOrigin.y + 110).draw(Util::Palette::Brown);

					// その上からテキスト
					FontAsset(Util::FontFamily::YuseiMagic)(U"猫種").drawAt(20, breedBox.center(), Util::Palette::LightBrownAlt);

					// 猫種名を表示するエリア、マージン (20) 分調整する
					const RectF& breedRegion = breedBox.movedBy(breedBox.w + 20, 0).setSize(flyerRegion.w - breedBox.w - 20 - 20 - 20, breedBox.h);

					// 名前は短いのから長いのもあるので、
					// エリアから溢れない範囲でフォントサイズを可変にする
					double fontSize = 40;
					// このメソッドは矩形内にすべての文字列が収まらなかったら false を返すので
					while (not FontAsset(Util::FontFamily::YuseiMagic)(U"{}"_fmt(m_target->getCatData().breed))
						.draw(fontSize--, breedRegion, Util::Palette::Brown))
					{
						// そのとき、描画されてしまった文字列を上から塗りつぶして隠す
						breedRegion.draw(Util::Palette::LightBrownAlt);
					}
				}

				// ### 毛色の表示領域
				{
					// さっきのを下に動かしたもの
					colorBox = breedBox.movedBy(0, 40).draw(Util::Palette::Brown);
					FontAsset(Util::FontFamily::YuseiMagic)(U"毛色").drawAt(20, colorBox.center(), Util::Palette::LightBrownAlt);

					// 次々と色情報を表示する際に、基準にする前の表示範囲を保持する
					RectF previousRegion{ colorBox };

					for (const auto& [name, color] : m_target->getCatData().colors)
					{
						// 表示しながら、前の範囲を定める
						previousRegion = m_showColorData(name, color, previousRegion.rightCenter() + Point{ 20, 0 }, 10);
					}
				}

				// ### 模様の表示領域
				{
					patternBox = colorBox.movedBy(0, 40).draw(Util::Palette::Brown);
					FontAsset(Util::FontFamily::YuseiMagic)(U"模様").drawAt(20, patternBox.center(), Util::Palette::LightBrownAlt);

					// 模様名を表示するエリア、マージン (20) 分調整する
					const RectF& patternRegion = patternBox.movedBy(patternBox.w + 20, 0).setSize(flyerRegion.w - patternBox.w - 20 - 20 - 20, patternBox.h);

					FontAsset(Util::FontFamily::YuseiMagic)(U"{}"_fmt(m_target->getCatData().pattern)).draw(20, Arg::leftCenter = patternRegion.leftCenter(), Util::Palette::Brown);
				}
			}
		}		

		BrightenCursor();
	}
}
