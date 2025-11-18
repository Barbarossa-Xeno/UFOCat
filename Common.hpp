# pragma once
# include "GUI.hpp"
# include "CatObject.hpp"
# include "LevelData.hpp"

using namespace UFOCat::Core;

namespace UFOCat
{
	/// @brief インデックスを保存する変数のための無効値 @n
	/// このゲームで登場するUFO猫の種類は最大で 44 種類であり、
	/// テクスチャを読み込む際も、レベルデータを読み込む際も、その数字を超えることはない @n
	/// そのため、この名前空間内では特別に無効値として扱う
	/// @note Optional でもよかったけど、いちいち value() ってするのが面倒だったけん
	constexpr size_t InvalidIndex = 44;

	enum class State
	{
		/// @brief タイトル画面
		Title,
		/// @brief 捕まえるUFO猫が発表される画面 レベルシーンの初期化処理に使う
		Wanted,
		/// @brief レベル画面
		Level,
		/// @brief 結果画面 リタイアするか、失敗したときに移行できる
		Result
	};

	/// @brief スコアから決められる「称号」のデータ
	struct ScoreTitleData
	{
		/// @brief 称号名（漢字）
		String kanjiName;

		/// @brief 称号名のフリガナ（ルビ）
		String ruby;

		/// @brief 称号間の閾値
		double threshold = 0.0;
	};

	/// @brief レベルごとに集計するスコアのデータ
	struct ScoreData
	{
		/// @brief プレイしたレベル (1 ~ )
		size_t level = InvalidIndex;

		/// @brief 猫を捕まえたか
		bool isCaught = false;

		/// @brief 捕まえた猫が正解だったか
		bool isCorrect = false;

		/// @brief ターゲットが出現してから捕まえるまでかかった時間 [s]
		double response = 0.0;

		/// @brief 連続正解数
		size_t consecutiveCorrect = 0;

		/// @brief 総合得点
		size_t total = 0;
		
		ScoreData();

		/// @brief コンストラクタ
		/// @param level レベル (1 ~ )
		/// @param isCaught 何かしら捕まえたか
		/// @param isCorrect 正解したか
		/// @param response 捕まえるのにかかった時間 [s]
		/// @param consecutiveCorrect ここまでの連続正解数
		ScoreData(size_t level, bool isCaught, bool isCorrect, double response, size_t consecutiveCorrect);

		/// @brief このレベルでの総合得点を計算する
		/// @return このレベルでの総合得点
		size_t calculateTotal();

		/// @brief このゲームでの理論上の最大スコアを返す
		/// @remarks アプリが起動している間、変更されないレベル数をもとに計算されるので、引数を省略可能とする @n
		/// この計算は、レベルデータがアプリが最初に起動したときにのみ読み込まれる保証を利用している
		/// @param levelCount レベル数 省略された場合はアプリ起動時に設定されたレベル数が使われる
		/// @return 理論値
		static size_t GetMaxTheoretical(size_t levelCount = 0);

		/// @brief この1ゲームで行われる最大レベル数を決める @n
		/// 単純に level_data.json で定義されたレベルの数と同じだが、
		/// レベルの数をもとに最大スコアの理論値が決まるため、ここで保持しておき、
		/// アプリが起動している間、変更を許さない
		/// @param count レベル数
		static void SetLevelCount(size_t count);
	};

	/// @brief 1ゲームをプレイしたとき全体のスコアのデータ
	/// @note これが総合スコアとして残る
	struct Score
	{
		/// @brief 各レベルのスコアデータ
		/// @remarks 途中でレベルをクリアできなかった場合は、そのレベル以降のデータは存在しない
		Array<ScoreData> scoreDataList;

		/// @brief 称号データ
		ScoreTitleData titleData;

		size_t total = 0;

		/// @brief この1ゲーム全体での総合得点（複数レベルの総合得点の合計）を計算する
		/// @return この1ゲームでの総合得点
		size_t calculateTotal();

		/// @brief 称号の定義リスト
		const static std::array<ScoreTitleData, 5> Titles;
	};

	struct GameData
	{
		/// @brief 使用する全てのUFO猫のデータ
		Array<CatObject> cats;

		/// @brief 使用する全てのレベルデータ
		Array<LevelData> levels;

		/// @brief スポーンしている猫のリスト
		Array<std::unique_ptr<CatObject>> spawns;

		/// @brief アプリを起動してから終えるまで集計するスコアのリスト
		Array<Score> scores;

		/// @brief 使用する全ての背景画像のテクスチャ
		Array<Texture> backgrounds;

		/// @brief 現在のターゲットのインデックスを格納する変数
		size_t targetIndex = InvalidIndex;

		/// @brief 現在のレベルのインデックスを格納する変数
		size_t levelIndex = InvalidIndex;

		/// @brief グローバルタイマー @n 色んな場所で使いまわす
		Timer timer;
	};
	// TODO: 直近のスコアデータを残そうと思えば、二次元Arrayのフィールドを追加してもよい

	/// @brief マウスカーソルの周囲を明るくする（画面幅 or 高さの大きいほうの 2% 分の半径）
	void BrightenCursor();

	/// @brief 水玉模様の背景を描画する
	/// @param cellSize セルのサイズ
	/// @param circleScale 円のスケール
	/// @param color 色
	/// @note https://siv3d.github.io/ja-jp/reference/game_tips/
	void DrawPolkaDotBackground(int32 cellSize, double circleScale, const ColorF& color);

	/// @brief UFO猫のデータをJSONから読み込んでそれら全てのインスタンスを作成する
	/// @return 全てのUFO猫のインスタンスリスト
	Array<CatObject> LoadCatData();

	/// @brief 各フェーズのデータをJSONから読み込んでそれらすべてのインスタンスを作成する
	/// @return 全てのフェーズのリスト
	Array<LevelData> LoadLevelData();

	/// @brief 使用する背景画像を読み込んでそれら全てのテクスチャを作成する
	/// @return 全ての背景画像のテクスチャリスト
	Array<Texture> LoadBackgrounds();

	using App = SceneManager<State, GameData>;
}
