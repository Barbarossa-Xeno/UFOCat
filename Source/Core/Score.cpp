# include "Score.hpp"

namespace UFOCat::Core::Score
{
	LevelRecord::LevelRecord(size_t level, bool isCaught, bool isCorrect, double response, size_t consecutiveCorrect)
		: level{ level }
		, isCaught{ isCaught }
		, isCorrect{ isCorrect }
		, response{ response }
		, consecutiveCorrect{ consecutiveCorrect }
	{ }

	size_t LevelRecord::calculateTotal()
	{
		total = static_cast<size_t>
			(
				// 捕まえたら +22
				isCaught ?
				(
					22.0 *
					// 正解なら x2.2、更に反応速度に応じたボーナスを乗算
					// 不正解なら x1
					(
						isCorrect ? 2.2 * (2.2 + 1 / (2.2 * response)) : 1.0
						) *
					// 正誤にかかわらずレベルに応じたボーナスを乗算し、
					// 連続正解ボーナスを加算
					Math::Exp(2.2 * level / 10.0) + 222 * consecutiveCorrect
					)
				// 捕まえなかったら 0 点
				: 0
			);

		return total;
	}

	size_t ResultRecord::calculateTotal()
	{
		// 一度すべてのスコアで総合得点を反映させる
		records.each([](LevelRecord& data) { data.calculateTotal(); });

		// それらを合計する
		total = (records >> [](const LevelRecord& data) { return data.total; }).sum();

		return total;
	}

	size_t GetMaxTheoretical(size_t levelCount)
	{
		// ローカル static 変数のため初回呼び出し時にしか計算されない
		static size_t sum = Array<LevelRecord>{ levelCount }
			.each_index([](size_t i, LevelRecord& score)
			{
				// 反応時間は 0.1s ということで、とても早くしておく
				// これのせいで理論値が高くなりすぎる場合は調整する
				score = LevelRecord{ i + 1, true, true, 0.1, i };
			})
			.map([](LevelRecord score)
			{
				return score.calculateTotal();
			})
			.sum();

		return sum;
	}

	void SetLevelCount(size_t count)
	{
		GetMaxTheoretical(count);
	}
}
