# pragma once
# include "GUI.hpp"
# include "Core/Score.hpp"
# include "Core/CatObject.hpp"
# include "Core/LevelData.hpp"

using namespace UFOCat::Core;

namespace UFOCat
{
	namespace Core
	{
		/// @brief 背景画像のデータ
		struct BackgroundData
		{
			/// @brief テクスチャデータ
			Texture texture;

			/// @brief この背景の上に描画するものに対して使用するとちょうどよくなる影の色
			ColorF shadowColor;
		};

		/// @brief ゲーム全体で共有するデータ
		struct GameData
		{
			/// @brief 使用する全てのUFO猫のデータ
			Array<std::shared_ptr<CatData>> cats;

			/// @brief 使用する全てのレベルデータ
			Array<LevelData> levels;

			/// @brief スポーンしている猫のリスト
			Array<std::unique_ptr<CatObject>> spawns;

			/// @brief アプリを起動してから終えるまで集計するスコアのリスト
			// TODO: 直近のスコアデータを残そうと思えば、辞書で管理するか ResultRecord にタイムスタンプなどを入れる
			Array<Score::ResultRecord> scores;

			/// @brief 使用する全ての背景画像のテクスチャとその上から猫を描画するときに使う影の色の組み合わせを記録するリスト
			Array<BackgroundData> backgrounds;

			/// @brief 現在BGMとして再生しているオーディオの名前（1 つのみ）
			String bgmName;

			/// @brief 現在のターゲットの ID（= インデックスと同義）を格納する変数
			size_t targetId;

			/// @brief 現在のレベルのインデックスを格納する変数
			size_t levelIndex;

			/// @brief グローバルタイマー @n 色んな場所で使いまわす
			Timer timer;
		};
	}

	enum class SceneState
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

	/// @brief マウスカーソルの周囲を明るくする（画面幅 or 高さの大きいほうの 2% 分の半径）
	void BrightenCursor();

	/// @brief 水玉模様の背景を描画する
	/// @param cellSize セルのサイズ
	/// @param circleScale 円のスケール
	/// @param color 色
	/// @note https://siv3d.github.io/ja-jp/reference/game_tips/
	void DrawPolkaDotBackground(int32 cellSize, double circleScale, const ColorF &color);

	/// @brief アセット登録された猫の名前を ID から取得する
	/// @param id UFO猫の ID
	/// @return 名前 TextureAsset で使える
	String Cat(size_t id);

	/// @brief UFO猫のデータをJSONから読み込んでそれら全てのインスタンスを作成する
	/// @return 全てのUFO猫のインスタンスリスト
	Array<CatData> LoadCatData();

	/// @brief 各フェーズのデータをJSONから読み込んでそれらすべてのインスタンスを作成する
	/// @return 全てのフェーズのリスト
	Array<LevelData> LoadLevelData();

	/// @brief 使用する背景画像を読み込んでそれら全てのテクスチャと影の色のペアを作成する
	/// @return 全ての背景画像のテクスチャと使用する影の色のペアのリスト
	Array<BackgroundData> LoadBackgrounds();

	using App = SceneManager<SceneState, GameData>;
}
