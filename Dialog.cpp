# include "GUI.hpp"

namespace UFOCat::Component::GUI
{
	Dialog &Dialog::set(const Font &font, const String &text, const SizeF &size)
	{

		m_font = font;
		m_text = text;
		m_size = size;
		m_region = RectF{ Arg::center(Scene::Center()), size };

		// ボタンは再確保せず中身を更新
		Console << m_okButton.getRegion();
		m_okButton.set(font, U"Yes").setPosition(Arg::bottomCenter = Vec2{ m_region.centerX() - m_region.w / 4.0, m_region.bottomCenter().y - 20 });
		m_cancelButton.set(font, U"No").setPosition(Arg::bottomCenter = Vec2{ m_region.centerX() + m_region.w / 4.0, m_region.bottomCenter().y - 20 });

		return *this;
	}

	Dialog::Dialog(const Font &font, const String &text, const SizeF &size)
		: m_font{ font }
		, m_text{ text }
		, m_okButton{ font, U"No!" }
		, m_cancelButton{ font, U"Yes!" }
		, m_size{ size }
		, m_region{ Arg::center(Scene::Center()), size }
	{}

	bool Dialog::isPressedOK()
	{
		return m_isOpen and m_okButton.isPressed();
	}

	bool Dialog::isPressedCancel()
	{
		// 押されたとき、ダイアログが消える
		if (m_cancelButton.isPressed())
		{
			m_isOpen = false;
			return true;
		}
		else
		{
			return false;
		}
	}

	void Dialog::open()
	{
		m_isOpen = true;
	}

	void Dialog::draw() const
	{
		// フラグが立っていないときは描画しない
		if (not m_isOpen)
		{
			return;
		}

		// 背景
		m_region.rounded(12)
				.drawShadow(Vec2{ 4, 4 }, 16, 0)
				.draw(ColorF{ 0.95, 0.9, 0.8 });

		// OKボタン
		m_okButton.draw();
		
		// キャンセルボタン
		m_cancelButton.draw();

		// 区切り線
		RoundRect line{ RectF{ Arg::center(Scene::CenterF().x, m_okButton.getRegion().topY() - 20), SizeF(0.9, 0.01) * m_region.size }, 4 };

		line.draw(ColorF{ 0.8, 0.7, 0.6 });

		// テキストは区切り線と背景上との中央に描画
		m_font(m_text).drawAt(m_region.center().x, m_region.topY() + (line.topCenter().y - m_region.topY()) / 2, ColorF{ 0.4, 0.3, 0.2 });
	}
}
