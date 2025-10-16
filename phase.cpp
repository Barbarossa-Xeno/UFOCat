#include "phase.hpp"
#include <regex>
#include <span>

using EasingF = CatObject::EasingFunction;

const HashTable<String, Array<Phase::Signature>> Phase::m_actionInfo
{
	// 奇遇にも JSON みたいな書き方になった
	{
		U"bound",
		{
			{ &typeid(void) }
		}
	},
	{
		U"cross",
		{
			{ &typeid(Duration), &typeid(uint32) },
			{ &typeid(Duration) }
		}
	},
	{
		U"appear",
		{
			{ &typeid(Duration), &typeid(EasingF), &typeid(Duration), &typeid(EasingF), &typeid(Duration), &typeid(Rect) },
			{ &typeid(Duration), &typeid(Duration), &typeid(Duration), &typeid(Rect) },
			{ &typeid(Duration), &typeid(EasingF), &typeid(Duration), &typeid(Rect) },
			{ &typeid(Duration), &typeid(Duration), &typeid(Rect) },
			{ &typeid(Duration), &typeid(EasingF), &typeid(Duration), &typeid(EasingF), &typeid(Duration) },
			{ &typeid(Duration), &typeid(Duration), &typeid(Duration) },
			{ &typeid(Duration), &typeid(EasingF), &typeid(Duration) },
			{ &typeid(Duration), &typeid(Duration) }
		}
	},
	{
		U"appearFromEdge",
		{
			{ &typeid(Duration), &typeid(EasingF), &typeid(Duration), &typeid(EasingF), &typeid(Duration), &typeid(std::array<double, 4>) },
			{ &typeid(Duration), &typeid(Duration), &typeid(Duration), &typeid(std::array<double, 4>) },
			{ &typeid(Duration), &typeid(EasingF), &typeid(Duration), &typeid(std::array<double, 4>) },
			{ &typeid(Duration), &typeid(Duration), &typeid(std::array<double, 4>) }
		}
	}
};

bool Phase::updateAtInterval()
{
	m_stopwatch.forward();
	
	return m_stopwatch.isOver(intervalData.period);
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
	const auto &&data = json[U"data"];
	
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

		const auto isEasing = [](const String &str) { return str.substr(0, 2) == U"e_"; };

		// 文字列が Duration を表すならパースする
		const auto parseDuration = [](const String &str) -> Duration
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

		// このメソッドで、テンプレート引数 N と 実際のパラメータ数が一致していることは保証されない
		// そのため、呼び出し元で確認する
		const auto parseParameters = [&isDuration, &isRect, &isEasing, &parseDuration, &parseRect, &parseEasing]
		<size_t N>
		(
			const std::pair<String, Signature>& confirmedSignature,
			const JSON& jsonParams
		)
			{
				if (jsonParams.size() != confirmedSignature.second.size())
				{
					throw Error(U"Size of json array parameter is not match to size of confirmed signature's patameter.");
				}

				std::array<std::any, N> parsed;

				for (int32 i = 0; i < N; i++)
				{
					if (jsonParams[i].isNumber())
					{
						parsed[i] = jsonParams[i].get<uint32>();
					}
					else if (jsonParams[i].isString())
					{
						const String& str = jsonParams[i].getString();

						if (isDuration(str))
						{
							parsed[i] = parseDuration(str);
						}
						else if (isRect(str))
						{
							parsed[i] = parseRect;
						}
						else if (isEasing(str))
						{
							parsed[i] = parseEasing(str);
						}
						else
						{
							throw Error(U"{} - Failed to parse parameter."_fmt(jsonParams[i]));
						}
					}
					// 配列だった場合特殊
					// appearFromEdgeでしか配列を引数にとらない
					// そのため、関数名がそれかどうか知らべて、サイズ4の配列を決め打ちで渡す
					else if (jsonParams[i].isArray() and (confirmedSignature.first == U"appearFromEdge"))
					{
						if (jsonParams.size() == 4)
						{
							std::array<double, 4> temp_overflow{ };

							for (int32 j = 0; j < 4; j++)
							{
								temp_overflow[j] = jsonParams[j].get<double>();
							}

							parsed[i] = temp_overflow;
						}
						else
						{
							throw Error(U"Invalid array format. Array (double, size 4) type parameter is only allowed in `appearFromEdge`");
						}
					}
					else
					{
						throw Error(U"");
					}
				}

				std::apply([](auto &&...args) { return std::make_tuple(args...); }, parsed);
			};
		
		/* -- END: JSONデータのパースに用いるローカル関数などの機能群 -- */

		// JSONデータの全てを走査
		for (const auto &d : data)
		{
			if (d.value.isObject())
			{
				// 以後、JSONから抽出する仮の文字列格納用変数は data_ で始める

				// 1フェーズの時間制限
				Duration timeLimit = parseDuration(d.value[U"timeLimit"].get<String>());

				// 類似度
				uint32 similarity = d.value[U"similarity"].get<uint32>();


				BreedData breedData{ };

				if (const auto &data_breedData = d.value[U"breedData"];
					data_breedData.getType() == JSONValueType::Object)
				{
					breedData.similar = data_breedData[U"similar"].get<uint32>();
					breedData.other = data_breedData[U"other"].get<uint32>();
				}

				IntervalData intervalData{ };

				if (const auto &data_intervalData = d.value[U"intervalData"];
					data_intervalData.getType() == JSONValueType::Object)
				{
					intervalData.count = data_intervalData[U"count"].get<uint32>();
					intervalData.period = parseDuration(data_intervalData[U"period"].getString());
				}

				// 
				Array<ActionData> actionDataList{ };

				if (const auto &data_actionData = d.value[U"actionData"];
					data_actionData.getType() == JSONValueType::Array)
				{
					size_t currentIndex = 0;

					for (const auto &md : data_actionData)
					{

						const String name = md.value[U"name"].getString();

						const size_t overload = md.value[U"overload"].get<size_t>();

						const auto &data_params = md.value[U"params"];

						const double probability = md.value[U"probability"].get<double>();

						std::variant<std::monostate, cact::cross::TSignatures, cact::appear::TSignatures, cact::appearFromEdge::TSignatures> params;

						const auto parseP = []
						<typename TTuple, typename ...TArgs>
						requires requires { std::tuple_size<TTuple>::type; }
						(
							TArgs &...args
						)
							{
								std::array<
									std::variant<
										uint32,
										Duration,
										Rect,
										CatObject::EasingFunction,
										std::array<double, 4>
									>,
									std::tuple_size_v<TTuple>
								> temp; 

								return std::apply([](auto &&...args)
									{
										return TTuple{ args... };
									}, temp);
							};


						bool isSame = true;

						std::pair<String, Signature> confirmedSignature;

						if (not (overload >= 0))
						{
							throw Error(U"`overload` number is out of range in `actionData` in phases.json.");
						}

						if (data_params.isArray())
						{
							Signature parsedParamsInfo;

							// const auto &candidates = m_actionInfo.at(name).filter([&data_params](const Signature &i) { return i.size() == data_params.size(); });

							if (name == U"bound")
							{
								params = std::monostate{};
							}
							else if (name == U"cross")
							{
								if (overload < std::variant_size_v<cact::cross::TSignatures>)
								{
									switch (overload)
									{
										case 0:
										{
											params = cact::cross::_0{};
											break;
										}
										case 1:
									}
								}
							}
							else if (name == U"")
							{

							}
							else if (name == U"")
							{

							}

							for (const auto &p : data_params)
							{
								// parsedParamsInfo << &typeid(p);

								
							}

							size_t scannedCount = 0;

							for (auto &&candidate : candidates)
							{
								if (scannedCount > 0 and isSame)
								{
									// 前回走査した候補が適切なシグネチャとなる
									confirmedSignature = { name, candidates[scannedCount - 1] };
									break;
								}

								for (int32 i = 0; i < candidate.size(); i++)
								{
									isSame = (isSame and candidate[i] == parsedParamsInfo[i]);

									if (not isSame)
									{
										isSame = true;
										continue;
									}
								}

								++scannedCount;
							}
						}
						else if (data_params.isNull())
						{
							isSame = m_actionInfo.at(name).filter([](const Signature& i)
								{
									return i.at(0) == &typeid(void);
								})
								.size() == 1;
						}

						if (not isSame)
						{
							throw Error(U"Specified parameters signature is not correct in `methodData[{}]`"_fmt(currentIndex));
						}
		
						const auto parseParams = [&](std::ranges::sized_range auto &tempParam)
							{
								// `params` と 確定したシグネチャの引数の数が等しかったら
								if (tempParam.size() == confirmedSignature.second.size())
								{

									for (int32 i = 0; i < tempParam.size(); i++)
									{
										// かつ、各引数の型が等しいかどうか調べて
										if (typeid(tempParam[i]) == typeid(confirmedSignature.second[i]))
										{

											if (data_params[i].isNumber())
											{
												tempParam[i] = data_params[i].get<uint32>();
											}
											else if (data_params[i].isString())
											{
												const String& parsed = data_params[i].getString();

												if (isDuration(parsed))
												{
													tempParam[i] = parseDuration(parsed);
												}
												else if (isRect(parsed))
												{
													tempParam[i] = parseRect;
												}
												else if (isEasing(parsed))
												{
													tempParam[i] = parseEasing(parsed);
												}
												else
												{
													throw Error(U"");
												}
											}
											// 配列だった場合特殊
											// appearFromEdgeでしか配列を引数にとらない
											// そのため、関数名がそれかどうか知らべて、サイズ4の配列を決め打ちで渡す
											else if (data_params[i].isArray() and confirmedSignature.first == U"appearFromEdge")
											{
												std::array<double, 4> temp_overflow{ 0.0, 0.0, 0.0, 0.0 };

												if (data_params.size() == 4)
												{
													for (int32 j = 0; j < 4; j++)
													{
														temp_overflow[j] = data_params[j].get<double>();
													}

													tempParam[i] = temp_overflow;
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
										
									}

									return std::apply([](auto &&...args)
										{
											return std::make_tuple(args...);
										}, tempParam);
								}
							};

						std::any temp_mp;
						parseParameters.operator()<5>(confirmedSignature, data_params);

						// TODO: 引数あってもnoneになるようなので、ここの条件を見直す
						// 引数が無いとき
						if (confirmedSignature.first.isEmpty() and confirmedSignature.second.isEmpty())
						{
							temp_mp = none;
						}
						else if (data_params.size() == 1)
						{
							// std::array<std::any, 1> tempParams;
							// temp_mp = parseParams(tempParams);
							parseParameters.operator()<1>(confirmedSignature, data_params);
						}
						else if (data_params.size() == 2)
						{
							std::array<std::any, 1> tempParams;
							temp_mp = parseParams(tempParams);
						}
						else if (data_params.size() == 3)
						{
							std::array<std::any, 1> tempParams;
							temp_mp = parseParams(tempParams);
						}
						else if (data_params.size() == 4)
						{
							std::array<std::any, 1> tempParams;
							temp_mp = parseParams(tempParams);
						}
						else if (data_params.size() == 5)
						{
							std::array<std::any, 1> tempParams;
							temp_mp = parseParams(tempParams);
						}
						else if (data_params.size() == 6)
						{
							std::array<std::any, 1> tempParams;
							temp_mp = parseParams(tempParams);
						}
						else if (data_params.size() == 7)
						{
							std::array<std::any, 1> tempParams;
							temp_mp = parseParams(tempParams);
						}
						else if (data_params.size() == 8)
						{
							std::array<std::any, 1> tempParams;
							temp_mp = parseParams(tempParams);
						}

						actionDataList << ActionData{ name, temp_mp, probability };

						++currentIndex;
					}
				}
				result << Phase{ timeLimit, similarity, breedData, intervalData, actionDataList };
			}
		}

		return result;
	}

	throw Error(U"`phase.json is invalid format.`");
}
