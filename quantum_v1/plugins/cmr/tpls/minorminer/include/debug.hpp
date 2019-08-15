#pragma once

#ifdef CPPDEBUG
#ifdef NDBUG
#undef NDBUG
#endif
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#define minorminer_assert(X) assert(X)
#define ONDEBUG(X) X
#else
#ifndef minorminer_assert
#define minorminer_assert(X)
#define ONDEBUG(X) /*X*/
#endif
#endif
