# pragma once

namespace UFOCat::Util
{
	/// @brief Scene::DeltaTime() を積算することで動く自作ストップウォッチ @n
	class Stopwatch
	{
	private:
		/// @brief 内部で使う経過時間
		double m_elapsedTime = 0.0;

		/// @brief 指定時間を超えた回数
		uint32 m_overCount = 0;

	public:
		/// @brief 現在の時間を返す
		/// @return 現在の時間
		const double &now() const noexcept;

		/// @brief タイマーを進める
		/// @details 呼び出されるとデルタタイムを加算する
		/// @return 加算後の時間
		const double &forward() noexcept;

		/// @brief 指定した数値で現在の時間を引いてリセットする
		/// @param time 時間
		/// @return リセット後の時間
		const double &resetTime(double time) noexcept;

		/// @brief 指定した数値で現在の時間を引いてリセットする
		/// @param time 時間 [Duration]
		/// @return リセット後の時間
		const double &resetTime(const Duration &time) noexcept;

		/// @brief タイマーを 0 にリセットする
		/// @return リセット後の時間
		const double &resetTime() noexcept;

		/// @brief 超過カウントをリセットする
		void resetOverCount() noexcept;

		/// @brief タイマーも超過カウントもリセットする
		void reset() noexcept;

		/// @brief タイマーが指定した経過時間を超えているかを返す @n
		/// デフォルトでは、指定した時間を超えていたらタイマーを自動リセットする
		/// @param time 時間
		/// @param isAutoReset 指定した時間を超えていたら自動でリセットするか（デフォルト `true`）
		/// @return 指定した時間を超えていたら `true`
		bool isOver(double time, bool isAutoReset = true) noexcept;

		/// @brief タイマーが指定した経過時間を超えているかを返す @n
		/// デフォルトでは、指定した時間を超えていたらタイマーを自動リセットする
		/// @param time 時間 [Duration]
		/// @param isAutoReset 指定した時間を超えていたら自動でリセットするか（デフォルト `true`）
		/// @return 指定した時間を超えていたら `true`
		bool isOver(const Duration &time, bool isAutoReset = true) noexcept;

		/// @brief 指定時間経過したあとに、コールバックを 1度 実行する @n
		/// このメソッドは呼び出されるたびにタイマーを進めている（`forward()` を実行している）ので、ループ内で呼び出す必要がある
		/// @tparam Func コールバックの型 呼び出し可能 (std::invocable)
		/// @param callback コールバック ラムダ式で記述する
		/// @param time 時間
		/// @return コールバックに戻り値があればそれを返す
		template <std::invocable Func>
		void setTimeout(Func &&callback, const Duration &time)
		{
			// isOver() の isAutoReset を false にしておく
			if (isOver(time, false))
			{
				callback();

				// 時間に達したら経過時間だけリセット
				// 超過回数はしない
				resetTime();
			}
			// 1回目だけ時間を進める
			else if (m_overCount == 0)
			{
				forward();
			}
		}

		/// @brief 指定時間でコールバックの実行を繰り返す @n
		/// このメソッドは呼び出されるたびにタイマーを進めている（`forward()` を実行している）ので、ループ内で呼び出す必要がある
		/// @tparam Func コールバックの型 呼び出し可能 (std::invocable)
		/// @param callback コールバック
		/// @param time 時間
		/// @return コールバックに戻り値があればそれを返す
		template <std::invocable Func>
		void setInterval(Func &&callback, const Duration& time)
		{
			// setTimeout() とほぼ変わらないが
			// 時間と超過回数を毎回リセットしてインターバルを作る
			if (isOver(time))
			{
				callback();
			}
			else
			{
				forward();
			}
		}

		// TODO: タイムオーバーしてないときにも逐次発火できる関数を指定するようにしたくはある
	};
}
