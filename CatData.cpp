# include "CatData.hpp"

namespace UFOCat
{
	uint32 CatData::getSameDataCount(const CatData& target) const
	{
		uint32 count = 0;

		if (breed == target.breed)
		{
			++count;
		}

		if (pattern == target.pattern)
		{
			++count;
		}
		// トラ と タビー、ミケ と キャリコ はほぼ同義なので、「等しい」ものとしてカウントする
		else if ((pattern == U"トラ" and target.pattern == U"タビー")
			or (pattern == U"タビー" and target.pattern == U"トラ"))
		{
			++count;
		}
		else if ((pattern == U"三毛" and target.pattern == U"キャリコ")
			or (pattern == U"キャリコ" and target.pattern == U"三毛"))
		{
			++count;
		}

		if (isLongHair == target.isLongHair)
		{
			++count;
		}

		for (const auto& [name, color] : colors)
		{
			if (target.colors.contains(name))
			{
				++count;
			}
		}

		return count;
	}

	bool CatData::operator==(const CatData& target) const
	{
		return id == target.id;
	}
}
