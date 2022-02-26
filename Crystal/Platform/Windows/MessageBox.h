#pragma once

#include <string_view>
#include "CrystalWindow.h"

//God-damn Macros
#ifdef MessageBox
#undef MessageBox
#endif

namespace Crystal::MessageBox {
	enum class DialogResult {
		None = 0x0,
		OK = 0x1,
		Cancel = 0x2,
		Abort = 0x3,
		Retry = 0x4,
		Ignore = 0x5,
		Yes = 0x6,
		No = 0x7,
	};

	enum class Icon {
		None = 0x0,
		Hand = 0x10,
		Error = Hand,
		Stop = Error,
		Question = 0x20,
		Exclamation = 0x30,
		Warning = Exclamation,
		Asterisk = 0x40,
		Information = Asterisk
	};

	enum class Buttons {
		OK = 0x0,
		OKCancel = 0x1,
		AbortRetryIgnore = 0x2,
		YesNoCancel = 0x3,
		YesNo = 0x4,
		RetryCancel = 0x5
	};

	enum class DefaultButton {
		Button1 = 0x0,
		Button2 = 0x100,
		Button3 = 0x200,
	};

	enum class Options {
		Default = 0x0,
		RightAlign = 0x80000,
		DefaultDesktopOnly = 0x20000,
		RtlReading = 0x100000,
		ServiceNotification = 0x200000
	};

	inline Buttons operator |(Buttons a, Icon b) noexcept {
		return static_cast<Buttons>(static_cast<int>(a) | static_cast<int>(b));
	}

	inline Buttons operator |(Buttons a, DefaultButton b) noexcept {
		return static_cast<Buttons>(static_cast<int>(a) | static_cast<int>(b));
	}

	inline Buttons operator |(Buttons a, Options b) noexcept {
		return static_cast<Buttons>(static_cast<int>(a) | static_cast<int>(b));
	}

	DialogResult Show(
            std::string_view text,
            std::string_view caption,
            Buttons buttons,
            Icon icon,
            DefaultButton defaultButton,
            Options options) noexcept
	{
		return static_cast<DialogResult>(MessageBoxA(nullptr, text.data(), caption.data(), static_cast<int>(buttons | icon | defaultButton | options)));
	}

	DialogResult Show(
            std::string_view text,
            std::string_view caption,
            Buttons buttons,
            Icon icon,
            DefaultButton defaultButton) noexcept
	{
		return Show(text, caption, buttons, icon, defaultButton, Options::Default);
	}

	DialogResult Show(
            std::string_view text,
            std::string_view caption,
            Buttons buttons,
            Icon icon) noexcept
	{
		return Show(text, caption, buttons, icon, DefaultButton::Button1);
	}

	DialogResult Show(std::string_view text, std::string_view caption, Buttons buttons) noexcept {
		return Show(text, caption, buttons, Icon::None);
	}

	DialogResult Show(std::string_view text, std::string_view caption) noexcept { return Show(text, caption, Buttons::OKCancel); }
	DialogResult Show(std::string_view text) noexcept { return Show(text, ""); }
}