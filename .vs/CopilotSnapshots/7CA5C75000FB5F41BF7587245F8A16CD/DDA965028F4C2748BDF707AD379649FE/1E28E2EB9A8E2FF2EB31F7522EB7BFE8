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
			GUI::Button toLevel;

			/// @brief 「あそび方」を開くボタン
			GUI::Button howToPlayButton;

			GUI::Button lisenceButton;

			/// @brief 「あそび方」のメッセージボックス
			GUI::MessageBox howToPlay;

			GUI::MessageBox lisence;

			/// @brief ロゴタイプ
			Texture logo;
		}
		m_gui;

		/// @brief 背景データ
		Util::BackgroundData m_bg;

	public:
		Title(const InitData &init);

		~Title();

		void update() override;

		void draw() const override;
	};
}
