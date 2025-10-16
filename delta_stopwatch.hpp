#pragma once
#include <Siv3D.hpp>

/// @brief Scene::DeltaTime() を積算することで動く自作ストップウォッチ @n
/// 
class DeltaStopwatch
{
private:
	// 内部で使う経過時間
	double m_elapsedTime = 0.0;

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
	const double &reset(double time) noexcept;

	/// @brief 指定した数値で現在の時間を引いてリセットする
	/// @param time 時間 [Duration]
	/// @return リセット後の時間
	const double& reset(Duration &time) noexcept;

	/// @brief タイマーを 0 にリセットする
	/// @return リセット後の時間
	const double &reset() noexcept;

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
	bool isOver(Duration &time, bool isAutoReset = true) noexcept;
};
