# pragma once

namespace UFOCat::Core
{
	/// @brief UFO猫のデータ（品種や模様など）
	struct CatData
	{
		// ID
		size_t id;

		// 品種
		String breed;

		// 色 複数あればその全てが含まれており、色名と実際の値のペアで表される
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
		CatData(size_t id, String breed, HashTable<String, Color> colors, String pattern, bool isLongHair)
			: id{ id }
			, breed{ breed }
			, colors{ colors }
			, pattern{ pattern }
			, isLongHair{ isLongHair }
		{}

		/// @brief 自身と比較対象で共通している特徴の数を取得する @n
		/// 実質的に模様が同じだったり、毛色に含まれている色が同じだったりすることも「同じ特徴」としてカウントする
		/// @param target 比較対象
		/// @return 同じ特徴の数
		uint32 getSameDataCount(const CatData& target) const;

		/// @brief データが等しいかどうかインスタンスではなく id の比較で比べる
		/// @param target 比較対象
		/// @return 等しいデータなら true
		bool operator==(const CatData& target) const;
	};
}
