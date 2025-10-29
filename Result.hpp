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

			Component::ProgressBar scoreBar;
		}
		m_gui{};

		ScoreData& m_currentScoreData() const;

		// TODO: m_currentScoreDatas() は2個目 親クラスでの共通化を考える
		Array<ScoreData> &m_currentScoreDatas() const;

	public:
		Result(const InitData &init);

		void update() override;

		void draw() const override;
	};
}
