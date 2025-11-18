# include "GUI.hpp"

namespace UFOCat::GUI
{
	double MessageBox::m_getButtonTextSize() const
	{
		// ウィンドウ幅の 5%
		return 0.05 * m_region.size.x;
	}

	MessageBox::MessageBox(const Font &font, double fontSize, const String &text, const SizeF &windowSize)
		: m_region{ Arg::center = Scene::Center(), windowSize }
		, m_font{ font }
		, m_fontSize{ fontSize }
		, m_text{ text }
		, m_okButton{ font, Ceil(m_getButtonTextSize()), U"OK" }
	{
		// ボタンは下部中央
		m_okButton.setPosition(Arg::bottomCenter = Vec2{ m_region.centerX(), m_region.bottomCenter().y - 20 });
	}

	MessageBox &MessageBox::set(double fontSize, const String &text, const SizeF &windowSize)
	{
		m_fontSize = fontSize;
		m_text = text;
		// setPos(Arg::center = ...) を使うとなぜか値がバグるので、直接 RectF を再確保
		m_region = RectF{ Arg::center = Scene::Center(), windowSize };

		// ボタンは再確保せず中身を更新
		m_okButton.set(Ceil(m_getButtonTextSize()), U"OK")
				  .setPosition(Arg::bottomCenter = Vec2{ m_region.centerX(), m_region.bottomCenter().y - 20 });

		return *this;
	}

	MessageBox &MessageBox::setFont(const Font &font)
	{
		m_font = font;
		// ボタンのフォントは変更しない
		return *this;
	}

	bool MessageBox::isOpen() const
	{
		return m_isOpen;
	}

	void MessageBox::open()
	{
		m_isOpen = true;
	}

	void MessageBox::close()
	{
		m_isOpen = false;
	}

	bool MessageBox::isPressedOK()
	{
		// 押されたとき、ダイアログが消える
		if (m_isOpen and m_okButton.isPressed())
		{
			close();

			// 押されたということで true を返す
			return true;
		}

		// それ以外 false
		return false;
	}

	void MessageBox::m_drawBackground() const
	{
		ColorF lightBrown2{ 0.95, 0.85, 0.65 };
		// ウィンドウ範囲を角丸にして背景
		m_region.rounded(12)
				.drawShadow(Vec2{ 4, 4 }, 16, 0)
				.draw(Util::Palette::LightBrownAlt);
	}

	void MessageBox::m_drawSeparatorAndText(double buttonsTopY) const
	{
		// 区切り線
		RoundRect line
		{
			RectF
			{
				Arg::center(Scene::CenterF().x, buttonsTopY - 20),
				SizeF(0.9, 0.01) * m_region.size
			},
			4
		};

		ColorF brown2 = Util::Palette::Brown;
		brown2.a = 0.6;
		line.draw(brown2);

		// テキストは区切り線と背景上との中央に描画
		m_font(m_text).drawAt
		(
			m_fontSize,
			m_region.center().x,
			m_region.topY() + (line.topCenter().y - m_region.topY()) / 2,
			Util::Palette::Brown
		);
	}

	void MessageBox::draw() const
	{
		// フラグが立ってるときのみ描画
		if (m_isOpen)
		{
			// 背景
			m_drawBackground();

			// 区切り線とテキスト
			m_drawSeparatorAndText(m_okButton.getRegion().topY());

			// OKボタン
			m_okButton.draw();
		}
	}
}
