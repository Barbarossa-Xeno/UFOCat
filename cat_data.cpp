#include "cat_data.hpp"

bool CatData::operator==(const CatData &target) const
{
    return id == target.id;
}
