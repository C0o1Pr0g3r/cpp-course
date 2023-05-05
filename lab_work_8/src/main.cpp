#include <cstddef>
#include <array>
#include <condition_variable>

#include "notification.h"
#include "notification_queue.h"
#include "notification_queue_analyzer.h"


using MessageType = std::size_t;
using Duration = std::chrono::milliseconds;
using TimePoint = std::chrono::time_point<std::chrono::system_clock>;

const Duration HOW_LONG_SHOULD_PROGRAM_RUN = std::chrono::minutes(5);
std::condition_variable cv;

Notification<MessageType> generate_notification() {
    static int number_of_notifications = 0;

    return Notification<MessageType>(
        static_cast<typename Notification<MessageType>::LevelOfUrgency>(
            generate_number_in_range_inclusive(0, static_cast<int>(
                Notification<MessageType>::LevelOfUrgency::SIZE) - 1
        )),
        std::chrono::system_clock::now() + std::chrono::hours(
            generate_number_in_range_inclusive(-2, 2)
        ),
        number_of_notifications++
    );
}

std::tuple<bool, Duration> terminate_thread(
    const TimePoint& start_time,
    const Duration& how_long_should_thread_sleep
) {
    Duration remaining_time =
        HOW_LONG_SHOULD_PROGRAM_RUN - std::chrono::duration_cast<Duration>(
            std::chrono::system_clock::now() - start_time
        );
    return std::tuple<bool, Duration>(
        remaining_time > std::chrono::milliseconds(0),
        std::min(how_long_should_thread_sleep, remaining_time)
    );
}

template<class T, std::size_t N>
void add_and_remove_notification(
    NotificationQueue<T, N>& notification_queue
) {
    const auto start_time = std::chrono::system_clock::now();

    while (true) {      
        if (static_cast<bool>(generate_number_in_range_inclusive(0, 1))) {
            notification_queue.push(generate_notification());
            if (notification_queue.completely_filled()) {
                cv.notify_one();
            }
        } else {
            notification_queue.get_out_of();
        }

        auto how_long_should_thread_sleep = Duration(
            std::chrono::milliseconds(generate_number_in_range_inclusive(10, 500))
        );

        Duration remaining_time =
            HOW_LONG_SHOULD_PROGRAM_RUN - std::chrono::duration_cast<Duration>(
                std::chrono::system_clock::now() - start_time
        );

        if (remaining_time <= Duration(0)) {
            return;
        }
        
        const auto time_to_sleep = std::min(
            how_long_should_thread_sleep, remaining_time
        );

        std::this_thread::sleep_for(time_to_sleep);
    }
}template<class T, std::size_t N>
void remove_notification(
    NotificationQueue<T, N>& notification_queue
) {
    const auto start_time = std::chrono::system_clock::now();
    const Duration HOW_LONG_SHOULD_THREAD_SLEEP = std::chrono::seconds(1);

    while (true) {    
        notification_queue.get_out_of();

        Duration remaining_time =
            HOW_LONG_SHOULD_PROGRAM_RUN - std::chrono::duration_cast<Duration>(
                std::chrono::system_clock::now() - start_time
        );

        if (remaining_time <= Duration(0)) {
            return;
        }
        
        const auto time_to_sleep = std::min(
            HOW_LONG_SHOULD_THREAD_SLEEP, remaining_time
        );

        std::this_thread::sleep_for(time_to_sleep);
    }
}

template<class T, std::size_t N>
void run_notification_queue_analyzer(
    NotificationQueue<T, N>& notification_queue,
    NotificationQueueAnalyzer& notification_queue_analyzer
) {
    const auto start_time = std::chrono::system_clock::now();
    const Duration HOW_LONG_SHOULD_THREAD_SLEEP = std::chrono::minutes(1);

    while (true) {
        Duration remaining_time =
            HOW_LONG_SHOULD_PROGRAM_RUN - std::chrono::duration_cast<Duration>(
                std::chrono::system_clock::now() - start_time
        );

        if (remaining_time <= Duration(0)) {
            return;
        }
        
        const auto time_to_sleep = std::min(
            HOW_LONG_SHOULD_THREAD_SLEEP, remaining_time
        );

        std::unique_lock<std::mutex> lock(notification_queue.get_mutex());

        std::cv_status status = cv.wait_for(lock, time_to_sleep);
        
        switch (status) {
        case std::cv_status::no_timeout:
            std::cout << "The queue analyzer started because the queue was full"
                << std::endl;
            break;
        case std::cv_status::timeout:
            std::cout << "The queue analyzer started because"
                " 1 minute has passed since the previous run" << std::endl;
            break;
        }

        notification_queue_analyzer.analyze(notification_queue);

        lock.unlock();
    }
}


int main() {
    const auto start_time = std::chrono::system_clock::now();

    const std::size_t QUEUE_CAPACITY = 10;
    const std::size_t NUMBER_OF_THREADS = 5;
    NotificationQueue<MessageType, QUEUE_CAPACITY> notification_queue;
    NotificationQueueAnalyzer notification_queue_analyzer;
    std::array<std::thread, NUMBER_OF_THREADS> threads {
        std::thread(
            add_and_remove_notification<MessageType, QUEUE_CAPACITY>,
            std::ref(notification_queue)
        ),
        std::thread(
            add_and_remove_notification<MessageType, QUEUE_CAPACITY>,
            std::ref(notification_queue)
        ),
        std::thread(
            add_and_remove_notification<MessageType, QUEUE_CAPACITY>,
            std::ref(notification_queue)
        ),
        std::thread(
            remove_notification<MessageType, QUEUE_CAPACITY>,
            std::ref(notification_queue)
        ),
        std::thread(
            run_notification_queue_analyzer<MessageType, QUEUE_CAPACITY>,
            std::ref(notification_queue),
            std::ref(notification_queue_analyzer)
        ),
    };

    for (auto& thread: threads) {
        thread.join();
    }

    std::cout
        << "Number of calls to the 'analyze' method"
        " of the 'notification_queue_analyzer' object: "
        << notification_queue_analyzer.get_number_of_launches()
        << std::endl;

    const auto running_time = std::chrono::system_clock::now() - start_time;

    std::cout
        << "Program execution time: "
        << std::chrono::duration_cast<std::chrono::minutes>(
            running_time).count() << " minutes ("
        << std::chrono::duration_cast<std::chrono::milliseconds>(
            running_time).count() << " milliseconds)"
        << std::endl;
    

    return 0;
}
