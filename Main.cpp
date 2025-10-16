#include "game_manager.hpp"
#include <Siv3D.hpp> // Siv3D v0.6.16

enum class IngameState
{
	PhaseStart,
	AnnounceTarget,
	Finding,
	PhaseEnd
};

void Main()
{
	// テスト用フォント
	const Font font{ 36 };

	// インゲームの状態（上手くループと合わせる必要）
	IngameState ingameState = IngameState::PhaseStart;

	// インゲームタイマー
	Timer ingameTimer;

	Array<CatObject> cats = GameManager::Instance().cats;
	cats.each([](auto &cat) { cat.position = { -100, -100 }; });

	cats[0].vx = 300;
	cats[0].vy = 200;
	cats[7].velocity = { 100, 100 };
	cats[8].velocity = { 250, 400 };

	// TODO: 乱数を使って見つけるネコを設定する
	// 将来的にはこの部分もゲームループに入れることになるのは承知のうえで
	GameManager::Instance().setTarget();
	// CatObject::targetData = cats[targetIndex].getCatData();
	// TODO: ネコどうしの類似度を設定して、難易度によって出現させるべきネコを変える

	// 各ステートを何秒ごとに切り替えていくかは、何かの形でまとめておきたい
	// ここでは仮にターゲットをアナウンスする画面が消えるのに 3s かけるとして
	{
		ingameState = IngameState::AnnounceTarget;
		ingameTimer.restart(3s);
	}

	{
		auto res = Phase::LoadData();
		Console << Unicode::FromUTF8(std::string(res[0].actionDataList[1].methodParameters.type().name()));
	}
	
	while (System::Update())
	{
		switch (ingameState)
		{
			case IngameState::AnnounceTarget:
			{
				font(U"見つけるUFOネコは").drawAt(Scene::Center().x, 50);
				font(U"{}"_fmt(GameManager::Instance().getTarget().breed)).drawAt(Scene::Center().x, 100);

				if (ingameTimer.reachedZero())
				{
					ingameState = IngameState::Finding;
					ingameTimer.restart(30s);
				}

				break;
			}

			case IngameState::Finding:
			{
				{
					cats[0].bound().draw().checkCatchable(GameManager::Instance().getTarget());
					cats[0].drawHitArea();

					cats[1].appear(3s, 1s).draw();
					cats[2].appear(2s, 0.6s, 0.4s).draw();
					cats[3].appear(1.5s, Easing::Quint, 0.4s, Easing::Sine, 0.8s).draw();

					cats[4].appearFromEdge(2s, 0.4s, { 60, 40, 80, 60 }).draw();
					cats[5].appearFromEdge(3s, Easing::Bounce, 1s, { 100, 0, 0, 50 }).draw();
					cats[6].appearFromEdge(3.3s, Easing::Expo, 1.5s, Easing::Sine, 0.8s, { 0, 0, 0, 30 }).draw();

					cats[7].cross(2s, 3).draw();
					cats[8].cross(4s).draw();
				}

				break;
			}

			default: break;
		}
		
		switch (GameManager::Instance().currentState)
		{
			case GameManager::State::TargetAnnounce:
			{
				GameManager::Instance().announceTarget();
				break;
			}
			case GameManager::State::PhaseStart:
			{
				GameManager::Instance().startPhase();
				break;
			}
			case GameManager::State::InPhase:
			{
				// Phase の更新間隔に合わせてスポーンを行うような処理
				// GameManager::Instance().
			}
		}

		// TODO: 描画処理は共通して行う？
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
