# include "Component.hpp"

namespace UFOCat::Component
{
	Button &Button::set(const Font& font, const String& text, bool isEnabled, const Vec2& padding) const
	{
		m_instance = std::make_unique<Button>(font, text, isEnabled, padding);
		return *m_instance;
	}

	Button::Button(const Font &font, const String &text, bool isEnabled, const Vec2 &padding)
		: m_font(font)
		, m_text(text)
		, m_isEnabled(isEnabled)
		, m_padding(padding)
		// フォントの描画領域 (左上(0, 0)の位置) を特定して、パディング分を足したサイズで Rect を作成
		, m_region{ m_font(m_text).region().size + m_padding }
	{}

	bool Button::isPressed() const
	{
		// 束縛している Button があればそれを使う
		// なければ自分自身を使う
		// （ないのは、インスタンスを作ったきりで set 系メソッドを実行していないとき）
		const Button *const ptr = m_instance ? m_instance.get() : this;

		// マウスカーソルがボタンの上にある場合
		if (ptr->m_isEnabled and ptr->m_region.mouseOver())
		{
			// マウスカーソルを手の形にする
			Cursor::RequestStyle(CursorStyle::Hand);
		}

		// ボタンが押されたら true を返す
		return (ptr->m_isEnabled and ptr->m_region.leftClicked());
	}

	void Button::draw(const Vec2 &position) const
	{
		// 範囲を指定位置にずらしたうえで、描画用の角丸長方形を作成
		// mutable だから、できること。
		const RoundRect roundRect = m_region.moveBy(position).rounded(6);

		// 影と背景を描く
		roundRect.drawShadow(Vec2{ 2, 2 }, 12, 0)
			.draw(ColorF{ 0.9, 0.8, 0.6 });

		// 枠を描く
		m_region.stretched(-3).rounded(3)
			.drawFrame(2, ColorF{ 0.4, 0.3, 0.2 });

		// テキストを描く
		m_font(m_text).drawAt(roundRect.center(), ColorF{ 0.4, 0.3, 0.2 });
		
		// 無効の場合は
		if (not m_isEnabled)
		{
			// グレーの半透明を重ねる
			roundRect.draw(ColorF{ 0.8, 0.8 });
		}
	}

	void Button::draw(const Arg::topCenter_<Vec2> &position) const
	{
		// 中央基準位置から自分の長さの半分ずらして左上位置とする、yはそのまま
		double x = position->x - m_region.w / 2;
		return draw(Vec2{ x, position->y });
	}

	void Button::draw(const Arg::topRight_<Vec2> &position) const
	{
		// 右端基準位置から自分の長さ分ずらして左上位置とする、yはそのまま
		double x = position->x - m_region.w;
		return draw(Vec2{ x, position->y });
	}

	void Button::draw(const Arg::leftCenter_<Vec2> &position) const
	{
		// 左端中央基準位置から自分の高さの半分ずらす、xはそのまま
		double y = position->y - m_region.h / 2;
		return draw(Vec2{ position->x, y });
	}

	void Button::draw(const Arg::rightCenter_<Vec2> &position) const
	{
		// 右端中央基準位置から自分の高さの半分ずらす、xはそのまま
		double y = position->y - m_region.h / 2;
		return draw(Vec2{ position->x - m_region.w, y });
	}

	void Button::draw(const Arg::bottomLeft_<Vec2> &position) const
	{
		// 左下基準位置から自分の高さ分ずらす、xはそのまま
		double y = position->y - m_region.h;
		return draw(Vec2{ position->x, y });
	}

	void Button::draw(const Arg::bottomCenter_<Vec2> &position) const
	{
		// 下中央基準位置から自分の高さ分と半分ずらす、xは中央基準位置から自分の長さの半分ずらす
		double x = position->x - m_region.w / 2;
		double y = position->y - m_region.h;
		return draw(Vec2{ x, y });
	}

	void Button::draw(const Arg::bottomRight_<Vec2> &position) const
	{
		// 右下基準位置から自分の高さ分ずらす、xは右端基準位置から自分の長さ分ずらす
		double x = position->x - m_region.w;
		double y = position->y - m_region.h;
		return draw(Vec2{ x, y });
	}

	void Button::drawAt(const Vec2 &position) const
	{
		// 中央基準位置から自分の長さと高さの半分ずらして左上位置とする
		double x = position.x - m_region.w / 2;
		double y = position.y - m_region.h / 2;
		return draw(Vec2{ x, y });
	}
}
