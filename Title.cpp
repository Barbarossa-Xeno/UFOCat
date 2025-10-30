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
	}

	void Title::update()
	{
		if (MouseL.down())
		{
			// とりあえずタイトルまだ作ってないのですぐに wanted シーンへ移動
			changeScene(State::Wanted);
		}


	}

	void Title::draw() const
	{
		FontAsset(U"Test")(U"タイトル").drawAt(Scene::Center());
	}
}
