#include "CatData.hpp"

uint32 CatData::getSameDataCount(const CatData &target) const
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
	if (isLongHair == target.isLongHair)
	{
		++count;
	}
	for (const auto& c : color)
	{
		if (target.color.contains(c))
		{
			++count;
		}
	}
	return count;
}

bool CatData::operator==(const CatData &target) const
{
    return id == target.id;
}
