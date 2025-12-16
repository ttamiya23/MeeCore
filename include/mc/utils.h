#ifndef MC_UTILS_H_
#define MC_UTILS_H_

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#define MC_ON 1
#define MC_OFF 0
#define MC_BIT_CHECK(val, bit) ((val) & (1U << (bit)))
#define MC_BIT_SET(val, bit) ((val) |= (1U << (bit)))
#define MC_BIT_CLEAR(val, bit) ((val) &= ~(1U << (bit)))
#define MC_BIT_TOGGLE(val, bit) ((val) ^= (1U << (bit)))
#define MC_STR_HELPER(x) #x
#define MC_STR(x) MC_STR_HELPER(x)

#ifndef MC_MIN
#define MC_MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef MC_MAX
#define MC_MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif

#ifndef MC_ARRAY_SIZE
#define MC_ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
#endif

// Magic number to define whether something is initialized or not
#define MC_INITIALIZED 0x23

void mc_assert_handler(const char *expr, const char *file, int line);

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#define MC_ASSERT(expr)                                   \
    if (!(expr))                                          \
    {                                                     \
        mc_assert_handler(#expr, __FILENAME__, __LINE__); \
    }

#endif /* MC_UTILS_H_ */