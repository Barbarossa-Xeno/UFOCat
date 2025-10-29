# pragma once

namespace UFOCat::Component
{
	/// @brief ボタン
	class Button
	{

		Font m_font;

		String m_text;

		bool m_isEnabled = true;

		Vec2 m_padding = { 30.0, 10.0 };

		/// @brief ボタンの描画領域
		/// @remarks mutable にして描画メソッドでの値の更新を許可する
		mutable RectF m_region;

		/// @brief ボタンのデータから生成する ButtonRegion のポインタ
		/// @remarks ButtonRegion のメソッドチェーンと、Button のインスタンスを参照したときに、
		/// 同じ ButtonRegion を返すために使用するので、mutable にしている
		/// なお、ButtonRegion には継承されない（参照一貫性を保つため）
		mutable std::unique_ptr<Button> m_instance = nullptr;

	public:

		/// @brief ボタンの各種パラメータを一括で設定する
		/// @param font テキストに使うフォント
		/// @param text テキスト
		/// @param isEnabled 有効かどうか
		/// @param padding ボタンの内側余白 (デフォルトは (30, 10))
		Button &set(const Font& font, const String& text, bool isEnabled = true, const Vec2& padding = { 30.0, 10.0 }) const;

		/// @brief デフォルトコンストラクタ
		Button() = default;

		// TODO: アイコンを引数にとれるようにしたい
		// TODO: フォントサイズを指定できるようにしてもいいかも
		/// @brief ボタンを初期化する
		/// @param font テキストに使うフォント
		/// @param text テキスト
		/// @param isEnabled 有効かどうか
		/// @param padding ボタンの内側余白 (デフォルトは (30, 10))
		/// @note https://siv3d.github.io/ja-jp/tutorial2/button/ を参考に改変
		Button(const Font &font, const String &text, bool isEnabled = true, const Vec2 &padding = { 30.0, 10.0 });

		/// @brief ボタンが押されたかを返す
		/// @return 押されたら `true`
		bool isPressed() const;

		// TODO: 色の指定もほしいかも
		/// @brief 左上位置を指定してボタンを描画する
		/// @param position 左上位置
		void draw(const Vec2& position) const;

		/// @brief 中央上位置を指定してボタンを描画する
		/// @param position 中央上位置
		void draw(const Arg::topCenter_<Vec2>& position) const;

		/// @brief 右上位置を指定してボタンを描画する
		/// @param position 右上位置
		void draw(const Arg::topRight_<Vec2>& position) const;

		/// @brief 中央左位置を指定してボタンを描画する
		/// @param position 中央左上位置
		void draw(const Arg::leftCenter_<Vec2>& position) const;

		/// @brief 中央右位置を指定してボタンを描画する
		/// @param position 中央右位置
		void draw(const Arg::rightCenter_<Vec2>& position) const;

		/// @brief 左下位置を指定してボタンを描画する
		/// @param position 左下位置
		void draw(const Arg::bottomLeft_<Vec2>& position) const;

		/// @brief 中央下位置を指定してボタンを描画する
		/// @param position 中央下位置
		void draw(const Arg::bottomCenter_<Vec2>& position) const;

		/// @brief 右下位置を指定しボタンを描画する
		/// @param position 右下位置
		void draw(const Arg::bottomRight_<Vec2>& position) const;

		/// @brief 中央位置を指定してボタンを描画する
		/// @param position 中央位置
		void drawAt(const Vec2& position) const;
	};


	class ProgressBar
	{
		RectF m_region;

		double m_progress = 0.0;

		mutable std::unique_ptr<ProgressBar> m_instance = nullptr;

		const double m_roundness = 6;

	public:

		ProgressBar &setSize(const SizeF &size) const;

		/// @brief プログレスバーの値を設定する
		/// @param progress パラメータ (0.0 〜 1.0)
		ProgressBar &setProgress(double progress) const;

		ProgressBar() = default;

		ProgressBar(const SizeF &size, double roundness, double progress = 0.0);

		/// @brief 左上位置を指定してボタンを描画する
		/// @param position 左上位置
		void draw(const Vec2& position, const SizeF &barScale) const;

		/// @brief 中央上位置を指定してボタンを描画する
		/// @param position 中央上位置
		void draw(const Arg::topCenter_<Vec2>& position, const SizeF &barScale) const;

		/// @brief 右上位置を指定してボタンを描画する
		/// @param position 右上位置
		void draw(const Arg::topRight_<Vec2>& position, const SizeF& barScale) const;

		/// @brief 中央左位置を指定してボタンを描画する
		/// @param position 中央左上位置
		void draw(const Arg::leftCenter_<Vec2>& position, const SizeF& barScale) const;

		/// @brief 中央右位置を指定してボタンを描画する
		/// @param position 中央右位置
		void draw(const Arg::rightCenter_<Vec2>& position, const SizeF& barScale) const;

		/// @brief 左下位置を指定してボタンを描画する
		/// @param position 左下位置
		void draw(const Arg::bottomLeft_<Vec2>& position, const SizeF& barScale) const;

		/// @brief 中央下位置を指定してボタンを描画する
		/// @param position 中央下位置
		void draw(const Arg::bottomCenter_<Vec2>& position, const SizeF& barScale) const;

		/// @brief 右下位置を指定しボタンを描画する
		/// @param position 右下位置
		void draw(const Arg::bottomRight_<Vec2>& position, const SizeF& barScale) const;

		/// @brief 中央位置を指定してボタンを描画する
		/// @param position 中央位置
		void drawAt(const Vec2& position, const SizeF& barScale) const;
	};

	// TODO: 後で作る！！
	class Scrollable
	{
	protected:

		Font m_font;

		String m_text;

		Vec2 m_padding = { 40.0, 20.0 };

		mutable RectF m_region;

		RectF m_innerRegion;

	public:

		Scrollable() = default;

		Scrollable(const SizeF &size, const Font &font, const String &text, const Vec2 &padding = { 40.0, 20.0 });

		void update();
	};

	struct ScrollableRegion : public Scrollable
	{
		ScrollableRegion();

		ScrollableRegion(const SizeF &size, const Font &font, const String &text, const Vec2 &padding = { 40.0, 20.0 });

		void draw(const Vec2 &position) const;

		void drawAt(const Vec2 &position) const;
	};
}
