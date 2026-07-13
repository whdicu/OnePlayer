// MSVC forced-include fix for Qt 5.9.x + MSVC 2017 constexpr bug
// Qt's qcompilerdetection.h checks __cpp_constexpr to decide if relaxed constexpr
// is supported. MSVC 2017 reports >= 201304 but its intrinsics (__popcnt) aren't
// actually constexpr-compatible. Force Qt to think relaxed constexpr is unavailable.
#ifdef __cpp_constexpr
#undef __cpp_constexpr
#endif
