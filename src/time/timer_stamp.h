#ifndef SABER_TIMESTAMP_H
#define SABER_TIMESTAMP_H

#include <cstdint>
#include <algorithm>

class Timestamp {
    friend bool operator< (Timestamp lhs, Timestamp rhs);
public:
    static const int kMicroSecondsPerSecond = 1000 * 1000;

    Timestamp()
      : micro_seconds_(0)
    { }
    explicit Timestamp(int64_t micro_seconds)
      : micro_seconds_(micro_seconds)
    { }

    void Swap(Timestamp& that) 
    {
        std::swap(micro_seconds_, that.micro_seconds_);
    }

public:
    static Timestamp Now();
    static Timestamp AddTime(Timestamp timestamp, double seconds);

    int64_t micro_seconds() const { return micro_seconds_; }
    bool Valid() const { return micro_seconds_ > 0; }

private:
    int64_t  micro_seconds_;
};

inline bool operator< (Timestamp lhs, Timestamp rhs) {
    return lhs.micro_seconds_ < rhs.micro_seconds_;
}

#endif //SABER_TIMESTAMP_H
