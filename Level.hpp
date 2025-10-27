# pragma once
# include "Common.hpp"
# include "DeltaStopwatch.hpp"

namespace UFOCat
{
	class Level : public App::Scene
	{
		/// @brief レベルシーン内のステート
		enum class State
		{
			/// @brief プレイ前（初期化フェーズ）
			Before,
			/// @brief プレイ中
			Playing,
			/// @brief タッチや制限時間で終わったとき
			Finish,
			/// @brief 結果表示
			After
		};

		/* -- フィールド -- */

		/// @brief ターゲット
		std::shared_ptr<CatObject> m_target = nullptr;

		/// @brief このレベルで出現させる猫を絞り込んだリスト
		Array<std::shared_ptr<CatObject>> m_selections;

		/// @brief フェーズ中にターゲットが出現する時刻 @n
		/// 途中、初めてターゲットが視認できるようになったことを確認できた時点で、数値を入れ替える！！ @n
		/// （スポーン時刻 -> 視認可能時刻 へ切り替わる）
		Duration m_targetAppearTime;

		/// @brief 初めてターゲットが画面上に見えた（見えるようになった）かどうか
		bool m_targetFirstVisible = false;

		/// @brief 現在のフェーズで起こせるアクションの確率分布
		DiscreteDistribution m_actionProbabilities;

		/// @brief シーン内ステート
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

		/// @brief GUI要素
		struct
		{
			/// @brief 結果シーンに進むボタン
			Component::Button toResult;
		} m_gui;

		/* -- ゲッター / セッター -- */

		/// @brief 現在のレベル (非 const)
		/// @return 現在の `LevelData` の参照
		/// @remarks ゲッターとか言いながらオブジェクトの変更も許している（？）
		LevelData &m_currentLevel() const;

		/// @brief ターゲットが出現する時間を引数に応じてランダムに決め、`m_targetAppearTime` に設定する
		/// @param level 整数値（1 ~ 10 の範囲で、特に現在のレベル値を入れることを想定）
		/// @remarks ただし、この値はあくまでスポーンのタイミングで使われるだけで、その時点ですぐにターゲットが視認できるとは限らない -> `m_targetFirstVisible`
		void m_setTargetSpawnTime(size_t level);

		/// @brief ターゲットが出現したかどうか
		/// @return 既に出現させていたら `true`
		bool m_appearedTarget() const;

	public:
		Level(const InitData &init);

		void update() override;

		void draw() const override;
	};
}
