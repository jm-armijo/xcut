#ifndef JM_LINE_QUEUE_HPP
#define JM_LINE_QUEUE_HPP

#include <mutex>
#include <unordered_map>

class DataQueue {
public:
	void        push(const std::string& value);
	void        push(const std::string& value, unsigned line_num);
	std::string pull();
	std::string pull(unsigned line_num);
    unsigned    size() const;

private:
	std::unordered_map<unsigned, std::string> m_queue;
	std::mutex m_mtx_queue;
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

unsigned DataQueue::size() const
{
    return m_queue.size();
}

#endif //JM_LINE_QUEUE_HPP
