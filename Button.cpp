# include "GUI.hpp"

namespace UFOCat::GUI
{
	Button::Button(const Font &font, double fontSize, const String &text, const Audio &se, PositionType positionType, bool isEnabled, const Vec2 &padding)
		: m_font(font)
		, m_fontSize(fontSize)
		, m_text(text)
		, m_se(se)
		, m_isEnabled(isEnabled)
		, m_padding(padding)
	{
		m_positionType = positionType;

		// フォントの描画領域 (左上(0, 0)の位置) を特定して、パディング分を足したサイズで Rect を作成
		m_region = RectF{ m_font(m_text).region(fontSize).size + m_padding };
	}

	Button::Button(double fontSize, const String &text, const Audio &se, PositionType positionType, bool isEnabled, const Vec2 &padding)
		// ここでフォントを決め打ちにする
		: Button(FontAsset(Util::FontFamily::YuseiMagic), fontSize, text, se, positionType, isEnabled, padding)
	{}

	Button::Button(const Font &font, double fontSize, const String &text, PositionType positionType, bool isEnabled, const Vec2 &padding)
		// ここで SE を決め打ちにする
		: Button(font, fontSize, text, AudioAsset(Util::AudioList::SE::Open), positionType, isEnabled, padding)
	{}

	Button& Button::set(const Font &font, double fontSize, const String &text, const Audio &se, PositionType positionType, bool isEnabled, const Vec2 &padding)
	{
		m_font = font;
		m_fontSize = fontSize;
		m_text = text;
		m_se = se;
		m_positionType = positionType;
		m_isEnabled = isEnabled;
		m_padding = padding;
		m_region = RectF{ m_font(m_text).region(fontSize).size + m_padding };

		return *this;
	}

	Button &Button::set(double fontSize, const String &text, const Audio &se, PositionType positionType, bool isEnabled, const Vec2 &padding)
	{
		return set(m_font, fontSize, text, se, positionType, isEnabled, padding);
	}

	Button &Button::set(const Font &font, double fontSize, const String &text, PositionType positionType, bool isEnabled, const Vec2 &padding)
	{
		return set(font, fontSize, text, m_se, positionType, isEnabled, padding);
	}

	Button &Button::set(double fontSize, const String &text, PositionType positionType, bool isEnabled, const Vec2 &padding)
	{
		return set(m_font, fontSize, text, m_se, positionType, isEnabled, padding);
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

	bool Button::isPressed() const
	{	
		// マウスカーソルがボタンの上にある場合
		if (m_isEnabled and m_region.mouseOver())
		{
			// マウスカーソルを手の形にする
			Cursor::RequestStyle(CursorStyle::Hand);
		}

		// ボタンが押されたらSEを鳴らして true を返す
		if (m_isEnabled and m_region.leftClicked())
		{
			m_se.playOneShot();
			return true;
		}
		else
		{
			return false;
		}
	}

	void Button::draw() const
	{
		// 描画用の角丸長方形を作成
		const RoundRect roundRect = m_region.rounded(6);

		// 影と背景を描く
		roundRect.drawShadow(Vec2{ 2, 2 }, 12, 0)
			.draw(Util::Palette::LightBrown);

		// 枠を描く
		m_region.stretched(-3).rounded(3)
			.drawFrame(2, Util::Palette::Brown);

		// テキストを描く
		m_font(m_text).drawAt(m_fontSize, roundRect.center(), Util::Palette::Brown);
		
		// 無効の場合は
		if (not m_isEnabled)
		{
			// グレーの半透明を重ねる
			roundRect.draw(ColorF{ 0.8, 0.8 });
		}
	}
}
