# pragma once
# include "../Common.hpp"

namespace UFOCat::Core::Score
{
	/// @brief スコアから決められる「称号」のデータ
	struct Title
	{
		/// @brief 称号名（漢字）
		String kanjiName;

		/// @brief 称号名のフリガナ（ルビ）
		String ruby;

		/// @brief 称号間の閾値
		double threshold = 0.0;
	};

	/// @brief 称号の定義リスト @n
	/// スコアの閾値が小さい順に並んでいるので、総合得点と比較していく際には
	/// どんどんインデックスの値を増やして走査していけばいい
	const static std::array<Title, 5> Titles =
	{ {
		{ U"新米", U"しんまい", 0.25 },
		{ U"逸材", U"いつざい", 0.5 },
		{ U"手練", U"てだれ", 0.75 },
		{ U"究極", U"きゅうきょく", 0.9 },
		{ U"神秘", U"しんぴ", 1.0 }
	} };

	/// @brief レベルごとに集計するスコアデータ
	struct LevelRecord
	{
		/// @brief プレイしたレベル (1 ~ )
		size_t level = InvalidIndex;

		/// @brief 猫を捕まえたか
		bool isCaught = false;

		/// @brief 捕まえた猫が正解だったか
		bool isCorrect = false;

		/// @brief ターゲットが出現してから捕まえるまでかかった時間 [s]
		double response = 0.0;

		/// @brief 連続正解数
		size_t consecutiveCorrect = 0;

		/// @brief 総合得点
		size_t total = 0;

		LevelRecord() = default;

		/// @brief コンストラクタ
		/// @param level レベル (1 ~ )
		/// @param isCaught 何かしら捕まえたか
		/// @param isCorrect 正解したか
		/// @param response 捕まえるのにかかった時間 [s]
		/// @param consecutiveCorrect ここまでの連続正解数
		LevelRecord(size_t level, bool isCaught, bool isCorrect, double response, size_t consecutiveCorrect);

		/// @brief このレベルでの総合得点を計算する
		/// @return このレベルでの総合得点
		size_t calculateTotal();
	};

	/// @brief 1ゲームをプレイし終わった結果のスコアデータ
	/// @note これが総合スコアとして残る
	struct ResultRecord
	{
		/// @brief 各レベルのスコアデータ
		/// @remarks 途中でレベルをクリアできなかった場合は、そのレベル以降のデータは存在しない
		Array<LevelRecord> records;

		/// @brief 称号データ
		Title title;

		size_t total = 0;

		/// @brief この1ゲーム全体での総合得点（複数レベルの総合得点の合計）を計算する
		/// @return この1ゲームでの総合得点
		size_t calculateTotal();
	};

	/// @brief このゲームでの理論上の最大スコアを返す
	/// @remarks アプリが起動している間、変更されないレベル数をもとに計算されるので、引数を省略可能とする @n
	/// この計算は、レベルデータがアプリが最初に起動したときにのみ読み込まれる保証を利用している
	/// @param levelCount レベル数 省略された場合はアプリ起動時に設定されたレベル数が使われる
	/// @return 理論値
	size_t GetMaxTheoretical(size_t levelCount = 0);

	/// @brief この1ゲームで行われる最大レベル数を決める @n
	/// 単純に level_data.json で定義されたレベルの数と同じだが、
	/// レベルの数をもとに最大スコアの理論値が決まるため、ここで保持しておき、
	/// アプリが起動している間、変更を許さない
	/// @param count レベル数
	void SetLevelCount(size_t count);
}
