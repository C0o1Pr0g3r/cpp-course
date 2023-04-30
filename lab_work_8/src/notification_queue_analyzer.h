#ifndef NOTIFICATION_QUEUE_ANALYZER_H_INCLUDED
#define NOTIFICATION_QUEUE_ANALYZER_H_INCLUDED

#include <fstream>


class NotificationQueueAnalyzer {
public:
    template<class T, std::size_t N>
    void analyze(const NotificationQueue<T, N>& notification_queue) {
        if (!this->ofs.is_open()) {
            return;
        }

        std::cout << "Running the queue analyzer #" << this->number_of_launches + 1 << std::endl;

        if (this->number_of_launches > 0) {
            this->ofs << std::endl << std::endl << std::endl << std::endl << std::endl;
        }

        this->ofs << "Running the queue analyzer #" << this->number_of_launches + 1 << std::endl << std::endl;

        this->ofs << "Queued notifications (not necessarily stored in order of priority):" << std::endl;
        for (const auto& element: notification_queue) {
            this->ofs << element << std::endl;
        }
        this->ofs << std::endl;

        this->ofs << "1. Current date and time: " << get_current_date_and_time() << std::endl;
        this->ofs << std::endl;

        this->ofs << "2. Queue size (in bytes): " << sizeof(notification_queue) << std::endl;
        this->ofs << "Container size (in bytes): " << sizeof(notification_queue.container) << std::endl;
        this->ofs << "The size of the variable that stores the current number of notifications (in bytes): " << sizeof(notification_queue.length) << std::endl;
        this->ofs << std::endl;

        std::array<int, static_cast<std::size_t>(Notification<T>::LevelOfUrgency::SIZE)> number_of_messages_per_urgency_level;
        number_of_messages_per_urgency_level.fill(0);
        for (const auto& element: notification_queue) {
            ++number_of_messages_per_urgency_level.at(static_cast<std::size_t>(element.level_of_urgency()));
        }
        this->ofs << "3. Percentage of messages with different levels of urgency:" << std::endl;
        for (std::size_t i = 0; i < static_cast<std::size_t>(Notification<T>::LevelOfUrgency::SIZE); ++i) {
            this->ofs << Notification<T>::level_of_urgency_to_string(static_cast<typename Notification<T>::LevelOfUrgency>(i)) << ": " << static_cast<float>(number_of_messages_per_urgency_level.at(i)) / notification_queue.size() * 100 << "%" << std:: endl;
        }
        this->ofs << std::endl;

        const float NUMBER_OF_BYTES_IN_KIBIBYTES = 1024.0;
        this->ofs << "4. Queue size (in KiB): " << sizeof(notification_queue) / NUMBER_OF_BYTES_IN_KIBIBYTES << std::endl;
        this->ofs << "Container size (in KiB) " << sizeof(notification_queue.container) / NUMBER_OF_BYTES_IN_KIBIBYTES<< std::endl;
        this->ofs << "The size of the variable that stores the current number of notifications (in KiB): " << sizeof(notification_queue.length) / NUMBER_OF_BYTES_IN_KIBIBYTES << std::endl;
        this->ofs << std::endl;

        auto compare = [](const typename NotificationQueue<T, N>::value_type& a, const typename NotificationQueue<T, N>::value_type& b) { return a.valid_until() < b.valid_until(); };
        const auto& min = std::min_element(
            notification_queue.begin(), notification_queue.end(),
            compare
        );
        const auto& max = std::max_element(
            notification_queue.begin(), notification_queue.end(),
            compare
        );
        this->ofs << "5. The maximum difference between the validity periods of messages: "
            << (std::chrono::duration_cast<std::chrono::seconds>(
                max->valid_until() - min->valid_until())
            ).count() << " seconds" << std::endl;
        this->ofs << "The message with the earliest expiration date: " << *min << std::endl;
        this->ofs << "Notifications with the latest expiration date: " << *max << std::endl;

        ++this->number_of_launches;
    }

    template<class T, std::size_t N>
    void operator()(const NotificationQueue<T, N>& notification_queue) {
        this->analyze(notification_queue);
    }

    std::size_t get_number_of_launches() {
        return this->number_of_launches;
    }

    NotificationQueueAnalyzer()
    :
        ofs(generate_filename()),
        number_of_launches(0)
    {
        if (!this->ofs.is_open()) {
            std::cerr << "The queue analyzer could not open the file" << std::endl;
        }
    }

    ~NotificationQueueAnalyzer() {
        if (this->ofs.is_open()) {
            this->ofs.close();
        }
    }

private:
    std::string get_current_date_and_time() {
        return convert_time_point_to_iso_date(std::chrono::system_clock::now());
    }

    std::string generate_filename() {
        std::string filename = "Notification queue analysis log (" + this->get_current_date_and_time() + ").txt";
        std::replace(filename.begin(), filename.end(), ':', '-');
        return filename;
    }

    std::ofstream ofs;
    std::size_t number_of_launches;
};

#endif // NOTIFICATION_QUEUE_ANALYZER_H_INCLUDED
