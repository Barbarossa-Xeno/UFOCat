# pragma once
# include "Scrollable.hpp"
# include "Button.hpp"

namespace UFOCat::GUI
{
	/// @brief 簡単なテキストをウィンドウとして 画面中央に表示し、1ボタンで閉じるコンポーネント
	/// ウィンドウ内コンテナはスクロール可能
	/// デフォルトのフォントは 油性マジック を使用する
	class MessageBox : public Drawable, public Scrollable<MessageBox>
	{
	protected:

		/// @brief OK ボタン
		Button m_okButton;

		/// @brief ウィンドウの大きさ
		SizeF m_windowSize;

		/// @brief 開いているか
		bool m_isOpen = false;

		/// @brief 区切り線やボタンやウィンドウ端との間の間隔に使う値
		constexpr static double m_Margin = 20.0;

		/// @brief ボタンのサイズを返す
		/// 基本 ウィンドウ幅の 5% で、それよりも 24.0 のほうが小さい値であればそれを返す
		/// @note 正確にはボタンに表示するテキストのサイズを決める -> テキストサイズによりボタンサイズが決まる
		/// @return サイズ
		double m_buttonSize() const noexcept;

		/// @brief OK ボタンの下中央基準の位置を返す @n
		/// x 方向はウィンドウ中央、y 方向はボタン下がウィンドウ下部から 20px 上になる位置
		/// @return 下中央基準の座標データ 実際の基準値が入っているわけではないので注意
		virtual Arg::bottomCenter_<Vec2> m_okButtonPosition() const noexcept;

		/// @brief 区切り線の位置を返す
		/// x 方向はウィンドウ中央、y 方向はOK ボタンの 上部から 20px 上の位置
		/// @return 中央基準の座標データ 実際の基準値が入っているわけではないので注意
		virtual Arg::center_<Vec2> m_separatorPosition() const noexcept;

		/// @brief コンテナ (m_container) の表示領域を返す
		/// 区切り線とウィンドウ上部との間らへんを上手いこと指定した範囲
		/// @return 範囲データ
		RectF m_containerRegion() const noexcept;

	public:

		/// @brief コンストラクタ
		/// @param windowSize ウィンドウサイズ デフォルトは (350, 300)
		/// @param buttonStyle ボタンのスタイルを Button コンポーネントで指定する 指定しないとデフォルトのスタイルが使われる
		explicit MessageBox(const SizeF &windowSize = { 350, 300 }, Optional<Button> buttonStyle = none);
		// デフォルトスタイルはコンストラクタの定義を参照

		/// @brief ウィンドウサイズを変更する
		/// @param windowSize 新しいウィンドウサイズ デフォルトは (350, 300)
		/// @return 自分自身の参照
		virtual MessageBox &setSize(const SizeF &windowSize = { 350, 300 });

		/// @brief 開いているか
		/// @return ウィンドウが開いているか
		bool isOpen() const noexcept;

		/// @brief ウィンドウを開く
		void open() noexcept;

		/// @brief ウィンドウを閉じる
		void close() noexcept;

		/// @brief ボタンが押されたか（押されたら閉じる）
		/// 監視用に update 内で毎フレーム実行する必要がある
		/// @return 押されたら `true`
		virtual bool isPressedOK();

		/// @brief 描画する
		virtual void draw() const override;
	};
}
