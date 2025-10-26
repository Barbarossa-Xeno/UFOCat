# pragma once
# include "cat_object.hpp"
# include "LevelData.hpp"

namespace UFOCat
{
	/// @brief インデックスを保存する変数のための無効値 @n
	/// このゲームで登場するUFO猫の種類は最大で 44 種類であり、
	/// テクスチャを読み込む際も、レベルデータを読み込む際も、その数字を超えることはない @n
	/// そのため、この名前空間内では特別に無効値として扱う
	/// @note Optional でもよかったけど、いちいち value() ってするのが面倒だったけん
	constexpr size_t InvalidIndex = 44;

	enum class State
	{
		Title,
		Wanted,
		Level,
		Result
	};

	struct GameData
	{
		Array<CatObject> cats;

		Array<LevelData> phases;

		Array<std::unique_ptr<CatObject>> spawns;

		size_t targetIndex = InvalidIndex;

		size_t levelIndex = InvalidIndex;

		Timer timer;

		uint32 score;
	};

	/// @brief UFO猫のデータをJSONから読み込んでそれら全てのインスタンスを作成する
	/// @return 全てのUFO猫のインスタンス配列
	Array<CatObject> LoadCatData();

	/// @brief 各フェーズのデータをJSONから読み込んでそれらすべてのインスタンスを作成する
	/// @return 全てのフェーズの配列
	Array<LevelData> LoadLevelData();

	using App = SceneManager<State, GameData>;
}
