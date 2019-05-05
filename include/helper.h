#ifndef HELPER_H_INCLUDED
#define HELPER_H_INCLUDED
#include <stdint.h>
#include <limits>
#include <type_traits>

namespace webrtc
{

template <unsigned long M>                                    // NOLINT
inline unsigned long Add(unsigned long a, unsigned long b)    // NOLINT
{
//  RTC_DCHECK_LT(a, M);
    unsigned long t = M - b % M;  // NOLINT
    unsigned long res = a - t;    // NOLINT
    if (t > a)
        return res + M;
    return res;
}

template <unsigned long M>                                         // NOLINT
inline unsigned long Subtract(unsigned long a, unsigned long b)    // NOLINT
{
//  RTC_DCHECK_LT(a, M);
    unsigned long sub = b % M;  // NOLINT
    if (a < sub)
        return M - (sub - a);
    return a - sub;
}

// Calculates the forward difference between two wrapping numbers.
//
// Example:
// uint8_t x = 253;
// uint8_t y = 2;
//
// ForwardDiff(x, y) == 5
//
//   252   253   254   255    0     1     2     3
// #################################################
// |     |  x  |     |     |     |     |  y  |     |
// #################################################
//          |----->----->----->----->----->
//
// ForwardDiff(y, x) == 251
//
//   252   253   254   255    0     1     2     3
// #################################################
// |     |  x  |     |     |     |     |  y  |     |
// #################################################
// -->----->                              |----->---
//
// If M > 0 then wrapping occurs at M, if M == 0 then wrapping occurs at the
// largest value representable by T.
template <typename T, T M>
inline typename std::enable_if<(M > 0), T>::type ForwardDiff(T a, T b)
{
    static_assert(std::is_unsigned<T>::value,
                  "Type must be an unsigned integer.");
//  RTC_DCHECK_LT(a, M);
//  RTC_DCHECK_LT(b, M);
    return a <= b ? b - a : M - (a - b);
}

template <typename T, T M>
inline typename std::enable_if<(M == 0), T>::type ForwardDiff(T a, T b)
{
    static_assert(std::is_unsigned<T>::value,
                  "Type must be an unsigned integer.");
    return b - a;
}

template <typename T>
inline T ForwardDiff(T a, T b)
{
    return ForwardDiff<T, 0>(a, b);
}

// Calculates the reverse difference between two wrapping numbers.
//
// Example:
// uint8_t x = 253;
// uint8_t y = 2;
//
// ReverseDiff(y, x) == 5
//
//   252   253   254   255    0     1     2     3
// #################################################
// |     |  x  |     |     |     |     |  y  |     |
// #################################################
//          <-----<-----<-----<-----<-----|
//
// ReverseDiff(x, y) == 251
//
//   252   253   254   255    0     1     2     3
// #################################################
// |     |  x  |     |     |     |     |  y  |     |
// #################################################
// ---<-----|                             |<-----<--
//
// If M > 0 then wrapping occurs at M, if M == 0 then wrapping occurs at the
// largest value representable by T.
template <typename T, T M>
inline typename std::enable_if<(M > 0), T>::type ReverseDiff(T a, T b)
{
    static_assert(std::is_unsigned<T>::value,
                  "Type must be an unsigned integer.");
    RTC_DCHECK_LT(a, M);
    RTC_DCHECK_LT(b, M);
    return b <= a ? a - b : M - (b - a);
}

template <typename T, T M>
inline typename std::enable_if<(M == 0), T>::type ReverseDiff(T a, T b)
{
    static_assert(std::is_unsigned<T>::value,
                  "Type must be an unsigned integer.");
    return a - b;
}

template <typename T>
inline T ReverseDiff(T a, T b)
{
    return ReverseDiff<T, 0>(a, b);
}

// Calculates the minimum distance between to wrapping numbers.
//
// The minimum distance is defined as min(ForwardDiff(a, b), ReverseDiff(a, b))
template <typename T, T M = 0>
inline T MinDiff(T a, T b)
{
    static_assert(std::is_unsigned<T>::value,
                  "Type must be an unsigned integer.");
    return std::min(ForwardDiff<T, M>(a, b), ReverseDiff<T, M>(a, b));
}

// Test if the sequence number |a| is ahead or at sequence number |b|.
//
// If |M| is an even number and the two sequence numbers are at max distance
// from each other, then the sequence number with the highest value is
// considered to be ahead.
template <typename T, T M>
inline typename std::enable_if<(M > 0), bool>::type AheadOrAt(T a, T b)
{
    static_assert(std::is_unsigned<T>::value,
                  "Type must be an unsigned integer.");
    const T maxDist = M / 2;
    if (!(M & 1) && MinDiff<T, M>(a, b) == maxDist)
        return b < a;
    return ForwardDiff<T, M>(b, a) <= maxDist;
}

template <typename T, T M>
inline typename std::enable_if<(M == 0), bool>::type AheadOrAt(T a, T b)
{
    static_assert(std::is_unsigned<T>::value,
                  "Type must be an unsigned integer.");
    const T maxDist = std::numeric_limits<T>::max() / 2 + T(1);
    if (a - b == maxDist)
        return b < a;
    return ForwardDiff(b, a) < maxDist;
}

template <typename T>
inline bool AheadOrAt(T a, T b)
{
    return AheadOrAt<T, 0>(a, b);
}

// Test if the sequence number |a| is ahead of sequence number |b|.
//
// If |M| is an even number and the two sequence numbers are at max distance
// from each other, then the sequence number with the highest value is
// considered to be ahead.
template <typename T, T M = 0>
inline bool AheadOf(T a, T b)
{
    static_assert(std::is_unsigned<T>::value,
                  "Type must be an unsigned integer.");
    return a != b && AheadOrAt<T, M>(a, b);
}

// Comparator used to compare sequence numbers in a continuous fashion.
//
// WARNING! If used to sort sequence numbers of length M then the interval
//          covered by the sequence numbers may not be larger than floor(M/2).
template <typename T, T M = 0>
struct AscendingSeqNumComp
{
    bool operator()(T a, T b) const
    {
        return AheadOf<T, M>(a, b);
    }
};

// Comparator used to compare sequence numbers in a continuous fashion.
//
// WARNING! If used to sort sequence numbers of length M then the interval
//          covered by the sequence numbers may not be larger than floor(M/2).
template <typename T, T M = 0>
struct DescendingSeqNumComp
{
    bool operator()(T a, T b) const
    {
        return AheadOf<T, M>(b, a);
    }
};

// A sequence number unwrapper where the first unwrapped value equals the
// first value being unwrapped.
template <typename T, T M = 0>
class SeqNumUnwrapper {
  static_assert(
      std::is_unsigned<T>::value &&
          std::numeric_limits<T>::max() < std::numeric_limits<int64_t>::max(),
      "Type unwrapped must be an unsigned integer smaller than int64_t.");

 public:
  int64_t Unwrap(T value) {
    if (!last_value_) {
      last_unwrapped_ = {value};
    } else {
      last_unwrapped_ += ForwardDiff<T, M>(last_value_, value);

      if (!AheadOrAt<T, M>(value, last_value_)) {
        constexpr int64_t kBackwardAdjustment =
            M == 0 ? int64_t{std::numeric_limits<T>::max()} + 1 : M;
        last_unwrapped_ -= kBackwardAdjustment;
      }
    }

    last_value_ = value;
    return last_unwrapped_;
  }

 private:
  int64_t last_unwrapped_ = 0;
  T last_value_;
};

}  // namespace webrtc

#endif // HELPER_H_INCLUDED
