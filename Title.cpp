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
			getData().cats = LoadCatData();
			getData().levels = LoadLevelData();
			getData().backgrounds = LoadBackgrounds();

			// 最大レベル数の情報をスコアデータに共通のものとして設定しておく
			Score::Generic::ByLevel::SetLevelCount(getData().levels.size());

			// スコアデータはレベル数に合わせて確保してから、1プレイ分として追加しておく
			getData().scores << Score::Generic{ Array<Score::Generic::ByLevel>{ getData().levels.size() }, Score::Generic::Title{} };	
		}
		// それ以降でタイトルに戻ってきた場合
		else
		{
			// 読み込んでいるレベルデータのクリアフラグをリセット
			getData().levels.each([](LevelData &level) { level.isCleared = false; });
		}

		// # GUI 要素設定
		{
			m_gui.toLevel.set(36, U"あそぶ", true, { 96, 10 })
						 .setPositionAt(Scene::Center() + Vec2{ 0, 60 });

			m_gui.howToPlayButton.set(36, U"あそび方", true, { 60, 10 })
								 .setPositionAt(Scene::Center() + Vec2{ 0, 160 });
			
			m_gui.howToPlay.setContents
			(
				GUI::TextBox
				{
					FontAsset(Util::FontFamily::YuseiMagic)
						(U"ときは21XX年、猫がUFOを乗りこなす時代。"
						 U"UFOに乗った猫、「UFO猫」は飼い主の家から脱走するのが、おおきな社会問題になっていた…。\n"
						 U"というのはおいといて……、"),
					16, Util::Palette::Brown
				}.setPosition({ 0, 5 }),
				GUI::TextBox
				{
					FontAsset(Util::FontFamily::YuseiMagic)
						(U"にている中からただしい「UFO猫」を捕まえるゲームです！"),
					22, Util::Palette::Brown
				}.setPosition({ 0, 120 }),
				GUI::TextBox
				{
					FontAsset(Util::FontFamily::KoharuiroSunray)
						(U"■ あそび方"),
					44, Util::Palette::Brown
				}.setPosition({ 0, 200 }),
				GUI::TextBox
				{
					FontAsset(Util::FontFamily::YuseiMagic)
						(U"レベル ごとにきめられたUFO猫を、間違えないようにつかまえましょう。ターゲット は レベル のはじめにお知らせします。"),
					18, Util::Palette::Brown
				}.setPosition({ 40, 240 }),
				GUI::TextBox
				{
					FontAsset(Util::FontFamily::KoharuiroSunray)
						(U"■ レベル (★) について"),
					44, Util::Palette::Brown
				}.setPosition({ 0, 340 }),
				GUI::TextBox
				{
					FontAsset(Util::FontFamily::YuseiMagic)
						(U"レベル が上がるとUFO猫がすばしっこくなったり、ほかのUFO猫がたくさんあらわれたりして、だんだんむずかしくなります。"),
					18, Util::Palette::Brown
				}.setPosition({ 40, 380 }),
				GUI::TextBox
				{
					FontAsset(Util::FontFamily::KoharuiroSunray)
						(U"■ スコアについて"),
					44, Util::Palette::Brown
				}.setPosition({ 0, 480 }),
				GUI::TextBox
				{
					FontAsset(Util::FontFamily::YuseiMagic)
						(U"ターゲットをつかまえれたり、つかまえるのが速かったりしたら、スコア が上がります。"
						 U"レベルが進んで スコア がたまると、すごい しょうごう がもらえます。高みをめざそう！"),
					18, Util::Palette::Brown
				}.setPosition({ 40, 520 })
			).setSize({ 500, 500 });

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
														// クローンして unique_ptr にする
													   .map([](const CatObject &cat)
													   {
													       return std::make_unique<CatObject>(cat.clone());
													   })
														// 作ったポインタのリストに対して
														// （このリストと `demoActions` の長さはどちらも `count` なので）
														// インデックスを参照しながらアクションをセット
													   .each_index([&demoActions](size_t i, const auto &ptr)
													   {
													       ptr->setAction(demoActions[i]).setRandomVelocity(Random(1, 5));
													   }));
			// CatObject が自前でコピーコンストラクタを定義している都合上
			// ムーブ代入も禁止なので（そもそも const メンバーを決め打ちにしている以上ムーブが難しい）
			// 新しい unique_ptr<CatObject> の ** リストをムーブして ** 代入する
		}

		// 背景を決める		
		m_bg = getData().backgrounds.choice();
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
			if (m_gui.toLevel.isPressed() and (not m_gui.howToPlay.isOpen()))
			{
				changeScene(State::Wanted, 0.7s);
			}

			if (m_gui.howToPlayButton.isPressed())
			{
				m_gui.howToPlay.open();
			}

			m_gui.howToPlay.isPressedOK();
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
		m_gui.howToPlay.draw();

		BrightenCursor();
	}
}
