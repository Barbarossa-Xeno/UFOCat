# include "GUI.hpp"

namespace UFOCat::GUI
{
	TextBox::TextBox(const DrawableText &text, double fontSize, const Vec2 &position)
		: m_text{ text }
		, m_fontSize{ fontSize }
	{
		m_region = RectF{ text.region(fontSize).setPos(position) };
	}

	TextBox &TextBox::set(const DrawableText &text, double fontSize, const Vec2 &position)
	{
		m_region = text.region(fontSize).setPos(position);
		m_text = text;
		m_fontSize = fontSize;

		return *this;
	}

	void TextBox::draw() const
	{
		m_text.draw(m_fontSize, m_region);
	}

	void TextBox::draw(const SizeF &size) const
	{
		m_text.draw(m_fontSize, RectF{ m_region.pos, size });
	}
}
