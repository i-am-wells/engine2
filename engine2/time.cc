#include "engine2/time.h"

#include <SDL2/SDL_timer.h>

namespace engine2 {

Time* Time::now_override_ = nullptr;

double Time::Delta::ToSeconds() const {
  return MicrosToSeconds(micros_);
}

bool Time::Delta::operator==(const Delta& other) const {
  return micros_ == other.micros_;
}

bool Time::Delta::operator<(const Delta& other) const {
  return micros_ < other.micros_;
}

bool Time::Delta::operator>(const Delta& other) const {
  return micros_ > other.micros_;
}

bool Time::Delta::operator<=(const Delta& other) const {
  return micros_ <= other.micros_;
}

bool Time::Delta::operator>=(const Delta& other) const {
  return micros_ >= other.micros_;
}

Time::Delta& Time::Delta::operator-=(const Delta& other) {
  micros_ -= other.micros_;
  return *this;
}

Time::Delta Time::Delta::operator-(const Delta& other) const {
  return Delta(micros_) -= other;
}

Time::Delta& Time::Delta::operator+=(const Delta& other) {
  micros_ += other.micros_;
  return *this;
}

Time::Delta Time::Delta::operator+(const Delta& other) const {
  return Delta(micros_) += other;
}

Time Time::Delta::operator+(const Time& time) const {
  return time + *this;
}

Time::Delta& Time::Delta::operator*=(double factor) {
  micros_ *= factor;
  return *this;
}

Time::Delta Time::Delta::operator*(double factor) const {
  return Delta(micros_) *= factor;
}

Time::Delta& Time::Delta::operator/=(double factor) {
  micros_ /= factor;
  return *this;
}

Time::Delta Time::Delta::operator/(double factor) const {
  return Delta(micros_) /= factor;
}

Time::Delta& Time::Delta::operator%=(const Delta& other) {
  micros_ %= other.micros_;
  return *this;
}

Time::Delta Time::Delta::operator%(const Delta& other) const {
  return Delta(micros_) %= other;
}

double Time::ToSeconds() const {
  return MicrosToSeconds(micros_);
}

bool Time::operator==(const Time& other) const {
  return micros_ == other.micros_;
}

bool Time::operator<(const Time& other) const {
  return micros_ < other.micros_;
}

bool Time::operator>(const Time& other) const {
  return micros_ > other.micros_;
}

bool Time::operator<=(const Time& other) const {
  return micros_ <= other.micros_;
}

bool Time::operator>=(const Time& other) const {
  return micros_ >= other.micros_;
}

Time& Time::operator-=(const Delta& delta) {
  micros_ -= delta.ToMicroseconds();
  return *this;
}

Time::Delta Time::operator-(const Time& other) const {
  return Delta::FromMicroseconds(micros_ - other.micros_);
}

Time Time::operator-(const Delta& delta) const {
  return Time(micros_) -= delta;
}

Time& Time::operator+=(const Delta& delta) {
  micros_ += delta.ToMicroseconds();
  return *this;
}

Time Time::operator+(const Delta& delta) const {
  return Time(micros_) += delta;
}

// static
Time Time::Now() {
  if (now_override_)
    return *now_override_;
  return Time(1000 * SDL_GetTicks());
}

}  // namespace engine2