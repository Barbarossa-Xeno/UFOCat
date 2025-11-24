# pragma once
# include "FontFamily.hpp"
# include "Palette.hpp"

namespace UFOCat::GUI
{
	/// @brief 描画可能コンポーネントのインターフェース
	class IDrawable
	{
	protected:
		/// @brief この GUI コンポーネントの画面上の領域
		RectF m_region;

	public:
		virtual ~IDrawable() = default;

		/// @brief この GUI コンポーネントの画面上の領域を取得する
		/// @return 領域 (RectF)
		const RectF& getRegion() const noexcept
		{
			return m_region;
		}

		/// @brief 描画する
		virtual void draw() const = 0;
	};

	/// @brief 再配置可能コンポーネント
	/// 継承先でそのクラス専用のメソッドチェーンを利用する場合、共変オーバーライドする
	class Relocatable : public IDrawable
	{
	protected:
		/// @brief 座標設定の初期値
		/// @note インスタンス化以降 `setPosition()` を初めて呼び出すと初期化され、それ以降変更されない
		Optional<Vec2> m_initialPosition = none;
		// 今は初期値を固定することで後述の Scrollable を実現させているけど
		// この設計方法で対応できないことがあれば、この初期値を変えられるセッターの導入など、また検討

	public:
		/// @brief 座標設定の初期値を取得する
		/// @return 座標の初期値 まだ設定がない場合は必ず (0, 0) を返す
		inline const Vec2 &getInitialPosition() noexcept
		{
			return m_initialPosition ? *m_initialPosition : Vec2::Zero();
		}

		/// @brief 描画位置に左上位置を指定する
		/// @param position 左上位置
		inline virtual Relocatable &setPosition(const Vec2 &position) noexcept
		{
			if (not m_initialPosition)
			{
				m_initialPosition = position;
			}
			m_region.setPos(position);
			return *this;
		}

		/// @brief 描画位置に中央上位置を指定する
		/// @param position 中央上位置
		inline virtual Relocatable &setPosition(const Arg::topCenter_<Vec2> &position) noexcept
		{
			m_region.setPos(position);

			// 左上以外は、一旦 RectF の指定をさせたあとにそこから取得してくる
			if (not m_initialPosition)
			{
				m_initialPosition = m_region.pos;
			}
			return *this;
		}

		/// @brief 描画位置に右上位置を指定する
		/// @param position 右上位置
		inline virtual Relocatable &setPosition(const Arg::topRight_<Vec2> &position) noexcept
		{
			m_region.setPos(position);
			if (not m_initialPosition)
			{
				m_initialPosition = m_region.pos;
			}
			return *this;
		}

		/// @brief 描画位置に中央左位置を指定する
		/// @param position 中央左上位置
		inline virtual Relocatable &setPosition(const Arg::leftCenter_<Vec2>& position) noexcept
		{
			m_region.setPos(position);
			if (not m_initialPosition)
			{
				m_initialPosition = m_region.pos;
			}
			return *this;
		}

		/// @brief 描画位置に中央右位置を指定する
		/// @param position 中央右位置
		inline virtual Relocatable &setPosition(const Arg::rightCenter_<Vec2> &position) noexcept
		{
			m_region.setPos(position);
			if (not m_initialPosition)
			{
				m_initialPosition = m_region.pos;
			}
			return *this;
		}

		/// @brief 描画位置に左下位置を指定する
		/// @param position 左下位置
		inline virtual Relocatable &setPosition(const Arg::bottomLeft_<Vec2> &position) noexcept
		{
			m_region.setPos(position);
			if (not m_initialPosition)
			{
				m_initialPosition = m_region.pos;
			}
			return *this;
		}

		/// @brief 描画位置に中央下位置を指定する
		/// @param position 中央下位置
		inline virtual Relocatable &setPosition(const Arg::bottomCenter_<Vec2> &position) noexcept
		{
			m_region.setPos(position);
			if (not m_initialPosition)
			{
				m_initialPosition = m_region.pos;
			}
			return *this;
		}

		/// @brief 描画位置に右下位置を指定しボタンを描画する
		/// @param position 右下位置
		inline virtual Relocatable &setPosition(const Arg::bottomRight_<Vec2> &position) noexcept
		{
			m_region.setPos(position);
			if (not m_initialPosition)
			{
				m_initialPosition = m_region.pos;
			}
			return *this;
		}

		/// @brief 描画位置に中央位置を指定する
		/// @param position 中央位置
		inline virtual Relocatable &setPositionAt(const Vec2 &position) noexcept
		{
			m_region.setPos(Arg::center = position);
			if (not m_initialPosition)
			{
				m_initialPosition = m_region.pos;
			}
			return *this;
		}
	};
	// ほんとは CRTP（奇妙に再帰したテンプレートパターン）を使ってオーバーライドを避けたかったけど
	// それをやめるとコンパイル時点で継承元があいまいになって
	// CRTP 先のクラスを 基底に入れられない問題が起きてしまった
	// しかし CRTP 先のクラスはテンプレート引数が必要なのでそれなしで実体化不可能
	// 詰んだので共変オーバーライドをいちいちする方式にした

	/// @brief ボタンコンポーネント @n
	/// デフォルトのフォントは 油性マジック を使用する
	class Button : public Relocatable
	{
		Font m_font = FontAsset(Util::FontFamily::YuseiMagic);

		double m_fontSize;

		String m_text;

		bool m_isEnabled = true;

		Vec2 m_padding = { 30.0, 10.0 };

	public:

		/// @brief デフォルトコンストラクタ
		Button() = default;

		// TODO: アイコンを引数にとれるようにしたい
		/// @brief ボタンを初期化する
		/// @param font テキストに使うフォント
		/// @param fontSize
		/// @param text テキスト
		/// @param isEnabled 有効かどうか
		/// @param padding ボタンの内側余白 (デフォルトは (30, 10))
		/// @note https://siv3d.github.io/ja-jp/tutorial2/button/ を参考に改変
		Button(const Font &font, double fontSize, const String &text, bool isEnabled = true, const Vec2 &padding = { 30.0, 10.0 });

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

		inline Button &setPosition(const Vec2 &position) noexcept override
		{
			Relocatable::setPosition(position);
			return *this;
		}

		inline Button &setPosition(const Arg::topCenter_<Vec2> &position) noexcept override
		{
			Relocatable::setPosition(position);
			return *this;
		}

		inline Button &setPosition(const Arg::topRight_<Vec2> &position) noexcept override
		{
			Relocatable::setPosition(position);
			return *this;
		}

		inline Button &setPosition(const Arg::leftCenter_<Vec2>& position) noexcept override
		{
			Relocatable::setPosition(position);
			return *this;
		}

		inline Button &setPosition(const Arg::rightCenter_<Vec2> &position) noexcept override
		{
			Relocatable::setPosition(position);
			return *this;
		}

		inline Button &setPosition(const Arg::bottomLeft_<Vec2> &position) noexcept override
		{
			Relocatable::setPosition(position);
			return *this;
		}

		inline Button &setPosition(const Arg::bottomCenter_<Vec2> &position) noexcept override
		{
			Relocatable::setPosition(position);
			return *this;
		}

		inline Button &setPosition(const Arg::bottomRight_<Vec2> &position) noexcept override
		{
			Relocatable::setPosition(position);
			return *this;
		}

		inline Button &setPositionAt(const Vec2 &position) noexcept override
		{
			Relocatable::setPositionAt(position);
			return *this;
		}

		/// @brief ボタンが押されたかを返す
		/// @return 押されたら `true`
		bool isPressed() const;

		void draw() const override;
	};

	/// @brief プログレスバーを描画するコンポーネント
	class ProgressBar : public Relocatable
	{
		/// @brief 進捗状況 (0 ~ 1)
		double m_progress = 0.0;

		/// @brief 角丸の丸み
		double m_roundness = 9.0;

		/// @brief 
		ColorF m_color;

	public:

		ProgressBar() = default;

		ProgressBar(const SizeF& size, ColorF color, double roundness = 9.0, double progress = 0.0);

		/// @brief 各パラメータを設定する
		/// @param size プログレスバーの背景領域の大きさ
		/// @param color
		/// @param roundness 角丸の丸み (デフォルトは 9.0)
		/// @return 
		ProgressBar &set(const SizeF &size, ColorF color, double roundness = 9.0);

		/// @brief プログレスバーの値を設定する
		/// @param progress パラメータ (0.0 〜 1.0)
		ProgressBar &setProgress(double progress);

		inline ProgressBar &setPosition(const Vec2 &position) noexcept override
		{
			Relocatable::setPosition(position);
			return *this;
		}

		inline ProgressBar &setPosition(const Arg::topCenter_<Vec2> &position) noexcept override
		{
			Relocatable::setPosition(position);
			return *this;
		}

		inline ProgressBar &setPosition(const Arg::topRight_<Vec2> &position) noexcept override
		{
			Relocatable::setPosition(position);
			return *this;
		}

		inline ProgressBar &setPosition(const Arg::leftCenter_<Vec2>& position) noexcept override
		{
			Relocatable::setPosition(position);
			return *this;
		}

		inline ProgressBar &setPosition(const Arg::rightCenter_<Vec2> &position) noexcept override
		{
			Relocatable::setPosition(position);
			return *this;
		}

		inline ProgressBar &setPosition(const Arg::bottomLeft_<Vec2> &position) noexcept override
		{
			Relocatable::setPosition(position);
			return *this;
		}

		inline ProgressBar &setPosition(const Arg::bottomCenter_<Vec2> &position) noexcept override
		{
			Relocatable::setPosition(position);
			return *this;
		}

		inline ProgressBar &setPosition(const Arg::bottomRight_<Vec2> &position) noexcept override
		{
			Relocatable::setPosition(position);
			return *this;
		}

		inline ProgressBar &setPositionAt(const Vec2 &position) noexcept override
		{
			Relocatable::setPositionAt(position);
			return *this;
		}

		/// @brief 描画する
		void draw() const override;
	};

	/// @brief シンプルなテキストボックスの機能を提供するコンポーネント
	class TextBox : public Relocatable
	{
		/// @brief Font()(U"") の形で描画可能にしたテキストデータ
		DrawableText m_text;

		/// @brief フォントサイズ
		double m_fontSize;

	public:
		TextBox() = default;

		TextBox(const DrawableText& text, double fontSize);

		TextBox& set(const DrawableText& text, double fontSize);

		inline TextBox& setPosition(const Vec2& position) noexcept override
		{
			Relocatable::setPosition(position);
			return *this;
		}

		inline TextBox& setPosition(const Arg::topCenter_<Vec2>& position) noexcept override
		{
			Relocatable::setPosition(position);
			return *this;
		}

		inline TextBox& setPosition(const Arg::topRight_<Vec2>& position) noexcept override
		{
			Relocatable::setPosition(position);
			return *this;
		}

		inline TextBox& setPosition(const Arg::leftCenter_<Vec2>& position) noexcept override
		{
			Relocatable::setPosition(position);
			return *this;
		}

		inline TextBox& setPosition(const Arg::rightCenter_<Vec2>& position) noexcept override
		{
			Relocatable::setPosition(position);
			return *this;
		}

		inline TextBox& setPosition(const Arg::bottomLeft_<Vec2>& position) noexcept override
		{
			Relocatable::setPosition(position);
			return *this;
		}

		inline TextBox& setPosition(const Arg::bottomCenter_<Vec2>& position) noexcept override
		{
			Relocatable::setPosition(position);
			return *this;
		}

		inline TextBox& setPosition(const Arg::bottomRight_<Vec2>& position) noexcept override
		{
			Relocatable::setPosition(position);
			return *this;
		}

		inline TextBox& setPositionAt(const Vec2& position) noexcept override
		{
			Relocatable::setPositionAt(position);
			return *this;
		}

		void draw() const override;

		void draw(const SizeF& size) const;
	};

	/// @brief `Relocatable` な GUI コンポーネントを複数入れてビューポートでスクロール可能にするコンポーネント
	class Scrollable : IDrawable
	{
		/// @brief スクロール可能なコンポーネント
		struct ScrollableComponent
		{
			/// @brief 描画領域
			RectF region;

			/// @brief スクロール範囲の Y 座標の最小値
			double minY;

			/// @brief スクロール範囲の X 座標の最大値
			double maxY;

			ScrollableComponent() = default;

			/// @brief コンストラクタ
			/// @param region 描画領域
			/// @param minY スクロール最小値
			/// @param maxY スクロール最大値
			ScrollableComponent(const RectF& region, double minY, double maxY);

			/// @brief スクロール範囲を取得する
			/// @return 最大値 - 最小値
			double getRange() const;
		};

		/// @brief ビューポートの中の要素
		ScrollableComponent m_inner;

		/// @brief スクロールバー
		ScrollableComponent m_bar;

		/// @brief 中に入れておくコンポーネント
		Array<std::unique_ptr<Relocatable>> m_contents{};

		/// @brief 現在のスクロール割合 (0.0 ~ 1.0)
		double m_progress = 0.0;

		/// @brief スクロールバーがホバーされているか
		/// @note バーの色を変えるときに使う
		bool m_isHoverBar = false;

		/// @brief スクロールバーのサイズ
		constexpr static SizeF m_BarSize{ 5, 60 };

		/// @brief クロールする必要があるかどうか
		/// インナー要素の長さを判定して決められる
		bool m_shouldScroll() const;

		/// @brief スクロール可能要素を動かす
		/// @param target スクロールさせる要素
		/// @param dy Y 方向の移動量
		/// @return 自分自身の参照
		Scrollable& m_scroll(ScrollableComponent& target, double dy);

		/// @brief 直接操作しないスクロール要素とインナー要素を現在のスクロール進捗に同期させる
		/// @param target スクロールを合わせたい要素
		/// @return 自分自身の参照
		Scrollable& m_scrollSync(ScrollableComponent& target);

		/// @brief 現在のインナー要素の高さにおいて、Y 座標をスクロールするときの最小値（上方向の最大値 =< 0）を返す
		/// @param viewportHeight ビューポートの高さ
		/// @param innerHeight インナー要素の高さ
		/// @return スクロール座標の最小値
		static double m_CurrentMinimumScroll(double viewportHeight, double innerHeight);

	public:

		Scrollable() = default;

		Scrollable(const Vec2& position, const SizeF& viewportSize);

		/// @brief インナー要素内に配置するコンテンツを追加する
		/// @tparam ...TContents `Relocatable` なコンポーネント（パラメータパック）
		/// @param ...contents `Relocatable` なコンポーネントを複数指定
		/// @return 自分自身の参照
		template <std::derived_from<Relocatable> ...TContents>
		inline Scrollable& setContents(const TContents &...contents)
		{
			// Fold 式
			// 左辺（リストへの代入部分）をパラメータパックの長さ分くりかえす命令になる
			// ちゃっぴー参照
			((m_contents << std::make_unique<TContents>(contents)), ...);

			// 要素全ての高さは一番最後の要素の左上位置とそれ自体の高さに、ビューポート自体の Y 座標を足して計算できるものとする
			m_inner.region.h = m_contents.back()->getRegion().y + m_contents.back()->getRegion().h + m_region.y;

			// スクロールの最小値を更新する
			m_inner.minY = m_CurrentMinimumScroll(m_region.h, m_inner.region.h);

			return *this;
		}

		void update();

		void draw() const override;
	};

	/// @brief 簡単なテキストをウィンドウとして 画面中央に表示し、1ボタンで閉じるコンポーネント
	/// デフォルトのフォントは 油性マジック を使用する
	/// 現時点ではボタンのフォントを変更することはできない（しないと思う）し、ボタンのフォントサイズはウィンドウ幅によって決まります
	class MessageBox : public IDrawable
	{
	protected:
		Font m_font = FontAsset(Util::FontFamily::YuseiMagic);

		double m_fontSize;

		String m_text;

		Scrollable m_content;

		/// @brief OK ボタン
		Button m_okButton;

		/// @brief ウィンドウの大きさ
		SizeF m_windowSize;

		/// @brief 開いているか
		bool m_isOpen = false;

		/// @brief ボタンのサイズを返す
		/// 基本 ウィンドウ幅の 5% で、それよりも 24.0 のほうが小さい値であればそれを返す
		/// @note 正確にはボタンに表示するテキストのサイズを決める -> テキストサイズによりボタンサイズが決まる
		/// @return サイズ
		double m_buttonSize() const;

		/// @brief OK ボタンの下中央基準の位置を返す @n
		/// 横方向はウィンドウ中央、縦方向はボタン下がウィンドウ下部から 20px 上になる位置
		/// @return 下中央基準の座標データ 実際の基準値が入っているわけではないので注意が必要
		virtual Arg::bottomCenter_<Vec2> m_okButtonPosition() const;

		virtual Arg::center_<Vec2> m_separatorPosition() const;

		/// @brief 背景を描画する
		void m_drawBackground() const;

		/// @brief 区切り線とテキストを描画する @n
		/// 区切り線はボタン上端部の Y 座標を基準に配置、テキストはそれと背景の上端部との中央に配置される
		/// @param buttonsTopY ボタン上端部の Y 座標
		void m_drawSeparator() const;

		void m_drawContent() const;

	public:
		MessageBox() = default;

		MessageBox(const Font &font, double fontSize, const String &text, const SizeF &windowSize = { 350, 300 });

		MessageBox(const SizeF& windowSize = { 350, 300 }, Optional<Button> buttonStyle = none);

		virtual MessageBox &set(double fontSize, const String &text, const SizeF &windowSize = { 350, 300 });

		virtual MessageBox &setSize(const SizeF &windowSize = { 350, 300 });

		//virtual MessageBox &setContents

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
		virtual void draw() const override;
	};

	/// @brief 2ボタン（Yes / No）でプレイヤーの意思を確認するためのダイアログを表示するコンポーネント
	class Dialog final : public MessageBox
	{
	protected:

		/// @brief キャンセルボタン
		Button m_cancelButton;

		/// @brief OK ボタンの下中央基準の位置を返す @n
		/// 横方向はウィンドウの 25% の位置、縦方向はボタン下がウィンドウ下部から 20px 上になる位置
		/// @return 下中央基準の座標データ 実際の基準値が入っているわけではないので注意が必要
		Arg::bottomCenter_<Vec2> m_okButtonPosition() const override;

		/// @brief キャンセルボタンの下中央基準の位置を返す @n
		/// 横方向はウィンドウの 75% の位置、縦方向はボタン下がウィンドウ下部から 20px 上になる位置
		/// @return 下中央基準の座標データ 実際の基準値が入っているわけではないので注意が必要
		Arg::bottomCenter_<Vec2> m_cancelButtonPosition() const;

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
}
