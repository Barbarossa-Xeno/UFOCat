# pragma once
# include "MessageBox.hpp"

namespace UFOCat::GUI
{
	/// @brief 2ボタン（Yes / No）でプレイヤーの意思を確認するためのダイアログを表示するコンポーネント
	/// @note これ以上継承しない
	class Dialog final : public MessageBox
	{
	protected:

		/// @brief キャンセルボタン
		Button m_cancelButton;

		/// @brief OK ボタンの下中央基準の位置を返す @n
		/// 横方向はウィンドウの 25% の位置、縦方向はボタン下がウィンドウ下部から 20px 上になる位置
		/// @return 下中央基準の座標データ 実際の基準値が入っているわけではないので注意が必要
		Arg::bottomCenter_<Vec2> m_okButtonPosition() const noexcept override;

		/// @brief キャンセルボタンの下中央基準の位置を返す @n
		/// 横方向はウィンドウの 75% の位置、縦方向はボタン下がウィンドウ下部から 20px 上になる位置
		/// @return 下中央基準の座標データ 実際の基準値が入っているわけではないので注意が必要
		Arg::bottomCenter_<Vec2> m_cancelButtonPosition() const noexcept;

	public:

		/// @brief コンストラクタ
		/// @param windowSize ウィンドウサイズ デフォルトは (350, 300)
		/// @param okButtonStyle OKボタンのスタイルを Button コンポーネントで指定する 指定しないとデフォルトのスタイルが使われる
		/// @param cancelButtonStyle キャンセルボタンのスタイルを Button コンポーネントで指定する 指定しないとデフォルトのスタイルが使われる
		explicit Dialog(const SizeF &windowSize = { 350, 300 }, Optional<Button> okButtonStyle = none, Optional<Button> cancelButtonStyle = none);

		/// @brief ウィンドウサイズを設定する
		/// @param windowSize ウィンドウサイズ デフォルトは (350, 300)
		/// @return 自分自身の参照
		virtual Dialog& setSize(const SizeF &windowSize = { 350, 300 }) override;

		/// @brief 配置するコンテンツを追加する
		/// @tparam ...TContents `Relocatable` なコンポーネント（パラメータパック）
		/// @param ...contents `Relocatable` なコンポーネントを複数指定
		/// @return 自分自身の参照
		template <std::derived_from<Relocatable> ...TContents>
		inline Dialog &addContents(const TContents &...contents)
		{
			m_contents.addContents(contents...);
			return *this;
		}

		/// @brief 配置するコンテンツを追加する
		/// @tparam ...TContents `Relocatable` なコンポーネント（パラメータパック）
		/// @param ...contents `Relocatable` なコンポーネントを複数指定
		/// @return 自分自身の参照
		template <std::derived_from<Relocatable> ...TContents>
		inline Dialog &setContents(const TContents &...contents)
		{
			m_contents.setContents(contents...);
			return *this;
		}

		/// @brief OKボタンが押されたか（押されたら閉じる）
		/// 監視用に update 内で毎フレーム実行する必要がある
		/// @return 押されたら `true`
		bool isPressedOK() override;

		/// @brief キャンセルボタンが押されたか（押されたら閉じる）
		/// 監視用に update 内で毎フレーム実行する必要がある
		/// @return 押されたら `true`
		bool isPressedCancel();

		/// @brief 描画する
		void draw() const override;
	};
}
