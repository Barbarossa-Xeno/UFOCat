#include "game_manager.hpp"

const CatData& GameManager::getTarget() const
{
	return m_target_ptr->getCatData();
}

const CatData& GameManager::setTarget()
{
	// 全種の中からランダムに選ぶ
	m_target_ptr = &cats.choice();
	return getTarget();
}

void GameManager::announceTarget()
{
	setTarget();
	
}

void GameManager::startPhase()
{
	// クリアしているフェーズの数は、現在行っているフェーズのインデックスと同じなのを利用する
	// （そのフェーズが終わり次第、isCleared のフラグを上げるため）
// 	m_currentPhaseIndex = phases.filter([](const Phase& p) { return p.isCleared; }).size();
// 
// 	currentState = State::InPhase;
}

void GameManager::inPhase()
{
// 	if (phases[m_currentPhaseIndex].updateAtInterval())
// 	{
// 		spawn();
// 	}
}

void GameManager::spawn()
{
	const CatObject& choice = cats.choice();
	spawns << std::make_unique<CatObject>(choice);
}

void GameManager::draw()
{
	for (const auto& cat : spawns)
	{
		cat->draw();
	}
}

GameManager& GameManager::Instance()
{
	static std::unique_ptr<GameManager> ptr{ new GameManager() };
	return *ptr;
}

Array<CatObject> GameManager::LoadCatData()
{
	// JSON ファイルからデータを読み込む
	const JSON json = JSON::Load(U"ufo_cats_data.json");

	// もし読み込みに失敗したら
	if (not json)
	{
		throw Error{ U"Failed to load `ufo_cats_data.json`" };
	}

	// data プロパティの中に配列として格納されている（元JSON参照）
	const auto&& data = json[U"data"];

	// 結果格納用
	Array<CatObject> results;

	// テクスチャフォルダから初めに全ての猫の画像を読み込んでおく
	const auto&& textures = FileSystem::DirectoryContents(U"texture").map([](const String& path) { return Texture{ path }; });

	if (data.getType() == JSONValueType::Array)
	{
		for (const auto& d : data)
		{
			if (d.value.getType() == JSONValueType::Object)
			{
				int32 id = d.value[U"id"].get<int32>();
				String breed = d.value[U"breed"].get<String>();

				// 以後、仮の文字列格納用変数は data_ で始める
				String data_color = d.value[U"color"].get<String>();
				String data_pattern = d.value[U"pattern"].get<String>();

				// ダイリュートかどうか
				bool isDilute = data_color.substr(0, 2) == U"D_";

				// ダイリュートなら先頭の "D_" を取り除く
				data_color = isDilute ? data_color.substr(2) : data_color;

				// 色データを区切り文字で分割して取得
				auto&& color = data_color.split(U'|').map([&isDilute](const auto& c)
					{
						Color temp = Palette::White;

						if (c == U"white")
						{
							temp = Color{ U"#f2fafe" };
						}
						else if (c == U"black")
						{
							temp = Color{ U"#272a2e" };
						}
						else if (c == U"gray")
						{
							temp = Color{ U"#9da5a9" };
						}
						else if (c == U"brown")
						{
							temp = Color{ U"#e8ae77" };
						}
						else if (c == U"ocher")
						{
							temp = Color{ U"#a87e39" };
						}
						else if (c == U"redbrown")
						{
							temp = Color{ U"#d67d36" };
						}
						else if (c == U"cream")
						{
							temp = Color{ U"#e5d5b6" };
						}
						else if (c == U"kiji")
						{
							temp = Color{ U"#7e7360" };
						}
						else if (c == U"shilver")
						{
							temp = Color{ U"#a8b7c1" };
						}
						else if (c == U"ruddy")
						{
							temp = Color{ U"#805308" };
						}
						else if (c == U"fawn")
						{
							temp = Color{ U"#d3c5a8" };
						}
						else if (c == U"bluegray")
						{
							temp = Color{ U"#90989b" };
						}
						else if (c == U"bluegray2")
						{
							temp = Color{ U"#70748d" };
						}
						else if (c == U"gold")
						{
							temp = Color{ U"#aa9263" };
						}

						if (isDilute)
						{
							HSV hsv{ temp };
							// 彩度を半分に、明度を少し上げる
							hsv.s *= 0.5;
							hsv.v += 0.1;
							temp = Color{ hsv };
						}

						return temp;
					});

				// 模様を調べる
				CatData::Pattern pattern;

				if (data_pattern == U"tora")
				{
					pattern = CatData::Pattern::Tora;
				}
				else if (data_pattern == U"tabby")
				{
					pattern = CatData::Pattern::Tabby;
				}
				else if (data_pattern == U"mike")
				{
					pattern = CatData::Pattern::Mike;
				}
				else if (data_pattern == U"calico")
				{
					pattern = CatData::Pattern::Calico;
				}
				else if (data_pattern == U"hachiware")
				{
					pattern = CatData::Pattern::Hachiware;
				}
				else if (data_pattern == U"sabi")
				{
					pattern = CatData::Pattern::Sabi;
				}
				else
				{
					pattern = CatData::Pattern::None;
				}

				bool isLongHair = d.value[U"isLongHair"].get<bool>();

				// 作成したインスタンスを格納
				results << CatObject{ textures[id] }.setCatData(CatData{ id, breed, color, pattern, isLongHair });
			}
		}

		return results;
	}

	throw Error{ U"Parameter is not JSONValueType::Array." };
}

Array<Phase> GameManager::LoadPhaseData()
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
		// JSONデータの全てを走査
		for (const auto &d : data)
		{
			if (d.value.isObject())
			{
				/*
				 * 以後、JSONから抽出する仮の文字列格納用変数は data_ で始める
				 * JSONのプロパティ値が
				 *	数値型の場合はs3d::JSON の get<uint32>() を使ってパース
				 *	文字列型の場合は、複数の型表現を包括しているため
				 *	 一旦 getString() で文字列として取得し、
				 *	 Duration や Rect、EasingFunction に変換する
				 */

				 // 1フェーズの時間制限
				Duration timeLimit = Phase::ParseDuration(d.value[U"timeLimit"].get<String>());

				// 類似度
				uint32 similarity = d.value[U"similarity"].get<uint32>();

				// 品種
				Phase::BreedData breedData{ };

				// 品種データを読み込む
				if (const auto &data_breedData = d.value[U"breedData"];
					data_breedData.isObject())
				{
					breedData.similar = data_breedData[U"similar"].get<uint32>();
					breedData.other = data_breedData[U"other"].get<uint32>();
				}
				else
				{
					throw Error(U"`breedData` is not object type.");
				}

				// 出現ペース
				Phase::IntervalData intervalData{ };

				// 出現ペースのデータを読み込む
				if (const auto &data_intervalData = d.value[U"intervalData"];
					data_intervalData.isObject())
				{
					intervalData.count = data_intervalData[U"count"].get<uint32>();
					intervalData.period = Phase::ParseDuration(data_intervalData[U"period"].getString());
				}
				else
				{
					throw Error(U"`intervalData` is not object type.");
				}

				// このフェーズでの使用アクション
				Array<Phase::ActionData> actionDataList{ };

				// 配列であることを確認してからアクションデータを全走査
				if (const auto &data_actionData = d.value[U"actionData"];
					data_actionData.isArray())
				{
					for (const auto &md : data_actionData)
					{

						const String name = md.value[U"name"].getString();

						const size_t overload = md.value[U"overload"].get<size_t>();

						const auto &data_params = md.value[U"params"];

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
										auto p = Phase::ParseParameters<cact::cross::_0>(data_params);
										params = std::make_tuple(get_at.operator() < 0, cact::cross::_0 > (p), get_at.operator() < 1, cact::cross::_0 > (p));
										break;
									}
									case 1:
									{
										auto p = Phase::ParseParameters<cact::cross::_1>(data_params);
										params = std::make_tuple(get_at.operator() < 0, cact::cross::_1 > (p));
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
										auto p = Phase::ParseParameters<cact::appear::_0>(data_params);
										params = std::make_tuple(get_at.operator() < 0, cact::appear::_0 > (p),
											get_at.operator() < 1, cact::appear::_0 > (p),
											get_at.operator() < 2, cact::appear::_0 > (p),
											get_at.operator() < 3, cact::appear::_0 > (p),
											get_at.operator() < 4, cact::appear::_0 > (p),
											get_at.operator() < 5, cact::appear::_0 > (p)
										);
										break;
									}
									case 1:
									{
										auto p = Phase::ParseParameters<cact::appear::_1>(data_params);
										params = std::make_tuple(get_at.operator() < 0, cact::appear::_1 > (p),
											get_at.operator() < 1, cact::appear::_1 > (p),
											get_at.operator() < 2, cact::appear::_1 > (p),
											get_at.operator() < 3, cact::appear::_1 > (p)
										);
										break;
									}
									case 2:
									{
										auto p = Phase::ParseParameters<cact::appear::_2>(data_params);
										params = std::make_tuple(get_at.operator() < 0, cact::appear::_2 > (p),
											get_at.operator() < 1, cact::appear::_2 > (p),
											get_at.operator() < 2, cact::appear::_2 > (p),
											get_at.operator() < 3, cact::appear::_2 > (p)
										);
										break;
									}
									case 3:
									{
										auto p = Phase::ParseParameters<cact::appear::_3>(data_params);
										params = std::make_tuple(get_at.operator() < 0, cact::appear::_3 > (p),
											get_at.operator() < 1, cact::appear::_3 > (p),
											get_at.operator() < 2, cact::appear::_3 > (p)
										);
										break;
									}
									case 4:
									{
										auto p = Phase::ParseParameters<cact::appear::_4>(data_params);
										params = std::make_tuple(get_at.operator() < 0, cact::appear::_4 > (p),
											get_at.operator() < 1, cact::appear::_4 > (p),
											get_at.operator() < 2, cact::appear::_4 > (p),
											get_at.operator() < 3, cact::appear::_4 > (p),
											get_at.operator() < 4, cact::appear::_4 > (p)
										);
										break;
									}
									case 5:
									{
										auto p = Phase::ParseParameters<cact::appear::_5>(data_params);
										params = std::make_tuple(get_at.operator() < 0, cact::appear::_5 > (p),
											get_at.operator() < 1, cact::appear::_5 > (p),
											get_at.operator() < 2, cact::appear::_5 > (p)
										);
										break;
									}
									case 6:
									{
										auto p = Phase::ParseParameters<cact::appear::_6>(data_params);
										params = std::make_tuple(get_at.operator() < 0, cact::appear::_6 > (p),
											get_at.operator() < 1, cact::appear::_6 > (p),
											get_at.operator() < 2, cact::appear::_6 > (p)
										);
										break;
									}
									case 7:
									{
										auto p = Phase::ParseParameters<cact::appear::_7>(data_params);
										params = std::make_tuple(get_at.operator() < 0, cact::appear::_7 > (p),
											get_at.operator() < 1, cact::appear::_7 > (p)
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
										auto p = Phase::ParseParameters<cact::appearFromEdge::_0>(data_params);
										params = std::make_tuple(get_at.operator() < 0, cact::appearFromEdge::_0 > (p),
											get_at.operator() < 1, cact::appearFromEdge::_0 > (p),
											get_at.operator() < 2, cact::appearFromEdge::_0 > (p),
											get_at.operator() < 3, cact::appearFromEdge::_0 > (p),
											get_at.operator() < 4, cact::appearFromEdge::_0 > (p),
											get_at.operator() < 5, cact::appearFromEdge::_0 > (p)
										);
										break;
									}
									case 1:
									{
										auto p = Phase::ParseParameters<cact::appearFromEdge::_1>(data_params);
										params = std::make_tuple(get_at.operator() < 0, cact::appearFromEdge::_1 > (p),
											get_at.operator() < 1, cact::appearFromEdge::_1 > (p),
											get_at.operator() < 2, cact::appearFromEdge::_1 > (p),
											get_at.operator() < 3, cact::appearFromEdge::_1 > (p)
										);
										break;
									}
									case 2:
									{
										auto p = Phase::ParseParameters<cact::appearFromEdge::_2>(data_params);
										params = std::make_tuple(get_at.operator() < 0, cact::appearFromEdge::_2 > (p),
											get_at.operator() < 1, cact::appearFromEdge::_2 > (p),
											get_at.operator() < 2, cact::appearFromEdge::_2 > (p),
											get_at.operator() < 3, cact::appearFromEdge::_2 > (p)
										);
										break;
									}
									case 3:
									{
										auto p = Phase::ParseParameters<cact::appearFromEdge::_3>(data_params);
										params = std::make_tuple(get_at.operator() < 0, cact::appearFromEdge::_3 > (p),
											get_at.operator() < 1, cact::appearFromEdge::_3 > (p),
											get_at.operator() < 2, cact::appearFromEdge::_3 > (p)
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

						actionDataList << Phase::ActionData{ name, params, probability };
					}
				}
				else
				{
					throw Error(U"`actionData` is not array type.");
				}

				result << Phase{ timeLimit, similarity, breedData, intervalData, actionDataList };
			}

		}
		return result;
	}
	throw Error(U"`phase.json is invalid format.`");
}
