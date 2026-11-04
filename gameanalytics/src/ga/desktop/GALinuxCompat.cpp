// libGameAnalytics.a for Linux is built on a glibc >= 2.38 runner, where the
// C23 headers redirect strtol & co. to __isoc23_* symbols. The Defold build
// server links against an older glibc that does not have them, so provide
// them here. They only differ from the classic functions in also accepting
// the "0b" binary prefix, which the SDK never parses.
//
// Remove this file once gameanalytics-sdk-cpp builds its Linux release on an
// older glibc.

#if defined(DM_PLATFORM_LINUX)

#include <stdlib.h>

extern "C"
{
    long __isoc23_strtol(const char* str, char** end, int base)
    {
        return strtol(str, end, base);
    }

    unsigned long __isoc23_strtoul(const char* str, char** end, int base)
    {
        return strtoul(str, end, base);
    }

    long long __isoc23_strtoll(const char* str, char** end, int base)
    {
        return strtoll(str, end, base);
    }

    unsigned long long __isoc23_strtoull(const char* str, char** end, int base)
    {
        return strtoull(str, end, base);
    }
}

#endif // DM_PLATFORM_LINUX
