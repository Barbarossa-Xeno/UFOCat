# include "Common.hpp"

Array<CatObject> UFOCat::LoadCatData()
{
	// JSON ファイルからデータを読み込む
	const JSON json = JSON::Load(U"cat_data.json");

	// もし読み込みに失敗したら
	if (not json)
	{
		throw Error{ U"Failed to load `cat_data.json`" };
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

Array<LevelData> UFOCat::LoadLevelData()
{
	// JSON からデータを読み込む
	const JSON json = JSON::Load(U"level_data.json");

	// もし読み込みに失敗したら
	if (not json)
	{
		throw Error{ U"Failed to load `level_data.json`" };
	}

	// data プロパティの中に配列として格納されている各レベルのデータ（元JSON参照）
	const auto&& data = json[U"data"];

	// 結果格納用
	Array<LevelData> result;

	// まずは、配列形式でいろんなPhaseのデータが入っているのが前提
	if (data.isArray())
	{
		// JSONデータの全てを走査
		for (const auto& d : data)
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

				 // 1レベルの時間制限
				Duration timeLimit = LevelData::ParseDuration(d.value[U"timeLimit"].get<String>());

				// 類似度
				uint32 similarity = d.value[U"similarity"].get<uint32>();

				// 品種
				LevelData::BreedData breedData{ };

				// 品種データを読み込む
				if (const auto& data_breedData = d.value[U"breedData"];
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
				LevelData::IntervalData intervalData{ };

				// 出現ペースのデータを読み込む
				if (const auto& data_intervalData = d.value[U"intervalData"];
					data_intervalData.isObject())
				{
					intervalData.count = data_intervalData[U"count"].get<uint32>();
					intervalData.period = LevelData::ParseDuration(data_intervalData[U"period"].getString());
				}
				else
				{
					throw Error(U"`intervalData` is not object type.");
				}

				// このレベルでの使用アクション
				Array<LevelData::ActionData> actionDataList{ };

				// 配列であることを確認してからアクションデータを全走査
				if (const auto& data_actionData = d.value[U"actionData"];
					data_actionData.isArray())
				{
					for (const auto& md : data_actionData)
					{
						// アクション名（メソッド名）
						const String name = md.value[U"name"].getString();

						// オーバーロード番号（何がどれに対応するかは、`cact` 参照）
						const size_t overload = md.value[U"overload"].get<size_t>();

						// `params` プロパティを格納する仮変数
						const auto& data_params = md.value[U"params"];

						// 発生確率
						const double probability = md.value[U"probability"].get<double>();

						/* -- こっから実際の引数として使えるタプルにするためのパース処理 -- */

						// 引数リスト
						CAct::Generic params;

						// 配列形式であることを確認
						if (data_params.isArray())
						{
							// タプルの N 番目の要素を variant を外して取り出す
							// tparam: N - 取り出す要素のインデックス
							// tparam: TTuple - 取り出す対象のタプル型
							// param: source - `Phase::ParseParameters()` の戻り値として得られた tuple (中身 variant)
							const auto get_at = []<size_t N, typename TTuple>(const auto& source)
							{
								// source (タプル) の N 番目の要素を取り出す
								// これは、variant<> であることが期待される
								auto&& element = std::get<N>(source);

								// TTuple の N 番目の要素の型を取得
								// この型は、element の variant に格納されている本来の型と同じはず
								using RealType = std::tuple_element_t<N, TTuple>;

								// つまりこれはvariant を外す処理となる
								return std::get<RealType>(element);
							};
							// ジェネリックラムダのテンプレートは、実質ローカル関数でもテンプレートが使えるので便利だけど
							// 明示的にテンプレートを指定するのは、operator() を使わなければならないので少し面倒

							// 各アクションごとに場合分けしてパース処理
							if (name == U"bound")
							{
								// `bound` は引数を取らないので、monostate をセットしておく
								params = std::monostate{};
							}
							else if (name == U"cross")
							{
								switch (overload)
								{
								case 0:
								{
									auto p = LevelData::ParseParameters<CAct::cross::_0>(data_params);
									params = std::make_tuple(
										get_at.operator()<0, CAct::cross::_0>(p),
										get_at.operator()<1, CAct::cross::_0>(p)
									);
									break;
								}
								case 1:
								{
									auto p = LevelData::ParseParameters<CAct::cross::_1>(data_params);
									params = std::make_tuple(
										get_at.operator()<0, CAct::cross::_1>(p)
									);
									break;
								}
								default:
								{
									throw Error(U"`cross` overload index is invalid. (valid range: 0 ~ {})"_fmt(CAct::cross::Count - 1));
								}
								}
							}
							else if (name == U"appear")
							{
								switch (overload)
								{
								case 0:
								{
									auto p = LevelData::ParseParameters<CAct::appear::_0>(data_params);
									// 直書きでタプル作るのはちょっとダサいけど
									// 固定長で作るという制約上しゃあないところが大きい
									params = std::make_tuple
									(
										get_at.operator()<0, CAct::appear::_0>(p),
										get_at.operator()<1, CAct::appear::_0>(p),
										get_at.operator()<2, CAct::appear::_0>(p),
										get_at.operator()<3, CAct::appear::_0>(p),
										get_at.operator()<4, CAct::appear::_0>(p),
										get_at.operator()<5, CAct::appear::_0>(p)
									);
									break;
								}
								case 1:
								{
									auto p = LevelData::ParseParameters<CAct::appear::_1>(data_params);
									params = std::make_tuple
									(
										get_at.operator()<0, CAct::appear::_1>(p),
										get_at.operator()<1, CAct::appear::_1>(p),
										get_at.operator()<2, CAct::appear::_1>(p),
										get_at.operator()<3, CAct::appear::_1>(p)
									);
									break;
								}
								case 2:
								{
									auto p = LevelData::ParseParameters<CAct::appear::_2>(data_params);
									params = std::make_tuple
									(
										get_at.operator()<0, CAct::appear::_2>(p),
										get_at.operator()<1, CAct::appear::_2>(p),
										get_at.operator()<2, CAct::appear::_2>(p),
										get_at.operator()<3, CAct::appear::_2>(p)
									);
									break;
								}
								case 3:
								{
									auto p = LevelData::ParseParameters<CAct::appear::_3>(data_params);
									params = std::make_tuple
									(
										get_at.operator()<0, CAct::appear::_3>(p),
										get_at.operator()<1, CAct::appear::_3>(p),
										get_at.operator()<2, CAct::appear::_3>(p)
									);
									break;
								}
								case 4:
								{
									auto p = LevelData::ParseParameters<CAct::appear::_4>(data_params);
									params = std::make_tuple
									(
										get_at.operator()<0, CAct::appear::_4>(p),
										get_at.operator()<1, CAct::appear::_4>(p),
										get_at.operator()<2, CAct::appear::_4>(p),
										get_at.operator()<3, CAct::appear::_4>(p),
										get_at.operator()<4, CAct::appear::_4>(p)
									);
									break;
								}
								case 5:
								{
									auto p = LevelData::ParseParameters<CAct::appear::_5>(data_params);
									params = std::make_tuple
									(
										get_at.operator()<0, CAct::appear::_5>(p),
										get_at.operator()<1, CAct::appear::_5>(p),
										get_at.operator()<2, CAct::appear::_5>(p)
									);
									break;
								}
								case 6:
								{
									auto p = LevelData::ParseParameters<CAct::appear::_6>(data_params);
									params = std::make_tuple
									(
										get_at.operator()<0, CAct::appear::_6>(p),
										get_at.operator()<1, CAct::appear::_6>(p),
										get_at.operator()<2, CAct::appear::_6>(p)
									);
									break;
								}
								case 7:
								{
									auto p = LevelData::ParseParameters<CAct::appear::_7>(data_params);
									params = std::make_tuple
									(
										get_at.operator()<0, CAct::appear::_7>(p),
										get_at.operator()<1, CAct::appear::_7>(p)
									);
									break;
								}
								default:
								{
									throw Error(U"`appear` overload index is invalid. (valid range: 0 ~ {})"_fmt(CAct::appear::Count - 1));
								}
								}
							}
							else if (name == U"appearFromEdge")
							{
								switch (overload)
								{
								case 0:
								{
									auto p = LevelData::ParseParameters<CAct::appearFromEdge::_0>(data_params);
									params = std::make_tuple
									(
										get_at.operator()<0, CAct::appearFromEdge::_0>(p),
										get_at.operator()<1, CAct::appearFromEdge::_0>(p),
										get_at.operator()<2, CAct::appearFromEdge::_0>(p),
										get_at.operator()<3, CAct::appearFromEdge::_0>(p),
										get_at.operator()<4, CAct::appearFromEdge::_0>(p),
										get_at.operator()<5, CAct::appearFromEdge::_0>(p)
									);
									break;
								}
								case 1:
								{
									auto p = LevelData::ParseParameters<CAct::appearFromEdge::_1>(data_params);
									params = std::make_tuple
									(
										get_at.operator()<0, CAct::appearFromEdge::_1>(p),
										get_at.operator()<1, CAct::appearFromEdge::_1>(p),
										get_at.operator()<2, CAct::appearFromEdge::_1>(p),
										get_at.operator()<3, CAct::appearFromEdge::_1>(p)
									);
									break;
								}
								case 2:
								{
									auto p = LevelData::ParseParameters<CAct::appearFromEdge::_2>(data_params);
									params = std::make_tuple
									(
										get_at.operator()<0, CAct::appearFromEdge::_2>(p),
										get_at.operator()<1, CAct::appearFromEdge::_2>(p),
										get_at.operator()<2, CAct::appearFromEdge::_2>(p),
										get_at.operator()<3, CAct::appearFromEdge::_2>(p)
									);
									break;
								}
								case 3:
								{
									auto p = LevelData::ParseParameters<CAct::appearFromEdge::_3>(data_params);
									params = std::make_tuple
									(
										get_at.operator()<0, CAct::appearFromEdge::_3>(p),
										get_at.operator()<1, CAct::appearFromEdge::_3>(p),
										get_at.operator()<2, CAct::appearFromEdge::_3>(p)
									);
									break;
								}
								default:
								{
									throw Error(U"`appearFromEdge` overload index is invalid. (valid range: 0 ~ {})"_fmt(CAct::appearFromEdge::Count - 1));
								}
								}
							}
							else
							{
								throw Error(U"`{}` is not registered as action (method) name."_fmt(name));
							}
						}

						// アクションデータのパース1周したら、リストに追加
						actionDataList << LevelData::ActionData{ name, params, probability };
					}
				}
				else
				{
					throw Error(U"`actionData` is not array type.");
				}

				// 1レベル走査したら、結果に追加
				result << LevelData{ timeLimit, similarity, breedData, intervalData, actionDataList };
			}

		}
		return result;
	}
	throw Error(U"`level_data.json is invalid format.`");
}
