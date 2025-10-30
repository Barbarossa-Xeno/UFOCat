# pragma once
# include "Common.hpp"

namespace UFOCat
{
	class Title : public App::Scene
	{
		Component::Dialog dialog;
	public:
		Title(const InitData &init);

		void update() override;

		void draw() const override;
	};
}
