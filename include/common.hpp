#pragma once

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
    #define OS_WIN
#endif
#if defined(__APPLE__)
    #define OS_MAC
    #define OS_UNIX
#endif
#if defined(__linux) || defined(__linux__)
    #define OS_LINUX
#endif
#if defined(__unix) || defined(__unix__)
    #define OS_UNIX
#endif

#if defined(__posix) || defined(_POSIX_VERSION)
    #define OS_POSIX
#endif

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
    #define API __declspec(dllexport)
#else
    #define API
#endif

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
    #define CAPI extern "C" __declspec(dllexport)
#else
    #define CAPI extern "C"
#endif