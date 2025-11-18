#pragma once

/// @brief UFO猫がとるアクションについて定義した名前空間
namespace UFOCat::Action
{
	/// @brief Siv3D のイージング関数の型
	using EasingFunction = std::function<double(double)>;

	/// @brief `CatObject::cross()` のシグネチャなど
	namespace Cross
	{
		/// @brief `CatObject::cross(Duration period, uint32 count)` を表すタプル
		using _0 = std::tuple<Duration, uint32>;
		/// @brief `CatObject::cross(Duration period)` を表すタプル
		using _1 = std::tuple<Duration>;

		/// @brief `CatObject::cross()` の引数シグネチャの全てのタプルを含む variant
		using TSignatures = std::variant<_0, _1>;

		/// @brief `TSignatures` が含む型の数
		constexpr size_t Count = std::variant_size_v<TSignatures>;

		/// @brief `CatObject::cross()` の有効な引数シグネチャの条件
		/// @tparam 登録されたシグネチャとの比較対象
		template <typename T>
		concept ValidSignature =
			(
				std::same_as<T, _0> or
				std::same_as<T, _1>
			);
	}

	/// @brief `CatObject::appear()` のシグネチャなど
	namespace Appear
	{
		/// @brief `CatObject::appear(Duration period, EasingFunction fadeInFunc, Duration fadeIn, EasingFunction fadeOutFunc, Duration fadeOut, const Rect &range)` を表すタプル
		using _0 = std::tuple<Duration, EasingFunction, Duration, EasingFunction, Duration, Rect>;
		/// @brief `CatObject::appear(Duration period, Duration fadeIn, Duration fadeOut, const Rect &range)` を表すタプル
		using _1 = std::tuple<Duration, Duration, Duration, Rect>;
		/// @brief `CatObject::appear(Duration period, EasingFunction fadeFunc, Duration fade, const Rect &range)` を表すタプル
		using _2 = std::tuple<Duration, EasingFunction, Duration, Rect>;
		/// @brief `CatObject::appear(Duration period, Duration fade, const Rect &range)` を表すタプル
		using _3 = std::tuple<Duration, Duration, Rect>;
		/// @brief `CatObject::appear(Duration period, EasingFunction fadeInFunc, Duration fadeIn, EasingFunction fadeOutFunc, Duration fadeOut)` を表すタプル
		using _4 = std::tuple<Duration, EasingFunction, Duration, EasingFunction, Duration>;
		/// @brief `CatObject::appear(Duration period, Duration fadeIn, Duration fadeOut)` を表すタプル
		using _5 = std::tuple<Duration, Duration, Duration>;
		/// @brief `CatObject::appear(Duration period, EasingFunction fadeFunc, Duration fade)` を表すタプル
		using _6 = std::tuple<Duration, EasingFunction, Duration>;
		/// @brief `CatObject::appear(Duration period, Duration fade)` を表すタプル
		using _7 = std::tuple<Duration, Duration>;

		/// @brief `CatObject::appear()` の引数シグネチャの全てのタプルを含む variant
		using TSignatures = std::variant<_0, _1, _2, _3, _4, _5, _6, _7>;

		/// @brief `TSignatures` が含む型の数
		constexpr size_t Count = std::variant_size_v<TSignatures>;

		/// @brief `CatObject::appear()` の有効な引数シグネチャの条件
		/// @tparam 登録されたシグネチャとの比較対象
		template <typename T>
		concept ValidSignature =
			(
				std::same_as<T, _0> or
				std::same_as<T, _1> or
				std::same_as<T, _2> or
				std::same_as<T, _3> or
				std::same_as<T, _4> or
				std::same_as<T, _5> or
				std::same_as<T, _6> or
				std::same_as<T, _7>
			);
	}

	/// @brief /// @brief `CatObject::appearFromEdge()` のシグネチャなど
	namespace AppearFromEdge
	{
		/// @brief `CatObject::appearFromEdge(Duration period, EasingFunction inFunc, Duration in, EasingFunction outFunc, Duration out, const std::array<double, 4> &overflow)` を表すタプル
		using _0 = std::tuple<Duration, EasingFunction, Duration, EasingFunction, Duration, std::array<double, 4>>;
		/// @brief `CatObject::appearFromEdge(Duration period, Duration in, Duration out, const std::array<double, 4> &overflow)` を表すタプル
		using _1 = std::tuple<Duration, Duration, Duration, std::array<double, 4>>;
		/// @brief `CatObject::appearFromEdge(Duration period, EasingFunction inAndOutFunc, Duration inAndOut, const std::array<double, 4> &overflow)` を表すタプル
		using _2 = std::tuple<Duration, EasingFunction, Duration, std::array<double, 4>>;
		/// @brief `CatObject& appearFromEdge(Duration period, Duration inAndOut, const std::array<double, 4> &overflow)` を表すタプル
		using _3 = std::tuple<Duration, Duration, std::array<double, 4>>;

		/// @brief `CatObject::appearFromEdge()` の引数シグネチャの全てのタプルを含む variant
		using TSignatures = std::variant<_0, _1, _2, _3>;

		/// @brief `TSignatures` が含む型の数
		constexpr size_t Count = std::variant_size_v<TSignatures>;

		/// @brief `CatObject::appearFromEdge()` の有効な引数シグネチャの条件
		/// @tparam 登録されたシグネチャとの比較対象
		template <typename T>
		concept ValidSignature =
			(
				std::same_as<T, _0> or
				std::same_as<T, _1> or
				std::same_as<T, _2> or
				std::same_as<T, _3>
			);
	}

	/// @brief `CatObject` 内全ての行動系メソッドのシグネチャ（猫を行動させるパラメータ）が入る万能型
	using Generic = std::variant<
		std::monostate,
		Cross::_0, Cross::_1,
		Appear::_0, Appear::_1, Appear::_2, Appear::_3, Appear::_4, Appear::_5, Appear::_6, Appear::_7,
		AppearFromEdge::_0, AppearFromEdge::_1, AppearFromEdge::_2, AppearFromEdge::_3>;

	/// @brief `CatObject` 内いずれかの行動系メソッドに有効なシグネチャの条件
	/// @tparam 有効なシグネチャとの比較対象を入れる
	template <typename T>
	concept ValidSignature =
	(
		std::same_as<T, std::monostate> or
		Action::Cross::ValidSignature<T> or
		Action::Appear::ValidSignature<T> or
		Action::AppearFromEdge::ValidSignature<T>
	);
}
