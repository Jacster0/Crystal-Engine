#pragma once

#include <optional>
#include <queue>
#include <mutex>

namespace Crystal {
	template<class T>
	class ThreadSafeQueue {
	public:
		ThreadSafeQueue() noexcept;
		ThreadSafeQueue(const ThreadSafeQueue& rhs) noexcept;

		void Push(T value) noexcept;
		bool TryPop(T& value) noexcept;

		bool Empty() const noexcept;
		size_t Size() const noexcept;
	private:
		std::queue<T> m_queue;
		std::mutex m_mutex;
	};
}


template<class T>
Crystal::ThreadSafeQueue<T>::ThreadSafeQueue() noexcept { }

template<class T>
Crystal::ThreadSafeQueue<T>::ThreadSafeQueue(const ThreadSafeQueue& rhs) noexcept {
	std::scoped_lock lock(m_mutex);
	m_queue = rhs.m_queue;
}

template<class T>
void Crystal::ThreadSafeQueue<T>::Push(T value) noexcept {
	std::scoped_lock lock(m_mutex);
	m_queue.push();
}

template<class T>
bool Crystal::ThreadSafeQueue<T>::TryPop(T& value) noexcept {
	std::scoped_lock lock(m_mutex);

	if (m_queue.empty()) {
		return false;
	}

	value = m_queue.front();
	m_queue.pop();

	return true;
}

template<class T>
bool Crystal::ThreadSafeQueue<T>::Empty() const noexcept {
	std::scoped_lock lock(m_mutex);
	return m_queue.empty();
}

template<class T>
size_t Crystal::ThreadSafeQueue<T>::Size() const noexcept {
	std::scoped_lock lock(m_mutex);
	return m_queue.size();
}
