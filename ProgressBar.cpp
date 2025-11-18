# include "GUI.hpp"

namespace UFOCat::GUI
{
	ProgressBar::ProgressBar(const SizeF &size, ColorF color, double roundness, double progress)
		: m_region{ size }
		, m_color{ color }
		, m_roundness{ roundness }
		, m_progress{ progress }
	{}

	RectF ProgressBar::getRegion() const
	{
		return m_region;
	}

	ProgressBar &ProgressBar::set(const SizeF &size, ColorF color, double roundness)
	{
		m_region = RectF{ size };
		m_color = color;
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
		// バーの大きさを定義する
		RoundRect bar = m_region.rounded(0.75 * m_roundness);

		// バーの枠
		bar.drawFrame(2, s3d::Palette::Dimgray);

		// バーが 100% (t = 1.0) の長さの状態
		const RoundRect full = bar.stretched(-2);

		// 線形補間
		RoundRect{ bar.rect.pos, SizeF{ 0.0, bar.h }, bar.r }.lerp(full, m_progress).draw(m_color);
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
