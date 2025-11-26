# include "GUI.hpp"

namespace UFOCat::GUI
{
	TextBox::TextBox(const DrawableText &text, double fontSize, const Color &color, PositionType positionType)
		: m_text{ text }
		, m_fontSize{ fontSize }
		, m_color{ color }
	{
		m_region = text.region(fontSize);
		m_positionType = positionType;
	}

	TextBox &TextBox::set(const DrawableText &text, double fontSize, const Color &color, PositionType positionType)
	{
		m_region = text.region(fontSize);
		m_text = text;
		m_fontSize = fontSize;
		m_color = color;
		m_positionType = positionType;

		return *this;
	}

	TextBox &TextBox::setIndent(double px)
	{
		m_region.setPos(px, m_region.y);
		return *this;
	}

	bool TextBox::adjustWidth(double width)
	{
		// カウンタをセット
		double i = 0;

		// テキストが全て収まりきるまで高さを増やしていく
		// 初めから収まりきっている場合は、このループに入らない
		while (not m_text.fits(m_fontSize, RectF{ m_region.pos, width - m_region.pos.x, m_region.h + i }))
		{
			++i;
		}

		// 収まったら、一旦適用
		m_region.setSize(width - m_region.pos.x, m_region.h + i);

		// 再度リセット
		i = 0;

		// 今度はテキストがギリギリ収まりきらなくなるまで高さを減らしていく
		while (m_text.fits(m_fontSize, RectF{ m_region.pos, width - m_region.pos.x, m_region.h + i }))
		{
			--i;
		}

		// カウンタを1つ増やしなおして収まる範囲に補正
		m_region.setSize(width - m_region.pos.x, m_region.h + (++i));

		// i の変更なしでここまで到達していたら、新たな変更がなかったということなので
		// その判定を戻り値にする
		return i == 0;
	}

	void TextBox::draw() const
	{
		m_text.draw(m_fontSize, m_region, m_color);
	}
}
