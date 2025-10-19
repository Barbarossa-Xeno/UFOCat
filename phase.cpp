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

bool Phase::m_IsDuration(const String &str)
{
	return str.ends_with(U"s");
}

bool Phase::m_IsRect(const String &str)
{
	const std::regex rectPattern{ R"(^((\d+),\s*(\d+))$)" };
	return std::regex_match(str.narrow(), rectPattern);
}

bool Phase::m_IsEasing(const String &str)
{
	return str.substr(0, 2) == U"e_";
}

Duration Phase::m_ParseDuration(const String &str)
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

Rect Phase::m_ParseRect(const String &str)
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

cact::EasingFunction Phase::m_ParseEasing(const String &str)
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
auto Phase::m_ParseParameters(const JSON &paramData)
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

			if (m_IsDuration(parsed))
			{
				temp[i] = m_ParseDuration(parsed);
			}
			else if (m_IsRect(parsed))
			{
				temp[i] = m_ParseRect(parsed);
			}
			else if (m_IsEasing(parsed))
			{
				temp[i] = m_ParseEasing(parsed);
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

Array<Phase> Phase::LoadData()
{
	// JSON からデータを読み込む
	const JSON json = JSON::Load(U"phases.json");

	// もし読み込みに失敗したら
	if (not json)
	{
		throw Error{ U"Failed to load `phases.json`" };
	}

	// data プロパティの中に配列として格納されている各フェーズのデータ（元JSON参照）
	const auto&& data = json[U"data"];

	// 結果格納用
	Array<Phase> result;

	// まずは、配列形式でいろんなPhaseのデータが入っているのが前提
	if (data.isArray())
	{

		/* -- JSONデータのパースに用いるローカル関数などの機能群 -- */

		// 文字列データが Duration かどうかを調べる
		// Duration を表す文字列は、最後に "s" がついている
		const auto isDuration = [](const String& str) { return str.ends_with(U"s"); };

		// 文字列データが Rect であるときのパターン
		// \s は空白文字、有無は問わないようにする
		const std::regex rectPattern{ R"(^((\d+),\s*(\d+))$)" };

		// 文字列データが Rect かどうかを調べる
		const auto isRect = [&rectPattern](const String& str) { return std::regex_match(str.narrow(), rectPattern); };

		const auto isEasing = [](const String& str) { return str.substr(0, 2) == U"e_"; };

		// 文字列が Duration を表すならパースする
		const auto parseDuration = [](const String& str) -> Duration
			{
				if (str.ends_with(U"s"))
				{
					return Duration{ ParseFloat<double>(str.removed(U"s")) };
				}
				else
					throw Error(U"String to Duration casting was failed.");
			};

		// 文字列が Rect を表すならパースする
		const auto parseRect = [&rectPattern](const String& str) -> Rect
			{
				// s3d::String から std::string への変換
				std::string cast = str.narrow();

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
					throw Error(U"String to Rect casting was failed.");
			};

		// ここの str は & にしてはいけない
		const auto parseEasing = [](String str)
			{
				if (str.substr(0, 2) == U"e_")
				{
					// "e_" を削除し、小文字にして表記揺れ統一
					str = str.substr(2).lowercase();

					if (str == U"back")
					{
						return Easing::Back;
					}
					else if (str == U"bounce")
					{
						return Easing::Bounce;
					}
					else if (str == U"circ")
					{
						return Easing::Circ;
					}
					else if (str == U"cubic")
					{
						return Easing::Cubic;
					}
					else if (str == U"elastic")
					{
						return Easing::Elastic;
					}
					else if (str == U"expo")
					{
						return Easing::Expo;
					}
					else if (str == U"Linear")
					{
						return Easing::Linear;
					}
					else if (str == U"quad")
					{
						return Easing::Quad;
					}
					else if (str == U"Quart")
					{
						return Easing::Quart;
					}
					else if (str == U"sine")
					{
						return Easing::Sine;
					}
					else
					{
						throw Error(U"`{}` is not registered as easing function."_fmt(str));
					}
				}
				else
				{
					throw Error(U"Prefix of easing function `e_` is not found.");
				}
			};

		/* -- END: JSONデータのパースに用いるローカル関数などの機能群 -- */

		// JSONデータの全てを走査
		for (const auto& d : data)
		{
			if (d.value.isObject())
			{
				// 以後、JSONから抽出する仮の文字列格納用変数は data_ で始める

				// 1フェーズの時間制限
				Duration timeLimit = parseDuration(d.value[U"timeLimit"].get<String>());

				// 類似度
				uint32 similarity = d.value[U"similarity"].get<uint32>();


				BreedData breedData{ };

				if (const auto& data_breedData = d.value[U"breedData"];
					data_breedData.getType() == JSONValueType::Object)
				{
					breedData.similar = data_breedData[U"similar"].get<uint32>();
					breedData.other = data_breedData[U"other"].get<uint32>();
				}

				IntervalData intervalData{ };

				if (const auto& data_intervalData = d.value[U"intervalData"];
					data_intervalData.getType() == JSONValueType::Object)
				{
					intervalData.count = data_intervalData[U"count"].get<uint32>();
					intervalData.period = parseDuration(data_intervalData[U"period"].getString());
				}

				// 
				Array<ActionData> actionDataList{ };

				if (const auto& data_actionData = d.value[U"actionData"];
					data_actionData.getType() == JSONValueType::Array)
				{
					size_t currentIndex = 0;

					for (const auto& md : data_actionData)
					{

						const String name = md.value[U"name"].getString();

						const size_t overload = md.value[U"overload"].get<size_t>();

						const auto& data_params = md.value[U"params"];

						const double probability = md.value[U"probability"].get<double>();

						cact::Generic params;

						if (data_params.isArray())
						{

							const auto get_at = []<size_t N, typename TTuple>(const auto &source)
								{
									return std::get<std::tuple_element_t<N, TTuple>>(std::get<N>(source));
								};

							if (name == U"bound")
							{
								params = std::monostate{};
							}
							else if (name == U"cross")
							{
								switch (overload)
								{
									case 0:
									{
										auto p = m_ParseParameters<cact::cross::_0>(data_params);
										params = std::make_tuple(get_at.operator()<0, cact::cross::_0>(p), get_at.operator()<1, cact::cross::_0>(p));
										break;
									}
									case 1:
									{
										auto p = m_ParseParameters<cact::cross::_1>(data_params);
										params = std::make_tuple(get_at.operator()<0, cact::cross::_1>(p));
										break;
									}
									default:
									{
										throw Error(U"`cross` overload index is invalid. (valid range: 0 ~ {})"_fmt(cact::cross::Count - 1));
									}
								}
							}
							else if (name == U"appear")
							{
								switch (overload)
								{
									case 0:
									{
										auto p = m_ParseParameters<cact::appear::_0>(data_params);
										params = std::make_tuple(get_at.operator()<0, cact::appear::_0>(p),
																 get_at.operator()<1, cact::appear::_0>(p),
																 get_at.operator()<2, cact::appear::_0>(p),
																 get_at.operator()<3, cact::appear::_0>(p),
																 get_at.operator()<4, cact::appear::_0>(p),
																 get_at.operator()<5, cact::appear::_0>(p)
																);
										break;
									}
									case 1:
									{
										auto p = m_ParseParameters<cact::appear::_1>(data_params);
										params = std::make_tuple(get_at.operator()<0, cact::appear::_1>(p),
																 get_at.operator()<1, cact::appear::_1>(p),
																 get_at.operator()<2, cact::appear::_1>(p),
																 get_at.operator()<3, cact::appear::_1>(p)
																);
										break;
									}
									case 2:
									{
										auto p = m_ParseParameters<cact::appear::_2>(data_params);
										params = std::make_tuple(get_at.operator()<0, cact::appear::_2>(p),
																 get_at.operator()<1, cact::appear::_2>(p),
																 get_at.operator()<2, cact::appear::_2>(p),
																 get_at.operator()<3, cact::appear::_2>(p)
																);
										break;
									}
									case 3:
									{
										auto p = m_ParseParameters<cact::appear::_3>(data_params);
										params = std::make_tuple(get_at.operator()<0, cact::appear::_3>(p),
																 get_at.operator()<1, cact::appear::_3>(p),
																 get_at.operator()<2, cact::appear::_3>(p)
																);
										break;
									}
									case 4:
									{
										auto p = m_ParseParameters<cact::appear::_4>(data_params);
										params = std::make_tuple(get_at.operator()<0, cact::appear::_4>(p),
																 get_at.operator()<1, cact::appear::_4>(p),
																 get_at.operator()<2, cact::appear::_4>(p),
																 get_at.operator()<3, cact::appear::_4>(p),
																 get_at.operator()<4, cact::appear::_4>(p)
																);
										break;
									}
									case 5:
									{
										auto p = m_ParseParameters<cact::appear::_5>(data_params);
										params = std::make_tuple(get_at.operator()<0, cact::appear::_5>(p),
																 get_at.operator()<1, cact::appear::_5>(p),
																 get_at.operator()<2, cact::appear::_5>(p)
																);
										break;
									}
									case 6:
									{
										auto p = m_ParseParameters<cact::appear::_6>(data_params);
										params = std::make_tuple(get_at.operator()<0, cact::appear::_6>(p),
																 get_at.operator()<1, cact::appear::_6>(p),
																 get_at.operator()<2, cact::appear::_6>(p)
																);
										break;
									}
									case 7:
									{
										auto p = m_ParseParameters<cact::appear::_7>(data_params);
										params = std::make_tuple(get_at.operator()<0, cact::appear::_7>(p),
																 get_at.operator()<1, cact::appear::_7>(p)
																);
										break;
									}
									default:
									{
										throw Error(U"`appear` overload index is invalid. (valid range: 0 ~ {})"_fmt(cact::appear::Count - 1));
									}
								}
							}
							else if (name == U"appearFromEdge")
							{
								switch (overload)
								{
									case 0:
									{
										auto p = m_ParseParameters<cact::appearFromEdge::_0>(data_params);
										params = std::make_tuple(get_at.operator()<0, cact::appearFromEdge::_0>(p),
																 get_at.operator()<1, cact::appearFromEdge::_0>(p),
																 get_at.operator()<2, cact::appearFromEdge::_0>(p),
																 get_at.operator()<3, cact::appearFromEdge::_0>(p),
																 get_at.operator()<4, cact::appearFromEdge::_0>(p),
																 get_at.operator()<5, cact::appearFromEdge::_0>(p)
																);
										break;
									}
									case 1:
									{
										auto p = m_ParseParameters<cact::appearFromEdge::_1>(data_params);
										params = std::make_tuple(get_at.operator()<0, cact::appearFromEdge::_1>(p),
																 get_at.operator()<1, cact::appearFromEdge::_1>(p),
																 get_at.operator()<2, cact::appearFromEdge::_1>(p),
																 get_at.operator()<3, cact::appearFromEdge::_1>(p)
																);
										break;
									}
									case 2:
									{
										auto p = m_ParseParameters<cact::appearFromEdge::_2>(data_params);
										params = std::make_tuple(get_at.operator()<0, cact::appearFromEdge::_2>(p),
																 get_at.operator()<1, cact::appearFromEdge::_2>(p),
																 get_at.operator()<2, cact::appearFromEdge::_2>(p),
																 get_at.operator()<3, cact::appearFromEdge::_2>(p)
																);
										break;
									}
									case 3:
									{
										auto p = m_ParseParameters<cact::appearFromEdge::_3>(data_params);
										params = std::make_tuple(get_at.operator()<0, cact::appearFromEdge::_3>(p),
																 get_at.operator()<1, cact::appearFromEdge::_3>(p),
																 get_at.operator()<2, cact::appearFromEdge::_3>(p)
																);
										break;
									}
									default:
									{
										throw Error(U"`appearFromEdge` overload index is invalid. (valid range: 0 ~ {})"_fmt(cact::appearFromEdge::Count - 1));
									}
								}
							}
							else
							{
								throw Error(U"`{}` is not registered as action (method) name."_fmt(name));
							}
						}

						actionDataList << ActionData{ name, params, probability };
					}
				}

				result << Phase{ timeLimit, similarity, breedData, intervalData, actionDataList };
			}
			
		}
		return result;
	}
	throw Error(U"`phase.json is invalid format.`");
}
