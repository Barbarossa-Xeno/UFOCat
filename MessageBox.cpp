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
		return Arg::bottomCenter(Vec2{ m_region.centerX(), m_region.bottomCenter().y - m_Margin });
	}

	Arg::center_<Vec2> MessageBox::m_separatorPosition() const
	{
		return Arg::center(Scene::CenterF().x, m_okButton.getRegion().topY() - m_Margin);
	}

	RectF MessageBox::m_contentsRegion() const
	{
		Vec2 &&position{ m_region.tl().x + m_Margin, m_region.tl().y + m_Margin };

		// ウィンドウ幅は、スクロールバーサイズも考慮
		double &&width = m_region.w - m_Margin * 2;

		// ウィンドウ高さから 区切り線までの高さになる
		double &&height = m_region.h - (m_region.bottomY() - m_separatorPosition()->y) - m_Margin * 2;

		return RectF{ position, SizeF{ width, height } };
	}

	MessageBox::MessageBox(const SizeF& windowSize, Optional<Button> buttonStyle)
	{
		m_region = RectF{ Arg::center = Scene::Center(), windowSize };

		// m_region を設定しないと、m_buttonSize() が正しく動かないため、初期化フィールドを使わずここで代入する
		// ボタンのスタイル指定がなければデフォルト設定で通す
		// そして下部中央
		m_okButton = Button{ buttonStyle ? *buttonStyle : Button{ Ceil(m_buttonSize()), U"OK", AudioAsset(Util::AudioSource::SE::OK) } }.setPosition(m_okButtonPosition());

		// ウィンドウから (20, 20) 離れた位置に、右と下方向も同じだけ間隔を開けたスクロールをつくる
		m_contents = Scrollable{ m_contentsRegion().pos, m_contentsRegion().size };
	}

	MessageBox &MessageBox::setSize(const SizeF &windowSize)
	{
		// setPos(Arg::center = ...) を使うとなぜか値がバグるので、直接 RectF を再確保
		m_region = RectF{ Arg::center = Scene::Center(), windowSize };

		// ボタンサイズもウィンドウサイズを参照するので再設定
		m_okButton.set(Ceil(m_buttonSize()), U"OK", AudioAsset(Util::AudioSource::SE::OK)).setPosition(m_okButtonPosition());

		m_contents.setRegion(m_contentsRegion());

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

		m_contents.update();

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
						.drawShadow(Vec2{ 0, 0 }, 16, 8)
						.drawFrame(5.0, HSV{ Util::Palette::Brown }.withV(0.7))
						.draw(Util::Palette::LightBrownAlt);
			}

			// # 区切り線
			{
				// OK ボタンより 20 px 上の位置に、
				// ウィンドウ幅の 90 % の長さ、
				// ウィンドウ高さの 1% or 4px のうち小さいほうの高さで描画される
				RoundRect
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
			}

			// Scrollable
			m_contents.draw();

			// OKボタン
			m_okButton.draw();
		}
	}
}
