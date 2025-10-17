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

Array<CatObject> GameManager::LoadData()
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
