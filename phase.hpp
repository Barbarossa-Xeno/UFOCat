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
	
	struct BreedData
	{
		uint32 similar;

		uint32 other;
	};

	struct IntervalData
	{
		uint32 count;

		Duration period;
	};

	struct ActionData
	{
		String name;

		cact::Generic params;

		double probability = 0.0;
	};

	/* -- フィールド -- */
public:

	Duration timeLimit;

	uint32 similarity;

	BreedData breedData;

	IntervalData intervalData;

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

	static Array<Phase> LoadData();

private:
	static bool m_IsDuration(const String &str);

	static bool m_IsRect(const String &str);

	static bool m_IsEasing(const String &str);

	static Duration m_ParseDuration(const String &str);

	static Rect m_ParseRect(const String &str);

	static cact::EasingFunction m_ParseEasing(const String &str);

	template <typename TTuple>
	static auto m_ParseParameters(const JSON &paramData)
		requires requires { typename std::tuple_size<TTuple>; };
};
