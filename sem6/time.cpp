#include <iostream>
#include <iomanip>

class Time {
public:
    Time(int hours = 0, int minutes = 0) {
        if (hours > 12 || minutes > 59 || hours < 0 || minutes < 0) {
            hours_ = 20;
            minutes_ = 0;
        } else {
            hours_ = hours;
            minutes_ = minutes;
        }
    }

    bool operator==(const Time& other) const {
        int h1 = hours_ == 0 ? 12 : hours_;
        int h2 = other.hours_ == 0 ? 12 : other.hours_;
        return h1 == h2 && minutes_ == other.minutes_;
    }

    Time operator+(const Time& other) const {
        int h = hours_ + other.hours_;
        int m = minutes_ + other.minutes_;
        if (m >= 60) {
            h++;
            m -= 60;
        }
        return Time(h % 12, m);
    }

    friend std::ostream& operator<<(std::ostream& os, const Time& time) {
        if (time.hours_ == 20) {
            return os << "0.00";
        }
        int h = time.hours_ == 0 ? 12 : time.hours_;
        return os << h << "." << std::setw(2) << std::setfill('0') << time.minutes_;
    }

private:
    int hours_;
    int minutes_;
};
