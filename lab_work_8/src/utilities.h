#ifndef UTILITIES_H_INCLUDED
#define UTILITIES_H_INCLUDED

#include <string>
#include <chrono>
#include <random>


template<class Clock>
std::string convert_time_point_to_iso_date(const std::chrono::time_point<Clock>& tp) {
    const std::time_t t{std::chrono::system_clock::to_time_t(tp)};
    const std::size_t MAX_SIZE = 128;
    char buffer[MAX_SIZE];
    std::strftime(buffer, MAX_SIZE, "%Y-%m-%dT%H:%M:%S%z", std::localtime(&t));

    return std::string(buffer);
}

int generate_number_in_range_inclusive(int min, int max) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> distr(min, max);

    return distr(gen);
}

#endif // UTILITIES_H_INCLUDED
