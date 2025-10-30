# include "GUI.hpp"

namespace UFOCat::Component::GUI
{
	ProgressBar::ProgressBar(const SizeF &size, const SizeF &barScale, double roundness, double progress)
		: m_region{ size }
		, m_barScale{ barScale }
		, m_roundness{ roundness }
		, m_progress{ progress }
	{}

	ProgressBar &ProgressBar::set(const SizeF &size, const SizeF &barScale, double roundness)
	{
		m_region = RectF{ size };
		m_barScale = barScale;
		m_roundness = roundness;
		return *this;
	}

	ProgressBar &ProgressBar::setProgress(double progress)
	{
		m_progress = progress;
		return *this;
	}

	void ProgressBar::draw() const
	{
		// 角丸四角形を描画
		m_region.rounded(m_roundness).draw();

		// バーの大きさを定義する
		RoundRect bar = m_region.scaled(m_barScale).rounded(0.75 * m_roundness);

		// バーの枠
		bar.drawFrame(2, Palette::Dimgray);

		// バーが 100% (t = 1.0) の長さの状態
		const RoundRect full = bar.stretched(-2);

		// 線形補間
		RoundRect{ bar.rect.pos, SizeF{ 0.0, bar.h }, bar.r }.lerp(full, m_progress).draw(Palette::Bisque);
	}

	ProgressBar &ProgressBar::setPosition(const Vec2 &position)
	{
		m_region.setPos(position);
		return *this;
	}

	ProgressBar &ProgressBar::setPosition(const Arg::topCenter_<Vec2> &position)
	{
		m_region.setPos(position);
		return *this;
	}

	ProgressBar &ProgressBar::setPosition(const Arg::topRight_<Vec2> &position)
	{
		m_region.setPos(position);
		return *this;
	}

	ProgressBar &ProgressBar::setPosition(const Arg::leftCenter_<Vec2> &position)
	{
		m_region.setPos(position);
		return *this;
	}

	ProgressBar &ProgressBar::setPosition(const Arg::rightCenter_<Vec2> &position)
	{
		m_region.setPos(position);
		return *this;
	}

	ProgressBar &ProgressBar::setPosition(const Arg::bottomLeft_<Vec2> &position)
	{
		m_region.setPos(position);
		return *this;
	}

	ProgressBar &ProgressBar::setPosition(const Arg::bottomCenter_<Vec2> &position)
	{
		m_region.setPos(position);
		return *this;
	}

	ProgressBar &ProgressBar::setPosition(const Arg::bottomRight_<Vec2> &position)
	{
		m_region.setPos(position);
		return *this;
	}

	ProgressBar &ProgressBar::setPositionAt(const Vec2 &position)
	{
		m_region.setPos(Arg::center(position));
		return *this;
	}
}
