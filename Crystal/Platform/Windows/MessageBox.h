#pragma once

#include <string_view>
#include "CrystalWindow.h"

//God damn Macros
#ifdef MessageBox
#undef MessageBox
#endif

namespace Crystal {
	namespace MessageBox {
		enum class DialogResult {
			None   = 0x0,
			OK     = 0x1,
			Cancel = 0x2,
			Abort  = 0x3,
			Retry  = 0x4,
			Ignore = 0x5,
			Yes    = 0x6,
			No     = 0x7,
		};

		enum class MessageBoxIcon {
			None        = 0x0,
			Hand        = 0x10,
			Error       = Hand,
			Stop        = Error,
			Question    = 0x20,
			Exlamation  = 0x30,
			Warning     = Exlamation,
			Asterisk    = 0x40,
			Information = Asterisk
		};

		enum class MessageBoxButtons {
			OK               = 0x0,
			OKCancel         = 0x1,
			AbortRetryIgnore = 0x2,
			YesNoCancel      = 0x3,
			YesNo            = 0x4,
			RetryCancel      = 0x5
		};

		enum class MessageBoxDefaultButton {
			Button1 = 0x0,
			Button2 = 0x100,
			Button3 = 0x200,
		};

		enum class MessageBoxOptions {
			Default             = 0x0,
			RightAlign          = 0x80000,
			DefaultDesktopOnly  = 0x20000,
			RtlReading          = 0x100000,
			ServiceNotification = 0x200000
		};

		inline MessageBoxButtons operator |(MessageBoxButtons a, MessageBoxIcon b) noexcept {
			return static_cast<MessageBoxButtons>(static_cast<int>(a) | static_cast<int>(b));
		}

		inline MessageBoxButtons operator |(MessageBoxButtons a, MessageBoxDefaultButton b) noexcept {
			return static_cast<MessageBoxButtons>(static_cast<int>(a) | static_cast<int>(b));
		}

		inline MessageBoxButtons operator |(MessageBoxButtons a, MessageBoxOptions b) noexcept {
			return static_cast<MessageBoxButtons>(static_cast<int>(a) | static_cast<int>(b));
		}

		DialogResult Show(
			std::string_view text,
			std::string_view caption,
			MessageBoxButtons buttons,
			MessageBoxIcon icon,
			MessageBoxDefaultButton defaultButton,
			MessageBoxOptions options) noexcept 
		{
			return static_cast<DialogResult>(MessageBoxA(nullptr, text.data(), caption.data(), static_cast<int>(buttons | icon | defaultButton | options)));
		}

		DialogResult Show(
			std::string_view text,
			std::string_view caption,
			MessageBoxButtons buttons,
			MessageBoxIcon icon,
			MessageBoxDefaultButton defaultButton) noexcept
		{
			return Show(text, caption, buttons, icon, defaultButton, MessageBoxOptions::Default);
		}

		DialogResult Show(
			std::string_view text,
			std::string_view caption,
			MessageBoxButtons buttons,
			MessageBoxIcon icon) noexcept
		{
			return Show(text, caption, buttons, icon, MessageBoxDefaultButton::Button1);
		}

		DialogResult Show(std::string_view text, std::string_view caption, MessageBoxButtons buttons) noexcept {
			return Show(text, caption, buttons, MessageBoxIcon::None);
		}

		DialogResult Show(std::string_view text, std::string_view caption) noexcept { return Show(text, caption, MessageBoxButtons::OKCancel); }
		DialogResult Show(std::string_view text) noexcept { return Show(text, ""); }
	}
}