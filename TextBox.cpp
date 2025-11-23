# include "GUI.hpp"

namespace UFOCat::GUI
{
	TextBox::TextBox(const DrawableText &text, double fontSize)
		: m_text{ text }
		, m_fontSize{ fontSize }
	{
		m_region = text.region(fontSize);
	}

	TextBox &TextBox::set(const DrawableText &text, double fontSize)
	{
		m_region = text.region(fontSize);
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
