# pragma once
# include "Common.hpp"
# include "DeltaStopwatch.hpp"

namespace UFOCat
{
	class Result : public App::Scene
	{
		/// @brief スコア表示のカウントアップに使う変数
		size_t m_scoreCount = 0;

		/// @brief カウントアップさせるときのスピード（倍率）
		double countUpSpeed = 1.0;

		/// @brief カウントアップするためのストップウォッチ
		DeltaStopwatch m_scoreCountUpWatch;

		struct
		{
			/// @brief タイトルに行くボタン
			Component::GUI::Button toTitle;

			/// @brief スコアに比例して伸びる称号ゲージ
			Component::GUI::ProgressBar scoreTitleGauge;
		}
		m_gui{};

		/// @brief 
		/// @return 
		ScoreData& m_currentScoreData() const;

		// TODO: m_currentScoreDatas() は2個目 親クラスでの共通化を考える
		Array<ScoreData> &m_currentScoreDatas() const;

	public:
		Result(const InitData &init);

		void update() override;

		void draw() const override;
	};
}
