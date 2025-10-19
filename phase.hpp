#pragma once
#include "cat_action.hpp"
#include "delta_stopwatch.hpp"

class Phase
{
	/* -- エイリアス --  */
public:
	/// @brief `std::type_info` 型の配列で引数の型情報（シグネチャ）を表す
	using Signature = Array<const std::type_info*>;

	/* -- クラス -- */
public:

	/// @brief 品種のデータ
	struct BreedData
	{
		/// @brief ターゲットと類似している猫を何匹登場させるか
		/// @details ターゲットと似ている猫の定義は、`Phase::similarity` フィールドを参照
		uint32 similar;

		/// @brief 別にターゲットとは類似ではない猫を何匹登場させるか
		uint32 other;
	};

	/// @brief 出現ペースのデータ
	struct IntervalData
	{
		/// @brief 1周期で登場させる猫の数
		uint32 count;

		/// @brief 1周期（インターバル）の時間
		Duration period;
	};

	/// @brief UFO猫が行うアクションのデータ
	struct ActionData
	{
		/// @brief アクション名（`CatObject` のメソッド名と同じ）
		String name;

		/// @brief メソッドを実行させるときのパラメータ情報
		cact::Generic params;

		/// @brief このアクションが選択される確率（0.0 ～ 1.0）
		double probability = 0.0;
	};

	/* -- フィールド -- */
public:

	Duration timeLimit;

	uint32 similarity;

	BreedData breedData;

	IntervalData intervalData;

	/// @brief このフェーズで使用し得るすべてのアクション
	Array<ActionData> actionDataList;

	bool isCleared = false;

private:

	DeltaStopwatch m_stopwatch;

	/* -- メソッド -- */
public:
	Phase(const Duration& timeLimit, uint32 similarity, BreedData &breedData, IntervalData &intervalData, Array<ActionData> &actionDataList)
		: timeLimit{ timeLimit }
		, similarity{ similarity }
		, breedData{ breedData }
		, intervalData{ intervalData }
		, actionDataList{ actionDataList }
	{ }

	bool updateAtInterval();

	static bool IsDuration(const String &str);

	static bool IsRect(const String &str);

	static bool IsEasing(const String &str);

	static Duration ParseDuration(const String &str);

	static Rect ParseRect(const String &str);

	static cact::EasingFunction ParseEasing(const String &str);

	template <typename TTuple>
	static auto ParseParameters(const JSON &paramData)
		requires requires { typename std::tuple_size<TTuple>; };
};
