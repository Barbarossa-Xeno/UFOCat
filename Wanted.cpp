# include "Wanted.hpp"

namespace UFOCat
{
	Wanted::Wanted(const InitData& init)
		: IScene{ init }
	{
		getData().targetIndex = Random(getData().cats.size() - 1);
		m_target = std::make_unique<CatObject>(getData().cats[getData().targetIndex]);

		// 現在行っているレベルのインデックスは、クリアしているレベルの数と同じなのを利用する
		// （そのレベルが終わり次第、isCleared のフラグを上げるため）
		getData().levelIndex = getData().levels.filter([](const LevelData &p) { return p.isCleared; }).size();

		// TODO: レベルが進むごとにちょっと時間を短くしたら面白いかも
		getData().timer.set(5s);
	}

	void Wanted::update()
	{
		if (not getData().timer.isStarted())
		{
			getData().timer.start();
		}
		else if (getData().timer.reachedZero())
		{
			changeScene(State::Level);
		}
		
# if _DEBUG    // デバッグ機能：Ctrl + Shift + S でスキップ
		else if (KeyControl.pressed() and KeyShift.pressed() and KeyS.pressed())
		{
			getData().timer.reset();
			changeScene(State::Level);
		}
	}
# endif

	void Wanted::draw() const
	{
		FontAsset(U"Test")(U"見つけるUFOネコは").drawAt(Scene::Center().x, 50);
		FontAsset(U"Test")(U"{}"_fmt(m_target->getCatData().breed)).drawAt(Scene::Center().x, 100);
		m_target->getTexture().scaled(0.5).drawAt(Scene::Center());
		FontAsset(U"Test")(U"あと {}"_fmt(getData().timer.s())).drawAt(Scene::Center().x, Scene::Center().y + 100);
	}
}
