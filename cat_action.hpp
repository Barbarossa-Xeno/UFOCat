#pragma once

/// @brief UFO猫がとるアクションについて定義した名前空間
/// @details cat_action の略で cact
namespace cact
{
	/// @brief Siv3D のイージング関数の型
	using EasingFunction = std::function<double(double)>;

	/// @brief `CatObject::cross()` のシグネチャなど
	namespace cross
	{
		
		using _0 = std::tuple<Duration, uint32>;
		using _1 = std::tuple<Duration>;

		using TSignatures = std::variant<_0, _1>;

		constexpr size_t Count = std::variant_size_v<TSignatures>;

		template <typename T>
		concept ValidSignature =
			(
				std::same_as<T, _0> ||
				std::same_as<T, _1>
			);
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

		template <typename T>
		concept ValidSignature =
			(
				std::same_as<T, _0> ||
				std::same_as<T, _1> ||
				std::same_as<T, _2> ||
				std::same_as<T, _3> ||
				std::same_as<T, _4> ||
				std::same_as<T, _5> ||
				std::same_as<T, _6> ||
				std::same_as<T, _7>
			);
	}

	namespace appearFromEdge
	{
		using _0 = std::tuple<Duration, EasingFunction, Duration, EasingFunction, Duration, std::array<double, 4>>;
		using _1 = std::tuple<Duration, Duration, Duration, std::array<double, 4>>;
		using _2 = std::tuple<Duration, EasingFunction, Duration, std::array<double, 4>>;
		using _3 = std::tuple<Duration, Duration, std::array<double, 4>>;

		using TSignatures = std::variant<_0, _1, _2, _3>;

		constexpr size_t Count = std::variant_size_v<TSignatures>;

		template <typename T>
		concept ValidSignature =
			(
				std::same_as<T, _0> ||
				std::same_as<T, _1> ||
				std::same_as<T, _2> ||
				std::same_as<T, _3>
			);
	}

	using Generic = std::variant<
		std::monostate,
		cross::_0, cross::_1,
		appear::_0, appear::_1, appear::_2, appear::_3, appear::_4, appear::_5, appear::_6, appear::_7,
		appearFromEdge::_0, appearFromEdge::_1, appearFromEdge::_2, appearFromEdge::_3>;

	template <typename T>
	concept ValidSignature =
	(
		std::same_as<T, std::monostate> ||
		cact::cross::ValidSignature<T> ||
		cact::appear::ValidSignature<T> ||
		cact::appearFromEdge::ValidSignature<T>
	);
}
