#include <cstddef>
#include <array>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>

#include "notification.h"
#include "notification_queue.h"
#include "notification_queue_analyzer.h"


using MessageType = std::size_t;

const std::chrono::milliseconds HOW_LONG_SHOULD_THE_PROGRAM_RUN =
    std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::minutes(5));

std::mutex m;
std::condition_variable cv;
bool is_it_time_to_run_the_analyzer = false;

Notification<MessageType> generate_notification() {
    static int number_of_notifications = 0;

    return Notification<MessageType>(
        static_cast<typename Notification<MessageType>::LevelOfUrgency>(
            generate_number_in_range_inclusive(0, static_cast<int>(Notification<MessageType>::LevelOfUrgency::SIZE) - 1)
        ),
        std::chrono::system_clock::now() + std::chrono::seconds(generate_number_in_range_inclusive(-3600, 3600)),
        number_of_notifications++
    );
}

std::chrono::milliseconds get_elapsed_time(
    const std::chrono::time_point<std::chrono::system_clock>& starting_point
) {
    return std::chrono::duration_cast<
        std::chrono::milliseconds
    >(
        std::chrono::system_clock::now() - starting_point
    );
}

std::chrono::milliseconds get_shorter_duration(
    const std::chrono::milliseconds& a,
    const std::chrono::milliseconds& b
) {
    return std::min(a, b);
}

std::tuple<bool, std::chrono::milliseconds> continue_running_thread(
    const std::chrono::time_point<std::chrono::system_clock>& starting_point,
    const std::chrono::milliseconds& how_long_should_thread_sleep
) {
    auto remaining_time = HOW_LONG_SHOULD_THE_PROGRAM_RUN - get_elapsed_time(starting_point);
    return std::tuple<bool, std::chrono::milliseconds>(
        remaining_time > std::chrono::milliseconds(0),
        get_shorter_duration(how_long_should_thread_sleep, remaining_time)
    );
}

void perform_the_operation_thread_safely(std::function<void ()> operation) {
    std::lock_guard<std::mutex> lock(m);
    operation();
}

template<class T, std::size_t N>
void add_to_or_remove_from_notification_queue_randomly(
    NotificationQueue<T, N>& notification_queue
) {
    const auto starting_point = std::chrono::system_clock::now();
    while (true) {
        perform_the_operation_thread_safely([&]() {
            if (static_cast<bool>(generate_number_in_range_inclusive(0, 1))) {
                notification_queue.push(generate_notification());
                if (notification_queue.completely_filled()) {
                    is_it_time_to_run_the_analyzer = true;
                    cv.notify_one();
                }
            } else {
                notification_queue.get_out_of();
            }
        });

        auto how_long_should_thread_sleep = std::chrono::milliseconds(
            generate_number_in_range_inclusive(100, 1000)
        );

        bool continue_execution;
        std::chrono::milliseconds remaining_time;
        std::tie(continue_execution, remaining_time) = continue_running_thread(
            starting_point, how_long_should_thread_sleep
        );

        if (!continue_execution) {
            return;
        }

        std::this_thread::sleep_for(remaining_time);
    }
}

template<class T, std::size_t N>
void remove_notifications_from_queue_every_second(
    NotificationQueue<T, N>& notification_queue
) {
    const auto starting_point = std::chrono::system_clock::now();
    const auto HOW_LONG_SHOULD_THREAD_SLEEP =
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::seconds(1));
    while (true) {
        perform_the_operation_thread_safely([&]() {
            notification_queue.get_out_of();
        });

        bool continue_execution;
        std::chrono::milliseconds remaining_time;
        std::tie(continue_execution, remaining_time) = continue_running_thread(
            starting_point, HOW_LONG_SHOULD_THREAD_SLEEP
        );

        if (!continue_execution) {
            return;
        }

        std::this_thread::sleep_for(remaining_time);
    }
}

template<class T, std::size_t N>
void run_notification_queue_analyzer(
    NotificationQueue<T, N>& notification_queue,
    NotificationQueueAnalyzer& notification_queue_analyzer
) {
    const auto starting_point = std::chrono::system_clock::now();
    const auto HOW_LONG_SHOULD_THREAD_SLEEP =
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::minutes(1));

    while (true) {
        bool continue_execution;
        std::chrono::milliseconds remaining_time;
        std::tie(continue_execution, remaining_time) = continue_running_thread(
            starting_point, HOW_LONG_SHOULD_THREAD_SLEEP
        );

        if (!continue_execution) {
            return;
        }

        std::unique_lock<std::mutex> lock(m);

        if (cv.wait_for(
            lock,
            remaining_time,
            []{
                return is_it_time_to_run_the_analyzer;
            })
        ) {
            std::cout << "The queue analyzer started because 1 minute has passed since the previous run" << std::endl;
        } else {
            std::cout << "The queue analyzer started because the queue was full" << std::endl;
        }

        notification_queue_analyzer.analyze(notification_queue);

        is_it_time_to_run_the_analyzer = false;

        lock.unlock();
    }
}

int main() {
    const auto starting_point = std::chrono::system_clock::now();
    const std::size_t QUEUE_CAPACITY = 10;

    NotificationQueue<MessageType, QUEUE_CAPACITY> notification_queue;
    NotificationQueueAnalyzer notification_queue_analyzer;


    const int NUMBER_OF_ADDING_AND_REMOVING_THREADS = 3;
    std::array<std::thread, NUMBER_OF_ADDING_AND_REMOVING_THREADS> adding_and_removing_threads {
        std::thread(
            add_to_or_remove_from_notification_queue_randomly<MessageType, QUEUE_CAPACITY>,
            std::ref(notification_queue)
        ),
        std::thread(
            add_to_or_remove_from_notification_queue_randomly<MessageType, QUEUE_CAPACITY>,
            std::ref(notification_queue)
        ),
        std::thread(
            add_to_or_remove_from_notification_queue_randomly<MessageType, QUEUE_CAPACITY>,
            std::ref(notification_queue)
        ),
    };
    std::thread only_removing_thread(
        remove_notifications_from_queue_every_second<MessageType, QUEUE_CAPACITY>,
        std::ref(notification_queue)
    );
    std::thread thread_analysing_the_queue(
        run_notification_queue_analyzer<MessageType, QUEUE_CAPACITY>,
        std::ref(notification_queue),
        std::ref(notification_queue_analyzer)
    );

    for (auto& thread: adding_and_removing_threads) {
        thread.join();
    }
    only_removing_thread.join();
    thread_analysing_the_queue.join();

    std::cout
        << "Number of calls to the 'analyze' method"
        " of the 'notification_queue_analyzer' object: "
        << notification_queue_analyzer.get_number_of_launches()
        << std::endl;

    const auto end_point = std::chrono::system_clock::now();

    std::cout
        << "Program execution time: "
        << std::chrono::duration_cast<std::chrono::minutes>(
            end_point - starting_point).count() << " minutes ("
        << std::chrono::duration_cast<std::chrono::milliseconds>(
            end_point - starting_point).count() << " milliseconds)"
        << std::endl;

    return 0;
}
