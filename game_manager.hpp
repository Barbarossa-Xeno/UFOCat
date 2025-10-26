#pragma once
#include "CatObject.hpp"
#include "LevelData.hpp"

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

	Array<LevelData> levels;

	State currentState = State::Title;

private:
	/// @brief ターゲット
	std::shared_ptr<CatObject> m_target = nullptr;

	/// @brief 現在のフェーズ（のインデックス）
	size_t m_currentPhaseIndex = 0;

	/// @brief フェーズの時間を計測するタイマー
	Timer m_phaseTimer;

	/// @brief フェーズ中にターゲットが出現する時刻
	Duration m_targetAppearTime;

	/// @brief ターゲットが出現したかどうか
	bool m_appearedTarget = false;

	/// @brief 現在のフェーズで起こせるアクションの確率分布
	DiscreteDistribution m_actionProbabilities;

	/* -- ゲッター -- */
public:
	/// @brief 現在のターゲットを取得する
	/// @return 現在のターゲットの `CatData` の参照
	const CatData &getTargetData() const;

	/// @brief 現在のフェーズを取得する
	/// @return 現在の `Phase` の参照
	const LevelData &getCurrentPhase();

private:
	/// @brief 現在のフェーズ (非 const)
	/// @return 現在の `Phase` の参照
	/// @remarks ゲッターとか言いながらオブジェクトの変更も許しているので、private にしておく
	LevelData &m_currentPhase();

	/* -- セッター -- */
public:

	/// @brief 全種類のUFO猫の中からランダムに1体ターゲットを設定する
	/// @return 設定後のターゲットの `CatData` の参照
	const CatData &setTargetData();

private:
	/// @brief ターゲットが出現する時間を引数に応じてランダムに決め、`m_targetAppearTime` に設定する
	/// @param level 整数値（1 ~ 10 の範囲で、特に現在のフェーズレベル (`m_currentPhaseIndex`) を入れることを想定）
	/// @return  ターゲットが出現する時間
	const Duration &m_setTargetAppearTime(uint32 level);

	/* -- メソッド -- */
public:

	/// @brief コンストラクタ
	explicit GameManager()
		: cats{ LoadCatData() }
		, levels{ LoadLevelData() }
	{ }

	void announceTarget();

	void startPhase();

	void inPhase();

	void endPhase();

	void result();

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
	Array<LevelData> LoadLevelData();
};
