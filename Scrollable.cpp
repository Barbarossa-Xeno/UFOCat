# include "GUI.hpp"

namespace UFOCat::GUI
{
	Scrollable::Scrollable(const Vec2 &position, const SizeF &viewportSize)
		: m_region{ position, viewportSize }
		// 一旦仮の領域で y 座標だけはみ出るパターンを試す
		, m_inner{ Vec2::Zero(), SizeF{ m_region.size.x, 600 } }
		, m_bar{ Arg::topRight(viewportSize.x - viewportSize.x * 0.025, viewportSize.x * 0.025), SizeF{ viewportSize.x * 0.025, 30 } }
	{}

	bool Scrollable::m_scroll(double dy)
	{
		Vec2 test = m_inner.pos.movedBy(0, dy);

		if (not (-Math::AbsDiff(m_inner.size.y, m_region.size.y) <= test.y))
		{
			m_inner.setPos(m_inner.x, -Math::AbsDiff(m_inner.size.y, m_region.size.y));

			return false;
		}
		else if (not (test.y <= 0))
		{
			m_inner.setPos(m_inner.x, 0);

			return false;
		}
		else
		{
			m_inner.moveBy(0, dy);

			return true;
		}
	}

	bool Scrollable::m_moveBar(double dy)
	{
		Vec2 test = m_bar.pos.movedBy(0, dy);

		if (not (m_bar.w <= test.y))
		{
			m_bar.setPos(m_bar.x, m_bar.w);

			return false;
		}
		else if (not (test.y <= (m_region.h - m_bar.h - m_bar.w)))
		{
			m_bar.setPos(m_bar.x, m_region.h - m_bar.h - m_bar.w);

			return false;
		}
		else
		{
			m_bar.moveBy(0, dy);

			return true;
		}
	}

	// 
	bool Scrollable::m_scrolledByWheel()
	{
		if (m_region.mouseOver())
		{

			if (-Math::AbsDiff(m_inner.size.y, m_region.size.y) <= m_inner.pos.y and m_inner.pos.y <= 0)
			{
				// スクロールスピードに比例して
				return m_scroll(Mouse::Wheel() * -30);
			}
		}
	}

	bool Scrollable::m_scrolledByBar()
	{
		m_isHoverBar = m_bar.movedBy(m_bar.w, 0).stretched(m_bar.w, m_bar.pos.y).mouseOver();

		if (m_isHoverBar and MouseL.pressed())
		{
			bool res = m_moveBar(Cursor::DeltaF().y );

			return res;
		}
	}

	void Scrollable::update()
	{
		m_scrolledByWheel();
		m_scrolledByBar();
	}

	void Scrollable::draw() const
	{
		// 参考
		// https://siv3d.github.io/ja-jp/tutorial3/2d-render-state/?h=viewpo#4813-%E3%83%93%E3%83%A5%E3%83%BC%E3%83%9D%E3%83%BC%E3%83%88

		{
			const ScopedViewport2D viewport{ m_region.asRect() };

			m_inner.draw(Palette::Lightseagreen);

			FontAsset(Util::FontFamily::YuseiMagic)(U"てすとてすとてすとてすとてすとてすと").draw(m_inner);

			m_bar.rounded(2)
				 .drawShadow(Vec2::Zero(), 2, 1, Palette::Gray)
				 .draw(m_isHoverBar ? Palette::Lightgray : Palette::White);
		}

		

		m_region.drawFrame(1, Palette::Seagreen);
	}
}
