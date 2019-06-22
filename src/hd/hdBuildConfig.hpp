#pragma once

#ifdef NDEBUG
#   define HD_BUILDMODE_RELEASE
#else
#   define HD_BUILDMODE_DEBUG
#endif

#if defined(_MSC_VER)
#   define HD_COMPILER_VC
#elif defined(__GNUC__)
#   define HD_COMPILER_GCC
#else
#   pragma error("Failed to detect compiler")
#endif

#if defined(_WIN32) || defined(_WIN64)
#   define HD_PLATFORM_WIN
#elif defined(__unix__)
#   define HD_PLATFORM_UNIX
#else
#   pragma error("Failed to detect host OS")
#endif

#ifdef HD_COMPILER_VC
#   ifdef _M_X64
#       define HD_ARCH_X64
#   elif defined(_M_IX86)
#       define HD_ARCH_X86
#   else
#       pragma error("Failed to detect host platform architecture")
#   endif
#elif defined(HD_COMPILER_GCC)
#   if __SIZEOF_POINTER__ == 8
#       define HD_ARCH_X64
#   elif __SIZEOF_POINTER__ == 4
#       define HD_ARCH_X86
#   else
#       pragma error("Failed to detect host platform architecture")
#   endif
#endif

#define HD_GAPI_D3D11