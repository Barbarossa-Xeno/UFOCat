# pragma once
# include "Common.hpp"

namespace UFOCat
{
	/// @brief タイトルシーン
	class Title : public App::Scene
	{
		/// @brief GUI要素
		struct
		{
			/// @brief ゲームを始める（レベルシーンに遷移する）ボタン
			Component::GUI::Button toLevel;

			/// @brief 「あそび方」を開くボタン
			Component::GUI::Button howToPlayButton;

			/// @brief 「あそび方」のメッセージボックス
			Component::GUI::MessageBox howToPlay;

			/// @brief ロゴタイプ
			Texture logo;
		}
		m_gui;

		/// @brief 背景画像
		Texture m_background;

		/// @brief 「あそび方」に表示するテキスト
		const String m_howToPlayText =
			U"「UFO猫」をつかまえるゲームです！\n"
			U"\n"
			U"・あそび方\n"
			U"　画面に出てくる UFO猫 をタッチしてつかまえましょう。\n"
			U"　はじめにつかまえてほしい猫をお知らせするので、間違えないように注意！\n"
			U"\n"
			U"・「ターゲット」について\n"
			U"　レベル ごとにきめられたUFO猫を、間違えないようにつかまえましょう。\n"
			U"　ターゲット は レベル のはじめにお知らせします。\n"
			U"\n"
			U"・「レベル（★）」について\n"
			U"　レベル が上がると、UFO猫がすばしっこくなったり、\n"
			U"　ほかのUFO猫がたくさんあらわれたりして、だんだんむずかしくなります。\n"
			U"\n"
			U"・スコアについて\n"
			U"　ターゲットをつかまえれたり、つかまえるのが速かったりしたら、スコア が上がります。\n"
			U"　レベルが進んで スコア がたまると、すごい しょうごう がもらえます。高みをめざそう！\n";

	public:
		Title(const InitData &init);

		void update() override;

		void draw() const override;
	};
}
