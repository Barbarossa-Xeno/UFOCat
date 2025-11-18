# include "DeltaStopwatch.hpp"

namespace UFOCat::Util
{
	const double &DeltaStopwatch::now() const noexcept
	{
		return m_elapsedTime;
	}

	const double &DeltaStopwatch::forward() noexcept
	{
		m_elapsedTime += Scene::DeltaTime();
		return m_elapsedTime;
	}

	const double &DeltaStopwatch::resetTime(double time) noexcept
	{
		// もし m_elapsedTime が time とほぼ同じくらいならこの演算で 0 くらいになる
		// DeltaTime の誤差は考慮しない
		m_elapsedTime -= time;
		return m_elapsedTime;
	}

	const double &DeltaStopwatch::resetTime(const Duration &time) noexcept
	{
		return resetTime(time.count());
	}

	const double &DeltaStopwatch::resetTime() noexcept
	{
		m_elapsedTime = 0;
		return m_elapsedTime;
	}

	void DeltaStopwatch::resetOverCount() noexcept
	{
		m_overCount = 0;
	}

	void DeltaStopwatch::reset() noexcept
	{
		resetTime();
		resetOverCount();
	}

	bool DeltaStopwatch::isOver(double time, bool isAutoReset) noexcept
	{
		bool result = m_elapsedTime >= time;

		// 超えていたら、超過回数をカウント
		if (result)
		{
			++m_overCount;

			// かつ、自動リセットが true ならリセットしてカウントを戻す
			if (isAutoReset)
			{
				resetTime(time);
				m_overCount = 0;
			}
		}

		return result;
	}

	bool DeltaStopwatch::isOver(const Duration& time, bool isAutoReset) noexcept
	{
		return isOver(time.count(), isAutoReset);
	}
}

