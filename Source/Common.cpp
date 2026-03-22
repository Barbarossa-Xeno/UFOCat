# include "Common.hpp"

namespace UFOCat
{
	void BrightenCursor()
	{
		if (Cursor::OnClientRect())
		{
			Circle{ Cursor::Pos(), 0.02 * Max(Scene::Width(), Scene::Height()) }.draw(ColorF{1.0, 0.7});
		}
	}

	void DrawPolkaDotBackground(int32 cellSize, double circleScale, const ColorF &color)
	{
		for (int32 y = 0; y < (Scene::Height() / cellSize); ++y)
		{
			for (int32 x = 0; x < (Scene::Width() / cellSize); ++x)
			{
				if (IsEven(x + y))
				{
					Circle{ (Vec2{ (x + 0.5), (y + 0.5) } *cellSize), (cellSize * circleScale) }.draw(color);
				}
			}
		}
	}

	String Cat(size_t id)
	{
		return U"Cat{}"_fmt(id);
	}

	Array<CatData> LoadCatData()
	{
		// JSON ファイルからデータを読み込む
		const JSON json = JSON::Load(U"cat_data.json");

		// もし読み込みに失敗したら
		if (not json)
		{
			throw Error{ U"cat_data.json の読み込みに失敗しました。" };
		}

		// data プロパティの中に配列として格納されている（元JSON参照）
		const auto&& data = json[U"data"];

		// 結果格納用
		Array<CatData> results;

		if (not data.isArray())
		{
			throw Error{ U"cat_data.json の `data` プロパティが配列形式になっていません。" };
		}

		for (const auto &d : data)
		{
			if (not d.value.isObject())
			{
				throw Error{ U"cat_data.json の `data` のいずれかの要素がオブジェクト形式になっていません。\n{}"_fmt(d.value.format()) };
			};

			size_t id = d.value[U"id"].get<size_t>();
			String breed = d.value[U"breed"].get<String>();

			// 以後、仮の文字列格納用変数は data_ で始める
			String data_color = d.value[U"color"].get<String>();
			String pattern = d.value[U"pattern"].get<String>();

			// ダイリュートかどうか
			bool isDilute = data_color.substr(0, 2) == U"D_";

			// ダイリュートなら先頭の "D_" を取り除く
			data_color = isDilute ? data_color.substr(2) : data_color;

			// テーブルをつくり、色データを区切り文字で分割して取得しながら記録
			HashTable<String, Color> colors;

			for (const auto &name : data_color.split(U'|'))
			{
				Color temp = Palette::White;

				if (name == U"白")
				{
					temp = Color{ U"#f2fafe" };
				}
				else if (name == U"黒")
				{
					temp = Color{ U"#272a2e" };
				}
				else if (name == U"灰")
				{
					temp = Color{ U"#9da5a9" };
				}
				else if (name == U"うす茶")
				{
					temp = Color{ U"#e8ae77" };
				}
				else if (name == U"おうど")
				{
					temp = Color{ U"#a87e39" };
				}
				else if (name == U"オレンジ")
				{
					temp = Color{ U"#d67d36" };
				}
				else if (name == U"クリーム")
				{
					temp = Color{ U"#e5d5b6" };
				}
				else if (name == U"きじ")
				{
					temp = Color{ U"#7e7360" };
				}
				else if (name == U"銀")
				{
					temp = Color{ U"#a8b7c1" };
				}
				else if (name == U"茶")
				{
					temp = Color{ U"#805308" };
				}
				else if (name == U"フォーン")
				{
					temp = Color{ U"#d3c5a8" };
				}
				else if (name == U"灰青")
				{
					temp = Color{ U"#70748d" };
				}
				else if (name == U"金")
				{
					temp = Color{ U"#aa9263" };
				}

				if (isDilute)
				{
					HSV hsv{ temp };

					// ダイリュートは薄い毛色なので
					// 彩度を半分に、明度を少し上げる処理を施す
					hsv.s *= 0.5;
					hsv.v += 0.1;
					temp = Color{ hsv };
				}

				colors[name] = temp;
			}
				
			bool isLongHair = d.value[U"isLongHair"].get<bool>();

			// 作成したインスタンスを格納
			results << CatData{ id, breed, colors, pattern, isLongHair };
		}

		return results;
	}

	Array<LevelData> LoadLevelData()
	{
		// JSON からデータを読み込む
		const JSON json = JSON::Load(U"level_data.json");

		// もし読み込みに失敗したら
		if (not json)
		{
			throw Error{ U"level_data.json の読み込みに失敗しました。" };
		}

		// data プロパティの中に配列として格納されている各レベルのデータ（元JSON参照）
		// まずは、配列形式でいろんなPhaseのデータが入っているのが前提
		if (not json[U"data"].isArray())
		{
			throw Error(U"level_data.json の `data` プロパティが配列形式になっていません。");
		}

		// 結果格納用
		Array<LevelData> result;

		// JSONデータの全てを走査
		for (const auto &data : json[U"data"])
		{
			if (not data.value.isObject())
			{
				throw Error(U"level_data.json の `data` のいずれかの要素がオブジェクト形式になっていません。\n{}"_fmt(data.value.format()));
			}

			/*
			 * 以後、JSONから抽出する仮の文字列格納用変数は data_ で始める
			 * JSONのプロパティ値が
			 * - 数値型の場合はs3d::JSON の get<uint32>() を使ってパース
			 * - 文字列型の場合は、複数の型表現を包括しているため
			 *	 一旦 getString() で文字列として取得し、
			 *	 Duration や Rect、EasingFunction に変換する
			 */

			 // 1レベルの時間制限
			Duration timeLimit = LevelData::ParseDuration(data.value[U"timeLimit"].get<String>());

			// 類似度
			uint32 similarity = data.value[U"similarity"].get<uint32>();

			// 品種データがオブジェクト形式じゃないならエラー
			if (not data.value[U"breedData"].isObject())
			{
				throw Error(U"level_data.json[`data`] 内の `breedData` プロパティがオブジェクト形式になっていません。\n{}"_fmt(data.value[U"breedData"].format()));
			}

			// 品種データ
			LevelData::BreedData breedData
			{
				data.value[U"breedData"][U"similar"].get<uint32>(),
				data.value[U"breedData"][U"other"].get<uint32>()
			};

			// 出現ペースのデータがオブジェクト形式じゃないならエラー
			if (not data.value[U"intervalData"].isObject())
			{
				throw Error(U"level_data.json[`data`] 内の `intervalData` プロパティがオブジェクト形式になっていません。\n{}"_fmt(data.value[U"intervalData"].format()));
			}

			// 出現ペース
			LevelData::IntervalData intervalData
			{
				data.value[U"intervalData"][U"count"].get<uint32>(),
				LevelData::ParseDuration(data.value[U"intervalData"][U"period"].get<String>())
			};

			// アクションデータが配列形式じゃないならエラー
			if (not data.value[U"actionData"].isArray())
			{
				throw Error(U"level_data.json[`data`] 内の `actionData` プロパティが配列形式になっていません。\n{}"_fmt(data.value[U"actionData"].format()));
			}

			// このレベルで使われるアクションのデータを格納するリスト
			Array<LevelData::ActionData> actionDataList;

			// アクションデータを全走査
			for (const auto &actionData : data.value[U"actionData"])
			{
				// そのアクションの引数リストが配列か nullでないならエラー
				// null の場合引数無しのアクションということであり、bound に該当する
				if (not actionData.value[U"params"].isArray() and
					not actionData.value[U"params"].isNull())
				{
					throw Error(U"level_data.json[`data`][`actionData`] 内の `params` プロパティが配列形式になっていません。\n{}"_fmt(actionData.value[U"params"].format()));
				}

				// アクション名（メソッド名）
				const String &name = actionData.value[U"name"].get<String>();

				// オーバーロード番号（何がどれに対応するかは、`namespace Action` 参照）
				const size_t overload = actionData.value[U"overload"].get<size_t>();

				// 発生確率
				const double probability = actionData.value[U"probability"].get<double>();

				/* -- ここから実際の引数として使えるタプルにするためのパース処理 -- */

				// 引数リスト
				Action::Generic params;

				// 各アクションの動作とシグネチャは `namespace Action` および `Core::CatObject` 参照
				// `bound: 画面端で跳ね返る`
				if (name == U"bound")
				{
					// 引数無ししかシグネチャがないので monostate
					params = std::monostate{};
				}
				// `cross: 画面縦横断`
				else if (name == U"cross")
				{
					switch (overload)
					{
						case 0:
						{
							params = LevelData::ParseParameters<Action::Cross::_0>(actionData.value[U"params"]);
							break;
						}
						case 1:
						{
							params = LevelData::ParseParameters<Action::Cross::_1>(actionData.value[U"params"]);
							break;
						}
						default:
						{
							throw Error(U"level_data.json[`data`][`actionData`][`params`] 内で定義されたアクション `cross` のオーバーロード番号が不正です。"
										+ U"(有効範囲 : 0 ~ {})\n"_fmt(Action::Cross::Count - 1)
										+ U"{}"_fmt(actionData.value[U"params"].format()));
						}
					}
				}
				// `appear: 画面内にランダムに出現`
				else if (name == U"appear")
				{
					switch (overload)
					{
						case 0:
						{
							params = LevelData::ParseParameters<Action::Appear::_0>(actionData.value[U"params"]);
							break;
						}
						case 1:
						{
							params = LevelData::ParseParameters<Action::Appear::_1>(actionData.value[U"params"]);
							break;
						}
						case 2:
						{
							params = LevelData::ParseParameters<Action::Appear::_2>(actionData.value[U"params"]);
							break;
						}
						case 3:
						{
							params = LevelData::ParseParameters<Action::Appear::_3>(actionData.value[U"params"]);
							break;
						}
						case 4:
						{
							params = LevelData::ParseParameters<Action::Appear::_4>(actionData.value[U"params"]);
							break;
						}
						case 5:
						{
							params = LevelData::ParseParameters<Action::Appear::_5>(actionData.value[U"params"]);
							break;
						}
						case 6:
						{
							params = LevelData::ParseParameters<Action::Appear::_6>(actionData.value[U"params"]);
							break;
						}
						case 7:
						{
							params = LevelData::ParseParameters<Action::Appear::_7>(actionData.value[U"params"]);
							break;
						}
						default:
						{
							throw Error(U"level_data.json[`data`][`actionData`][`params`] 内で定義されたアクション `appear` のオーバーロード番号が不正です。"
										+ U"(有効範囲 : 0 ~ {})\n"_fmt(Action::Appear::Count - 1)
										+ U"{}"_fmt(actionData.value[U"params"].format()));
						}
					}
				}
				// `appearFromEdge: 画面端からひょっこり出現`
				else if (name == U"appearFromEdge")
				{
					switch (overload)
					{
						case 0:
						{
							params = LevelData::ParseParameters<Action::AppearFromEdge::_0>(actionData.value[U"params"]);
							break;
						}
						case 1:
						{
							params = LevelData::ParseParameters<Action::AppearFromEdge::_1>(actionData.value[U"params"]);
							break;
						}
						case 2:
						{
							params = LevelData::ParseParameters<Action::AppearFromEdge::_2>(actionData.value[U"params"]);
							break;
						}
						case 3:
						{
							params = LevelData::ParseParameters<Action::AppearFromEdge::_3>(actionData.value[U"params"]);
							break;
						}
						default:
						{
							throw Error(U"level_data.json[`data`][`actionData`][`params`] 内で定義されたアクション `appearFromEdge` のオーバーロード番号が不正です。"
										+ U"(有効範囲 : 0 ~ {})\n"_fmt(Action::AppearFromEdge::Count - 1)
										+ U"{}"_fmt(actionData.value[U"params"].format()));
						}
					}
				}
				else
				{
					throw Error(U"level_data.json[`data`][`actionData`] 内で定義されたアクションの名前 `{}` は不正です。\n{}"_fmt(name, actionData.value.format()));
				}

				// アクションデータのパースが1周したら、リストに追加
				actionDataList << LevelData::ActionData{ name, params, probability };
			}

			// 1レベル走査したら、結果に追加
			result << LevelData{ timeLimit, similarity, breedData, intervalData, actionDataList };
		}

		return result;
	}

	Array<BackgroundData> LoadBackgrounds()
	{
		// 背景画像をイメージとして読み込む
		auto &&bgs = FileSystem::DirectoryContents(U"texture/background").map([](const String &path)
		{
			return Image{ path };
		});

		// 各背景画像のピクセルの色の平均値
		auto &&means = bgs.map([](const Image &bg)
		{			
			// 全てのピクセルの色をリストで返す
			// というわけで、この map の戻り値は 各画像のピクセル配列の配列 = 二次元配列 になる
			return bg.asArray();
		})
		.map([](const Array<Color> &pixels)
		{
			// 次はそれぞれのイメージのピクセルの平均をとる

			// 各チャンネルの和を入れる
			uint32 sumR = 0;
			uint32 sumG = 0;
			uint32 sumB = 0;

			for (auto &&pixel : pixels) {
				sumR += pixel.r;
				sumG += pixel.g;
				sumB += pixel.b;
			}

			// ピクセル数で割って平均化
			const auto &[aveR, aveG, aveB] = Vec3{ sumR, sumG, sumB } / pixels.size();

			// もとの色は Color なので 和を平均化すると 0 ~ 255 の値にになっているはず
			// それをさらに 255 で割って 0 ~ 1 に正規化
			return ColorF{ aveR / 255, aveG / 255, aveB / 255 };
		});

		// イメージと色のリスト2つに対して並列にループ
		return Range(0, bgs.size() - 1).map([&](size_t i)
		{
			// 平均色をモノクロ -> 色反転 したらだいたい反対の色になって見やすくなる
			const ColorF inversed{ 1.0 - means[i].grayscale() };

			return BackgroundData{ Texture{ bgs[i] }, inversed };
		});
	}
}

