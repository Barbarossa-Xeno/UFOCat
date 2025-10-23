#pragma once
#include "phase.hpp"
#include "cat_object.hpp"

class GameManager
{
	/* -- クラス -- */
public:
	enum class State
	{
		Title,
		TargetAnnounce,
		PhaseStart,
		InPhase,
		PhaseEnd,
		Result,
		GameOver
	};

	/* -- フィールド -- */
public:

	Array<CatObject> cats;

	Array<std::shared_ptr<CatObject>> catsInPhase;

	Array<std::unique_ptr<CatObject>> spawns;

	Array<Phase> phases;

	State currentState = State::Title;

private:

	const CatObject* m_target_ptr = nullptr;

	size_t m_currentPhaseIndex = 0;

	// Array<CatObject&> m_selectCats;

	/* -- ゲッター -- */
public:

	/// @brief 現在のターゲットを取得する
	/// @return 現在のターゲットの `CatData` の参照
	const CatData &getTarget() const;

	/// @brief 
	/// @return 
	const Phase &getCurrentPhase() const;

	/* -- セッター -- */
public:

	/// @brief 全種類のUFO猫の中からランダムに1体ターゲットを設定する
	/// @return 設定後のターゲットの `CatData` の参照
	const CatData &setTarget();

	/* -- メソッド -- */
public:

	/// @brief コンストラクタ
	explicit GameManager()
		: cats{ LoadCatData() }
		, phases{ LoadPhaseData() }
	{ }

	void announceTarget();

	void startPhase();

	void inPhase();

	void spawn();

	void draw();

	/// @brief ゲームマネージャーのインスタンス（シングルトン）
	/// @return 自分への参照（ただ1つ）
	static GameManager& Instance();

private:

	/// @brief UFO猫のデータをJSONから読み込んでそれら全てのインスタンスを作成する
	/// @return 全てのUFO猫のインスタンス配列
	Array<CatObject> LoadCatData();

	/// @brief 各フェーズのデータをJSONから読み込んでそれらすべてのインスタンスを作成する
	/// @return 全てのフェーズの配列
	Array<Phase> LoadPhaseData();
};
