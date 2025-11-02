# pragma once
# include "Common.hpp"

namespace UFOCat
{
	class Wanted : public App::Scene
	{
	private:

		std::unique_ptr<CatObject> m_target = nullptr;

		struct
		{
			Component::GUI::ProgressBar levelBar;

			Texture flyer;
		}
		m_gui;

	public:

		Wanted(const InitData &init);

		void update() override;

		void draw() const override;

	};
}
