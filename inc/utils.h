#ifndef AMVK_UTIL_H
#define AMVK_UTIL_H

#include <vector>
#include <limits>
#include <initializer_list>
#include <stdio.h>
#include <stdlib.h>
#include "macro.h"

namespace Utils {

static constexpr const int MAX_SIZE_NOT_FOUND = std::numeric_limits<int>::lowest();
static constexpr const int MIN_SIZE_NOT_FOUND = std::numeric_limits<int>::max();

template <class T>
inline int maxVectorSize(std::initializer_list<std::vector<T>& > vectors)
{
	int max = MAX_SIZE_NOT_FOUND;
	for (const auto& elem : vectors)
		if (elem.size() > max)
			max = elem.size();
	return max;
}

template <class T>
inline int minVectorSize(std::initializer_list<std::vector<T>& > vectors)
{
	int min = MIN_SIZE_NOT_FOUND;
	for (const auto& elem : vectors)
		if (elem.size() < min)
			min = elem.size();
	return min;
}

inline float frand(float from, float to) 
{
	return from + (float) rand() / ((float) RAND_MAX / (to - from));
}

inline glm::vec3 randVec3(float from, float to) 
{
	float x = frand(from, to);
	float y = frand(from, to);
	float z = frand(from, to);
	return glm::vec3(x, y, z);
}


/*
void* alignedAlloc(size_t size, size_t alignment)
{
	void *data = nullptr;
#if defined(_MSC_VER) || defined(__MINGW32__)
	data = _aligned_malloc(size, alignment);
#else 
	int res = posix_memalign(&data, alignment, size);
	if (res != 0)
		data = nullptr;
#endif
	return data;
}

void alignedFree(void* data)
{
#if	defined(_MSC_VER) || defined(__MINGW32__)
	_aligned_free(data);
#else 
	free(data);
#endif
}
*/
};
#endif
