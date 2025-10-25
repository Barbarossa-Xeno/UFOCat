# pragma once
# include "Common.hpp"
# include "delta_stopwatch.hpp"

namespace UFOCat
{
	class Level : public App::Scene
	{

		enum class State
		{
			Before,
			Playing,
			Finish,
			After
		};

		/* -- フィールド -- */

		std::shared_ptr<CatObject> m_target = nullptr;

		Array<std::shared_ptr<CatObject>> m_selections;

		/// @brief フェーズ中にターゲットが出現する時刻
		Duration m_targetAppearTime;

		/// @brief ターゲットが出現したかどうか
		bool m_appearedTarget = false;

		/// @brief 現在のフェーズで起こせるアクションの確率分布
		DiscreteDistribution m_actionProbabilities;

		/// @brief 
		Level::State m_state = Level::State::Before;

		/// @brief シーン内ステートの遷移などに使う内部ストップウォッチ
		DeltaStopwatch m_watch;

		/// @brief 猫を捕まえた（タッチした）か
		bool m_isCaught = false;

		/// @brief 捕まえた猫がターゲットと合っていたか
		bool m_isCorrect = false;

		/* -- ゲッター / セッター -- */

		/// @brief 現在のフェーズ (非 const)
		/// @return 現在の `Phase` の参照
		/// @remarks ゲッターとか言いながらオブジェクトの変更も許している（？）
		Phase &m_currentPhase();

		/// @brief ターゲットが出現する時間を引数に応じてランダムに決め、`m_targetAppearTime` に設定する
		/// @param level 整数値（1 ~ 10 の範囲で、特に現在のフェーズレベル (`m_currentPhaseIndex`) を入れることを想定）
		void m_setTargetAppearTime(size_t level);

	public:
		Level(const InitData &init);

		void update() override;

		void draw() const override;
	};
}
