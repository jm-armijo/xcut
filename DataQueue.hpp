#ifndef JM_LINE_QUEUE_HPP
#define JM_LINE_QUEUE_HPP

#include <atomic>
#include <mutex>
#include <unordered_map>

class DataQueue {
public:
    void        push(const std::string& value);
    void        push(const std::string& value, unsigned line_num);
    std::string pull();
    std::string pull(unsigned line_num);
    unsigned    size();
    bool        exists(unsigned key);
    bool        is_eof() const;
    void        set_eof();

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

std::string DataQueue::pull()
{
    std::lock_guard<std::mutex> guard(m_mtx_queue);
    auto it = m_queue.begin();
    auto value = it->second;
    m_queue.erase(m_queue.begin());

    return value;
}

std::string DataQueue::pull(unsigned line_num)
{
    std::lock_guard<std::mutex> guard(m_mtx_queue);
    auto value = m_queue.at(line_num);
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

bool DataQueue::is_eof() const
{
    return m_eof;
}

void DataQueue::set_eof()
{
    m_eof = true;
}
#endif //JM_LINE_QUEUE_HPP
