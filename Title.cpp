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
			Score::GeneralScoreData::ScoreData::SetLevelCount(getData().levels.size());

			// スコアデータはレベル数に合わせて確保してから、1プレイ分として追加しておく
			getData().scores << Score::GeneralScoreData{ Array<Score::GeneralScoreData::ScoreData>{ getData().levels.size() }, Score::GeneralScoreData::TitleData{} };	
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

			m_gui.logo = Texture{ U"texture/logo.png", TextureDesc::Mipped };
		}

		m_background = getData().backgrounds.choice();
	}

	void Title::update()
	{
		// # GUI 更新処理
		{
			if (m_gui.toLevel.isPressed() and (not m_gui.howToPlay.isOpen()))
			{
				changeScene(State::Wanted, 0.7s);
			}

			if (m_gui.howToPlayButton.isPressed())
			{
				// TODO: なぜ初めにダイアログを表示すると中心基準にならないのか調べる
				// というか、Debug では初回に表示すらされない場合がある
				m_gui.howToPlay.set(16, m_howToPlayText, { 700, 550 }).open();
			}

			m_gui.howToPlay.isPressedOK();
		}

		// TODO: タイトル画面でも猫がふわふわしてるといいよね
	}

	void Title::draw() const
	{
		// 背景描画（4:3 固定）
		m_background.fitted(Scene::Size()).draw();

		// GUI 要素描画
		m_gui.logo.resized(Scene::Width() * 0.6).draw(Arg::topCenter = Vec2{ Scene::Center().x, 50 });
		m_gui.toLevel.draw();
		m_gui.howToPlayButton.draw();
		m_gui.howToPlay.draw();

		BrightenCursor();
	}
}
