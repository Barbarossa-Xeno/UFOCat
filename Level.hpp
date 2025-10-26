# pragma once
# include "Common.hpp"
# include "DeltaStopwatch.hpp"

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

		/// @brief スポーン間隔の計測やシーン内ステートの遷移などに使う内部ストップウォッチ
		DeltaStopwatch m_watch;

		/// @brief このレベルでのスコア
		Score m_score;

		/// @brief 捕まえた猫のオブジェクトへのポインタへのポインタ
		/// @remarks 二重ポインタにしているのは、`spawns` からとってくるため `spawns`は unique_ptr で管理されている 
		const std::unique_ptr<CatObject> *m_caught = nullptr;

		/// @brief レベル終わりに自分の捕まえた猫やターゲットを表示する際の倍率
		constexpr static double m_CatImageScale = 0.4;

		struct
		{
			Component::Button toResult;
		} m_gui;

		/* -- ゲッター / セッター -- */

		/// @brief 現在のフェーズ (非 const)
		/// @return 現在の `Phase` の参照
		/// @remarks ゲッターとか言いながらオブジェクトの変更も許している（？）
		LevelData &m_currentPhase();

		/// @brief ターゲットが出現する時間を引数に応じてランダムに決め、`m_targetAppearTime` に設定する
		/// @param level 整数値（1 ~ 10 の範囲で、特に現在のフェーズレベル (`m_currentPhaseIndex`) を入れることを想定）
		void m_setTargetAppearTime(size_t level);

	public:
		Level(const InitData &init);

		void update() override;

		void draw() const override;
	};
}
