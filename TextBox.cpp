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

	TextBox &TextBox::adjustWidth(double width)
	{
		double i = 0;
		while (not m_text.fits(m_fontSize, RectF{ m_region.pos, width - m_region.pos.x, m_region.h + i }))
		{
			++i;
		}

		m_region = RectF{ m_region.pos, width, m_region.h + i };

		return *this;
	}

	void TextBox::draw() const
	{
		m_text.draw(m_fontSize, m_region);
	}
}
