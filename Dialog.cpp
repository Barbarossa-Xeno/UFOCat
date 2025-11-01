# include "GUI.hpp"

namespace UFOCat::Component::GUI
{
	Dialog &Dialog::set(double fontSize, const String &text, const SizeF &windowSize)
	{
		// 基底クラス呼び出し
		MessageBox::set(fontSize, text, windowSize);

		// ボタンは位置が変わったりテキストが変わったりするので個別に再セット
		// ウィンドウ下のほうに均等に配置する
		m_okButton.set(Ceil(m_getButtonTextSize()), U"Yes")
				  .setPosition(Arg::bottomCenter = Vec2{ m_region.centerX() - m_region.w / 4.0, m_region.bottomCenter().y - 20 });
		m_cancelButton.set(Ceil(m_getButtonTextSize()), U"No")
					  .setPosition(Arg::bottomCenter = Vec2{m_region.centerX() + m_region.w / 4.0, m_region.bottomCenter().y - 20});

		return *this;
	}

	Dialog::Dialog(const Font &font, double fontSize, const String &text, const SizeF &windowSize)
		: MessageBox{ font, fontSize, text, windowSize }
		, m_cancelButton{ font, Ceil(m_getButtonTextSize()), U"No" }
	{
		// 上書き
		m_okButton.set(Ceil(m_getButtonTextSize()), U"Yes")
				  .setPosition(Arg::bottomCenter = Vec2{ m_region.centerX() - m_region.w / 4.0, m_region.bottomCenter().y - 20 });
	}

	bool Dialog::isPressedOK()
	{
		return m_isOpen and m_okButton.isPressed();
	}

	bool Dialog::isPressedCancel()
	{
		// 押されたとき、ダイアログが消える
		if (m_cancelButton.isPressed())
		{
			close();
			return true;
		}
		return false;
	}

	void Dialog::draw() const
	{
		MessageBox::draw();

		if (m_isOpen)
		{
			// キャンセルボタン
			m_cancelButton.draw();
		}
	}
}
