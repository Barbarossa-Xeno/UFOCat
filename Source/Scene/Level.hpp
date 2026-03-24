# pragma once
# include "../Common.hpp"
# include "../Util/Stopwatch.hpp"

namespace UFOCat
{
	/// @brief レベルシーン
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
		std::shared_ptr<CatData> m_target = nullptr;

		/// @brief このレベルで出現させる猫を絞り込んだリスト
		Array<std::shared_ptr<CatData>> m_selections;

		/// @brief このレベルで出現させる猫のIDを重複を無視して絞り込んだリスト
		HashSet<size_t> m_selectionsId;

		/// @brief フェーズ中にターゲットが出現する時刻 @n
		/// 途中、初めてターゲットが視認できるようになったことを確認できた時点で、数値を入れ替える！！ @n
		/// （スポーン時刻 -> 視認可能時刻 へ切り替わる）
		Duration m_targetAppearTime;

		/// @brief 初めてターゲットが画面上に見えた（見えるようになった）かどうか
		bool m_targetFirstVisible = false;

		/// @brief 現在のフェーズで起こせるアクションの確率分布
		DiscreteDistribution m_actionProbabilities;

		/// @brief シーン内ステート
		State m_state = State::Before;

		/// @brief スポーン間隔の計測やシーン内ステートの遷移などに使う内部ストップウォッチ
		Util::Stopwatch m_watch;

		/// @brief カウントダウンの時に使う、1フレーム前の timer.s() を保存しておく変数
		/// はじめのカウントダウン時間の設定にも使う
		/// @note timer.s() は常に整数秒を返すため、その数が切り替わった瞬間をとることで、1 秒ごとの時間経過を明確に取得できる
		int32 m_prevTimerRemaining = 4;

		/// @brief このレベルでのスコア
		Score::LevelRecord m_score;

		/// @brief 捕まえた猫のオブジェクトへのポインタへのポインタ
		/// @remarks 二重ポインタにしているのは、`spawns` からとってくるから
		/// （`spawns`は unique_ptr で管理されているので、それが示している猫のインスタンスは参照できない）
		const std::unique_ptr<CatObject> *m_caught = nullptr;

		/// @brief レベル終わりに自分の捕まえた猫やターゲットを表示する際の倍率
		constexpr static double m_CatTextureScale = 0.4;

		/// @brief GUI要素
		struct
		{
			/// @brief 結果シーンに進むボタン
			GUI::Button toResult;

			/// @brief 次のレベルに進むボタン
			/// 状況に応じて無効になる
			GUI::Button toNextLevel;

			/// @brief 次のレベルに行けるのにやめようとしたときに警告するダイアログ
			GUI::Dialog quitAlert;

			/// @brief タイマーの画像
			Texture timer;
		} m_gui;

		/// @brief 背景データ
		Core::BackgroundData m_bg;

		/* -- プロパティ -- */

		/// @brief 現在のレベル (非 const)
		/// @return 共有データにある現在の `LevelData` の参照
		LevelData &m_currentLevel() const;

		/// @brief 現在の1ゲームにおいて、各レベルのスコア記録
		/// @return 共有データにある`LevelRecord` のリストへの参照
		Array<Score::LevelRecord> &m_currentRecords() const;

		/// @brief ターゲットが出現する時間を引数に応じてランダムに決め、`m_targetAppearTime` に設定する
		/// @param level 整数値（1 ~ 10 の範囲で、特に現在のレベル値を入れることを想定）
		/// @remarks ただし、この値はあくまでスポーンのタイミングで使われるだけで、その時点ですぐにターゲットが視認できるとは限らない -> `m_targetFirstVisible`
		void m_setTargetSpawnTime(size_t level);

		/// @brief ターゲットが出現したかどうか
		/// @return 既に出現させていたら `true`
		bool m_hasAppearedTarget() const;

		/// @brief 次のレベルに進めるかどうか
		/// @return 進めるなら `true`
		bool m_isAvailableNextLevel() const;

	public:
		Level(const InitData &init);

		~Level();

		void update() override;

		void draw() const override;
	};
}
