#pragma once
#include "../hdBuildConfig.hpp"
#include <exception>
#include <cstdint>

#if defined(HD_COMPILER_VC)
#   define HD_FORCEINLINE __forceinline
#elif defined(HD_COMPILER_GCC)
#   define HD_FORCEINLINE __attribute__((always_inline)) inline
#endif

#define HD_DELETE(ptr) do { delete ptr; ptr = nullptr; } while (false)
#define HD_DELETE_ARRAY(ptr) do { delete[] ptr; ptr = nullptr; } while (false)
#define HD_FLAG_EXIST(flags, flag) (((flags) & (flag)) == (flag))
#define HD_ARRAYSIZE(array) (sizeof(array) / sizeof(array[0]))
#define HD_UNUSED(obj) do { ((void)(obj)); } while (false)

template <size_t S> struct _EnumFlagIntForSize;
template <> struct _EnumFlagIntForSize<1> { typedef signed char type; };
template <> struct _EnumFlagIntForSize<2> { typedef signed short type; };
template <> struct _EnumFlagIntForSize<4> { typedef signed int type; };
template <class T> struct _EnumFlagSizedInt { typedef typename _EnumFlagIntForSize<sizeof(T)>::type type; };

#define HD_DECL_ENUM_OPS(enumType) \
   inline enumType operator | (enumType a, enumType b) \
       { return enumType(((_EnumFlagSizedInt<enumType>::type)a) | ((_EnumFlagSizedInt<enumType>::type)b)); } \
   inline enumType &operator |= (enumType &a, enumType b) \
       { return (enumType &)(((_EnumFlagSizedInt<enumType>::type &)a) |= ((_EnumFlagSizedInt<enumType>::type)b)); } \
   inline enumType operator & (enumType a, enumType b) \
       { return enumType(((_EnumFlagSizedInt<enumType>::type)a) & ((_EnumFlagSizedInt<enumType>::type)b)); } \
   inline enumType &operator &= (enumType &a, enumType b) \
       { return (enumType &)(((_EnumFlagSizedInt<enumType>::type &)a) &= ((_EnumFlagSizedInt<enumType>::type)b)); } \
   inline enumType operator ~ (enumType a) \
       { return enumType(~((_EnumFlagSizedInt<enumType>::type)a)); } \
   inline enumType operator ^ (enumType a, enumType b) \
       { return enumType(((_EnumFlagSizedInt<enumType>::type)a) ^ ((_EnumFlagSizedInt<enumType>::type)b)); } \
   inline enumType &operator ^= (enumType &a, enumType b) \
       { return (enumType &)(((_EnumFlagSizedInt<enumType>::type &)a) ^= ((_EnumFlagSizedInt<enumType>::type)b)); }

#define HD_LOG_INFO(fmt, ...) hd::debugLog("", fmt, __VA_ARGS__)
#define HD_LOG_WARNING(fmt, ...) do { hd::debugLog("\n######## WARNING ########\n", "FILE: %s\nFUNCTION: %s\nLINE: %d\n-------------------------\n" fmt "\n-------------------------\n", __FILE__, __FUNCSIG__, __LINE__, __VA_ARGS__); } while(0)
#define HD_LOG_ERROR(fmt, ...) do { hd::debugLog("\n######## ERROR ########\n", "FILE: %s\nFUNCTION: %s\nLINE: %d\n-------------------------\n" fmt "\n-------------------------\n", __FILE__, __FUNCSIG__, __LINE__, __VA_ARGS__); abort(); } while(0)
#define HD_ASSERT(expr) do { if (expr) {} else { hd::debugLog("\n#### ASSERTION FAILED ####\n", "FILE: %s\nFUNCTION: %s\nLINE: %d\nEXPRESSION: %s", __FILE__, __FUNCSIG__, __LINE__, #expr); std::terminate(); } } while(0)

#define HD_STATIC_CLASS(className) \
private: \
    className() = delete; \
    className(const className&) = delete; \
    className(const className&&) = delete; \
    ~className() = delete;

#define HD_NONCOPYABLE_CLASS(className) \
private: \
    className(const className&) = delete; \
    className &operator=(const className&) = delete;

namespace hd {

void debugLog(const char *prefix, const char *fmt, ...);

}
