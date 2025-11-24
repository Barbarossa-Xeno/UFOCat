# include "GUI.hpp"

namespace UFOCat::GUI
{
	double MessageBox::m_buttonSize() const
	{
		// 変にデカくならないようにするために設定
		return Min(0.05 * m_region.size.x, 24.0);
	}

	Arg::bottomCenter_<Vec2> MessageBox::m_okButtonPosition() const
	{
		return Arg::bottomCenter(Vec2{ m_region.centerX(), m_region.bottomCenter().y - 20 });
	}

	Arg::center_<Vec2> MessageBox::m_separatorPosition() const
	{
		return Arg::center(Scene::CenterF().x, m_okButton.getRegion().topY() - 20);
	}

	MessageBox::MessageBox(const SizeF& windowSize, Optional<Button> buttonStyle)
		// ボタンのスタイル指定がなければデフォルト設定で通す
		: m_okButton{ buttonStyle ? *buttonStyle : Button{ FontAsset(Util::FontFamily::YuseiMagic), Ceil(m_buttonSize()), U"OK" } }
	{
		m_region = RectF{ Arg::center = Scene::Center(), windowSize };
		
		// ボタンは下部中央
		m_okButton.setPosition(m_okButtonPosition());

		// ウィンドウから (20, 20) 離れた位置に、右と下方向も同じだけ間隔を開けたスクロールをつくる
		m_contents = Scrollable{ m_region.tl() + Point{ 20, 20 }, SizeF{ m_region.w - 40, m_separatorPosition()->y - 40 } };
	}

	MessageBox &MessageBox::setSize(const SizeF &windowSize)
	{
		// setPos(Arg::center = ...) を使うとなぜか値がバグるので、直接 RectF を再確保
		m_region = RectF{ Arg::center = Scene::Center(), windowSize };

		// ボタンサイズもウィンドウサイズを参照するので再設定
		m_okButton.set(Ceil(m_buttonSize()), U"OK").setPosition(m_okButtonPosition());

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

	void MessageBox::draw() const
	{
		// フラグが立ってるときのみ描画
		if (m_isOpen)
		{
			// # 背景
			{
				// ウィンドウ範囲を角丸にして背景
				m_region.rounded(12)
					.drawShadow(Vec2{ 4, 4 }, 16, 0)
					.draw(Util::Palette::LightBrownAlt);
			}

			// # 区切り線
			{
				// OK ボタンより 20 px 上の位置に、
				// ウィンドウ幅の 90 % の長さ、
				// ウィンドウ高さの 1% or 4px のうち小さいほうの高さで描画される
				const RoundRect &line = RoundRect
				{
					RectF
					{
						m_separatorPosition(),
						SizeF{ 0.9 * m_region.w, Min(0.01 * m_region.h, 4.0) }
					},
					// 丸みは 4px
					4
				}
				// 透明度少し下げて描画
				.draw(Util::Palette::Brown.withA(0.6));;

				// テキストは区切り線と背景上との中央に描画
				/*m_font(m_text).drawAt
				(
					m_fontSize,
					m_region.center().x,
					m_region.topY() + (line.topCenter().y - m_region.topY()) / 2,
					Util::Palette::Brown
				);*/
			}

			// Scrollable
			m_contents.draw();

			// OKボタン
			m_okButton.draw();
		}
	}
}
