# pragma once
# include "Component.hpp"
# include "CatObject.hpp"
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

	/// @brief レベルごとに集計するスコアのデータ
	struct Score
	{
		/// @brief プレイしたレベル
		size_t level = InvalidIndex;

		/// @brief 猫を捕まえたか
		bool isCaught = false;

		/// @brief 捕まえた猫が正解だったか
		bool isCorrect = false;

		/// @brief ターゲットが出現してから捕まえるまでかかった時間 [s]
		double response = 0.0;

		/// @brief 連続正解数
		uint32 consecutiveCorrect = 0;

		/// @brief 総合得点
		uint32 total = 0;

		/// @brief このレベルでの総合得点を計算する
		/// @return このレベルでの総合得点
		uint32 calculateTotal()
		{
			total = static_cast<uint32>
				(
					// 捕まえたら +22
					isCaught ? (22.0 *
						// 正解なら x2.2、更に反応速度に応じたボーナスを加算
						// 不正解なら x1
						(isCorrect ? 2.2 * (2.2 + 1 / (2.2 * response)) : 1.0) *
						// 正誤にかかわらずレベルに応じたボーナスを乗算し、
						// 連続正解ボーナスを加算
						Math::Exp(2.2 * level / 10.0) +
						222 * consecutiveCorrect)
					// 捕まえなかったら 0 点
					: 0
				);

			return total;
		}
	};

	struct GameData
	{
		Array<CatObject> cats;

		Array<LevelData> levels;

		Array<std::unique_ptr<CatObject>> spawns;

		Array<Score> scores;

		size_t targetIndex = InvalidIndex;

		size_t levelIndex = InvalidIndex;

		Timer timer;
	};

	/// @brief UFO猫のデータをJSONから読み込んでそれら全てのインスタンスを作成する
	/// @return 全てのUFO猫のインスタンス配列
	Array<CatObject> LoadCatData();

	/// @brief 各フェーズのデータをJSONから読み込んでそれらすべてのインスタンスを作成する
	/// @return 全てのフェーズの配列
	Array<LevelData> LoadLevelData();

	using App = SceneManager<State, GameData>;
}
