#include "Platform/GlfwPlatform/GlfwInput.h"
#include "Platform/GlfwPlatform/GlfwPlatform.h"
#include "ThirdParty/GLFW3.h"
#include "App/Editor.h"
#include "App/Event/KeyDownEvent.h"
#include "App/Event/KeyUpEvent.h"

#include <Common/assertion.h>
#include <Common/logging.h>

namespace ph::editor
{

PH_DEFINE_INTERNAL_LOG_GROUP(GlfwInput, EditorPlatform);

GlfwInput::GlfwInput()
	: PlatformInput()
	, m_glfwWindow(nullptr)
{}

GlfwInput::~GlfwInput() = default;

void GlfwInput::poll(const float64 deltaS)
{
	PH_ASSERT(m_glfwWindow);

	glfwPollEvents();
}

void GlfwInput::virtualizeCursor() const
{
	// TODO
}

void GlfwInput::unvirtualizeCursor() const
{
	// TODO
}

bool GlfwInput::isKeyDown(EKeyCode keyCode) const
{
	// TODO
	return false;
}

bool GlfwInput::isMouseButtonDown(EMouseCode mouseCode) const
{
	// TODO
	return false;
}

bool GlfwInput::isKeyUp(EKeyCode keyCode) const
{
	// TODO
	return false;
}

bool GlfwInput::isMouseButtonUp(EMouseCode mouseCode) const
{
	// TODO
	return false;
}

bool GlfwInput::isKeyHold(EKeyCode keyCode) const
{
	// TODO
	return false;
}

bool GlfwInput::isMouseButtonHold(EMouseCode mouseCode) const
{
	// TODO
	return false;
}

math::Vector2D GlfwInput::getCursorPositionPx() const
{
	// TODO
	return {};
}

math::Vector2D GlfwInput::getCursorMovementDeltaPx() const
{
	// TODO
	return {};
}

void GlfwInput::initialize(Editor& editor, GLFWwindow* const glfwWindow)
{
	if(!glfwWindow)
	{
		throw PlatformException(
			"cannot start input due to invalid GLFW window (null)");
	}

	m_glfwWindow = glfwWindow;

	glfwSetKeyCallback(m_glfwWindow,
		[](GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			Editor& editor = static_cast<GlfwPlatform*>(glfwGetWindowUserPointer(window))->getEditor();

			switch(action)
			{
			case GLFW_PRESS:
			{
				KeyDownEvent e(GLFW_TO_PHOTON_KEYCODE()[key]);
				editor.dispatchToEventQueue(e, editor.onKeyDown);
				break;
			}
			case GLFW_RELEASE:
			{
				KeyUpEvent e(GLFW_TO_PHOTON_KEYCODE()[key]);
				editor.dispatchToEventQueue(e, editor.onKeyUp);
				break;
			}
			case GLFW_REPEAT:
			{
				KeyDownEvent e(GLFW_TO_PHOTON_KEYCODE()[key], true);
				editor.dispatchToEventQueue(e, editor.onKeyDown);
				break;
			}
			}
		});

	// TODO: callbacks
}

void GlfwInput::terminate()
{
	glfwSetKeyCallback(m_glfwWindow, nullptr);

	m_glfwWindow = nullptr;
}

namespace
{

inline std::array<EKeyCode, GLFW_KEY_LAST + 1> make_key_code_map()
{
	std::array<EKeyCode, GLFW_KEY_LAST + 1> glfwToPhotonKeyCode;
	for(EKeyCode& keyCode : glfwToPhotonKeyCode)
	{
		keyCode = EKeyCode::Unknown;
	}

	glfwToPhotonKeyCode[GLFW_KEY_F1] = EKeyCode::F1;
	glfwToPhotonKeyCode[GLFW_KEY_F2] = EKeyCode::F2;
	glfwToPhotonKeyCode[GLFW_KEY_F3] = EKeyCode::F3;
	glfwToPhotonKeyCode[GLFW_KEY_F4] = EKeyCode::F4;
	glfwToPhotonKeyCode[GLFW_KEY_F5] = EKeyCode::F5;
	glfwToPhotonKeyCode[GLFW_KEY_F6] = EKeyCode::F6;
	glfwToPhotonKeyCode[GLFW_KEY_F7] = EKeyCode::F7;
	glfwToPhotonKeyCode[GLFW_KEY_F8] = EKeyCode::F8;
	glfwToPhotonKeyCode[GLFW_KEY_F9] = EKeyCode::F9;
	glfwToPhotonKeyCode[GLFW_KEY_F10] = EKeyCode::F10;
	glfwToPhotonKeyCode[GLFW_KEY_F11] = EKeyCode::F11;
	glfwToPhotonKeyCode[GLFW_KEY_F12] = EKeyCode::F12;

	glfwToPhotonKeyCode[GLFW_KEY_A] = EKeyCode::A;
	glfwToPhotonKeyCode[GLFW_KEY_B] = EKeyCode::B;
	glfwToPhotonKeyCode[GLFW_KEY_C] = EKeyCode::C;
	glfwToPhotonKeyCode[GLFW_KEY_D] = EKeyCode::D;
	glfwToPhotonKeyCode[GLFW_KEY_E] = EKeyCode::E;
	glfwToPhotonKeyCode[GLFW_KEY_F] = EKeyCode::F;
	glfwToPhotonKeyCode[GLFW_KEY_G] = EKeyCode::G;
	glfwToPhotonKeyCode[GLFW_KEY_H] = EKeyCode::H;
	glfwToPhotonKeyCode[GLFW_KEY_I] = EKeyCode::I;
	glfwToPhotonKeyCode[GLFW_KEY_J] = EKeyCode::J;
	glfwToPhotonKeyCode[GLFW_KEY_K] = EKeyCode::K;
	glfwToPhotonKeyCode[GLFW_KEY_L] = EKeyCode::L;
	glfwToPhotonKeyCode[GLFW_KEY_M] = EKeyCode::M;
	glfwToPhotonKeyCode[GLFW_KEY_N] = EKeyCode::N;
	glfwToPhotonKeyCode[GLFW_KEY_O] = EKeyCode::O;
	glfwToPhotonKeyCode[GLFW_KEY_P] = EKeyCode::P;
	glfwToPhotonKeyCode[GLFW_KEY_Q] = EKeyCode::Q;
	glfwToPhotonKeyCode[GLFW_KEY_R] = EKeyCode::R;
	glfwToPhotonKeyCode[GLFW_KEY_S] = EKeyCode::S;
	glfwToPhotonKeyCode[GLFW_KEY_T] = EKeyCode::T;
	glfwToPhotonKeyCode[GLFW_KEY_U] = EKeyCode::U;
	glfwToPhotonKeyCode[GLFW_KEY_V] = EKeyCode::V;
	glfwToPhotonKeyCode[GLFW_KEY_W] = EKeyCode::W;
	glfwToPhotonKeyCode[GLFW_KEY_X] = EKeyCode::X;
	glfwToPhotonKeyCode[GLFW_KEY_Y] = EKeyCode::Y;
	glfwToPhotonKeyCode[GLFW_KEY_Z] = EKeyCode::Z;

	return glfwToPhotonKeyCode;
}

inline std::array<EMouseCode, GLFW_MOUSE_BUTTON_LAST + 1> make_mouse_code_map()
{
	std::array<EMouseCode, GLFW_MOUSE_BUTTON_LAST + 1> glfwToPhotonMouseCode;
	for(EMouseCode& mouseCode : glfwToPhotonMouseCode)
	{
		mouseCode = EMouseCode::Unknown;
	}

	glfwToPhotonMouseCode[GLFW_MOUSE_BUTTON_LEFT] = EMouseCode::Left;
	glfwToPhotonMouseCode[GLFW_MOUSE_BUTTON_RIGHT] = EMouseCode::Right;
	glfwToPhotonMouseCode[GLFW_MOUSE_BUTTON_MIDDLE] = EMouseCode::Middle;

	return glfwToPhotonMouseCode;
}

}// end anonymous namespace

const std::array<EKeyCode, GLFW_KEY_LAST + 1>& GlfwInput::GLFW_TO_PHOTON_KEYCODE()
{
	static const auto glfwToPhotonKeyCode = make_key_code_map();
	return glfwToPhotonKeyCode;
}

const std::array<EMouseCode, GLFW_MOUSE_BUTTON_LAST + 1>& GlfwInput::GLFW_TO_PHOTON_MOUSECODE()
{
	static const auto glfwToPhotonMouseCode = make_mouse_code_map();
	return glfwToPhotonMouseCode;
}

}// end namespace ph::editor
