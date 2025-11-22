# include "GUI.hpp"

namespace UFOCat::GUI
{
	ProgressBar::ProgressBar(const SizeF &size, ColorF color, double roundness, double progress)
		: m_color{ color }
		, m_roundness{ roundness }
		, m_progress{ progress }
	{
		m_region = RectF{ size };
	}

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
}
