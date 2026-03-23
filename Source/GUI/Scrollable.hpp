# pragma once
# include "Relocatable.hpp"

namespace UFOCat::GUI
{
	/// @brief `Relocatable` な GUI コンポーネントを複数入れてビューポートでスクロール可能にするコンポーネント
	class Scrollable : public Drawable
	{
		/// @brief スクロール可能な部分の情報
		struct ScrollData
		{
			/// @brief 描画領域
			RectF region;

			/// @brief スクロール範囲の Y 座標の最小値
			double minY;

			/// @brief スクロール範囲の X 座標の最大値
			double maxY;

			ScrollData() = default;

			/// @brief コンストラクタ
			/// @param region 描画領域
			/// @param minY スクロール最小値
			/// @param maxY スクロール最大値
			ScrollData(const RectF &region, double minY, double maxY);

			/// @brief スクロール範囲を取得する
			/// @return 最大値 - 最小値
			inline double getRange() const noexcept
			{
				return maxY - minY;
			}
		};

		/// @brief ビューポートの中の要素
		ScrollData m_inner;

		/// @brief スクロールバー
		ScrollData m_bar;

		/// @brief ビューの中に入れるコンポーネント（コンテンツ）
		Array<std::unique_ptr<Relocatable>> m_contents{};

		/// @brief 現在のスクロール割合 (0.0 ~ 1.0)
		double m_progress = 0.0;

		/// @brief スクロールバーがホバーされているか
		/// @note バーの色を変えるときに使う
		bool m_isHoverBar = false;

	public:
		/// @brief スクロールバーのサイズ
		constexpr static SizeF BarSize{ 5, 60 };

	private:
		/// @brief クロールする必要があるかどうか
		/// インナー要素の長さを判定して決められる
		bool m_shouldScroll() const noexcept;

		/// @brief 現在のインナー要素の高さとビューポート領域を比較して、Y 座標をスクロールするときの最小値を返す
		/// @note Siv3Dの座標系は下方向ほど Y の値が大きいので、ビューポート内で上方向を表すこの値は必ず0以下になる 
		/// @return スクロール座標の最小値
		double m_getInnerMinScroll() const noexcept;

		/// @brief スクロール可能要素を動かす
		/// @param target スクロールさせる要素
		/// @param dy Y 方向の移動量
		/// @return 自分自身の参照
		Scrollable &m_scroll(ScrollData &target, double dy) noexcept;

		/// @brief 直接操作しないスクロール要素とインナー要素を現在のスクロール進捗に同期させる
		/// @param target スクロールを合わせたい要素
		/// @return 自分自身の参照
		Scrollable &m_scrollSync(ScrollData &target) noexcept;

		/// @brief インナーの高さやスクロール最小値を更新する
		void m_updateInner();

		/// @brief コンテンツの高さや位置を更新する
		void m_updateContents();

	public:

		Scrollable() = default;

		Scrollable(const Vec2 &position, const SizeF &viewportSize);

		Scrollable &setRegion(const RectF &viewport);

		/// @brief インナー要素内に配置するコンテンツを追加する
		/// @tparam ...TContents `Relocatable` なコンポーネント（パラメータパック）
		/// @param ...contents `Relocatable` なコンポーネントを複数指定
		/// @return 自分自身の参照
		template <std::derived_from<Relocatable> ...TContents>
		inline Scrollable &addContents(const TContents &...contents)
		{
			// Fold 式でループしながら追加
			((m_contents << std::make_unique<TContents>(contents)), ...);

			// スクロール位置をリセット
			m_progress = 0.0;
			m_inner.region.y = 0.0;
			m_bar.region.y = m_bar.minY;

			m_updateContents();
			m_updateInner();

			return *this;
		}

		/// @brief インナー要素内に配置するコンテンツを設定しなおす
		/// @tparam ...TContents `Relocatable` なコンポーネント（パラメータパック）
		/// @param ...contents `Relocatable` なコンポーネントを複数指定
		/// @return 自分自身の参照
		template <std::derived_from<Relocatable> ...TContents>
		inline Scrollable &setContents(const TContents &...contents)
		{
			// 既存のコンテンツをクリア
			m_contents.release();
			
			return addContents(contents...);
		}

		void update();

		void draw() const override;
	};
}
