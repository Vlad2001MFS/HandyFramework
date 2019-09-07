#pragma once
#include "hdBuildConfig.hpp"
#include <cstdint>

#if defined(HD_COMPILER_VC)
#   define HD_FORCEINLINE __forceinline
#elif defined(HD_COMPILER_GCC)
#   define HD_FORCEINLINE __attribute__((always_inline)) inline
#endif

#define HD_DELETE(ptr) do { delete (ptr); (ptr) = nullptr; } while (false)
#define HD_DELETE_ARRAY(ptr) do { delete[] (ptr); (ptr) = nullptr; } while (false)
#define HD_FLAG_EXIST(flags, flag) (((flags) & (flag)) == (flag))
#define HD_ARRAYSIZE(array) (sizeof(array) / sizeof((array)[0]))

namespace hd {
namespace details {

template <size_t S> struct _EnumFlagIntForSize;
template <> struct _EnumFlagIntForSize<1> { using type = signed char; };
template <> struct _EnumFlagIntForSize<2> { using type = short; };
template <> struct _EnumFlagIntForSize<4> { using type = int; };
template <class T> struct _EnumFlagSizedInt { using type = typename _EnumFlagIntForSize<sizeof(T)>::type; };

}
}

#define HD_DECL_ENUM_OPS(enumType) \
   inline enumType operator | (enumType a, enumType b) \
       { return enumType(((hd::details::_EnumFlagSizedInt<enumType>::type)a) | ((hd::details::_EnumFlagSizedInt<enumType>::type)b)); } \
   inline enumType &operator |= (enumType &a, enumType b) \
       { return (enumType &)(((hd::details::_EnumFlagSizedInt<enumType>::type &)a) |= ((hd::details::_EnumFlagSizedInt<enumType>::type)b)); } \
   inline enumType operator & (enumType a, enumType b) \
       { return enumType(((hd::details::_EnumFlagSizedInt<enumType>::type)a) & ((hd::details::_EnumFlagSizedInt<enumType>::type)b)); } \
   inline enumType &operator &= (enumType &a, enumType b) \
       { return (enumType &)(((hd::details::_EnumFlagSizedInt<enumType>::type &)a) &= ((hd::details::_EnumFlagSizedInt<enumType>::type)b)); } \
   inline enumType operator ~ (enumType a) \
       { return enumType(~((hd::details::_EnumFlagSizedInt<enumType>::type)a)); } \
   inline enumType operator ^ (enumType a, enumType b) \
       { return enumType(((hd::details::_EnumFlagSizedInt<enumType>::type)a) ^ ((hd::details::_EnumFlagSizedInt<enumType>::type)b)); } \
   inline enumType &operator ^= (enumType &a, enumType b) \
       { return (enumType &)(((hd::details::_EnumFlagSizedInt<enumType>::type &)a) ^= ((hd::details::_EnumFlagSizedInt<enumType>::type)b)); }

#define HD_STATIC_CLASS(className) \
    HD_NONCOPYABLE_CLASS(className); \
    HD_NONMOVEABLE_CLASS(className); \
private: \
    className() = delete; \
    ~className() = delete

#define HD_NONCOPYABLE_CLASS(className) \
private: \
    className(const className&) = delete; \
    className &operator=(const className&) = delete

#define HD_NONMOVEABLE_CLASS(className) \
private: \
    className(className&&) = delete; \
    className &operator=(className&&) = delete

#define HD_SINGLETON_CLASS(className) \
    HD_NONCOPYABLE_CLASS(className); \
    HD_NONMOVEABLE_CLASS(className); \
public: \
    static className &getInstance() { \
        static className obj; \
        return obj; \
    } \
private: \
    className(); \
    ~className()
