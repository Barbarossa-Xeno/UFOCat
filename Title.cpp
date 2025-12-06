# include "Title.hpp"

namespace UFOCat
{
	Title::Title(const InitData& init)
		: IScene{ init }
	{
		// 初回起動時
		if (getData().cats.isEmpty() or getData().levels.isEmpty() or getData().backgrounds.isEmpty())
		{
			// データがまだ読み込まれていなければ読み込む
			getData().cats = LoadCatData().map([](const auto &data) { return std::make_shared<CatData>(data); });
			getData().levels = LoadLevelData();
			getData().backgrounds = LoadBackgrounds();

			// 最大レベル数の情報をスコアデータに共通のものとして設定しておく
			Score::Generic::ByLevel::SetLevelCount(getData().levels.size());
		}
		// それ以降でタイトルに戻ってきた場合
		else
		{
			// 読み込んでいるレベルデータのクリアフラグをリセット
			getData().levels.each([](LevelData &level) { level.isCleared = false; });
		}

		// # GUI 要素設定
		{
			m_gui.toLevel.set(36, U"あそぶ", AudioAsset(Util::AudioSource::SE::OK), GUI::PositionType::Absolute, true, { 96, 10 })
						 .setPositionAt(Scene::Center() + Vec2{ 0, 60 });

			m_gui.howToPlayButton.set(36, U"あそび方", GUI::PositionType::Absolute, true, { 60, 10 })
								 .setPositionAt(Scene::Center() + Vec2{ 0, 160 });

			m_gui.lisenceButton.set(22, U"ライセンス", GUI::PositionType::Absolute, true)
							   .setPosition(Arg::bottomRight = Scene::Size() - Vec2{ 10, 10 });
			
			m_gui.howToPlay.setContents
			(
				GUI::TextBox
				{
					FontAsset(Util::FontFamily::YuseiMagic)
						(U"ときは21XX年、猫がUFOを乗りこなす、おてんばな時代。"
						 U"UFOに乗った猫、「UFO猫」は飼い主の家から脱走するのが、おおきな社会問題になっていた。\n"
						 U"そんな中、あなたは「UFO猫ハンター」として猫たちをつかまえることになったのだった…。"
						 U"というのはおいといて……、"),
					16, Util::Palette::Brown, GUI::PositionType::Relative
				}.setMargin({ 0, 5 }),
				GUI::TextBox
				{
					FontAsset(Util::FontFamily::YuseiMagic)
						(U"似ている猫たちの中から正しい「UFO猫」をつかまえるゲームです！"),
					22, Util::Palette::Brown, GUI::PositionType::Relative
				}.setMargin({ 5, 10 }),
				GUI::TextBox
				{
					FontAsset(Util::FontFamily::KoharuiroSunray)
						(U"■ あそび方"),
					44, Util::Palette::Brown, GUI::PositionType::Relative
				}.setMargin({ 10, 0 }),
				GUI::TextBox
				{
					FontAsset(Util::FontFamily::YuseiMagic)
						(U"レベル ごとにきめられたUFO猫を、まちがえないようにつかまえましょう。\n"
						 U"つかまえないといけない ターゲット は レベル のはじめに、画像といっしょにお知らせします。ちょっとの時間しか映りません。すぐ覚えないと……"),
					18, Util::Palette::Brown, GUI::PositionType::Relative
				}.setMargin({ 0, 5 }).setIndent(44),
				GUI::TextBox
				{
					FontAsset(Util::FontFamily::KoharuiroSunray)
						(U"■ レベル (★) について"),
					44, Util::Palette::Brown, GUI::PositionType::Relative
				}.setMargin({ 10, 0 }),
				GUI::TextBox
				{
					FontAsset(Util::FontFamily::YuseiMagic)
						(U"レベル が上がるとUFO猫がすばしっこくなったり、ほかのUFO猫がたくさんあらわれたりして、だんだんむずかしくなります。集中しないとまちがえるかも！"),
					18, Util::Palette::Brown, GUI::PositionType::Relative
				}.setMargin({ 0, 5 }).setIndent(44),
				GUI::TextBox
				{
					FontAsset(Util::FontFamily::KoharuiroSunray)
						(U"■ スコアについて"),
					44, Util::Palette::Brown, GUI::PositionType::Relative
				}.setMargin({ 10, 0 }),
				GUI::TextBox
				{
					FontAsset(Util::FontFamily::YuseiMagic)
						(U"ターゲットをつかまえたり、つかまえるのが速かったりしたら、その分だけ スコア が上がります。\n"
						 U"どんどん高いレベルへ進んでいって スコア がたまると、すごい しょうごう がもらえます。高みをめざせ！"),
					18, Util::Palette::Brown, GUI::PositionType::Relative
				}.setMargin({ 0, 15 }).setIndent(44)
			).setSize({ 500, 500 });

			m_gui.lisence.setContents(
				GUI::TextBox
				{
					FontAsset(Util::FontFamily::KoharuiroSunray)
						(U"ライセンス"),
					48, Util::Palette::Brown, GUI::PositionType::Relative
				}.setMargin({ 0, 10 }),
				GUI::TextBox
				{
					FontAsset(Util::FontFamily::YuseiMagic)
						(U"■ 画像アセット"),
					28, Util::Palette::Brown, GUI::PositionType::Relative
				}.setMargin({ 0, 10 }),
				GUI::TextBox
				{
					FontAsset(Util::FontFamily::YuseiMagic)
						(U"(1) 猫のイラスト素材"),
					22, Util::Palette::Brown, GUI::PositionType::Relative
				}.setMargin({ 5, 5 }),
				GUI::TextBox
				{
					FontAsset(Util::FontFamily::YuseiMagic)
						(U"猫画工房 - https://nekoillust.com/\n"
						 U"『まんじゅう猫全身A』全種 - https://nekoillust.com/manju-cat-whole-body-a/2/"),
					18, Util::Palette::Brown, GUI::PositionType::Relative
				}.setMargin({ 0, 0 }).setIndent(40),
				GUI::TextBox
				{
					FontAsset(Util::FontFamily::YuseiMagic)
						(U"『肉球足跡』- https://nekoillust.com/category/other/%e8%82%89%e7%90%83%e8%b6%b3%e8%b7%a1/"),
					18, Util::Palette::Brown, GUI::PositionType::Relative
				}.setMargin({ 0, 0 }).setIndent(40),
				GUI::TextBox
				{
					FontAsset(Util::FontFamily::YuseiMagic)
						(U"(2) UFOのイラスト素材"),
					22, Util::Palette::Brown, GUI::PositionType::Relative
				}.setMargin({ 5, 5 }),
				GUI::TextBox
				{
					FontAsset(Util::FontFamily::YuseiMagic)
						(U"illust image | イラストイメージ - https://illustimage.com/\n"
						 U"『未確認飛行物体UFOイラスト』- https://illustimage.com/?id=10469"),
					18, Util::Palette::Brown, GUI::PositionType::Relative
				}.setMargin({ 0, 0 }).setIndent(40),
				GUI::TextBox
				{
					FontAsset(Util::FontFamily::YuseiMagic)
						(U"使用にあたり画像データの加工をさせていただいております。"),
					18, Util::Palette::Brown, GUI::PositionType::Relative
				}.setMargin({ 2, 0 }).setIndent(40),
				GUI::TextBox
				{
					FontAsset(Util::FontFamily::YuseiMagic)
						(U"・背景イラスト画像については、ChatGPT による画像生成を利用しております。"),
					14, Util::Palette::Brown, GUI::PositionType::Relative
				}.setMargin({ 15, 0 }).setIndent(5),
				GUI::TextBox
				{
					FontAsset(Util::FontFamily::YuseiMagic)
						(U"■ フォントアセット"),
					28, Util::Palette::Brown, GUI::PositionType::Relative
				}.setMargin({ 10, 10 }),
				GUI::TextBox
				{
					FontAsset(Util::FontFamily::YuseiMagic)
						(U"(1) たぬき油性マジック / Yusei Magic"),
					22, Util::Palette::Brown, GUI::PositionType::Relative
				}.setMargin({ 5, 5 }),
				GUI::TextBox
				{
					FontAsset(Util::FontFamily::YuseiMagic)
						(U"たぬき油性マジック - たぬきフォント - https://tanukifont.com/tanuki-permanent-marker/"),
					18, Util::Palette::Brown, GUI::PositionType::Relative
				}.setMargin({ 0, 0 }).setIndent(40),
				GUI::TextBox
				{
					FontAsset(Util::FontFamily::KoharuiroSunray)
						(U"(2) こはるいろサンレイ"),
					28, Util::Palette::Brown, GUI::PositionType::Relative
				}.setMargin({ 5, 5 }).setIndent(5),
				GUI::TextBox
				{
					FontAsset(Util::FontFamily::YuseiMagic)
						(U"（配布サイト） - http://getsuren.com/"),
					18, Util::Palette::Brown, GUI::PositionType::Relative
				}.setMargin({ 0, 0 }).setIndent(40)
			).setSize({ 600, 500 });

			m_gui.logo = Texture{ U"texture/logo.png", TextureDesc::Mipped };
		}

		// # タイトル画面に現れる猫を決める
		{
			// スポーンリスト初期化
			getData().spawns.release();

			// 使うアクションを抽選して入れるための配列
			Array<Core::LevelData::ActionData> demoActions;

			// 読み込んだUFO猫のデータからアクションデータだけぬきとって１個ずつ代入
			getData().levels.each([&demoActions](const LevelData& level)
			{
					level.actionDataList.each([&](const auto& action)
					{
						demoActions << action;
					});
			});

			// スポーンさせる数を決める
			size_t count = Random(3, 5);

			// 全部入れたのをシャッフルしてから、スポーン数だけにする
			demoActions.shuffle().resize(count);

			// UFO猫のデータからランダムにスポーン数だけチョイスし、
			getData().spawns = std::move(getData().cats.choice(count)
														// 生成して unique_ptr にする
													   .map([](const auto &cat)
													   {
													       return std::make_unique<CatObject>(CatObject{ TextureAsset(Cat(cat->id)) });
													   })
														// 作ったポインタのリストに対して
														// （このリストと `demoActions` の長さはどちらも `count` なので）
														// インデックスを参照しながらアクションをセット
													   .each_index([&demoActions](size_t i, const auto &ptr)
													   {
													       ptr->setAction(demoActions[i]).setRandomVelocity(Random(1, 5));
													   }));
			// 最後に move で unique_ptr の権利を移譲する
		}

		// 背景を決める		
		m_bg = getData().backgrounds.choice();

		// もし他のBGMが流れていた場合も考えて、一度ストップ
		AudioAsset(getData().bgmName).stop();

		getData().bgmName = Util::AudioSource::BGM::Title;

		AudioAsset(getData().bgmName).play();
	}

	void Title::update()
	{
		// # 猫 更新処理
		{
			for (const auto &spawn : getData().spawns)
			{
				spawn->act();
			}
		}

		// # GUI 更新処理
		{
			if (not m_gui.howToPlay.isOpen() and not m_gui.lisence.isOpen())
			{
				if (m_gui.toLevel.isPressed())
				{
					// プレイされるときだけ
					// スコアデータはレベル数に合わせて確保してから、1プレイ分として追加しておく
					getData().scores << Score::Generic{ Array<Score::Generic::ByLevel>{ getData().levels.size() }, Score::Generic::Title{} };

					AudioAsset(Util::AudioSource::BGM::Title).fadeVolume(0.0, 0.2s);
					changeScene(State::Wanted, 1.2s);
				}

				if (m_gui.howToPlayButton.isPressed())
				{
					m_gui.howToPlay.open();
				}
			}

			if (m_gui.lisenceButton.isPressed())
			{
				m_gui.lisence.open();
			}

			m_gui.howToPlay.isPressedOK();
			m_gui.lisence.isPressedOK();
		}
	}

	void Title::draw() const
	{
		// 背景描画（4:3 固定）
		m_bg.texture.fitted(Scene::Size()).draw();

		// ロゴ描画
		m_gui.logo.resized(Scene::Width() * 0.6).draw(Arg::topCenter = Vec2{ Scene::Center().x, 50 });

		// 猫描画
		{
			for (const auto &spawn : getData().spawns)
			{
				spawn->drawShadow(m_bg.shadowColor).draw();
			}
		}

		// GUI 要素描画
		m_gui.toLevel.draw();
		m_gui.howToPlayButton.draw();
		m_gui.lisenceButton.draw();
		m_gui.howToPlay.draw();
		m_gui.lisence.draw();

		BrightenCursor();
	}

	Title::~Title()
	{
		AudioAsset(Util::AudioSource::BGM::Title).stop();
	}
}
