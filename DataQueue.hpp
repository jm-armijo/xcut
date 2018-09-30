#ifndef JM_LINE_QUEUE_HPP
#define JM_LINE_QUEUE_HPP

#include <atomic>
#include <mutex>
#include <unordered_map>

#include "Line.hpp"

class DataQueue {
public:
    void        push(const Line& line);
    Line        pull(unsigned line_num);
    Line        pullNext();
    unsigned    size();
    bool        exists(unsigned key);
    unsigned    getCountIn() const;
    unsigned    getCountOut() const;

private:
    std::unordered_map<unsigned,Line> m_queue;
    std::mutex m_mtx_queue;
    std::atomic<bool> m_eof{false};
    std::atomic<unsigned> m_count_in  {0u};
    std::atomic<unsigned> m_count_out {0u};
};

void DataQueue::push(const Line& line)
{
    std::lock_guard<std::mutex> guard(m_mtx_queue);
    m_queue.insert({line.getNum(), line});
    ++m_count_in;

    return;
}

Line DataQueue::pull(unsigned line_num)
{
    std::lock_guard<std::mutex> guard(m_mtx_queue);

    auto line = Line();
    if (m_queue.count(line_num) > 0) {
        line = m_queue[line_num];
        m_queue.erase(line_num);
        ++m_count_out;
    }

    return line;
}

Line DataQueue::pullNext()
{
    std::lock_guard<std::mutex> guard(m_mtx_queue);

    auto line = Line();
    if (m_queue.size() > 0) {
        auto it = m_queue.begin();
        line = it->second;
        m_queue.erase(it->first);
        ++m_count_out;
    }

    return line;
}

unsigned DataQueue::getCountIn() const
{
    return m_count_in;
}

unsigned DataQueue::getCountOut() const
{
    return m_count_out;
}


unsigned DataQueue::size()
{
    std::lock_guard<std::mutex> guard(m_mtx_queue);
    return m_queue.size();
}

bool DataQueue::exists(unsigned key)
{
    std::lock_guard<std::mutex> guard(m_mtx_queue);
    return m_queue.count(key) > 0;
}

#endif //JM_LINE_QUEUE_HPP
