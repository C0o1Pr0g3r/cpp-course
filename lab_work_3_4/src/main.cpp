#include <cstddef>
#include <array>

#include "notification.h"
#include "notification_queue.h"
#include "notification_queue_analyzer.h"


template<class T, std::size_t N>
void fill_array_with_random_notifications(std::array<Notification<T>, N>& notifications) {
    for (std::size_t i = 0; i < notifications.size(); ++i) {
        notifications.at(i) = Notification<T>(
            static_cast<typename Notification<T>::LevelOfUrgency>(
                generate_number_in_range_inclusive(0, static_cast<int>(Notification<T>::LevelOfUrgency::SIZE) - 1)
            ),
            std::chrono::system_clock::now() + std::chrono::seconds(generate_number_in_range_inclusive(-3600, 3600)),
            i + 1
        );
    }
}

int main() {
    using MessageType = std::size_t;

    const std::size_t NUMBER_OF_NOTIFICATIONS = 15;
    const std::size_t QUEUE_CAPACITY = 10;
    std::array<Notification<MessageType>, NUMBER_OF_NOTIFICATIONS> notifications;

    fill_array_with_random_notifications(notifications);

    NotificationQueue<MessageType, QUEUE_CAPACITY> notification_queue;

    std::cout << "Нехай message у повідомленні буде звичайним цілим числом: це допоможе нам зрозуміти, що серед повідомлень з однаковою терміновістю найпріоритетнішим буде те, що потрапило до черги першим" << std::endl;
    std::cout << std::endl;

    std::cout << "1. Місткість черги: " << notification_queue.capacity() << std::endl;
    std::cout << std::endl;

    std::cout << "2. Заповнимо чергу" << std::endl;
    std::cout << "Повідомлення, які додаватимуться в чергу:" << std::endl;
    for (const auto& element: notifications) {
        std::cout << element << std::endl;
    }
    std::cout << std::endl;
    std::cout << "Розмір черги до заповнення: " << notification_queue.size() << std::endl;
    std::size_t next_notification_index = 0;
    try {
        while (next_notification_index < NUMBER_OF_NOTIFICATIONS) {
            notification_queue.push(notifications.at(next_notification_index));
            ++next_notification_index;
            if (next_notification_index == notification_queue.capacity()) {
                std::cout << "Розмір черги після додавання " << next_notification_index << "-го елементу: " << notification_queue.size() << std::endl;
            } else if (next_notification_index == notification_queue.capacity() + 1) {
                std::cout << "Оскільки розмір черги досяг максимуму, черга спробує зменшити свій розмір, видаливши недійсні повідомлення" << std::endl;
                std::cout << "Розмір черги після додавання " << next_notification_index << "-го елементу: " << notification_queue.size() << std::endl;
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Помилка: " << e.what() << std::endl;
    }
    std::cout << "Кількість повідомлень, доданих у чергу (не всі з яких можуть бути там зараз): " << next_notification_index << std::endl;
    std::cout << "Розмір черги після заповнення: " << notification_queue.size() << std::endl;
    std::cout << std::endl;

    std::cout << "3. Отримаємо всі повідомлення з черги" << std::endl;
    std::cout << "Повідомлення:" << std::endl;
    try {
        while (true) {
            std::cout << notification_queue.get_out_of() << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Помилка: " << e.what() << std::endl;
    }
    std::cout << std::endl;

    fill_array_with_random_notifications(notifications);

    std::cout << "4. Проведемо аналіз черги" << std::endl;
    std::cout << "Для цього знову додамо в чергу повідомлення" << std::endl;
    std::cout << "Повідомлення, які додаватимуться в чергу:" << std::endl;
    for (const auto& element: notifications) {
        std::cout << element << std::endl;
    }
    std::cout << std::endl;
    std::cout << "Розмір черги до заповнення: " << notification_queue.size() << std::endl;
    next_notification_index = 0;
    try {
        while (next_notification_index < NUMBER_OF_NOTIFICATIONS) {
            notification_queue.push(notifications.at(next_notification_index));
            ++next_notification_index;
        }
    } catch (const std::exception& e) {
        std::cerr << "Помилка: " << e.what() << std::endl;
    }
    std::cout << "Кількість повідомлень, доданих у чергу (не всі з яких можуть бути там зараз): " << next_notification_index << std::endl;
    std::cout << "Розмір черги після заповнення: " << notification_queue.size() << std::endl;
    std::cout << std::endl;

    std::cout
        << (NotificationQueueAnalyzer::analyze(notification_queue) ? "Чергу успішно проаналізовано" : "Не вдалося проаналізувати чергу")
        << std::endl;

    return 0;
}
