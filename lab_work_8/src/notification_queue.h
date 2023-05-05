#ifndef NOTIFICATION_QUEUE_H_INCLUDED
#define NOTIFICATION_QUEUE_H_INCLUDED

#include <iostream>
#include <algorithm>
#include <tuple>
#include <thread>
#include <mutex>
#include <condition_variable>


template<class T, std::size_t N>
class NotificationQueue {
public:
    friend class NotificationQueueAnalyzer;

    using value_type = Notification<T>;
    using size_type = decltype(N);

    size_type size() const { return this->length; }
    size_type capacity() const { return N; }

    bool empty() const { return this->size() == 0; }
    bool completely_filled() const { return this->size() == this->capacity(); }

    std::mutex& get_mutex() { return this->m; }

    void push(const value_type& value) {
        std::lock_guard lock(this->m);

        this->try_to_remove_invalid_notifications_if_queue_is_full();
        if (this->completely_filled()) {
            std::cout << "The queue is full." << std::endl;
            return;
        }
        this->add_element(value);
        std::cout << "The notification added." << std::endl;
    }

    void push(value_type&& value) {
        std::lock_guard lock(this->m);

        this->try_to_remove_invalid_notifications_if_queue_is_full();
        if (this->completely_filled()) {
            std::cout << "The queue is full." << std::endl;
            return;
        }
        this->add_element(std::forward<value_type&&>(value));
        std::cout << "The notification added." << std::endl;
    }

    std::tuple<bool, value_type> get_out_of() {
        std::lock_guard lock(this->m);
        
        while (!this->empty()) {
            const auto element_it = std::max_element(this->begin(), this->end());
            const value_type element = *element_it;
            std::copy(element_it + 1, this->end(), element_it);
            --this->length;
            if (element.is_still_valid()) {
                std::cout << "Valid notification removed." << std::endl;
                return std::tuple<bool, value_type>(true, element);
            } else {
                std::cout << "Invalid notification removed." << std::endl;
            }
        }
        
        std::cout << "An attempt to receive a notification failed"
            " because there were no valid notifications in the queue."
            << std::endl;

        return std::tuple<bool, value_type>(false, Notification<T>());
    }

    template<class U, std::size_t V>
    friend std::ostream& operator<<(
        std::ostream& out, const NotificationQueue<U, V>& notification_queue
    );

    NotificationQueue()
    :   container(),
        length(0),
        m()
    {}

    ~NotificationQueue() {}
private:
    using iterator = typename std::array<value_type, N>::iterator;
    using const_iterator = typename std::array<value_type, N>::const_iterator;

    typename std::array<value_type, N>::iterator begin() {
        return this->container.begin();
    }

    typename std::array<value_type, N>::const_iterator end() const {
        return this->container.begin() + this->length;
    }

    typename std::array<value_type, N>::const_iterator begin() const {
        return this->container.begin();
    }

    typename std::array<value_type, N>::iterator end() {
        return this->container.begin() + this->length;
    }

    iterator find_notification_with_maximum_priority() {
        return std::max_element(this->begin(), this->end());
    }

    void add_element(const value_type& value) {
        *this->end() = value;
        ++this->length;
    }

    void add_element(value_type&& value) {
        *this->end() = std::move(value);
        ++this->length;
    }

    void remove_element(iterator &it) {
        std::copy(it + 1, this->end(), it);
        --this->length;
    }

    void try_to_remove_invalid_notifications_if_queue_is_full() {
        if (this->completely_filled()) {
            this->remove_invalid_notifications();
        }
    }

    void remove_invalid_notifications() {
        move_invalid_notifications_to_end();
        size_type length_before_queue_clearing = this->length;
        this->length = get_the_index_of_last_valid_notification();
    }

    void move_invalid_notifications_to_end() {
        std::stable_sort(
            this->begin(), this->end(),
            [](const value_type& a, const value_type& b) {
                return a.is_still_valid() > b.is_still_valid();
            }
        );
    }

    size_type get_the_index_of_last_valid_notification() {
        return std::find_if_not(
            this->begin(), this->end(),
            [](const value_type& a) { return a.is_still_valid(); }
        ) - this->begin();
    }

    std::array<value_type, N> container;
    size_type length;
    std::mutex m;
};

template<class U, std::size_t V>
std::ostream& operator<<(
    std::ostream& out, const NotificationQueue<U, V>& notification_queue
) {
    for (const auto& element: notification_queue) {
        out << element << std::endl;
    }

    return out;
}

#endif // NOTIFICATION_QUEUE_H_INCLUDED
