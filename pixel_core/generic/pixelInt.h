#ifndef PIXELINT_H
#define PIXELINT_H

#include <config.h>
#include <stdint.h>
#include <stddef.h>
#include <defer.h>

#include <pixel.h>

// 2d.c: the recycling of large pixel buffers (pixel::buffer_cache)
MODULE_SCOPE void pixbuf_cache_info(Tcl_WideInt limit, Tcl_WideInt* out_limit, Tcl_WideInt* out_held, int* out_n);

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
