#ifndef NOTIFICATION_QUEUE_H_INCLUDED
#define NOTIFICATION_QUEUE_H_INCLUDED

#include <iostream>
#include <algorithm>


template<class T, std::size_t N>
class NotificationQueue {
public:
    friend class NotificationQueueAnalyzer;

    using value_type = Notification<T>;
    using size_type = decltype(N);

    size_type size() const { return this->_end; }
    size_type capacity() const { return N; }

    bool empty() const { return this->size() == 0; }
    bool completely_filled() const { return this->size() == this->capacity(); }

    void push(const value_type& value) {
        this->_try_to_remove_invalid_notifications_if_queue_is_full();
        if (this->completely_filled()) {
            throw std::length_error("The queue is full");
        }
        *this->end() = value;
        ++this->_end;
    }

    void push(value_type&& value) {
        this->_try_to_remove_invalid_notifications_if_queue_is_full();
        if (this->completely_filled()) {
            throw std::length_error("The queue is full");
        }
        *this->end() = std::move(value);
        ++this->_end;
    }

    value_type get_out_of() {
        while (!this->empty()) {
            const auto element_it = std::max_element(this->begin(), this->end());
            const value_type element = *element_it;
            std::copy(element_it + 1, this->end(), element_it);
            --this->_end;
            if (element.is_still_relevant()) {
                return element;
            }
        }

        throw std::length_error("The queue is empty");
    }

    NotificationQueue()
    :   _container(),
        _end(0)
    {}

    ~NotificationQueue() {}

    void print() {
        for (const auto& element: *this) {
            std::cout << element << std::endl;
        }
        std::cout << std::endl;
    }

private:
    void _remove_invalid_notifications() {
        std::stable_sort(
            this->begin(), this->end(),
            [](const value_type& a, const value_type& b) { return a.is_still_relevant() > b.is_still_relevant(); }
        );
        this->_end = std::find_if_not(
            this->begin(), this->end(),
            [](const value_type& a) { return a.is_still_relevant(); }
        ) - this->begin();
    }

    void _try_to_remove_invalid_notifications_if_queue_is_full() {
        if (this->completely_filled()) {
            this->_remove_invalid_notifications();
        }
    }

    typename std::array<value_type, N>::iterator begin() {
        return this->_container.begin();
    }

    typename std::array<value_type, N>::const_iterator end() const {
        return this->_container.begin() + this->_end;
    }

    typename std::array<value_type, N>::const_iterator begin() const {
        return this->_container.begin();
    }

    typename std::array<value_type, N>::iterator end() {
        return this->_container.begin() + this->_end;
    }

    std::array<value_type, N> _container;
    size_type _end;
};

#endif // NOTIFICATION_QUEUE_H_INCLUDED
