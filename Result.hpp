# pragma once
# include "Common.hpp"
# include "DeltaStopwatch.hpp"

namespace UFOCat
{
	class Result : public App::Scene
	{
		size_t tempScore = 0;

		DeltaStopwatch m_scoreCountUpWatch;

		struct
		{
			Component::Button toTitle;
		} m_gui{ };

		const Score &m_getScore(size_t index) const;

		const Score &m_currentScore() const;

	public:
		Result(const InitData &init);

		void update() override;

		void draw() const override;
	};
}
