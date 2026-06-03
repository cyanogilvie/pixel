#ifndef PIXELINT_H
#define PIXELINT_H

#include <config.h>
#include <stdint.h>
#include <stddef.h>
#include <defer.h>

#include <pixel.h>

#if !HAVE_UNREACHABLE
# if defined(__GNUC__) || defined(__clang__)
#  define unreachable() __builtin_unreachable()
# elif defined(_MSC_VER)
#  define unreachable() __assume(0)
# else
#  define unreachable() ((void)0)
# endif
#endif

#endif
