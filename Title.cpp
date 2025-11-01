# include "Title.hpp"

namespace UFOCat
{
	Title::Title(const InitData& init)
		: IScene{ init }
	{
		// 初回起動時
		if (getData().cats.isEmpty() or getData().levels.isEmpty())
		{
			// データがまだ読み込まれていなければ読み込む
			getData().cats = LoadCatData();
			getData().levels = LoadLevelData();

			// 最大レベル数の情報をスコアデータに共通のものとして設定しておく
			ScoreData::SetLevelCount(getData().levels.size());

			// スコアデータはレベル数に合わせて確保してから、1プレイ分として追加しておく
			getData().scores << Score{ Array<ScoreData>{ getData().levels.size() }, ScoreTitleData{} };
			//getData().scores.resize(getData().levels.size());			
		}
		// それ以降でタイトルに戻ってきた場合
		else
		{
			// 読み込んでいるレベルデータのクリアフラグをリセット
			getData().levels.each([](LevelData &level) { level.isCleared = false; });
		}

		// # GUI 位置設定
		{
			m_gui.toLevel.set(36, U"あそぶ", true, { 96, 10 })
						 .setPositionAt(Scene::Center() + Vec2{ 0, 60 });

			m_gui.howToPlayButton.set(36, U"あそび方", true, { 60, 10 })
						   .setPositionAt(Scene::Center() + Vec2{ 0, 160 });
		}
	}

	void Title::update()
	{
		// # GUI 更新処理
		{
			if (m_gui.toLevel.isPressed() and (not m_gui.howToPlay.isOpen()))
			{
				changeScene(State::Wanted, 1s);
			}

			if (m_gui.howToPlayButton.isPressed())
			{
				m_gui.howToPlay.set(16, m_howToPlayText, { 700, 550 }).open();
			}

			m_gui.howToPlay.isPressedOK();
		}
	}

	void Title::draw() const
	{
		FontAsset(FontName::YuseiMagic)(U"UFO猫を\n　つかまえろ!!").draw(72, Arg::topCenter = Vec2{ Scene::Center().x, 50 });

		m_gui.toLevel.draw();
		m_gui.howToPlayButton.draw();
		m_gui.howToPlay.draw();
	}
}
