# pragma once

namespace UFOCat
{
	/// @brief UFO猫のデータ（品種や模様など）
	struct CatData
	{
		// ID（使わんかも）
		int32 id;

		// 品種
		String breed;

		// 色 複数あればその全て
		HashTable<String, Color> colors;

		// 模様の種類
		String pattern;

		// 長毛か
		// 表示はしないけど、類似条件のためのデータとして使う
		bool isLongHair;

		/// @brief デフォルトコンストラクタ
		CatData() = default;

		/// @brief 全てのフィールドを初期化するコンストラクタ
		/// @param id ID
		/// @param breed 品種
		/// @param colors すべての毛色と名前の組み合わせ
		/// @param pattern 模様
		/// @param isLongHair 長毛か
		CatData(int32 id, String breed, HashTable<String, Color> colors, String pattern, bool isLongHair)
			: id{ id }
			, breed{ breed }
			, colors{ colors }
			, pattern{ pattern }
			, isLongHair{ isLongHair }
		{
		}

		uint32 getSameDataCount(const CatData& target) const;

		/// @brief データが等しいかどうか id の比較で比べる
		/// @param target 比較対象
		/// @return 等しいデータなら true
		bool operator==(const CatData& target) const;
	};
}
