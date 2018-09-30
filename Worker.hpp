#ifndef JM_WORKER_HPP
#define JM_WORKER_HPP

#include <atomic>
#include <thread>

enum class Status {reading, processing, writing, done};

class Worker {
public:
    virtual void start();
    virtual bool done() const;
    Worker(const Arguments& args);
    virtual void update(const Status& status);
    virtual ~Worker();

protected:
    std::thread m_thread;
    std::atomic<bool> m_done{false};
    std::atomic<Status> m_status{Status::reading};
    const Arguments& m_args;

protected:
    virtual void doJob() = 0;
};

Worker::Worker(const Arguments& args) :
    m_args(args)
{
}

void Worker::update(const Status& status)
{
    m_status = status;
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
