# pragma once
# include "Relocatable.hpp"

namespace UFOCat::GUI
{
	/// @brief プログレスバーを描画するコンポーネント
	class ProgressBar : public Relocatable
	{
		/// @brief 進捗状況 (0 ~ 1)
		double m_progress = 0.0;

		/// @brief 角丸の丸み
		double m_roundness = 9.0;

		/// @brief バーのカラー
		ColorF m_color;

	public:

		ProgressBar() = default;

		ProgressBar(const SizeF &size, const ColorF &color, PositionType positionType = PositionType::Absolute, double roundness = 9.0, double progress = 0.0);

		/// @brief 各パラメータを設定する
		/// @param size プログレスバーの背景領域の大きさ
		/// @param color バーの色
		/// @param positionType 座標指定方法
		/// @param roundness 角丸の丸み (デフォルトは 9.0)
		/// @return 
		ProgressBar &set(const SizeF &size, const ColorF &color, PositionType positionType = PositionType::Absolute, double roundness = 9.0);

		/// @brief プログレスバーの値を設定する
		/// @param progress パラメータ (0.0 〜 1.0)
		ProgressBar &setProgress(double progress) noexcept;

		/// @brief 描画する
		void draw() const override;

		RelocatableTypeID typeID() const noexcept override;

		ProgressBar &setPosition(const Vec2 &position, bool isOverwriteDefault = false) noexcept override;

		ProgressBar &setPosition(const Arg::topCenter_<Vec2> &position, bool isOverwriteDefault = false) noexcept override;

		ProgressBar &setPosition(const Arg::topRight_<Vec2> &position, bool isOverwriteDefault = false) noexcept override;

		ProgressBar &setPosition(const Arg::leftCenter_<Vec2> &position, bool isOverwriteDefault = false) noexcept override;

		ProgressBar &setPosition(const Arg::rightCenter_<Vec2> &position, bool isOverwriteDefault = false) noexcept override;

		ProgressBar &setPosition(const Arg::bottomLeft_<Vec2> &position, bool isOverwriteDefault = false) noexcept override;

		ProgressBar &setPosition(const Arg::bottomCenter_<Vec2> &position, bool isOverwriteDefault = false) noexcept override;

		ProgressBar &setPosition(const Arg::bottomRight_<Vec2> &position, bool isOverwriteDefault = false) noexcept override;

		ProgressBar &setPositionAt(const Vec2 &position, bool isOverwriteDefault = false) noexcept override;

		ProgressBar &setMargin(const Margin &margin) noexcept override;
	};
}
