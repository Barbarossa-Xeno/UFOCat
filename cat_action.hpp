#pragma once
#include "cat_object.hpp"

namespace cact
{
	namespace cross
	{
		using _0 = std::tuple<Duration, uint32>;
		using _1 = std::tuple<Duration>;

		using TSignatures = std::variant<_0, _1>;
	}

	namespace appear
	{
		using _0 = std::tuple<Duration, CatObject::EasingFunction, Duration, CatObject::EasingFunction, Duration, Rect>;
		using _1 = std::tuple<Duration, Duration, Duration, Rect>;
		using _2 = std::tuple<Duration, CatObject::EasingFunction, Duration, Rect>;
		using _3 = std::tuple<Duration, Duration, Rect>;
		using _4 = std::tuple<Duration, CatObject::EasingFunction, Duration, CatObject::EasingFunction, Duration>;
		using _5 = std::tuple<Duration, Duration, Duration>;
		using _6 = std::tuple<Duration, CatObject::EasingFunction, Duration>;
		using _7 = std::tuple<Duration, Duration>;

		using TSignatures = std::variant<_0, _1, _2, _3, _4, _5, _6, _7>;
	}

	namespace appearFromEdge
	{
		using _0 = std::tuple<Duration, CatObject::EasingFunction, Duration, CatObject::EasingFunction, Duration, std::array<double, 4>>;
		using _1 = std::tuple<Duration, Duration, Duration, std::array<double, 4>>;
		using _2 = std::tuple<Duration, CatObject::EasingFunction, Duration, std::array<double, 4>>;
		using _3 = std::tuple<Duration, Duration, std::array<double, 4>>;

		using TSignatures = std::variant<_0, _1, _2, _3>;
	}
}
