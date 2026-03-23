# pragma once
# include "../Util/FontName.hpp"
# include "../Util/AudioName.hpp"
# include "Relocatable.hpp"

namespace UFOCat::GUI
{
	/// @brief ボタンコンポーネント @n
	/// デフォルトのフォントは 油性マジック を使用する
	class Button : public Relocatable
	{
		Font m_font = FontAsset(Util::FontName::YuseiMagic);

		double m_fontSize;

		String m_text;

		Audio m_se = AudioAsset(Util::AudioName::SE::Open);

		bool m_isEnabled = true;

		Vec2 m_padding = { 30.0, 10.0 };

	public:

		/// @brief デフォルトコンストラクタ
		Button() = default;

		// TODO: アイコンを引数にとれるようにしたい
		/// @brief コンストラクタ
		/// @param font テキストに使うフォント
		/// @param fontSize フォントサイズ
		/// @param text テキスト
		/// @param se ボタンを押したときに鳴らす効果音
		/// @param positionType 座標指定方法
		/// @param isEnabled 有効かどうか
		/// @param padding ボタンの内側余白 (デフォルトは (30, 10))
		/// @note https://siv3d.github.io/ja-jp/tutorial2/button/ を参考に改変
		Button(const Font &font, double fontSize, const String &text, const Audio &se, PositionType positionType = PositionType::Absolute, bool isEnabled = true, const Vec2 &padding = { 30.0, 10.0 });

		/// @brief コンストラクタ（フォントはデフォルト）
		/// @param fontSize フォントサイズ
		/// @param text テキスト
		/// @param se ボタンを押したときに鳴らす効果音
		/// @param positionType 座標指定方法
		/// @param isEnabled 有効かどうか
		/// @param padding ボタンの内側余白 (デフォルトは (30, 10))
		Button(double fontSize, const String &text, const Audio& se, PositionType positionType = PositionType::Absolute, bool isEnabled = true, const Vec2 &padding = { 30.0, 10.0 });

		/// @brief コンストラクタ（SE はデフォルト）
		/// @param font テキストに使うフォント
		/// @param fontSize フォントサイズ
		/// @param text テキスト
		/// @param positionType 座標指定方法
		/// @param isEnabled 有効かどうか
		/// @param padding ボタンの内側余白 (デフォルトは (30, 10))
		Button(const Font &font, double fontSize, const String &text, PositionType positionType = PositionType::Absolute, bool isEnabled = true, const Vec2 &padding = { 30.0, 10.0 });

		/// @brief ボタンの各種パラメータを一括で設定する
		/// @param font テキストに使うフォント
		/// @param fontSize フォントサイズ
		/// @param text テキスト
		/// @param se ボタンを押したときに鳴らす効果音
		/// @param positionType 座標指定方法
		/// @param isEnabled 有効かどうか
		/// @param padding ボタンの内側余白 (デフォルトは (30.0, 10.0))
		Button &set(const Font &font, double fontSize, const String &text, const Audio &se, PositionType positionType = PositionType::Absolute, bool isEnabled = true, const Vec2 &padding = { 30.0, 10.0 });

		/// @brief ボタンの各種パラメータを一括で設定する（フォントはデフォルト）
		/// @param fontSize フォントサイズ
		/// @param text テキスト
		/// @param se ボタンを押したときに鳴らす効果音
		/// @param positionType 座標指定方法
		/// @param isEnabled 有効かどうか
		/// @param padding ボタンの内側余白 (デフォルトは (30.0, 10.0))
		Button &set(double fontSize, const String &text, const Audio& se, PositionType positionType = PositionType::Absolute, bool isEnabled = true, const Vec2 &padding = { 30.0, 10.0 });

		/// @brief ボタンの各種パラメータを一括で設定する（SE はデフォルト）
		/// @param font テキストに使うフォント
		/// @param fontSize フォントサイズ
		/// @param text テキスト
		/// @param positionType 座標指定方法
		/// @param isEnabled 有効かどうか
		/// @param padding ボタンの内側余白 (デフォルトは (30.0, 10.0))
		Button &set(const Font &font, double fontSize, const String &text, PositionType positionType = PositionType::Absolute, bool isEnabled = true, const Vec2 &padding = { 30.0, 10.0 });

		/// @brief ボタンの各種パラメータを一括で設定する（フォントと SE はデフォルト）
		/// @param fontSize 
		/// @param text テキスト
		/// @param positionType 座標指定方法
		/// @param isEnabled 有効かどうか
		/// @param padding ボタンの内側余白 (デフォルトは (30.0, 10.0))
		Button &set(double fontSize, const String &text, PositionType positionType = PositionType::Absolute, bool isEnabled = true, const Vec2 &padding = { 30.0, 10.0 });

		/// @brief ボタンに表示するフォントを設定する
		/// デフォルトの表示フォントを変えたいときはこのメソッドから明示的に行うこと
		/// @param font フォント
		/// @return 
		Button &setFont(const Font &font);

		/// @brief ボタンに表示するテキストを設定する
		/// @param text テキスト
		/// @return 
		Button &setText(const String &text);

		/// @brief ボタンが押されたかを返す
		/// @return 押されたら `true`
		bool isPressed() const;

		void draw() const override;

		RelocatableTypeID typeID() const noexcept override;

		Button &setPosition(const Vec2 &position, bool isOverwriteDefault = false) noexcept override;

		Button &setPosition(const Arg::topCenter_<Vec2> &position, bool isOverwriteDefault = false) noexcept override;

		Button &setPosition(const Arg::topRight_<Vec2> &position, bool isOverwriteDefault = false) noexcept override;

		Button &setPosition(const Arg::leftCenter_<Vec2> &position, bool isOverwriteDefault = false) noexcept override;

		Button &setPosition(const Arg::rightCenter_<Vec2> &position, bool isOverwriteDefault = false) noexcept override;

		Button &setPosition(const Arg::bottomLeft_<Vec2> &position, bool isOverwriteDefault = false) noexcept override;

		Button &setPosition(const Arg::bottomCenter_<Vec2> &position, bool isOverwriteDefault = false) noexcept override;

		Button &setPosition(const Arg::bottomRight_<Vec2> &position, bool isOverwriteDefault = false) noexcept override;

		Button &setPositionAt(const Vec2& position, bool isOverwriteDefault = false) noexcept override;

		Button &setMargin(const Margin& margin) noexcept override;
	};
}
