# include "GUI.hpp"

namespace UFOCat::GUI
{
	// TODO: 今は場所指定を直接行っているが、上下マージンのデータによって行えるようにしてもいいかもしれん
	TextBox::TextBox(const DrawableText &text, double fontSize, const Color &color)
		: m_text{ text }
		, m_fontSize{ fontSize }
		, m_color{ color }
	{
		m_region = text.region(fontSize);
	}

	TextBox &TextBox::set(const DrawableText &text, double fontSize, const Color &color)
	{
		m_region = text.region(fontSize);
		m_text = text;
		m_fontSize = fontSize;
		m_color = color;

		return *this;
	}

	bool TextBox::adjustWidth(double width)
	{
		double i = 0;

		// テキストが全て収まりきるまで高さを増やしていく
		while (not m_text.fits(m_fontSize, RectF{ m_region.pos, width - m_region.pos.x, m_region.h + i }))
		{
			++i;
		}

		// 収まったら、範囲として適用
		m_region = RectF{ m_region.pos, width - m_region.pos.x, m_region.h + i };

		// i の変更なしでここまで到達していたら、新たな変更がなかったということなので
		// その判定を戻り値にする
		return i == 0;
	}

	void TextBox::draw() const
	{
		m_text.draw(m_fontSize, m_region, m_color);
	}
}
