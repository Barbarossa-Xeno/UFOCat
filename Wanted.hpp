# pragma once
# include "Common.hpp"

namespace UFOCat
{
	class Wanted : public App::Scene
	{
	private:

		std::unique_ptr<CatObject> m_target = nullptr;

	public:

		Wanted(const InitData &init);

		void update() override;

		void draw() const override;

	};
}
