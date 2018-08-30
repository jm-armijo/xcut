#ifndef JM_LINE_QUEUE_HPP
#define JM_LINE_QUEUE_HPP

#include <atomic>
#include <mutex>
#include <unordered_map>

class DataQueue {
public:
    void        push(const std::string& value);
    void        push(const std::string& value, unsigned line_num);
    unsigned    nextKey();
    std::string pull(unsigned line_num);
    unsigned    size();
    bool        exists(unsigned key);

private:
    std::unordered_map<unsigned, std::string> m_queue;
    std::mutex m_mtx_queue;
    std::atomic<bool> m_eof{false};
};

void DataQueue::push(const std::string& value)
{
    static auto line_num = 0u;
    std::lock_guard<std::mutex> guard(m_mtx_queue);
    m_queue.insert({line_num++, value});

    return;
}

void DataQueue::push(const std::string& value, unsigned line_num)
{
    std::lock_guard<std::mutex> guard(m_mtx_queue);
    m_queue.insert({line_num, value});

    return;
}

unsigned DataQueue::nextKey()
{
    std::lock_guard<std::mutex> guard(m_mtx_queue);
    auto it = m_queue.begin();

    return it->first;
}

std::string DataQueue::pull(unsigned line_num)
{
    std::lock_guard<std::mutex> guard(m_mtx_queue);

    auto value = m_queue[line_num];
    m_queue.erase(line_num);

    return value;
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
