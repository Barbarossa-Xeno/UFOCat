# pragma once

namespace UFOCat::Component::GUI
{
	/// @brief ボタンコンポーネント
	class Button
	{

		Font m_font;

		String m_text;

		bool m_isEnabled = true;

		Vec2 m_padding = { 30.0, 10.0 };

		/// @brief ボタンの描画領域
		RectF m_region;

	public:

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

		/// @brief ボタンの描画範囲を取得する
		/// @return 描画範囲
		RectF getRegion() const;

		/// @brief ボタンの各種パラメータを一括で設定する
		/// @param font テキストに使うフォント
		/// @param text テキスト
		/// @param isEnabled 有効かどうか
		/// @param padding ボタンの内側余白 (デフォルトは (30, 10))
		Button& set(const Font& font, const String& text, bool isEnabled = true, const Vec2& padding = { 30.0, 10.0 });

		/// @brief 描画位置に左上位置を指定する
		/// @param position 左上位置
		Button &setPosition(const Vec2& position);

		/// @brief 描画位置に中央上位置を指定する
		/// @param position 中央上位置
		Button &setPosition(const Arg::topCenter_<Vec2>& position);

		/// @brief 描画位置に右上位置を指定する
		/// @param position 右上位置
		Button &setPosition(const Arg::topRight_<Vec2>& position);

		/// @brief 描画位置に中央左位置を指定する
		/// @param position 中央左上位置
		Button &setPosition(const Arg::leftCenter_<Vec2>& position);

		/// @brief 描画位置に中央右位置を指定する
		/// @param position 中央右位置
		Button &setPosition(const Arg::rightCenter_<Vec2>& position);

		/// @brief 描画位置に左下位置を指定する
		/// @param position 左下位置
		Button &setPosition(const Arg::bottomLeft_<Vec2>& position);

		/// @brief 描画位置に中央下位置を指定する
		/// @param position 中央下位置
		Button &setPosition(const Arg::bottomCenter_<Vec2>& position);

		/// @brief 描画位置に右下位置を指定しボタンを描画する
		/// @param position 右下位置
		Button &setPosition(const Arg::bottomRight_<Vec2>& position);

		/// @brief 描画位置に中央位置を指定する
		/// @param position 中央位置
		Button &setPositionAt(const Vec2& position);

		/// @brief ボタンが押されたかを返す
		/// @return 押されたら `true`
		bool isPressed() const;

		// TODO: 色の指定もほしいかも
		void draw() const;
	};

	/// @brief プログレスバーとその下地の背景を描画するコンポーネント
	class ProgressBar
	{
		RectF m_region;

		/// @brief 進捗状況 (0 ~ 1)
		double m_progress = 0.0;

		/// @brief プログレスバーの背景領域全体に対する、バー自体の長さと高さの倍率
		SizeF m_barScale;

		/// @brief 角丸の丸み
		double m_roundness = 12.0;

	public:

		ProgressBar() = default;

		ProgressBar(const SizeF& size, const SizeF &barScale, double roundness = 12.0, double progress = 0.0);

		/// @brief 各パラメータを設定する
		/// @param size プログレスバーの背景領域の大きさ
		/// @param barScale プログレスバーの背景領域全体に対する、バー自体の長さと高さの倍率
		/// @param roundness 角丸の丸み (デフォルトは 12.0)
		/// @return 
		ProgressBar &set(const SizeF &size, const SizeF& barScale, double roundness = 12.0);

		/// @brief プログレスバーの値を設定する
		/// @param progress パラメータ (0.0 〜 1.0)
		ProgressBar &setProgress(double progress);

		/// @brief 左上位置を指定する
		/// @param position 左上位置
		ProgressBar &setPosition(const Vec2& position);

		/// @brief 中央上位置を指定する
		/// @param position 中央上位置
		ProgressBar &setPosition(const Arg::topCenter_<Vec2>& position);

		/// @brief 右上位置を指定する
		/// @param position 右上位置
		ProgressBar &setPosition(const Arg::topRight_<Vec2>& position);

		/// @brief 中央左位置を指定する
		/// @param position 中央左上位置
		ProgressBar &setPosition(const Arg::leftCenter_<Vec2>& position);

		/// @brief 中央右位置を指定する
		/// @param position 中央右位置
		ProgressBar &setPosition(const Arg::rightCenter_<Vec2>& position);

		/// @brief 左下位置を指定する
		/// @param position 左下位置
		ProgressBar &setPosition(const Arg::bottomLeft_<Vec2>& position);

		/// @brief 中央下位置を指定する
		/// @param position 中央下位置
		ProgressBar &setPosition(const Arg::bottomCenter_<Vec2>& position);

		/// @brief 右下位置を指定する
		/// @param position 右下位置
		ProgressBar &setPosition(const Arg::bottomRight_<Vec2>& position);

		/// @brief 中央位置を指定する
		/// @param position 中央位置
		ProgressBar &setPositionAt(const Vec2 &position);

		/// @brief 描画する
		void draw() const;
	};

	/// @brief 画面中央にダイアログウィンドウを表示するコンポーネント
	class Dialog
	{

		RectF m_region;

		Font m_font;

		String m_text;

		/// @brief OK ボタン
		Button m_okButton;

		/// @brief キャンセルボタン
		Button m_cancelButton;

		/// @brief ダイアログウィンドウの大きさ
		SizeF m_size;

		/// @brief 開いているか
		bool m_isOpen = false;

	public:

		Dialog() = default;

		Dialog(const Font &font, const String &text, const SizeF &size = { 350, 300 });

		Dialog &set(const Font &font, const String &text, const SizeF &size = { 350, 300 });

		/// @brief ダイアログを開く
		void open();

		/// @brief OKボタンが押されたか
		/// @return 押されたら `true`
		bool isPressedOK();

		/// @brief キャンセルボタンが押された
		/// @return 押されたら `true`
		bool isPressedCancel();

		/// @brief 描画する
		void draw() const;
	};

	// TODO: 後で作る！！
	class Scrollable;
}
