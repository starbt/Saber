//
// Created by xcv on 17-12-18.
//

#ifndef SABER_TIMESTAMP_H
#define SABER_TIMESTAMP_H


#include <cstdint>

class Timestamp {
public:
    static const int kMicroSecondsPerSecond = 1000 * 1000;
    static Timestamp addTime(Timestamp timestamp, double seconds);

    Timestamp()
      : microSeconds_(0)
    { }
    explicit Timestamp(int64_t microSeconds)
      : microSeconds_(microSeconds)
    { }

public:
    static Timestamp now();
    int64_t microSecondsSinceEpoch() const { return microSeconds_; }

private:
    int64_t  microSeconds_;

};

inline bool operator< (Timestamp lhs, Timestamp rhs) {
    return lhs.microSeconds_ < rhs.microSeconds_;
}


#endif //SABER_TIMESTAMP_H
