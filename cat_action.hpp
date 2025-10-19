#pragma once

namespace cact
{
	/// @brief Siv3D のイージング関数の型
	using EasingFunction = std::function<double(double)>;

	namespace cross
	{
		using _0 = std::tuple<Duration, uint32>;
		using _1 = std::tuple<Duration>;

		using TSignatures = std::variant<_0, _1>;
		constexpr size_t Count = std::variant_size_v<TSignatures>;
	}

	namespace appear
	{
		using _0 = std::tuple<Duration, EasingFunction, Duration, EasingFunction, Duration, Rect>;
		using _1 = std::tuple<Duration, Duration, Duration, Rect>;
		using _2 = std::tuple<Duration, EasingFunction, Duration, Rect>;
		using _3 = std::tuple<Duration, Duration, Rect>;
		using _4 = std::tuple<Duration, EasingFunction, Duration, EasingFunction, Duration>;
		using _5 = std::tuple<Duration, Duration, Duration>;
		using _6 = std::tuple<Duration, EasingFunction, Duration>;
		using _7 = std::tuple<Duration, Duration>;

		using TSignatures = std::variant<_0, _1, _2, _3, _4, _5, _6, _7>;
		constexpr size_t Count = std::variant_size_v<TSignatures>;
	}

	namespace appearFromEdge
	{
		using _0 = std::tuple<Duration, EasingFunction, Duration, EasingFunction, Duration, std::array<double, 4>>;
		using _1 = std::tuple<Duration, Duration, Duration, std::array<double, 4>>;
		using _2 = std::tuple<Duration, EasingFunction, Duration, std::array<double, 4>>;
		using _3 = std::tuple<Duration, Duration, std::array<double, 4>>;

		using TSignatures = std::variant<_0, _1, _2, _3>;
		constexpr size_t Count = std::variant_size_v<TSignatures>;
	}

	using Generic = std::variant<
		std::monostate,
		cross::_0, cross::_1,
		appear::_0, appear::_1, appear::_2, appear::_3, appear::_4, appear::_5, appear::_6, appear::_7,
		appearFromEdge::_0, appearFromEdge::_1, appearFromEdge::_2, appearFromEdge::_3>;

}
