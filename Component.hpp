# pragma once

namespace UFOCat::Component
{
	struct ButtonRegion;

	class Button
	{
	protected:

		Vec2 m_position;

		Font m_font;

		String m_text;

		bool m_isEnabled = true;

		Vec2 m_padding = { 30.0, 10.0 };

		/// @brief ボタンの描画領域
		RectF m_rect;

	private:
		/// @brief ボタンのデータから生成する ButtonRegion のポインタ
		/// @remarks ButtonRegion のメソッドチェーンと、Button のインスタンスを参照したときに、
		/// 同じ ButtonRegion を返すために使用するので、mutable にしている
		mutable std::unique_ptr<ButtonRegion> m_selfRegion = nullptr;

	public:

		/// @brief ボタンの各種パラメータを一括で設定する
		/// @param position 左上の位置
		/// @param font テキストに使うフォント
		/// @param text テキスト
		/// @param isEnabled 有効かどうか
		/// @param padding ボタンの内側余白 (デフォルトは (30, 10))
		ButtonRegion &set(const Vec2& position, const Font& font, const String& text, bool isEnabled = true, const Vec2& padding = { 30.0, 10.0 }) const;

		Button() = default;

		// TODO: アイコンを引数にとれるようにしたい
		/// @brief ボタンを左上位置に、フォントサイズを基準に描画し、押されたかどうかを返す
		/// @param position 左上の位置
		/// @param font テキストに使うフォント
		/// @param text テキスト
		/// @param isEnabled 有効かどうか
		/// @param padding ボタンの内側余白 (デフォルトは (30, 10))
		/// @note https://siv3d.github.io/ja-jp/tutorial2/button/ を参考に改変
		Button(const Vec2 &position, const Font &font, const String &text, bool isEnabled = true, const Vec2 &padding = { 30.0, 10.0 });

		/// @brief ボタンが押されたかを返す
		/// @return 押されたら `true`
		bool isPressed() const;
	};

	struct ButtonRegion : public Button
	{
		ButtonRegion() : Button() {}

		ButtonRegion(const Vec2 &position, const Font &font, const String &text, bool isEnabled = true, const Vec2 &padding = { 30.0, 10.0 })
			: Button(position, font, text, isEnabled, padding)
		{ }

		/// @brief ボタンを描画する
		void draw() const;
	};
}
