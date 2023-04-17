#ifndef NOTIFICATION_QUEUE_ANALYZER_H_INCLUDED
#define NOTIFICATION_QUEUE_ANALYZER_H_INCLUDED

#include <fstream>


class NotificationQueueAnalyzer {
public:
    template<class T, std::size_t N>
    static bool analyze(const NotificationQueue<T, N>& notification_queue) {
        std::ofstream ofs("output.txt");

        if (!ofs.is_open()) {
            std::cerr << "Аналізатору черги не вдалося відкрити файл" << std::endl;
            return false;
        }

        ofs << "Повідомлення у черзі (які не обов'язково зберігаються у порядку пріоритету):" << std::endl;
        for (const auto& element: notification_queue) {
            ofs << element << std::endl;
        }
        ofs << std::endl;

        ofs << "1. Поточна дата та час: " << convert_time_point_to_iso_date(std::chrono::system_clock::now()) << std::endl;
        ofs << std::endl;

        ofs << "2. Розмір черги у байтах: " << sizeof(notification_queue) << std::endl;
        ofs << "Розмір контейнера у байтах: " << sizeof(notification_queue._container) << std::endl;
        ofs << "Розмір змінної, що вказує на поточний кінець контейнера, у байтах: " << sizeof(notification_queue._end) << std::endl;
        ofs << std::endl;

        std::array<int, static_cast<std::size_t>(Notification<T>::LevelOfUrgency::SIZE)> number_of_messages_per_urgency_level;
        number_of_messages_per_urgency_level.fill(0);
        for (const auto& element: notification_queue) {
            ++number_of_messages_per_urgency_level.at(static_cast<std::size_t>(element.level_of_urgency()));
        }
        ofs << "3. Відсоткове відношення повідомлень з різними рівнями терміновості:" << std::endl;
        for (std::size_t i = 0; i < static_cast<std::size_t>(Notification<T>::LevelOfUrgency::SIZE); ++i) {
            ofs << Notification<T>::level_of_urgency_to_string(static_cast<typename Notification<T>::LevelOfUrgency>(i)) << ": " << static_cast<float>(number_of_messages_per_urgency_level.at(i)) / notification_queue.size() * 100 << "%" << std:: endl;
        }
        ofs << std::endl;

        const float NUMBER_OF_BYTES_IN_KIBIBYTES = 1024.0;
        ofs << "4. Розмір черги у KiB: " << sizeof(notification_queue) / NUMBER_OF_BYTES_IN_KIBIBYTES << std::endl;
        ofs << "Розмір контейнера у KiB: " << sizeof(notification_queue._container) / NUMBER_OF_BYTES_IN_KIBIBYTES<< std::endl;
        ofs << "Розмір змінної, що вказує на поточний кінець контейнера, у KiB: " << sizeof(notification_queue._end) / NUMBER_OF_BYTES_IN_KIBIBYTES << std::endl;
        ofs << std::endl;

        auto compare = [](const typename NotificationQueue<T, N>::value_type& a, const typename NotificationQueue<T, N>::value_type& b) { return a.valid_until() < b.valid_until(); };
        const auto& min = std::min_element(
            notification_queue.begin(), notification_queue.end(),
            compare
        );
        const auto& max = std::max_element(
            notification_queue.begin(), notification_queue.end(),
            compare
        );
        ofs << "5. Максимальна різниця між термінами дійсності повідомлень: "
            << (std::chrono::duration_cast<std::chrono::seconds>(
                max->valid_until() - min->valid_until())
            ).count() << " секунд" << std::endl;
        ofs << "Повідомлення з найбільш раннім терміном придатності: " << *min << std::endl;
        ofs << "Повідомлення з найпізнішим терміном придатності: " << *max << std::endl;

        ofs.close();

        return true;
    }

    NotificationQueueAnalyzer() = delete;
};

#endif // NOTIFICATION_QUEUE_ANALYZER_H_INCLUDED
