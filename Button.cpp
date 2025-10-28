# include "Component.hpp"

namespace UFOCat::Component
{
	ButtonRegion &Button::set(const Vec2& position, const Font& font, const String& text, bool isEnabled, const Vec2& padding) const
	{
		m_selfRegion = std::make_unique<ButtonRegion>(position, font, text, isEnabled, padding);
		return *m_selfRegion;
	}

	Button::Button(const Vec2 &position, const Font &font, const String &text, bool isEnabled, const Vec2 &padding)
		: m_position(position)
		, m_font(font)
		, m_text(text)
		, m_isEnabled(isEnabled)
		, m_padding(padding)
		// フォントの描画領域を特定して、パディング分を足したサイズで Rect を作成
		, m_rect{ m_position, (m_font(m_text).region(m_position).size + m_padding) }
	{ }

	bool Button::isPressed() const
	{
		// 束縛している ButtonRegion があればそれを使う
		// なければ自分自身を使う
		// （ないのは、インスタンスを作ったきりで set 系メソッドを実行していないとき）
		Button const* ptr = m_selfRegion ? m_selfRegion.get() : this;

		// マウスカーソルがボタンの上にある場合
		if (ptr->m_isEnabled and ptr->m_rect.mouseOver())
		{
			// マウスカーソルを手の形にする
			Cursor::RequestStyle(CursorStyle::Hand);
		}

		// ボタンが押されたら true を返す
		return (ptr->m_isEnabled and ptr->m_rect.leftClicked());
	}

	void ButtonRegion::draw() const
	{
		const RoundRect roundRect = m_rect.rounded(6);

		// 影と背景を描く
		roundRect.drawShadow(Vec2{ 2, 2 }, 12, 0)
			.draw(ColorF{ 0.9, 0.8, 0.6 });

		// 枠を描く
		m_rect.stretched(-3).rounded(3)
			.drawFrame(2, ColorF{ 0.4, 0.3, 0.2 });

		// テキストを描く
		m_font(m_text).drawAt(m_rect.center(), ColorF{ 0.4, 0.3, 0.2 });

		// 無効の場合は
		if (not m_isEnabled)
		{
			// グレーの半透明を重ねる
			roundRect.draw(ColorF{ 0.8, 0.8 });
		}
	}
}
