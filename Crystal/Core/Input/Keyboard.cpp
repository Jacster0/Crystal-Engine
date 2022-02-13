#include "Keyboard.h"

namespace Crystal {
    bool Keyboard::KeyIsPressed(KeyCode keycode) const noexcept {
        return KeyIsPressed(static_cast<uint8_t>(keycode));
    }

    bool Keyboard::KeyIsPressedOnce(KeyCode keycode) noexcept {
        return KeyIsPressedOnce(static_cast<uint8_t>(keycode));
    }

    bool Keyboard::KeyIsPressed(uint8_t keycode) const noexcept {
        return m_keyStates[keycode];
    }

    bool Keyboard::KeyIsPressedOnce(uint8_t keycode) noexcept {
        if (m_keyIsPressedOnce) {
            m_keyStates[keycode] = false;

            return false;
        }

        if (KeyIsPressed(keycode)) {
            m_keyStates[keycode] = false;
            m_keyIsPressedOnce = true;

            return true;
        }
        return false;
    }

    std::optional<Keyboard::Event> Keyboard::ReadKey() noexcept {
        if (!m_keyBuffer.empty()) {
            const Event e = m_keyBuffer.front();
            m_keyBuffer.pop();
            return e;
        }
        return std::nullopt;
    }

    bool Keyboard::KeyIsEmpty() const noexcept {
        return m_keyBuffer.empty();
    }

    void Keyboard::FlushKey() noexcept {
        m_keyBuffer = std::queue<Event>();
    }

    std::optional<char> Keyboard::ReadChar() noexcept {
        if (!m_charBuffer.empty()) {
	        const auto charCode = m_charBuffer.front();
            m_charBuffer.pop();
            return charCode;
        }
        return std::nullopt;;
    }

    bool Keyboard::CharIsEmpty() const noexcept {
        return m_charBuffer.empty();
    }

    void Keyboard::FlushChar() noexcept {
        m_charBuffer = std::queue<char>();
    }

    void Keyboard::Flush() noexcept {
        FlushKey();
        FlushChar();
    }

    void Keyboard::EnableAutorepeat() noexcept {
        m_autorepeatEnabled = true;
    }

    void Keyboard::DisableAutorepeat() noexcept {
        m_autorepeatEnabled = false;
    }

    bool Keyboard::AutorepeatIsEnabled() const noexcept {
        return m_autorepeatEnabled;;
    }

    void Keyboard::OnKeyPressed(KeyCode keycode) noexcept {
        OnKeyPressed(static_cast<uint8_t>(keycode));
    }

    void Keyboard::OnKeyReleased(KeyCode keycode) noexcept {
        OnKeyReleased(static_cast<uint8_t>(keycode));
    }

    void Keyboard::OnKeyPressed(uint8_t keycode) noexcept {
        m_keyStates[keycode] = true;
        m_keyBuffer.push(Event(Event::Type::Press, keycode));
        TrimBuffer(m_keyBuffer);
    }

    void Keyboard::OnKeyReleased(uint8_t keycode) noexcept {
        m_keyIsPressedOnce = false;
        m_keyStates[keycode] = false;
        m_keyBuffer.push(Event(Event::Type::Release, keycode));
        TrimBuffer(m_keyBuffer);
    }

    void Keyboard::OnChar(char character) noexcept {
        m_charBuffer.push(character);
        TrimBuffer(m_charBuffer);
    }

    void Keyboard::ClearState() noexcept {
        m_keyStates.reset();
    }

    template<typename T>
    void Keyboard::TrimBuffer(std::queue<T>& buffer) noexcept {
        while (buffer.size() > m_bufferSize) {
            buffer.pop();
        }
    }
}