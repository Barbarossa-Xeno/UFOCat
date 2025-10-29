# include "Component.hpp"

namespace UFOCat::Component
{
	Scrollable::Scrollable(const SizeF &size, const Font &font, const String &text, const Vec2 &padding)
		: m_font(font)
		, m_text(text)
		, m_padding(padding)
		, m_region{ size }
		, m_innerRegion{ padding, (size - padding) }
	{}

	void Scrollable::update()
	{
		if (m_innerRegion.mouseOver())
		{
			m_innerRegion.moveBy(0.0, Mouse::Wheel());
		}
	}

	ScrollableRegion::ScrollableRegion() : Scrollable() {}

	ScrollableRegion::ScrollableRegion(const SizeF &size, const Font &font, const String &text, const Vec2 &padding)
		: Scrollable(size, font, text, padding)
	{}

	void ScrollableRegion::draw(const Vec2 &position) const
	{
		const RectF current = m_region.movedBy(position);
		current.draw(Palette::Aliceblue);

		m_innerRegion.draw();

		m_font(m_text).draw(m_innerRegion, Palette::Black);

		/*current.left().draw(m_padding.x, Palette::Aliceblue);
		current.right().draw(m_padding.x, Palette::Aliceblue);
		current.top().draw(m_padding.y, Palette::Aliceblue);
		current.bottom().draw(m_padding.y, Palette::Aliceblue);*/
	}
}
