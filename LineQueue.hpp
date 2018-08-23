#ifndef JM_LINE_QUEUE_HPP
#define JM_LINE_QUEUE_HPP

#include <mutex>
#include <queue>

class LineQueue {
public:
	void        push(const std::string& input);
	std::string pull();
    unsigned    size() const;

private:
	std::queue<std::string> m_queue;
	std::mutex m_mtx_lines;
};

void LineQueue::push(const std::string& input)
{
	std::lock_guard<std::mutex> guard(m_mtx_lines);
	m_queue.push(input);
	return;
}

std::string LineQueue::pull()
{
	std::lock_guard<std::mutex> guard(m_mtx_lines);
	auto val = m_queue.front();
	m_queue.pop();
	return val;
}

unsigned LineQueue::size() const
{
    return m_queue.size();
}

#endif //JM_LINE_QUEUE_HPP
