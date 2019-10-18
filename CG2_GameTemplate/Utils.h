#pragma once

#include "stl.h"

template <typename T, typename IntType>
int InsertStable(T &&elem, stl::vector<T> &v, stl::vector<IntType> &freeIds)
{
    if (!freeIds.size())
    {
        v.push_back(stl::move(elem));
        return v.size() - 1;
    }
    else
    {
        int id = freeIds.back();
        v[freeIds.back()] = stl::move(elem);
        freeIds.pop_back();
        return id;
    }
}

template <typename T, typename IntType>
int InsertStable(const T &elem, stl::vector<T> &v, stl::vector<IntType> &freeIds)
{
    if (!freeIds.size())
    {
        v.push_back(elem);
        return v.size() - 1;
    }
    else
    {
        int id = freeIds.back();
        v[freeIds.back()] = elem;
        freeIds.pop_back();
        return id;
    }
}

template <typename T, typename IntType>
void RemoveStable(unsigned id, stl::vector<T> &v, stl::vector<IntType> &freeIds)
{
    if (id >= v.size())
        return;

    if (id == v.size() - 1)
        v.pop_back();
    else
        v[id].~T();
        freeIds.push_back(id);
}

template <typename IntType1, typename IntType2, typename VType1, typename VType2>
inline bool CmpVec(const VType1 &v1, const VType2 &v2)
{
    return *(const IntType1 *)&v1 == *(const IntType2 *)&v2 &&
        *((const IntType1 *)&v1 + 1) == *((const IntType2 *)&v2 + 1);
}

template <typename VType1, typename VType2>
inline bool CmpIntVec(const VType1 &v1, const VType2 &v2)
{
    return CmpVec<int, int>(v1, v2);
}

bool OneOf(char symbol, const char *set);

stl::vector<char *> SplitText(char *text, size_t length, const char *splitters, const char *ignoreBlockEnders);