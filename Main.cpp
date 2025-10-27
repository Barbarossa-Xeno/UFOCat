#include "game_manager.hpp"
#include <Siv3D.hpp> // Siv3D v0.6.16
# include "Common.hpp"
# include "Title.hpp"
# include "Wanted.hpp"
# include "Level.hpp"
# include "Result.hpp"

using namespace UFOCat;

void Main()
{
	// テスト用フォント
	FontAsset::Register(U"Test", 36);

	App app;

	app.add<Title>(State::Title);
	app.add<Wanted>(State::Wanted);
	app.add<Level>(State::Level);
	app.add<Result>(State::Result);

	// 動作確認用のコピー
	/*GameManager& game = GameManager::Instance();
	Array<CatObject> cats = game.cats;
	cats.each([](auto &cat) { cat.position = { -100, -100 }; });
	cats[0].vx = 700;
	cats[0].vy = 800;
	cats[7].velocity = { 100, 100 };
	cats[8].velocity = { 250, 400 };
	cats[0].setAction(GameManager::Instance().levels[0].actionDataList[1]);*/

	// 各ステートを何秒ごとに切り替えていくかは、何かの形でまとめておきたい
	// ここでは仮にターゲットをアナウンスする画面が消えるのに 3s かけるとして
	{
		//game.currentState = GameManager::State::TargetAnnounce;
	}
	
	while (System::Update())
	{
		//cats[0].act().draw();
		//cats[0].bound().draw().checkCatchable(GameManager::Instance().getTargetData());
		/*cats[0].drawHitArea();

		cats[1].appear(3s, 1s).draw();
		cats[2].appear(2s, 0.6s, 0.4s).draw();
		cats[3].appear(1.5s, Easing::Quint, 0.4s, Easing::Sine, 0.8s).draw();

		cats[4].appearFromEdge(2s, 0.4s, { 60, 40, 80, 60 }).draw();
		cats[5].appearFromEdge(3s, Easing::Bounce, 1s, { 100, 0, 0, 50 }).draw();
		cats[6].appearFromEdge(3.3s, Easing::Expo, 1.5s, Easing::Sine, 0.8s, { 0, 0, 0, 30 }).draw();*/

		/*cats[7].cross(2s, 3).draw();
		cats[8].cross(4s).draw();*/
		
		//switch (game.currentState)
		//{
		//	case GameManager::State::Title:
		//	{
		//		break;
		//	}
		//	case GameManager::State::TargetAnnounce:
		//	{
		//		game.announceTarget();
		//		break;
		//	}
		//	case GameManager::State::PhaseStart:
		//	{
		//		game.startPhase();
		//		break;
		//	}
		//	case GameManager::State::InPhase:
		//	{
		//		// LevelData の更新間隔に合わせてスポーンを行うような処理
		//		game.inPhase();
		//	}
		//}
		//game.draw();

		if (not app.update())
		{
			break;
		}
	}
}

//
// - Debug ビルド: プログラムの最適化を減らす代わりに、エラーやクラッシュ時に詳細な情報を得られます。
//
// - Release ビルド: 最大限の最適化でビルドします。
//
// - [デバッグ] メニュー → [デバッグの開始] でプログラムを実行すると、[出力] ウィンドウに詳細なログが表示され、エラーの原因を探せます。
//
// - Visual Studio を更新した直後は、プログラムのリビルド（[ビルド]メニュー → [ソリューションのリビルド]）が必要な場合があります。
//
