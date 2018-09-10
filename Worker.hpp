#ifndef JM_WORKER_HPP
#define JM_WORKER_HPP

#include <atomic>
#include <thread>

enum class Status {reading, processing, writing};

class Worker {
public:
    virtual void start();
    virtual bool done() const;
    Worker(const std::atomic<Status>& status, const Arguments& args);
    virtual ~Worker();

protected:
    std::thread m_thread;
    std::atomic<bool> m_done{false};
    const std::atomic<Status>& m_status;
    const Arguments& m_args;

protected:
    virtual void doJob() = 0;
};

Worker::Worker(const std::atomic<Status>& status, const Arguments& args) :
    m_status(status), m_args(args)
{
}

bool Worker::done() const
{
    return m_done;
}

void Worker::start()
{
    m_thread = std::thread(&Worker::doJob, this);
}

Worker::~Worker()
{
    if (m_thread.joinable()) {
        m_thread.join();
    }
}

#endif //JM_WORKER_HPP
