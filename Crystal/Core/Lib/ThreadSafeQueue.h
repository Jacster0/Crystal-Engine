#pragma once

#include <optional>
#include <queue>
#include <mutex>

namespace Crystal {
	template<class T>
	class ThreadSafeQueue {
	public:
		ThreadSafeQueue() noexcept = default;
		ThreadSafeQueue(const ThreadSafeQueue& rhs) noexcept {
            std::scoped_lock lock(m_mutex);
            m_queue = rhs.m_queue;
        }

		void Push(T value) noexcept {
            std::scoped_lock lock(m_mutex);
            m_queue.push();
        }
        [[nodiscard]] bool TryPop(T& value) noexcept {
            std::scoped_lock lock(m_mutex);

            if (m_queue.empty()) {
                return false;
            }

            value = m_queue.front();
            m_queue.pop();

            return true;
        }

		[[nodiscard]] bool Empty() const noexcept {
            std::scoped_lock lock(m_mutex);
            return m_queue.empty();
        }
		[[nodiscard]] size_t Size() const noexcept {
            std::scoped_lock lock(m_mutex);
            return m_queue.size();
        }
	private:
		std::queue<T> m_queue;
		std::mutex m_mutex;
	};
}