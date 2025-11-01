# include "GUI.hpp"

namespace UFOCat::Component::GUI
{
	Button::Button(const Font& font, double fontSize, const String& text, bool isEnabled, const Vec2& padding)
		: m_font(font)
		, m_fontSize(fontSize)
		, m_text(text)
		, m_isEnabled(isEnabled)
		, m_padding(padding)
		// フォントの描画領域 (左上(0, 0)の位置) を特定して、パディング分を足したサイズで Rect を作成
		, m_region{ m_font(m_text).region(fontSize).size + m_padding }
	{}

	RectF Button::getRegion() const
	{
		return m_region;
	}

	Button &Button::set(double fontSize, const String& text, bool isEnabled, const Vec2& padding)
	{
		m_fontSize = fontSize;
		m_text = text;
		m_isEnabled = isEnabled;
		m_padding = padding;
		m_region = RectF{ m_font(m_text).region(fontSize).size + m_padding };

		return *this;
	}

	Button &Button::setFont(const Font &font)
	{
		m_font = font;

		// フォントを変えたら描画領域も更新する
		m_region = RectF{ m_font(m_text).region(m_fontSize).size + m_padding };
		return *this;
	}

	Button& Button::setText(const String &text)
	{
		m_text = text;

		// テキストを変えたら描画領域も更新する
		m_region = RectF{ m_font(m_text).region(m_fontSize).size + m_padding };
		return *this;
	}

	Button& Button::setPosition(const Arg::topCenter_<Vec2>& position)
	{
		m_region.setPos(position);
		return *this;
	}

	Button& Button::setPosition(const Arg::topRight_<Vec2>& position)
	{
		m_region.setPos(position);
		return *this;
	}

	Button& Button::setPosition(const Arg::leftCenter_<Vec2>& position)
	{
		m_region.setPos(position);
		return *this;
	}

	Button& Button::setPosition(const Arg::rightCenter_<Vec2>& position)
	{
		m_region.setPos(position);
		return *this;
	}

	Button& Button::setPosition(const Arg::bottomLeft_<Vec2>& position)
	{
		m_region.setPos(position);
		return *this;
	}

	Button& Button::setPosition(const Arg::bottomCenter_<Vec2>& position)
	{
		m_region.setPos(position);
		return *this;
	}

	Button& Button::setPosition(const Arg::bottomRight_<Vec2>& position)
	{
		m_region.setPos(position);
		return *this;
	}

	Button& Button::setPositionAt(const Vec2& position)
	{
		m_region.setPos(Arg::center = position);
		return *this;
	}

	bool Button::isPressed() const
	{	
		// マウスカーソルがボタンの上にある場合
		if (m_isEnabled and m_region.mouseOver())
		{
			// マウスカーソルを手の形にする
			Cursor::RequestStyle(CursorStyle::Hand);
		}

		// ボタンが押されたら true を返す
		return (m_isEnabled and m_region.leftClicked());
	}

	void Button::draw() const
	{
		// 描画用の角丸長方形を作成
		const RoundRect roundRect = m_region.rounded(6);

		// 影と背景を描く
		roundRect.drawShadow(Vec2{ 2, 2 }, 12, 0)
			.draw(ColorF{ 0.9, 0.8, 0.6 });

		// 枠を描く
		m_region.stretched(-3).rounded(3)
			.drawFrame(2, ColorF{ 0.4, 0.3, 0.2 });

		// テキストを描く
		m_font(m_text).drawAt(m_fontSize, roundRect.center(), ColorF{ 0.4, 0.3, 0.2 });
		
		// 無効の場合は
		if (not m_isEnabled)
		{
			// グレーの半透明を重ねる
			roundRect.draw(ColorF{ 0.8, 0.8 });
		}
	}
}
