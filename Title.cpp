# include "Title.hpp"

namespace UFOCat
{
	Title::Title(const InitData& init)
		: IScene{ init }
	{
		if (getData().cats.isEmpty() or getData().levels.isEmpty())
		{
			// データがまだ読み込まれていなければ読み込む
			getData().cats = LoadCatData();
			getData().levels = LoadLevelData();

			// スコアデータはレベル数に合わせて確保しておく
			getData().scores.resize(getData().levels.size());
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

	}
}
