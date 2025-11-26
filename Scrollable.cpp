# include "GUI.hpp"

namespace UFOCat::GUI
{
	Scrollable::ScrollData::ScrollData(const RectF& region, double minY, double maxY)
		: region{ region }
		, minY{ minY }
		, maxY{ maxY }
	{}

	Scrollable::Scrollable(const Vec2 &position, const SizeF &viewportSize)
		: m_inner
		{
			// 一旦インナーはビューポートと同じ大きさにする
			// 座標はビューポート基準になるのに注意
			{ Vec2::Zero(), viewportSize },

			// 初めはインナー要素が空なのでスクロールしない
			0.0, 0.0
		}
		, m_bar
		{
			// 右端のほうにつくる
			{ Arg::topRight(viewportSize.x - BarSize.x, BarSize.x), BarSize },
			BarSize.x,
			viewportSize.y - BarSize.y - BarSize.x
		}
	{
		// このクラスで m_region は描画範囲というより
		// ビューポートサイズとして参照することのほうが多いことに注意
		m_region = RectF{ position, viewportSize };
	}

	bool Scrollable::m_shouldScroll() const
	{
		// ビューポートよりインナーのほうがでかければスクロールの必要あり
		return m_inner.region.h > m_region.h;
	}

	Scrollable &Scrollable::m_scroll(ScrollData &target, double dy)
	{
		// 次の座標を計算
		const double next = target.region.y + dy;

		// 次の座標が最小値と最大値の間に収まるようにする
		target.region.y = Clamp(next, target.minY, target.maxY);

		// 現在のターゲットを基準にしたスクロール進捗を計算
		m_progress = Clamp(Abs(target.region.y) / target.getRange(), 0.0, 1.0);

		return *this;
	}

	Scrollable &Scrollable::m_scrollSync(ScrollData &target)
	{
		// ターゲット要素のスクロール方向を求める
		// 下に動かすことで、要素的には上に移動する場合は、スクロール範を絶対値としたときの
		// 最小値が最大値よりも大きくなるはずなので、それで判定
		double direction = Abs(target.minY) > Abs(target.maxY) ? -1 : 1;

		// スクロール進捗をターゲットの位置に合わせる
		target.region.y = Clamp(direction * m_progress * target.getRange(), target.minY, target.maxY);

		for (const auto &content : m_contents)
		{
			// インナー要素すべてをデフォルト位置からインナー（下地）の移動分だけずらす
			content->setPosition(Vec2{ content->getInitialPosition().x, m_inner.region.y + content->getInitialPosition().y });
		}

		return *this;
	}

	double Scrollable::m_currentMinimumScroll()
	{
		// インナー要素 (m_innerなど) は上方向 = -Y 方向に移動するので、高さどうしの差の絶対値の負をとってやる
		return -AbsDiff(m_region.h, m_inner.region.h);
	}

	void Scrollable::m_updateInner()
	{
		if (not m_contents.empty())
		{
			// 要素全ての高さは一番最後の要素の左上位置とそれ自体の高さに、ビューポート自体の Y 座標を足して計算できるものとする
			m_inner.region.h = m_contents.back()->getInitialPosition().y + m_contents.back()->getRegion().h
								+ (m_contents.back()->positionType() == PositionType::Relative ? m_contents.back()->getMargin().bottom : 0);
		}
		// インナーのスクロール最小値を更新する（上に移動させる長さが変わりうるので）
		m_inner.minY = m_currentMinimumScroll();
	}

	void Scrollable::m_updateContents()
	{
		if (not m_contents.empty())
		{
			for (auto itr = m_contents.begin(); itr != m_contents.end(); ++itr)
			{
				if (const auto &current = *itr;
					current->positionType() == PositionType::Relative)
				{
					if (itr == (m_contents.end() - 1))
					{
						const auto& prev = *std::prev(itr);
						Print << current->getInitialPosition().y;
						Print << Vec2{ current->getRegion().x, prev->getRegion().bottomY() + prev->getMargin().bottom + current->getMargin().top }.y;
					}
					if (itr == m_contents.begin())
					{
						current->setPosition({ current->getRegion().x, current->getMargin().top });
					}
					else
					{
						const auto &prev = *std::prev(itr);
						current->setPosition({ current->getRegion().x, prev->getRegion().bottomY() + prev->getMargin().bottom + current->getMargin().top });

						if (itr == (m_contents.end() - 1))
						{
							Print << current->getInitialPosition().y;
							Print << Vec2{ current->getRegion().x, prev->getRegion().bottomY() + prev->getMargin().bottom + current->getMargin().top }.y;
						}
					}
				}
			}
		}
	}

	Scrollable &Scrollable::setRegion(const RectF &viewport)
	{
		m_region = viewport;

		// コンストラクタと同じ式を使ってスクロールバーの位置を調整
		m_bar.region.setPos({ Arg::topRight(Vec2{ viewport.size.x - BarSize.x, BarSize.x }) });
		// 最大値だけ変更する（下に移動させる長さが変わりうるので）
		m_bar.maxY = viewport.size.y - BarSize.y - BarSize.x;

		m_updateContents();

		m_updateInner();

		return *this;
	}

	void Scrollable::update()
	{
		// # 各タイプ固有処理
		if (not m_contents.empty())
		{
			for (const auto& content : m_contents)
			{
				// 外部からの指定が億劫だったのでこのクラス内で勝手に
				// ビューポート幅を参照して大きさ調整することにした
				switch (content->typeID())
				{
					case RelocatableTypeID::TextBox:
					{
						//
						if (static_cast<TextBox*>(content.get())->adjustWidth(m_region.w - BarSize.x * 2))
						{
							m_updateInner();
						}
					}
				}
			}
		}

		// 以降の処理はスクロールの必要がないなら何もしない
		if (not m_shouldScroll())
		{
			
			return;
		}

		// 中身にマウスカーソルがあるとき
		if (m_region.mouseOver())
		{
			// マウスホイールの回転に合わせてスクロール
			m_scroll(m_inner, Mouse::Wheel() * -30).m_scrollSync(m_bar);
		}

		// スクロールバーの当たり判定を広げておく
		// TODO: なぜかスクロールバーが移動後に変なところに行く、意味が分からない、スクロールバーで動かせないじゃん
		m_isHoverBar = m_bar.region.stretched(5 * m_bar.region.w, m_bar.region.h)
								   .movedBy(2 * m_bar.region.w, m_bar.region.h / 4)
								   .mouseOver();

		// スクロールバー付近にマウスがあれば
		if (m_isHoverBar)
		{
			Cursor::RequestStyle(CursorStyle::Hand);

			if (MouseL.pressed())
			{
				// 左クリックをホールドしながら上下させるとスクロールする
				m_scroll(m_bar, Cursor::DeltaF().y).m_scrollSync(m_inner);
			}
		}
	}

	void Scrollable::draw() const
	{
		// 参考
		// https://siv3d.github.io/ja-jp/tutorial3/2d-render-state/?h=viewpo#4813-%E3%83%93%E3%83%A5%E3%83%BC%E3%83%9D%E3%83%BC%E3%83%88

		{
			const ScopedViewport2D viewport{ m_region.asRect() };

			if (not m_contents.empty())
			{
				for (const auto& content : m_contents)
				{
					content->draw();
					content->getRegion().drawFrame(2, Palette::Black);
				}
			}
			
			if (m_shouldScroll())
			{
				m_bar.region.rounded(2)
							.drawShadow(Vec2::Zero(), 2, 1, Palette::Gray)
							.draw(m_isHoverBar ? Palette::Lightgray : Palette::White);
			}
		}

		// TODO: m_region を おしゃれな感じで drawFrame したくはある
	}
}
