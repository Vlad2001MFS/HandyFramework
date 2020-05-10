#pragma once
#include <cstddef>
#include <cstdint>

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

#if defined(HD_COMPILER_VC)
#   define HD_FORCEINLINE __forceinline
#elif defined(HD_COMPILER_GCC)
#   define HD_FORCEINLINE __attribute__((always_inline)) inline
#   define __FUNCSIG__ __PRETTY_FUNCTION__
#endif

#define HD_DELETE(ptr) do { delete (ptr); (ptr) = nullptr; } while (false)
#define HD_DELETE_ARRAY(ptr) do { delete[] (ptr); (ptr) = nullptr; } while (false)
#define HD_FLAG_EXIST(flags, flag) (((flags) & (flag)) == (flag))
#define HD_ARRAYSIZE(array) (sizeof(array) / sizeof((array)[0]))
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

#define HD_RUN_BEFORE_MAIN(...) __HD_AUTO_RUN_BEFORE_MAIN(__VA_ARGS__ ## __LINE__)

#define __HD_RUN_BEFORE_MAIN(...) \
	namespace { \
	static struct HD_CONCAT(hd_autorun_before_main_struct, __LINE__) { HD_CONCAT(hd_autorun_before_main_struct, __LINE__)() {__VA_ARGS__ ;} } HD_CONCAT(hd_autorun_before_main_struct_instance, __LINE__); \
	}

#define __HD_AUTO_RUN_BEFORE_MAIN(ln) \
	static void HD_CONCAT(hd_autorun_before_main_function, ln)(); \
	__HD_RUN_BEFORE_MAIN(HD_CONCAT(hd_autorun_before_main_function, ln)()); \
	static void HD_CONCAT(hd_autorun_before_main_function, ln)()

#define HD_CONCAT(a, b) _HD_CONCAT(a, b)
#define _HD_CONCAT(a, b) __HD_CONCAT(a ## b)
#define __HD_CONCAT(ab) ab

namespace hd {

namespace details {

template <size_t S> struct _EnumFlagIntForSize;
template <> struct _EnumFlagIntForSize<1> { using type = signed char; };
template <> struct _EnumFlagIntForSize<2> { using type = short; };
template <> struct _EnumFlagIntForSize<4> { using type = int; };
template <class T> struct _EnumFlagSizedInt { using type = typename _EnumFlagIntForSize<sizeof(T)>::type; };

}

class Noncopyable {
protected:
    Noncopyable() = default;
    Noncopyable(const Noncopyable&) = delete;
    Noncopyable &operator=(const Noncopyable&) = delete;
};

class Nonmovable {
protected:
    Nonmovable() = default;
    Nonmovable(const Nonmovable&&) = delete;
    Nonmovable &operator=(Nonmovable&&) = delete;
};

class StaticClass : public Noncopyable, public Nonmovable {
private:
    StaticClass() = delete;
};

template<typename T>
class Singleton : public Noncopyable, public Nonmovable {
public:
    static inline T &get() {
        static T obj;
        return obj;
    }

protected:
    Singleton() = default;
    ~Singleton() = default;
};

}