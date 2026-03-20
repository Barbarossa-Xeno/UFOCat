# pragma once
# include "Relocatable.hpp"

namespace UFOCat::GUI
{
	/// @brief シンプルなテキストボックスの機能を提供するコンポーネント
	class TextBox : public Relocatable
	{
		/// @brief Font()(U"") の形で描画可能にしたテキストデータ
		DrawableText m_text;

		/// @brief フォントサイズ
		double m_fontSize;

		/// @brief テキストの色
		Color m_color;

	public:
		TextBox() = default;

		/// @brief コンストラクタ
		/// @param text Font()(U"") の形で描画可能にしたテキストデータ
		/// @param fontSize フォントサイズ
		/// @param color テキストの色
		/// @param positionType 座標指定方法
		TextBox(const DrawableText &text, double fontSize, const Color &color, PositionType positionType = PositionType::Absolute);

		/// @brief 各種パラメータを一括で設定する
		/// @param text Font()(U"") の形で描画可能にしたテキストデータ
		/// @param fontSize フォントサイズ
		/// @param color テキストの色
		/// @param positionType 座標指定方法
		/// @return 自分自身の参照
		TextBox &set(const DrawableText &text, double fontSize, const Color &color, PositionType positionType = PositionType::Absolute);

		/// @brief 
		/// @param px 
		/// @return 
		TextBox &setIndent(double px);

		RelocatableTypeID typeID() const override;

		TextBox &setPosition(const Vec2& position, bool isOverwriteDefault) noexcept override;

		TextBox &setPosition(const Arg::topCenter_<Vec2> &position, bool isOverwriteDefault = false) noexcept override;

		TextBox &setPosition(const Arg::topRight_<Vec2> &position, bool isOverwriteDefault = false) noexcept override;

		TextBox &setPosition(const Arg::leftCenter_<Vec2> &position, bool isOverwriteDefault = false) noexcept override;

		TextBox &setPosition(const Arg::rightCenter_<Vec2> &position, bool isOverwriteDefault = false) noexcept override;

		TextBox &setPosition(const Arg::bottomLeft_<Vec2> &position, bool isOverwriteDefault = false) noexcept override;

		TextBox &setPosition(const Arg::bottomCenter_<Vec2> &position, bool isOverwriteDefault = false) noexcept override;

		TextBox &setPosition(const Arg::bottomRight_<Vec2> &position, bool isOverwriteDefault = false) noexcept override;

		TextBox &setPositionAt(const Vec2 &position, bool isOverwriteDefault = false) noexcept override;

		TextBox &setMargin(const Margin &margin) noexcept override;

		/// @brief 指定した横幅に合わせてテキストボックスの折り返しと高さ変更をする
		/// @param width 合わせる横幅
		/// @return テキストボックスの範囲が変化すれば true
		bool adjustWidth(double width);

		void draw() const override;
	};
}
