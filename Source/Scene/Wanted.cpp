# include "Wanted.hpp"

namespace UFOCat
{
	RectF Wanted::m_drawColorData(String name, Color color, Vec2 leftCenter, double size) const
	{
		// 色のアイコン (〇 <- こういう感じ) のをつくって描画
		const auto &icon = Circle{ leftCenter + Vec2{ size, 0 }, size }.draw(color).drawFrame(1.5, Util::Palette::LightBrown);

		// アイコンと色名表示の間の間隔
		const double margin = size / 2;

		// そのアイコンの右下隣りくらいに、少し小さく色名を描画する
		const RectF &nameRegion = FontAsset(Util::FontName::YuseiMagic)(U"{}"_fmt(name)).draw(1.2 * size, Arg::bottomLeft = icon.boundingRect().br() + Vec2{ margin, 0 }, Util::Palette::Brown);

		// アイコンとマージン、色名の表示領域全てを足した範囲を返す
		return icon.boundingRect().stretched(Arg::right = margin + nameRegion.w);
	}

	Wanted::Wanted(const InitData& init)
		: IScene{ init }
	{
		// ターゲットを選ぶ
		getData().targetId = getData().cats.choice()->id;
		m_target = getData().cats[getData().targetId];

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
			changeScene(SceneState::Level, 2.2s);
		}
		
# if _DEBUG
		// # デバッグ機能
		if (KeyControl.pressed() and KeyShift.pressed())
		{
			// Ctrl + Shift + S でスキップ
			if (KeyS.pressed())
			{
				getData().timer.reset();
				changeScene(SceneState::Level, 0.7s);
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

	void Wanted::updateFadeIn(double t)
	{
		// フェードインの 25 ~ 30% のところでキンコンカンコン鳴らす
		if (0.25 < t and t < 0.3)
		{
			// playOneShot では複数回呼び出されたとき
			// その分だけ再生されてしまうので、普通の play で鳴らす
			AudioAsset(Util::AudioName::SE::Announce).play();
		}
	}

	void Wanted::draw() const
	{
		// # 背景
		{
			// 放射状のグラデーションで疑似的にビネット効果を作る
			// 外縁部が円状にほんのり暗い緑の背景ができる
			Circle{ Scene::Center(), Scene::Size().length() * 0.5 }.draw(Util::Palette::DarkGreen, Util::Palette::DarkGreenAlt);
		}

		// # 画面下端の方にUFO猫がすーっと進むプログレスバー
		{
			// アナウンス時間のタイマーの進捗
			double t = getData().timer.progress0_1();

			// t を使って画面の右端から左端まで移動する
			double x = Scene::Width() * EaseInCubic(t);

			// プログレスバー的な
			const auto &bar = RectF{ Arg::bottomLeft(Vec2{ 0, Scene::Height() }), x, 10 }.draw();

			// 適当なUFO猫のテクスチャを再利用
			const auto &iconRegion = TextureAsset(Cat(0)).scaled(0.075);

			// 0 ~ 1 で変化する t において、指定した閾値に達したら 1 ということにする関数
			const auto t_0to = [&t](double threshold) { return Min((t), threshold) / threshold; };

			// アイコンの幅分を引いて、バーの先端の位置にアイコンが表示されるようにする
			// なお、t が 0.4（残り時間が40%）に達するまではバーの先端よりも後ろに位置するようにする
			// この式の場合、t = 0 のときは完全に画面外に出ていて、ちょっと焦る感じの動きでバーの動きに合流する
			double fixedX = x - (iconRegion.region().w / 2) * (1 + EaseInOutElastic(1 - t_0to(0.4)));
			
			{
				// 黒を乗算して描画したテクスチャに対して
				// 加算を行うことで白描画にするためのレンダーステート
				// https://siv3d.github.io/ja-jp/tutorial3/2d-render-state/#485-%E3%83%86%E3%82%AF%E3%82%B9%E3%83%81%E3%83%A3%E3%81%AE%E5%8D%98%E8%89%B2%E6%8F%8F%E7%94%BB
				const ScopedColorAdd2D colorAdd{ 1.0 };

				// 画面右端、左端ともにすこしマージンを作ってアイコンも移動させる
				// Clamp が上手く動かなかったので Max と Min の合わせ技
				iconRegion.draw(Arg::bottomLeft = Vec2{ Min(fixedX, Scene::Width() - iconRegion.region().w - 5.0), bar.topY() - 5 }, ColorF{ 0.0 });
			}
		}

		// # 左上のレベル表示
		{
			RoundRect back{ 5, 5, 180, 100, 6 };
			back.drawShadow(Point{ 2, 2 }, 4).draw(ColorF{ 0.95, 0.9, 0.8 });
			FontAsset(Util::FontName::YuseiMagic)(U"★ {}"_fmt(getData().levelIndex + 1)).draw(26, back.rect.tl() + Point{ 10, 5 }, ColorF{ 0.4, 0.3, 0.2 });
			m_gui.levelBar.draw();
		}

		// # チラシ部分
		{
			// チラシの描画とその範囲
			const RectF &flyerRegion = m_gui.flyer.resized(Scene::Height() - 150).drawAt(Scene::Center());

			// ターゲットのテクスチャ描画位置（中央基準）
			Vec2 targetOrigin{ Scene::Center().x, Scene::Center().y - 20 };

			// ターゲット猫の表示
			{
				const auto &region = TextureAsset(Cat(getData().targetId)).scaled(0.45);

				// ドロップシャドウ
				m_dropShadow.draw(region, ColorF{ 0.4, 0.3, 0.2 }, targetOrigin, { 2, 2 }, 1.05);

				// 実際の猫
				region.drawAt(targetOrigin);
			}			

			// ## ターゲット猫の各種情報を表示する部分
			{
				// 各データのタイトルを表示するためのテキストボックス範囲
				// チラシ全体の 30% x 40% の大きさにして、この大きさを基準とする
				const RectF titleBound{ 0.3 * flyerRegion.x, 0.4 * flyerRegion.y };

				RectF breedBound{}, colorBound{}, patternBound{};

				// ### 「猫種」の表示領域
				{
					// 「猫種」というタイトルとそのバックを塗りつぶすための RectF を計算
					// チラシの端から 20 だけ横に
					breedBound = titleBound.movedBy(flyerRegion.x + 20, targetOrigin.y + 110).draw(Util::Palette::Brown);

					// その上からタイトルのテキストを描画
					FontAsset(Util::FontName::YuseiMagic)(U"猫種").drawAt(20, breedBound.center(), Util::Palette::LightBrownAlt);

					// 猫種名を表示するエリア、マージン (20) 分調整する
					const RectF& breedRegion = breedBound.movedBy(breedBound.w + 20, 0).setSize(flyerRegion.w - breedBound.w - 20 - 20 - 20, breedBound.h);

					// 名前は短いのから長いのもあるので、
					// エリアから溢れない範囲でフォントサイズを可変にする
					double fontSize = 40;

					// このメソッドは矩形 (breedRegion) 内にすべての文字列が収まらなかったら false を返すので
					// true になるまでフォントサイズを小さくしていく (not で反転してるので収まっていない時ループ)
					while (not FontAsset(Util::FontName::YuseiMagic)(U"{}"_fmt(m_target->breed))
								.draw(fontSize--, breedRegion, Util::Palette::Brown))
					{
						// 収まりきっていないときもテキストは描画されてしまうので
						// その都度上から塗りつぶして隠す
						breedRegion.draw(Util::Palette::LightBrownAlt);
					}
				}

				// ### 「毛色」の表示領域
				{
					// さっきの「猫種」表示領域を 40px 下に動かしたものとする
					colorBound = breedBound.movedBy(0, 40).draw(Util::Palette::Brown);

					// その上からタイトルのテキストを描画
					FontAsset(Util::FontName::YuseiMagic)(U"毛色").drawAt(20, colorBound.center(), Util::Palette::LightBrownAlt);

					// 次々と色情報を表示する際に、色情報を追加する前の表示範囲を保持するようにして
					// 描画位置の基準にする
					RectF previousRegion{ colorBound };

					for (const auto &[name, color] : m_target->colors)
					{
						// 1つ前の色情報を描画したときの右中央座標を基準にしながら更に描画
						// 描画したら以前の範囲を更新する
						previousRegion = m_drawColorData(name, color, previousRegion.rightCenter() + Point{ 20, 0 }, 10);
					}
				}

				// ### 「模様」の表示領域
				{
					// さっきの「毛色」表示領域を 40px 下に動かしたものとする
					patternBound = colorBound.movedBy(0, 40).draw(Util::Palette::Brown);

					// その上からタイトルのテキストを描画
					FontAsset(Util::FontName::YuseiMagic)(U"模様").drawAt(20, patternBound.center(), Util::Palette::LightBrownAlt);

					// 模様名を表示するエリア、マージン (20) 分調整する
					const RectF& patternRegion = patternBound.movedBy(patternBound.w + 20, 0).setSize(flyerRegion.w - patternBound.w - 20 - 20 - 20, patternBound.h);

					// 模様名を表示
					FontAsset(Util::FontName::YuseiMagic)(U"{}"_fmt(m_target->pattern)).draw(20, Arg::leftCenter = patternRegion.leftCenter(), Util::Palette::Brown);
				}
			}
		}		

		BrightenCursor();
	}

	// シーンに入った時の演出 (Siv3D)
	void Wanted::drawFadeIn(double t) const
	{
		draw();

		// アイリスイン
		Circle{ Vec2{ Scene::Center().x, Scene::Center().y - 20 }, 600 }.drawFrame(EaseInSine(1 - t) * 600, 0, Palette::Black);

		// 画面全体のフェードイン
		Scene::Rect().draw(ColorF{ 0.0, EaseInSine(1 - t) });
	}

	// シーンを抜ける時の演出 (Siv3D)
	void Wanted::drawFadeOut(double t) const
	{
		draw();

		// 頸動脈からアイラブユーが噴き出てアイリスアウト
		Circle{ Vec2{ Scene::Center().x, Scene::Center().y - 20 }, 600 }.drawFrame(EaseInOutQuad(t) * 600, 0, Palette::Black);
	}

	// 遷移演出のカスタマイズ
	// https://siv3d.github.io/ja-jp/tutorial3/scene-manager/#585-%E9%81%B7%E7%A7%BB%E6%BC%94%E5%87%BA%E3%81%AE%E3%82%AB%E3%82%B9%E3%82%BF%E3%83%9E%E3%82%A4%E3%82%BA2
}
