#pragma once

#include "likely.h"

// overload if you want your own abort fn - w/e
#ifndef CHECK_ABORT_FUNCTION
// need that prototype def
# include <stdlib.h>
# define CHECK_ABORT_FUNCTION abort
#endif

// check and bail if nope
#ifndef CHECK
# define CHECK(expr) if (!likely(expr)) { CHECK_ABORT_FUNCTION(); }
#endif

// check and bail if nope. however, this only works if `-D DCHECK_IS_ON`
// or equivalent is defined
#ifdef DCHECK_IS_ON
#  define DCHECK(...) CHECK(__VA_ARGS__);
#else
#  define DCHECK(expr) ((void) (expr));
#endif


// equivalent to DCEHCK(false)
#ifndef NOTREACHED
#define NOTREACHED() DCHECK(false)
#endif

// alias `NOTREACHED()` because YOLO
#ifndef NOTREACHABLE
#define NOTREACHABLE NOTREACHED
#endif
