# pragma once
# include "../Common.hpp"
# include "../Util/Stopwatch.hpp"

namespace UFOCat
{
	class Result : public App::Scene
	{
		/// @brief カウントアップさせるスコア
		size_t m_ScoreCount = 0;

		/// @brief カウントアップに応じて変わる称号データを保持する
		Score::Title m_currentTitle = Score::Titles[0];

		/// @brief カウントアップさせるときの加速度 随時加算
		double m_countUpAcceleration = 1.0;

		/// @brief カウントアップするためのストップウォッチ
		Util::Stopwatch m_scoreCountUpWatch;

		bool m_isFinishedCountUp = false;

		struct
		{
			/// @brief タイトルに行くボタン
			GUI::Button toTitle;

			/// @brief スコアに比例して伸びる称号ゲージ
			GUI::ProgressBar scoreTitleGauge;

			/// @brief スコア計算の詳細を表示するウィンドウ
			GUI::MessageBox scoreDetails;

			/// @brief スコア詳細を開くボタン
			GUI::Button scoreDetailsButton;
		}
		m_gui;

		/// @brief 
		/// @return 
		Score::LevelRecord& m_currentScoreData() const;

		// TODO: m_currentScoreDatas() は2個目 親クラスでの共通化を考える
		Array<Score::LevelRecord> &m_currentScoreDatas() const;

	public:
		Result(const InitData &init);

		~Result();

		void update() override;

		void draw() const override;
	};
}
