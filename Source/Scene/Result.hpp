# pragma once
# include "../Common.hpp"
# include "../Util/Stopwatch.hpp"

namespace UFOCat
{
	class Result : public App::Scene
	{
		/// @brief 表示のためにカウントアップさせていくスコア
		size_t m_scoreCount = 0;

		/// @brief カウントアップに応じて変わる称号データを保持する
		Score::Title m_currentTitle = Score::Titles[0];

		/// @brief カウントアップするときの加速度 随時加算
		double m_countUpAcceleration = 1.0;

		/// @brief ある間隔でカウントアップするためのストップウォッチ
		Util::Stopwatch m_scoreCountUpWatch;

		/// @brief カウントアップが終わったか
		bool m_isFinishedCountUp = false;

		/// @brief GUI 要素
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

		/// @brief 現在の1ゲームにおいて、各レベルのスコア記録
		/// @return 共有データにある`LevelRecord` のリストへの参照
		Array<Score::LevelRecord> &m_currentRecords() const;

	public:

		Result(const InitData &init);

		~Result();

		void update() override;

		void draw() const override;
	};
}
