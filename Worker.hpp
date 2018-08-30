#ifndef JM_WORKER_HPP
#define JM_WORKER_HPP

#include <atomic>
#include <thread>

class Worker {
public:
    virtual void start();
    virtual bool done() const;
    virtual ~Worker();

protected:
    std::thread m_thread;
    std::atomic<bool> m_done{false};
    virtual void do_job() = 0;
};

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
