# include "GUI.hpp"

namespace UFOCat::GUI
{
	Scrollable::ScrollableComponent::ScrollableComponent(const RectF& region, double minY, double maxY)
		: region{ region }
		, minY{ minY }
		, maxY{ maxY }
	{}

	double Scrollable::ScrollableComponent::getRange() const
	{
		return maxY - minY;
	}

	Scrollable::Scrollable(const Vec2 &position, const SizeF &viewportSize)
		// 一旦仮の領域 あとでちゃんと既成のRectを代入させるようにする
		: m_inner
		{
			{ Vec2::Zero(), SizeF{ viewportSize.x, 600 } },
			-AbsDiff(viewportSize.y, 600.0),
			0.0
		}
		, m_bar
		{
			{ Arg::topRight(viewportSize.x - m_BarSize.x, m_BarSize.x), m_BarSize },
			m_BarSize.x,
			viewportSize.y - m_BarSize.y - m_BarSize.x
		}
		, m_shouldScroll{ m_inner.region.h > viewportSize.y }
	{
		m_region = RectF{ position, viewportSize };
	}

	Scrollable &Scrollable::m_scroll(ScrollableComponent &target, double dy)
	{
		// 次の座標を計算
		const double next = target.region.y + dy;

		// 次の座標が最小値と最大値の間に収まるようにする
		target.region.y = Clamp(next, target.minY, target.maxY);

		// 現在のターゲットを基準にしたスクロール進捗を計算
		m_progress = Clamp(Abs(target.region.y) / target.getRange(), 0.0, 1.0);

		return *this;
	}

	Scrollable &Scrollable::m_scrollSync(ScrollableComponent &target)
	{
		// ターゲット要素のスクロール方向を求める
		// 下に動かすことで、要素的には上に移動する場合は、スクロール範を絶対値としたときの
		// 最小値が最大値よりも大きくなるはずなので、それで判定
		double direction = Abs(target.minY) > Abs(target.maxY) ? -1 : 1;

		// スクロール進捗をターゲットの位置に合わせる
		target.region.y = Clamp(direction * m_progress * target.getRange(), target.minY, target.maxY);

		for (const auto& content : m_contents)
		{
			content->setPosition(Vec2{ content->getRegion().x, m_region.y + content->getRegion().y });
		}

		return *this;
	}

	void Scrollable::update()
	{
		if (not m_shouldScroll)
		{
			// スクロールの必要がないなら何もしない
			return;
		}

		// 中身にマウスカーソルがあるとき
		if (m_region.mouseOver())
		{
			// マウスホイールの回転に合わせてスクロール
			m_scroll(m_inner, Mouse::Wheel() * -30).m_scrollSync(m_bar);
		}

		// スクロールバーの当たり判定を広げておく
		m_isHoverBar = m_bar.region.stretched(4 * m_bar.region.w, m_bar.region.h)
								   .movedBy(2 * m_bar.region.w, m_bar.region.h / 4)
								   .mouseOver();

		// スクロールバー付近にマウスがあれば
		if (m_isHoverBar)
		{
			Cursor::RequestStyle(CursorStyle::Hand);

			if (MouseL.pressed())
			{
				// 左クリックをホールドしながら上下させるとスクロールする
				m_scroll(m_bar, Cursor::DeltaF().y).m_scrollSync(m_inner);
			}
		}
	}

	void Scrollable::draw() const
	{
		// 参考
		// https://siv3d.github.io/ja-jp/tutorial3/2d-render-state/?h=viewpo#4813-%E3%83%93%E3%83%A5%E3%83%BC%E3%83%9D%E3%83%BC%E3%83%88

		{
			const ScopedViewport2D viewport{ m_region.asRect() };

			// FontAsset(Util::FontFamily::YuseiMagic)(U"てすとてすとてすとてすとてすとてすと").draw(m_inner.region);

			if (not m_contents.empty())
			{
				for (const auto& content : m_contents)
				{
					content->draw();
				}
			}

			if (m_shouldScroll)
			{
				m_bar.region.rounded(2)
							.drawShadow(Vec2::Zero(), 2, 1, Palette::Gray)
							.draw(m_isHoverBar ? Palette::Lightgray : Palette::White);
			}
		}

		

		m_region.drawFrame(1, Palette::Seagreen);
	}
}
