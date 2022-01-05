#pragma once

#if __cplusplus >= 202002L
#define constexpr20 constexpr
#else
#define constexpr20 inline
#endif

using uint = unsigned int;

namespace Utils {

class SmallStringView;
template <uint Size>
class BasicSmallString;
using SmallString = BasicSmallString<31>;
using PathString = BasicSmallString<190>;

} // namespace Utils
