#ifndef NOTIFICATION_H_INCLUDED
#define NOTIFICATION_H_INCLUDED

#include <ostream>

#include "utilities.h"


template<class T>
class Notification {
public:
    using MessageType = T;

    enum class LevelOfUrgency {
        LOW,
        MEDIUM,
        HIGH,
        SIZE
    };

    LevelOfUrgency level_of_urgency() const {
        return this->_level_of_urgency;
    }

    std::chrono::time_point<std::chrono::system_clock> valid_until() const {
        return this->_valid_until;
    }

    bool is_still_valid() const {
        return this->_valid_until >= std::chrono::system_clock::now();
    }

    bool operator<(const Notification<T>& other) const {
        return this->_level_of_urgency < other._level_of_urgency;
    }

    static std::string level_of_urgency_to_string(
        LevelOfUrgency level_of_urgency
    ) {
        return Notification<T>::_levels_of_urgency[
            static_cast<std::size_t>(level_of_urgency)
        ];
    }

    template<class U>
    friend std::ostream& operator<<(
        std::ostream& out, const Notification<U>& notification
    );

    Notification() = default;

    Notification(
        LevelOfUrgency level_of_urgency,
        std::chrono::time_point<std::chrono::system_clock> valid_until,
        T message
    )
    :
        _level_of_urgency(level_of_urgency),
        _valid_until(valid_until),
        _message(message)
    {}

private:
    LevelOfUrgency _level_of_urgency;
    std::chrono::time_point<std::chrono::system_clock> _valid_until;
    T _message;

    static std::array<
        std::string, static_cast<std::size_t>(LevelOfUrgency::SIZE)
    > _levels_of_urgency;
};

template<class T>
std::array<
    std::string, static_cast<std::size_t>(Notification<T>::LevelOfUrgency::SIZE)
> Notification<T>::_levels_of_urgency {
    "LOW",
    "MEDIUM",
    "HIGH",
};

template<class T>
std::ostream& operator<<(std::ostream& out, const Notification<T>& notification) {
    const std::time_t t{
        std::chrono::system_clock::to_time_t(notification._valid_until)
    };
    out << "Notification: { "
        << "_level_of_urgency: " << Notification<T>::_levels_of_urgency.at(
            static_cast<std::size_t>(notification._level_of_urgency)
        )
        << ", _valid_until: " << convert_time_point_to_iso_date(
            notification._valid_until
        )
        << ", _message: " << notification._message
        << ", is_valid_now: " << (notification.is_still_valid() ? "true" : "false")
        << " }";

    return out;
}

#endif // NOTIFICATION_H_INCLUDED
