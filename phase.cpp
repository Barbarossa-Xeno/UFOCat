#include "cat_action.hpp"
#include "phase.hpp"
#include <regex>
#include <span>

// const HashTable<String, Array<Phase::Signature>> Phase::m_actionInfo
// {
// 	// 奇遇にも JSON みたいな書き方になった
// 	{
// 		U"bound",
// 		{
// 			{ &typeid(void) }
// 		}
// 	},
// 	{
// 		U"cross",
// 		{
// 			{ &typeid(Duration), &typeid(uint32) },
// 			{ &typeid(Duration) }
// 		}
// 	},
// 	{
// 		U"appear",
// 		{
// 			{ &typeid(Duration), &typeid(EasingF), &typeid(Duration), &typeid(EasingF), &typeid(Duration), &typeid(Rect) },
// 			{ &typeid(Duration), &typeid(Duration), &typeid(Duration), &typeid(Rect) },
// 			{ &typeid(Duration), &typeid(EasingF), &typeid(Duration), &typeid(Rect) },
// 			{ &typeid(Duration), &typeid(Duration), &typeid(Rect) },
// 			{ &typeid(Duration), &typeid(EasingF), &typeid(Duration), &typeid(EasingF), &typeid(Duration) },
// 			{ &typeid(Duration), &typeid(Duration), &typeid(Duration) },
// 			{ &typeid(Duration), &typeid(EasingF), &typeid(Duration) },
// 			{ &typeid(Duration), &typeid(Duration) }
// 		}
// 	},
// 	{
// 		U"appearFromEdge",
// 		{
// 			{ &typeid(Duration), &typeid(EasingF), &typeid(Duration), &typeid(EasingF), &typeid(Duration), &typeid(std::array<double, 4>) },
// 			{ &typeid(Duration), &typeid(Duration), &typeid(Duration), &typeid(std::array<double, 4>) },
// 			{ &typeid(Duration), &typeid(EasingF), &typeid(Duration), &typeid(std::array<double, 4>) },
// 			{ &typeid(Duration), &typeid(Duration), &typeid(std::array<double, 4>) }
// 		}
// 	}
// };

bool Phase::updateAtInterval()
{
	m_stopwatch.forward();
	
	return m_stopwatch.isOver(intervalData.period);
}

bool Phase::IsDuration(const String &str)
{
	return str.ends_with(U"s");
}

bool Phase::IsRect(const String &str)
{
	const std::regex rectPattern{ R"(^((\d+),\s*(\d+))$)" };
	return std::regex_match(str.narrow(), rectPattern);
}

bool Phase::IsEasing(const String &str)
{
	return str.substr(0, 2) == U"e_";
}

Duration Phase::ParseDuration(const String &str)
{
	if (str.ends_with(U"s"))
	{
		return Duration{ ParseFloat<double>(str.removed(U"s")) };
	}
	else
	{
		throw Error(U"String to Duration casting was failed.");
	}
}

Rect Phase::ParseRect(const String &str)
{
	// s3d::String から std::string への変換
	std::string cast = str.narrow();

	const std::regex rectPattern{ R"(^((\d+),\s*(\d+))$)" };

	// マッチ結果を格納する変数を作って、パターンを検証
	if (std::smatch match;
		std::regex_match(cast, match, rectPattern))
	{
		return Rect
		{
			// match[1]以降が、部分パターンに適合した文字列を格納してる
			static_cast<int32>(std::stoi(match[1].str())),
			static_cast<int32>(std::stoi(match[2].str()))
		};
	}
	else
	{
		throw Error(U"String to Rect casting was failed.");
	}
}

cact::EasingFunction Phase::ParseEasing(const String &str)
{
	if (str.substr(0, 2) == U"e_")
	{
		// "e_" を削除し、小文字にして表記揺れ統一
		auto &&removed = str.substr(2).lowercase();


		if (removed == U"back")
		{
			return Easing::Back;
		}
		else if (removed == U"bounce")
		{
			return Easing::Bounce;
		}
		else if (removed == U"circ")
		{
			return Easing::Circ;
		}
		else if (removed == U"cubic")
		{
			return Easing::Cubic;
		}
		else if (removed == U"elastic")
		{
			return Easing::Elastic;
		}
		else if (removed == U"expo")
		{
			return Easing::Expo;
		}
		else if (removed == U"Linear")
		{
			return Easing::Linear;
		}
		else if (removed == U"quad")
		{
			return Easing::Quad;
		}
		else if (removed == U"Quart")
		{
			return Easing::Quart;
		}
		else if (removed == U"sine")
		{
			return Easing::Sine;
		}
		else
		{
			throw Error(U"`{}` is not registered as easing function."_fmt(removed));
		}
	}
	else
	{
		throw Error(U"Prefix of easing function `e_` is not found.");
	}
}

template <typename TTuple>
auto Phase::ParseParameters(const JSON &paramData)
	requires requires { typename std::tuple_size<TTuple>; }
{
	if (paramData.size() != std::tuple_size_v<TTuple>)
	{
		throw Error(U"Specified `paramData` (JSON data) and TTuple is not match length.");
	}

	std::array<
		std::variant<
		uint32,
		Duration,
		Rect,
		cact::EasingFunction,
		std::array<double, 4>
		>,
		std::tuple_size_v<TTuple>
	> temp;

	for (size_t i = 0; i < std::tuple_size_v<TTuple>; i++)
	{
		if (paramData[i].isNumber())
		{
			temp[i] = paramData[i].get<uint32>();
		}
		else if (paramData[i].isString())
		{
			const String& parsed = paramData[i].getString();

			if (IsDuration(parsed))
			{
				temp[i] = ParseDuration(parsed);
			}
			else if (IsRect(parsed))
			{
				temp[i] = ParseRect(parsed);
			}
			else if (IsEasing(parsed))
			{
				temp[i] = ParseEasing(parsed);
			}
			else
			{
				throw Error(U"");
			}
		}
		// 配列だった場合特殊
		// `appearFromEdge` の `overflow` でしか配列を引数にとらない
		// そのため、サイズ4の配列を決め打ちで渡して問題はないはず
		else if (paramData[i].isArray())
		{
			std::array<double, 4> overflow{};

			if (paramData[i].size() == 4)
			{
				for (size_t j = 0; j < 4; j++)
				{
					overflow[j] = paramData[j].get<double>();
				}
				temp[i] = overflow;
			}
			else
			{
				throw Error(U"");
			}
		}
		else
		{
			throw Error(U"");
		}
	}

	return std::apply([](auto &&...args)
		{
			return std::make_tuple(args...);
		}, temp);
}
