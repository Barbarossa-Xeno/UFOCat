# pragma once

namespace UFOCat::Component
{
	/// @brief ボタン フォントサイズに大きさが適応する
	class Button
	{

		/// @brief 使用フォント（フォントサイズを任意に設定してあるもの）
		Font m_font;

		/// @brief 表示テキスト
		String m_text;

		/// @brief ボタンが有効か
		bool m_isEnabled = true;

		/// @brief 外枠からのパディング
		Vec2 m_padding = { 30.0, 10.0 };

		/// @brief ボタンの描画領域
		/// @remarks mutable にして描画メソッドでの値の更新を許可する
		mutable RectF m_region;

		/// @brief パラメータの変更があった際に生成するの自クラスのポインタ
		/// @remarks はじめに生成したインスタンスからメソッドチェーンを使ったり、
		/// 別の場所で参照したりしたときに、同じデータを扱えるようにするためのもの
		/// const メソッドでもキャッシュのために使用するので、mutable にしている
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

	/// @brief プログレスバー（ゲージ要素）
	class ProgressBar
	{
		/// @brief 表示範囲
		RectF m_region;

		/// @brief 進捗を表すパラメータ (0 ~ 1)
		double m_progress = 0.0;

		/// @brief パラメータの変更により生成する、自クラスのインスタンスを示すポインタ
		mutable std::unique_ptr<ProgressBar> m_instance = nullptr;

		// TODO: 角丸具合が外部から調整できてもいいかも
		/// @brief 角丸の丸み
		const double m_roundness = 6;

	public:

		/// @brief 大きさを設定する
		/// @param size x, y 方向の大きさ
		/// @return インスタンス
		ProgressBar &setSize(const SizeF &size) const;

		/// @brief プログレスバーの値を設定する
		/// @param progress パラメータ (0.0 〜 1.0)
		/// @return インスタンス
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
