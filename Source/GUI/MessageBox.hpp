# pragma once
# include "Scrollable.hpp"
# include "Button.hpp"

namespace UFOCat::GUI
{
	/// @brief 簡単なテキストをウィンドウとして 画面中央に表示し、1ボタンで閉じるコンポーネント
	/// デフォルトのフォントは 油性マジック を使用する
	class MessageBox : public IDrawable
	{
	protected:

		Scrollable m_contents;

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
		double m_buttonSize() const;

		/// @brief OK ボタンの下中央基準の位置を返す @n
		/// x 方向はウィンドウ中央、y 方向はボタン下がウィンドウ下部から 20px 上になる位置
		/// @return 下中央基準の座標データ 実際の基準値が入っているわけではないので注意
		virtual Arg::bottomCenter_<Vec2> m_okButtonPosition() const;

		/// @brief 区切り線の位置を返す
		/// x 方向はウィンドウ中央、y 方向はOK ボタンの 上部から 20px 上の位置
		/// @return 中央基準の座標データ 実際の基準値が入っているわけではないので注意
		virtual Arg::center_<Vec2> m_separatorPosition() const;

		/// @brief コンテンツの表示領域を返す
		/// 区切り線とウィンドウ上部との間らへんを上手いこと指定した範囲
		/// @return 範囲データ
		RectF m_contentsRegion() const;

	public:

		explicit MessageBox(const SizeF &windowSize = { 350, 300 }, Optional<Button> buttonStyle = none);

		virtual MessageBox &setSize(const SizeF &windowSize = { 350, 300 });

		/// @brief 配置するコンテンツを追加する
		/// @tparam ...TContents `Relocatable` なコンポーネント（パラメータパック）
		/// @param ...contents `Relocatable` なコンポーネントを複数指定
		/// @return 自分自身の参照
		template <std::derived_from<Relocatable> ...TContents>
		inline MessageBox &addContents(const TContents &...contents)
		{
			m_contents.addContents(contents...);
			return *this;
		}

		/// @brief 配置するコンテンツを設定しなおす
		/// @tparam ...TContents `Relocatable` なコンポーネント（パラメータパック）
		/// @param ...contents `Relocatable` なコンポーネントを複数指定
		/// @return 自分自身の参照
		template <std::derived_from<Relocatable> ...TContents>
		inline MessageBox &setContents(const TContents &...contents)
		{
			m_contents.setContents(contents...);
			return *this;
		}

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
		virtual void draw() const override;
	};
}
