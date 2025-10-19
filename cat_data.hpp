#pragma once
#include <Siv3D.hpp>

/// @brief UFO猫のデータ（品種や模様など）
struct CatData
{
	enum class Pattern
	{
		// トラ と タビー、ミケ と キャリコ はほぼ同義なので同じ番号にしておく
		None,
		Tora,
		Tabby = 1,
		Mike,
		Calico = 2,
		Hachiware,
		Sabi
	};

	// ID（使わんかも）
	int32 id;
	// 品種
	String breed;
	// 色 複数あればその全て
	Array<Color> color;
	// 模様の種類
	Pattern pattern;
	// 長毛か
	bool isLongHair;

	/// @brief デフォルトコンストラクタ
	CatData() = default;

	/// @brief 全てのフィールドを初期化するコンストラクタ
	/// @param id ID
	/// @param breed 品種
	/// @param color 色
	/// @param pattern 模様
	/// @param isLongHair 長毛か
	CatData(int32 id, String breed, Array<Color> color, Pattern pattern, bool isLongHair)
		: id{ id }
		, breed{ breed }
		, color{ color }
		, pattern{ pattern }
		, isLongHair{ isLongHair }
	{ }

	/// @brief データが等しいかどうか id の比較で比べる
	/// @param target 比較対象
	/// @return 等しいデータなら true
	bool operator==(const CatData &target) const;
};
