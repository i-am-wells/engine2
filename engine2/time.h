#ifndef ENGINE2_TIME_H_
#define ENGINE2_TIME_H_

#include <cstdint>

namespace engine2 {

class Time {
 public:
  enum class Unit {
    kMicroseconds,
    kMilliseconds,
    kSeconds,
    kMinutes,
    kHours,
    kDays
  };

  class Delta {
   public:
    static constexpr Delta FromSeconds(double seconds) {
      return Delta(SecondsToMicros(seconds));
    }
    static constexpr Delta FromMicroseconds(int64_t microseconds) {
      return Delta(microseconds);
    }

    constexpr Delta() : micros_(0) {}

    double ToSeconds() const;
    int64_t ToMicroseconds() const { return micros_; }

    bool operator==(const Delta& other) const;
    bool operator<(const Delta& other) const;
    bool operator<=(const Delta& other) const;
    bool operator>(const Delta& other) const;
    bool operator>=(const Delta& other) const;

    Delta& operator-=(const Delta& other);
    Delta operator-(const Delta& other) const;
    Delta& operator+=(const Delta& other);
    Delta operator+(const Delta& other) const;
    Time operator+(const Time& time) const;

    Delta& operator*=(double factor);
    Delta operator*(double factor) const;
    Delta& operator/=(double denom);
    Delta operator/(double denom) const;

   private:
    constexpr Delta(int64_t micros) : micros_(micros) {}
    int64_t micros_;
  };

  constexpr Time() : micros_(0) {}

  static constexpr Time FromSeconds(double seconds) {
    return Time(SecondsToMicros(seconds));
  }
  static constexpr Time FromMicroseconds(int64_t microseconds) {
    return Time(microseconds);
  }

  static Time Now();

  double ToSeconds() const;
  int64_t ToMicroseconds() const { return micros_; }

  bool operator==(const Time& other) const;
  bool operator<(const Time& other) const;
  bool operator<=(const Time& other) const;
  bool operator>(const Time& other) const;
  bool operator>=(const Time& other) const;

  Time& operator-=(const Delta& delta);
  Delta operator-(const Time& other) const;
  Time operator-(const Delta& delta) const;

  Time& operator+=(const Delta& delta);
  Time operator+(const Delta& delta) const;

 private:
  constexpr Time(int64_t micros) : micros_(micros) {}

  friend class Delta;
  static constexpr int64_t SecondsToMicros(double seconds) {
    return seconds * 1'000'000;
  }

  static constexpr double MicrosToSeconds(int64_t micros) {
    return micros / 1'000'000.;
  }

  int64_t micros_;
};

}  // namespace engine2

#endif  // ENGINE2_TIME_H_