# pragma once
# include "Layoutable.hpp"

namespace UFOCat::GUI
{
	/// @brief `Layoutable` な GUI コンポーネントを複数格納し、
	/// マウスのホイールスクロールやスワイプ、スクロールバー操作によるスクロール表示を可能にするコンポーネント
	class ScrollView final : public Drawable
	{
		/// @brief 実際にスクロールする（位置を移動させる）部分の情報
		struct ScrollPart
		{
			/// @brief 表示されない部分も含めた要素全体の領域
			RectF region;

			/// @brief スクロール範囲の Y 座標の最小値
			double minY;

			/// @brief スクロール範囲の X 座標の最大値
			double maxY;

			ScrollPart() = default;

			/// @brief コンストラクタ
			/// @param region 表示されない部分も含めた要素全体の領域
			/// @param minY スクロール最小値
			/// @param maxY スクロール最大値
			ScrollPart(const RectF &region, double minY, double maxY);

			/// @brief スクロール範囲を取得する
			/// @return 最大値 - 最小値
			inline double getRange() const noexcept
			{
				return maxY - minY;
			}
		};

		/// @brief ビューポートの中の要素（コンテナ）
		/// 一緒にスクロールさせたいコンテンツは、座標系ではこの領域内に入ることになる
		ScrollPart m_container;

		/// @brief スクロールバー
		ScrollPart m_bar;

		/// @brief ビューの中に入れるコンポーネント（コンテンツ）
		Array<std::unique_ptr<Layoutable>> m_contents{};

		/// @brief 現在のスクロール割合 (0.0 ~ 1.0)
		double m_progress = 0.0;

		/// @brief スクロールバーがホバーされているか
		/// @note バーの色を変えるときに使う
		bool m_isHoverBar = false;

	public:
		/// @brief スクロールバーのサイズ
		constexpr static SizeF BarSize{ 5, 60 };

	private:
		/// @brief スクロールする必要があるかどうか
		/// コンテナの長さを判定して決められる
		bool m_shouldScroll() const noexcept;

		/// @brief 現在のコンテナの高さとビューポート領域を比較して、Y 座標をスクロールするときの最小値を返す
		/// @note Siv3Dの座標系は下方向ほど Y の値が大きいので、ビューポート内で上方向を表すこの値は必ず0以下になる 
		/// @return スクロール座標の最小値
		double m_getContainerMinScroll() const noexcept;

		/// @brief スクロール可能要素（コンテナかスクロールバー）を動かす
		/// @param target スクロールさせる要素
		/// @param dy Y 方向の移動量
		/// @return 自分自身の参照
		ScrollView &m_scroll(ScrollPart &target, double dy) noexcept;

		/// @brief 直接操作しないスクロールバーかコンテナを、操作しているどちらか一方のスクロール進捗に同期させる
		/// @param target スクロールを合わせたい要素
		/// @return 自分自身の参照
		ScrollView &m_scrollSync(ScrollPart &target) noexcept;

		/// @brief コンテナの高さやスクロール最小値を更新する
		void m_updateContainer();

		/// @brief コンテンツの高さや位置を更新する
		void m_updateContents();

	public:

		ScrollView() = default;

		/// @brief コンストラクタ
		/// @param position コンテナを設置する左上位置
		/// @param viewportSize ビューポート（表示領域）のサイズ ここからはみ出した部分は映らない
		ScrollView(const Vec2 &position, const SizeF &viewportSize);

		/// @brief ビューポート（表示領域）を設定する
		/// @param region 領域
		/// @return 自分自身の参照
		ScrollView &setViewport(const RectF &region);

		/// @brief コンテナ内に配置するコンテンツを追加する
		/// @tparam ...TContents `Relocatable` なコンポーネント（パラメータパック）
		/// @param ...contents `Relocatable` なコンポーネントを複数指定（可変長引数）
		/// @return 自分自身の参照
		template <std::derived_from<Layoutable> ...TContents>
		inline ScrollView &addContents(const TContents &...contents)
		{
			// Fold 式でループしながら追加
			((m_contents << std::make_unique<TContents>(contents)), ...);

			// スクロール位置をリセット
			m_progress = 0.0;
			m_container.region.y = 0.0;
			m_bar.region.y = m_bar.minY;

			m_updateContents();
			m_updateContainer();

			return *this;
		}

		/// @brief コンテナ内に配置するコンテンツを設定しなおす
		/// @tparam ...TContents `Layoutable` なコンポーネント（パラメータパック）
		/// @param ...contents `Layoutable` なコンポーネントを複数指定（可変長引数）
		/// @return 自分自身の参照
		template <std::derived_from<Layoutable> ...TContents>
		inline ScrollView &setContents(const TContents &...contents)
		{
			// 既存のコンテンツをクリア
			m_contents.release();
			
			return addContents(contents...);
		}

		void update();

		void draw() const override;
	};
}
