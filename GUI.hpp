# pragma once
# include "FontName.hpp"
# include "Colors.hpp"

namespace UFOCat::Component::GUI
{
	/// @brief ボタンコンポーネント @n
	/// デフォルトのフォントは 油性マジック を使用する
	class Button
	{

		Font m_font = FontAsset(FontName::YuseiMagic);

		double m_fontSize;

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
		/// @param fontSize
		/// @param text テキスト
		/// @param isEnabled 有効かどうか
		/// @param padding ボタンの内側余白 (デフォルトは (30, 10))
		/// @note https://siv3d.github.io/ja-jp/tutorial2/button/ を参考に改変
		Button(const Font &font, double fontSize, const String &text, bool isEnabled = true, const Vec2 &padding = { 30.0, 10.0 });

		/// @brief ボタンの描画範囲を取得する
		/// @return 描画範囲
		RectF getRegion() const;

		/// @brief ボタンの各種パラメータを一括で設定する
		/// @param fontSize 
		/// @param text テキスト
		/// @param isEnabled 有効かどうか
		/// @param padding ボタンの内側余白 (デフォルトは (30, 10))
		Button &set(double fontSize, const String& text, bool isEnabled = true, const Vec2& padding = { 30.0, 10.0 });

		/// @brief ボタンに表示するフォントを設定する
		/// デフォルトの表示フォントを変えたいときはこのメソッドから明示的に行うこと
		/// @param font フォント
		/// @return 
		Button &setFont(const Font &font);

		/// @brief ボタンに表示するテキストを設定する
		/// @param text テキスト
		/// @return 
		Button &setText(const String &text);

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

	/// @brief プログレスバーを描画するコンポーネント
	class ProgressBar
	{
		RectF m_region;

		/// @brief 進捗状況 (0 ~ 1)
		double m_progress = 0.0;

		/// @brief 角丸の丸み
		double m_roundness = 9.0;

		/// @brief 
		ColorF m_color;

	public:

		ProgressBar() = default;

		ProgressBar(const SizeF& size, ColorF color, double roundness = 9.0, double progress = 0.0);

		/// @brief プログレスバーの領域を取得する
		/// @return 
		RectF getRegion() const;

		/// @brief 各パラメータを設定する
		/// @param size プログレスバーの背景領域の大きさ
		/// @param color
		/// @param roundness 角丸の丸み (デフォルトは 9.0)
		/// @return 
		ProgressBar &set(const SizeF &size, ColorF color, double roundness = 9.0);

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

	/// @brief 簡単なテキストをウィンドウとして 画面中央に表示し、1ボタンで閉じるコンポーネント
	/// デフォルトのフォントは 油性マジック を使用する
	/// 現時点ではボタンのフォントを変更することはできない（しないと思う）し、ボタンのフォントサイズはウィンドウ幅によって決まります
	class MessageBox
	{
	protected:
		RectF m_region;

		Font m_font = FontAsset(FontName::YuseiMagic);

		double m_fontSize;

		String m_text;

		/// @brief OK ボタン
		Button m_okButton;

		/// @brief グウィンドウの大きさ
		SizeF m_windowSize;

		/// @brief 開いているか
		bool m_isOpen = false;

		/// @brief ボタンのテキスト（フォント）サイズを返す
		/// @return サイズ
		double m_getButtonTextSize() const;

	public:
		MessageBox() = default;

		MessageBox(const Font &font, double fontSize, const String &text, const SizeF &windowSize = { 350, 300 });

		virtual MessageBox &set(double fontSize, const String &text, const SizeF &windowSize = { 350, 300 });

		MessageBox &setFont(const Font &font);

		/// @brief 開いているか
		/// @return 
		bool isOpen() const;

		/// @brief ダイアログを開く
		void open();

		/// @brief ダイアログを閉じる
		void close();

		/// @brief ボタンが押されたか（押されたら閉じる）
		/// @return 押されたら `true`
		virtual bool isPressedOK();

		/// @brief 描画する
		virtual void draw() const;

	protected:

		/// @brief 背景を描画する
		void m_drawBackground() const;

		/// @brief 区切り線とテキストを描画する @n
		/// 区切り線はボタン上端部の Y 座標を基準に配置、テキストはそれと背景の上端部との中央に配置される
		/// @param buttonsTopY ボタン上端部の Y 座標
		void m_drawSeparatorAndText(double buttonsTopY) const;
	};

	/// @brief 2ボタン（Yes / No）でプレイヤーの意思を確認するためのダイアログを表示するコンポーネント
	class Dialog : public MessageBox
	{
	protected:

		/// @brief キャンセルボタン
		Button m_cancelButton;

	public:

		Dialog() = default;

		Dialog(const Font &font, double fontSize, const String &text, const SizeF &windowSize = { 350, 300 });

		Dialog &set(double fontSize, const String &text, const SizeF &windowSize = { 350, 300 }) override;

		/// @brief OKボタンが押されたか
		/// @return 押されたら `true`
		bool isPressedOK() override;

		/// @brief キャンセルボタンが押された
		/// @return 押されたら `true`
		bool isPressedCancel();

		/// @brief 描画する
		void draw() const override;
	};

	// TODO: 後で作る！！
	class Scrollable;
}
