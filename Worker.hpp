#ifndef JM_WORKER_HPP
#define JM_WORKER_HPP

#include <atomic>
#include <thread>

enum class Status {reading, processing, writing};

class Worker {
public:
    virtual void start();
    virtual bool done() const;
    Worker(const std::atomic<Status>& status);
    virtual ~Worker();

protected:
    std::thread m_thread;
    std::atomic<bool> m_done{false};
    virtual void do_job() = 0;
    const std::atomic<Status>& m_status;
};

Worker::Worker(const std::atomic<Status>& status) : m_status(status)
{
}

bool Worker::done() const
{
    return m_done;
}

void Worker::start()
{
    m_thread = std::thread(&Worker::do_job, this);
}

Worker::~Worker()
{
    if (m_thread.joinable()) {
        m_thread.join();
    }
}

#endif //JM_WORKER_HPP
