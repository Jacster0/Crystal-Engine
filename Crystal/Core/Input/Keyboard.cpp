#include "Keyboard.h"

namespace Crystal {
    bool Keyboard::KeyIsPressed(KeyCode keycode) const noexcept {
        return KeyIsPressed(static_cast<uint8_t>(keycode));
    }

    bool Keyboard::KeyIsPressedOnce(KeyCode keycode) noexcept {
        return KeyIsPressedOnce(static_cast<uint8_t>(keycode));
    }

    bool Keyboard::KeyIsPressed(uint8_t keycode) const noexcept {
        return m_keystates[keycode];
    }

    bool Keyboard::KeyIsPressedOnce(uint8_t keycode) noexcept {
        if (m_keyIsPressedOnce) {
            m_keystates[keycode] = false;

            return false;
        }

        if (KeyIsPressed(keycode)) {
            m_keystates[keycode] = false;
            m_keyIsPressedOnce = true;

            return true;
        }
        return false;
    }

    std::optional<Keyboard::Event> Keyboard::ReadKey() noexcept {
        if (m_keybuffer.size() > 0u) {
            Keyboard::Event e = m_keybuffer.front();
            m_keybuffer.pop();
            return e;
        }
        return std::nullopt;
    }

    bool Keyboard::KeyIsEmpty() const noexcept {
        return m_keybuffer.empty();
    }

    void Keyboard::FlushKey() noexcept {
        m_keybuffer = std::queue<Event>();
    }

    std::optional<char> Keyboard::ReadChar() noexcept {
        if (m_charbuffer.size() > 0u) {
            auto charcode = m_charbuffer.front();
            m_charbuffer.pop();
            return charcode;
        }
        return std::nullopt;;
    }

    bool Keyboard::CharIsEmpty() const noexcept {
        return m_charbuffer.empty();
    }

    void Keyboard::FlushChar() noexcept {
        m_charbuffer = std::queue<char>();
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
        m_keystates[keycode] = true;
        m_keybuffer.push(Keyboard::Event(Keyboard::Event::Type::Press, keycode));
        TrimBuffer(m_keybuffer);
    }

    void Keyboard::OnKeyReleased(uint8_t keycode) noexcept {
        m_keyIsPressedOnce = false;
        m_keystates[keycode] = false;
        m_keybuffer.push(Keyboard::Event(Keyboard::Event::Type::Release, keycode));
        TrimBuffer(m_keybuffer);
    }

    void Keyboard::OnChar(char character) noexcept {
        m_charbuffer.push(character);
        TrimBuffer(m_charbuffer);
    }

    void Keyboard::ClearState() noexcept {
        m_keystates.reset();
    }

    template<typename T>
    void Keyboard::TrimBuffer(std::queue<T>& buffer) noexcept {
        while (buffer.size() > m_bufferSize) {
            buffer.pop();
        }
    }
}