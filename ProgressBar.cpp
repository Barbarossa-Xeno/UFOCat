# include "Component.hpp"

namespace UFOCat::Component
{
	ProgressBar::ProgressBar(const SizeF &size, double roundness, double progress)
		: m_region{ size }
		, m_roundness{ roundness }
		, m_progress{ progress }
	{}

	ProgressBar &ProgressBar::setSize(const SizeF &size) const
	{
		if (not m_instance)
		{
			m_instance = std::make_unique<ProgressBar>(size, m_roundness, m_progress);
		}
		else
		{
			m_instance->m_region.size = size;
		}
		
		return *m_instance;
	}

	ProgressBar &ProgressBar::setProgress(double progress) const
	{
		if (not m_instance)
		{
			m_instance = std::make_unique<ProgressBar>(m_region.size, m_roundness, Clamp(progress, 0.0, 1.0));
		}
		else
		{
			m_instance->m_progress = Clamp(progress, 0.0, 1.0);
		}
		
		return *m_instance;
	}

	void ProgressBar::draw(const Vec2 &position, const SizeF &barScale) const
	{
		m_region.movedBy(position).rounded(m_roundness).draw();

		RoundRect bar = m_region.movedBy(position).scaled(barScale).rounded(0.75 * m_roundness);

		bar.drawFrame(2, Palette::Dimgray);

		const RoundRect full = bar.stretched(-2);
		RoundRect{ bar.rect.pos, SizeF{ 0.0, bar.h }, bar.r }.lerp(full, m_progress).draw(Palette::Bisque);
	}

	void ProgressBar::draw(const Arg::topCenter_<Vec2> &position, const SizeF &barScale) const
	{
		double x = position->x - m_region.w / 2;
		draw(Vec2{ x, position->y }, barScale);
	}

	void ProgressBar::draw(const Arg::topRight_<Vec2> &position, const SizeF &barScale) const
	{
		double x = position->x - m_region.w;
		draw(Vec2{ x, position->y }, barScale);
	}

	void ProgressBar::draw(const Arg::leftCenter_<Vec2> &position, const SizeF &barScale) const
	{
		double y = position->y - m_region.h / 2;
		draw(Vec2{ position->x, y }, barScale);
	}

	void ProgressBar::draw(const Arg::rightCenter_<Vec2> &position, const SizeF &barScale) const
	{
		double x = position->x - m_region.w;
		double y = position->y - m_region.h / 2;
		draw(Vec2{ x, y }, barScale);
	}

	void ProgressBar::draw(const Arg::bottomLeft_<Vec2> &position, const SizeF &barScale) const
	{
		double y = position->y - m_region.h;
		draw(Vec2{ position->x, y }, barScale);
	}

	void ProgressBar::draw(const Arg::bottomCenter_<Vec2> &position, const SizeF &barScale) const
	{
		double x = position->x - m_region.w / 2;
		double y = position->y - m_region.h;
		draw(Vec2{ x, y }, barScale);
	}

	void ProgressBar::draw(const Arg::bottomRight_<Vec2> &position, const SizeF &barScale) const
	{
		double x = position->x - m_region.w;
		double y = position->y - m_region.h;
		draw(Vec2{ x, y }, barScale);
	}

	void ProgressBar::drawAt(const Vec2 &position, const SizeF &barScale) const
	{
		double x = position.x - m_region.w / 2;
		double y = position.y - m_region.h / 2;
		draw(Vec2{ x, y }, barScale);
	}
}
