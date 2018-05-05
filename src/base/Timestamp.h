#ifndef SABER_TIMESTAMP_H
#define SABER_TIMESTAMP_H

#include <cstdint>
#include <algorithm>

class Timestamp {
    friend bool operator< (Timestamp lhs, Timestamp rhs);
public:
    static const int kMicroSecondsPerSecond = 1000 * 1000;

    Timestamp()
      : microSeconds_(0)
    { }
    explicit Timestamp(int64_t microSeconds)
      : microSeconds_(microSeconds)
    { }

    void swap(Timestamp& that) 
    {
        std::swap(microSeconds_, that.microSeconds_);
    }

public:
    static Timestamp now();
    static Timestamp addTime(Timestamp timestamp, double seconds);

    int64_t microSecondsSinceEpoch() const { return microSeconds_; }
    bool valid() const { return microSeconds_ > 0; }

private:
    int64_t  microSeconds_;
};

inline bool operator< (Timestamp lhs, Timestamp rhs) {
    return lhs.microSeconds_ < rhs.microSeconds_;
}

#endif //SABER_TIMESTAMP_H
