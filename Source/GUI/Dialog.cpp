# include "Dialog.hpp"

namespace UFOCat::GUI
{
	Arg::bottomCenter_<Vec2> Dialog::m_okButtonPosition() const noexcept
	{
		// 横はウィンドウ横幅の 25%、盾はウィンドウ下部から縦マージン分上
		return Arg::bottomCenter(Vec2{ m_region.centerX() - m_region.w / 4.0, m_region.bottomCenter().y - m_Margin });
	}

	Arg::bottomCenter_<Vec2> Dialog::m_cancelButtonPosition() const noexcept
	{
		// 横はウィンドウ横幅の 75%、盾はウィンドウ下部から縦マージン分上
		return Arg::bottomCenter(Vec2{ m_region.centerX() + m_region.w / 4.0, m_region.bottomCenter().y - m_Margin });
	}

	Dialog::Dialog(const SizeF &windowSize, Optional<Button> okButtonStyle, Optional<Button> cancelButtonStyle)
		: MessageBox{ windowSize, okButtonStyle }
		, m_cancelButton{ cancelButtonStyle ? *cancelButtonStyle : Button{ Ceil(m_buttonSize()), U"NO", AudioAsset(Util::AudioName::SE::Cancel) }}
	{
		// 上書き
		m_okButton.setPosition(m_okButtonPosition());
		m_cancelButton.setPosition(m_cancelButtonPosition());
	}

	Dialog &Dialog::setSize(const SizeF &windowSize)
	{
		// 基底クラス呼び出し
		MessageBox::setSize(windowSize);

		// ボタンは位置更新しなおす
		m_okButton.set(Ceil(m_buttonSize()), U"Yes", AudioAsset(Util::AudioName::SE::OK)).setPosition(m_okButtonPosition());
		m_cancelButton.set(Ceil(m_buttonSize()), U"No", AudioAsset(Util::AudioName::SE::Cancel)).setPosition(m_cancelButtonPosition());

		return *this;
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

		// ダイアログウィンドウが開かれている状態で
		// キャンセルボタンを描画する処理を追加
		if (m_isOpen)
		{
			m_cancelButton.draw();
		}
	}
}
