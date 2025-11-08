# pragma once
# include "Common.hpp"

namespace UFOCat
{
	/// @brief ターゲットを知らせるシーン
	class Wanted : public App::Scene
	{
	private:

		/// @brief ターゲットの情報
		std::shared_ptr<CatObject> m_target = nullptr;

		/// @brief GUI 要素
		struct
		{
			/// @brief 現在のレベルを表現するバー
			Component::GUI::ProgressBar levelBar;

			/// @brief 「UFO猫を探しています」のチラシのテクスチャ
			Texture flyer;
		}
		m_gui;

	public:

		Wanted(const InitData &init);

		void update() override;

		void draw() const override;

	};
}
