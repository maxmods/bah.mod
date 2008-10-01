/*
  Copyright (c) 2008 Bruce A Henderson
 
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:
  
  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.
  
  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/ 

#include "glue.h"

	
MaxConnection::MaxConnection(CEGUI::Event::Connection conn)
	: conn(conn)
{
}

MaxEventArgs::MaxEventArgs(const CEGUI::EventArgs & args)
	: eventArgs(args)
{
}

const CEGUI::EventArgs & MaxEventArgs::Args() {
	return eventArgs;
}

MaxCEEventCallback::MaxCEEventCallback(BBObject * handle)
	: callbackHandle(handle)
{
}

bool MaxCEEventCallback::EventCallback(const CEGUI::EventArgs& args) {
	return _bah_cegui_TCEEventCallback__callback(callbackHandle, 
		newObjectForEventArgs((CEGUI::EventArgs *)&args, new MaxEventArgs(args)));
}

MaxConnection * MaxCEEventCallback::subscribeEvent(CEGUI::Window * handle, const CEGUI::utf8 * name) {
	return new MaxConnection(handle->subscribeEvent(name, CEGUI::Event::Subscriber(&MaxCEEventCallback::EventCallback, this)));
}

MaxLogger::MaxLogger(BBObject * handle)
	: maxHandle(handle)
{
}

void MaxLogger::logEvent(const CEGUI::String& message, CEGUI::LoggingLevel level) {
	_bah_cegui_TCECustomLogger__logEvent(maxHandle, message.data(), level);
}

// *************************************************

MaxCEEventCallback * bmx_cegui_eventcallback_new(BBObject * handle) {
	return new MaxCEEventCallback(handle);
}

MaxConnection * bmx_cegui_eventset_subscribeevent(CEGUI::Window * handle, const CEGUI::utf8 * name, MaxCEEventCallback * cb) {
	return cb->subscribeEvent(handle, name);
}

// *************************************************

BBObject * newObjectForWindow(CEGUI::Window * window) {

	if (dynamic_cast<CEGUI::Editbox*>(window)) {
		return _bah_cegui_TCEEditbox__create(window);
	}
	
	if (dynamic_cast<CEGUI::FrameWindow*>(window)) {
		return _bah_cegui_TCEFrameWindow__create(window);
	}
	
	if (dynamic_cast<CEGUI::GUISheet*>(window)) {
		return _bah_cegui_TCEGUISheet__create(window);
	}
	
	if (dynamic_cast<CEGUI::Checkbox*>(window)) {
		return _bah_cegui_TCECheckbox__create(window);
	}
	
	if (dynamic_cast<CEGUI::RadioButton*>(window)) {
		return _bah_cegui_TCERadioButton__create(window);
	}
	
	if (dynamic_cast<CEGUI::PushButton*>(window)) {
		return _bah_cegui_TCEPushButton__create(window);
	}
	
	if (dynamic_cast<CEGUI::TabButton*>(window)) {
		return _bah_cegui_TCETabButton__create(window);
	}
	
	if (dynamic_cast<CEGUI::Combobox*>(window)) {
		return _bah_cegui_TCECombobox__create(window);
	}
	
	if (dynamic_cast<CEGUI::ScrolledItemListBase*>(window)) {
		return _bah_cegui_TCEScrolledItemListBase__create(window);
	}
	
	if (dynamic_cast<CEGUI::ItemListbox*>(window)) {
		return _bah_cegui_TCEItemListBox__create(window);
	}
	
	if (dynamic_cast<CEGUI::PopupMenu*>(window)) {
		return _bah_cegui_TCEPopupMenu__create(window);
	}
	
	if (dynamic_cast<CEGUI::Menubar*>(window)) {
		return _bah_cegui_TCEMenubar__create(window);
	}
	
	if (dynamic_cast<CEGUI::MenuBase*>(window)) {
		return _bah_cegui_TCEMenuBase__create(window);
	}
	
	if (dynamic_cast<CEGUI::ItemListBase*>(window)) {
		return _bah_cegui_TCEItemListBase__create(window);
	}
	
	if (dynamic_cast<CEGUI::Tree*>(window)) {
		return _bah_cegui_TCETree__create(window);
	}
	
	if (dynamic_cast<CEGUI::Tooltip*>(window)) {
		return _bah_cegui_TCETooltip__create(window);
	}
	
	if (dynamic_cast<CEGUI::Titlebar*>(window)) {
		return _bah_cegui_TCETitlebar__create(window);
	}
	
	if (dynamic_cast<CEGUI::TabControl*>(window)) {
		return _bah_cegui_TCETabControl__create(window);
	}
	
	if (dynamic_cast<CEGUI::Spinner*>(window)) {
		return _bah_cegui_TCESpinner__create(window);
	}
	
	if (dynamic_cast<CEGUI::Slider*>(window)) {
		return _bah_cegui_TCESlider__create(window);
	}
	
	if (dynamic_cast<CEGUI::ScrolledContainer*>(window)) {
		return _bah_cegui_TCEScrolledContainer__create(window);
	}
	
	if (dynamic_cast<CEGUI::Scrollbar*>(window)) {
		return _bah_cegui_TCEScrollbar__create(window);
	}
	
	if (dynamic_cast<CEGUI::ScrollablePane*>(window)) {
		return _bah_cegui_TCEScrollablePane__create(window);
	}
	
	if (dynamic_cast<CEGUI::ProgressBar*>(window)) {
		return _bah_cegui_TCEProgressBar__create(window);
	}
	
	if (dynamic_cast<CEGUI::MultiLineEditbox*>(window)) {
		return _bah_cegui_TCEMultiLineEditbox__create(window);
	}
	
	if (dynamic_cast<CEGUI::MultiColumnList*>(window)) {
		return _bah_cegui_TCEMultiColumnList__create(window);
	}
	
	if (dynamic_cast<CEGUI::ListHeaderSegment*>(window)) {
		return _bah_cegui_TCEListHeaderSegment__create(window);
	}
	
	if (dynamic_cast<CEGUI::ListHeader*>(window)) {
		return _bah_cegui_TCEListHeader__create(window);
	}
	
	if (dynamic_cast<CEGUI::ComboDropList*>(window)) {
		return _bah_cegui_TCEComboDropList__create(window);
	}
	
	if (dynamic_cast<CEGUI::Listbox*>(window)) {
		return _bah_cegui_TCEListbox__create(window);
	}
	
	if (dynamic_cast<CEGUI::MenuItem*>(window)) {
		return _bah_cegui_TCEMenuItem__create(window);
	}
	
	if (dynamic_cast<CEGUI::ItemEntry*>(window)) {
		return _bah_cegui_TCEItemEntry__create(window);
	}
	
	if (dynamic_cast<CEGUI::DragContainer*>(window)) {
		return _bah_cegui_TCEDragContainer__create(window);
	}
	
	if (dynamic_cast<CEGUI::ClippedContainer*>(window)) {
		return _bah_cegui_TCEClippedContainer__create(window);
	}
	
	if (dynamic_cast<CEGUI::GroupBox*>(window)) {
		return _bah_cegui_TCEGroupBox__create(window);
	}
	
	// defaults to window...
	return _bah_cegui_TCEWindow__create(window);
}


// *************************************************

BBObject * newObjectForEventArgs(CEGUI::EventArgs * args, MaxEventArgs * maxArgs) {
	
	if (dynamic_cast<CEGUI::MouseCursorEventArgs *>(args)) {
		return _bah_cegui_TCEMouseCursorEventArgs__create(maxArgs);
	}
	
	if (dynamic_cast<CEGUI::KeyEventArgs *>(args)) {
		return _bah_cegui_TCEKeyEventArgs__create(maxArgs);
	}
	
	if (dynamic_cast<CEGUI::ActivationEventArgs *>(args)) {
		return _bah_cegui_TCEActivationEventArgs__create(maxArgs);
	}
	
	if (dynamic_cast<CEGUI::DragDropEventArgs *>(args)) {
		return _bah_cegui_TCEDragDropEventArgs__create(maxArgs);
	}
	
	if (dynamic_cast<CEGUI::HeaderSequenceEventArgs *>(args)) {
		return _bah_cegui_TCEHeaderSequenceEventArgs__create(maxArgs);
	}
	
	if (dynamic_cast<CEGUI::MouseEventArgs *>(args)) {
		return _bah_cegui_TCEMouseEventArgs__create(maxArgs);
	}

	if (dynamic_cast<CEGUI::TreeEventArgs *>(args)) {
		return _bah_cegui_TCETreeEventArgs__create(maxArgs);
	}

	if (dynamic_cast<CEGUI::UpdateEventArgs *>(args)) {
		return _bah_cegui_TCEUpdateEventArgs__create(maxArgs);
	}
	
	if (dynamic_cast<CEGUI::WindowEventArgs *>(args)) {
		return _bah_cegui_TCEWindowEventArgs__create(maxArgs);
	}
	
	// defaults to base type
	return _bah_cegui_TCEEventArgs__create(maxArgs);
}

// *************************************************

void bmx_cegui_eventargs_delete(MaxEventArgs * args) {
	delete args;
}

// *************************************************

CEGUI::Renderer * bmx_cegui_new_oglrenderer() {

	// we pass in the codec because otherwise it will attempt to load it from the shared library
	return new CEGUI::OpenGLRenderer(0);
}

CEGUI::System * bmx_cegui_new_system(CEGUI::Renderer * r) {
	// we pass in the renderer otherwise it will attepmt to load it from the shared library
	CEGUI::System * sys =  new CEGUI::System(r);
	
	return sys;
}

void bmx_cegui_delete_system(CEGUI::System * s) {
	delete s;
}

void bmx_cegui_delete_renderer(CEGUI::Renderer * r) {
	delete r;
}

// *************************************************

CEGUI::Scheme * bmx_cegui_schememanager_loadScheme(const CEGUI::utf8 * scheme, const CEGUI::utf8 * resourceGroup) {
	return CEGUI::SchemeManager::getSingleton().loadScheme(scheme, resourceGroup);
}

// *************************************************

void bmx_cegui_system_setDefaultFont(CEGUI::System * sys, const CEGUI::utf8 * font) {
	sys->setDefaultFont(font);
}

void bmx_cegui_system_setDefaultMouseCursor(CEGUI::System * sys, const CEGUI::utf8 * l, const CEGUI::utf8 * kind) {
	sys->setDefaultMouseCursor(l, kind);
}

void bmx_cegui_system_renderGUI(CEGUI::System * sys) {
	sys->renderGUI();
}

void bmx_cegui_system_setGUISheet(CEGUI::System * sys, CEGUI::Window * window) {
	sys->setGUISheet(window);
}

// *************************************************

CEGUI::WindowManager * bmx_cegui_windowmanager_getsingleton() {
	return &CEGUI::WindowManager::getSingleton();
}

BBObject * bmx_cegui_windowmanager_loadWindowLayout(CEGUI::WindowManager * mgr, const CEGUI::utf8 * layout, const CEGUI::utf8 * namePrefix, const CEGUI::utf8 * resourceGroup) {
	return newObjectForWindow(mgr->loadWindowLayout(layout, namePrefix, resourceGroup));
}

BBObject * bmx_cegui_windowmanager_getwindow(CEGUI::WindowManager * mgr, const CEGUI::utf8 * name) {
	return newObjectForWindow(mgr->getWindow(name));
}

BBObject * bmx_cegui_windowmanager_createwindow(CEGUI::WindowManager * mgr, const CEGUI::utf8 * windowType, const CEGUI::utf8 * name, const CEGUI::utf8 * prefix) {
	return newObjectForWindow(mgr->createWindow(windowType, name, prefix));
}

bool bmx_cegui_windowmanager_iswindowpresent(CEGUI::WindowManager * mgr, const CEGUI::utf8 * name) {
	return mgr->isWindowPresent(name);
}

void bmx_cegui_windowmanager_destroyallwindows(CEGUI::WindowManager * mgr) {
	mgr->destroyAllWindows();
}

void bmx_cegui_windowmanager_destroywindowwindow(CEGUI::WindowManager * mgr, CEGUI::Window * window) {
	mgr->destroyWindow(window);
}

void bmx_cegui_windowmanager_destroywindowname(CEGUI::WindowManager * mgr, const CEGUI::utf8 * window) {
	mgr->destroyWindow(window);
}

void bmx_cegui_windowmanager_renamewindowwindow(CEGUI::WindowManager * mgr, CEGUI::Window * window, const CEGUI::utf8 * newName) {
	mgr->renameWindow(window, newName);
}

void bmx_cegui_windowmanager_renamewindowname(CEGUI::WindowManager * mgr, const CEGUI::utf8 * window, const CEGUI::utf8 * newName) {
	mgr->renameWindow(window, newName);
}


// *************************************************

bool bmx_cegui_system_injectTimePulse(CEGUI::System * sys, float t) {
	return sys->injectTimePulse(t);
}

bool bmx_cegui_system_injectMousePosition(CEGUI::System * sys, int x, int y) {
	return sys->injectMousePosition(static_cast<float>(x),static_cast<float>(y)); 
}

bool bmx_cegui_system_injectMouseButtonDown(CEGUI::System * sys, int button) {
	switch (button) {
		case 1:
			return sys->injectMouseButtonDown(CEGUI::LeftButton);
		case 2:
			return sys->injectMouseButtonDown(CEGUI::RightButton);
		case 3:
			return sys->injectMouseButtonDown(CEGUI::MiddleButton);
		default:
			return sys->injectMouseButtonDown(CEGUI::NoButton);
	}
	return CEGUI::NoButton;
}

bool bmx_cegui_system_injectMouseButtonUp(CEGUI::System * sys, int button) {
	switch (button) {
		case 1:
			return sys->injectMouseButtonUp(CEGUI::LeftButton);
		case 2:
			return sys->injectMouseButtonUp(CEGUI::RightButton);
		case 3:
			return sys->injectMouseButtonUp(CEGUI::MiddleButton);
		default:
			return sys->injectMouseButtonUp(CEGUI::NoButton);
	}
	return CEGUI::NoButton;
}

bool bmx_cegui_system_injectkeydown(CEGUI::System * sys, int key) {
	return sys->injectKeyDown(bmx_cegui_mapmaxtocekey(key));
}

bool bmx_cegui_system_injectkeyup(CEGUI::System * sys, int key) {
	return sys->injectKeyUp(bmx_cegui_mapmaxtocekey(key));
}

bool bmx_cegui_system_injectchar(CEGUI::System * sys, CEGUI::utf32 key) {
	return sys->injectChar(key);
}

bool bmx_cegui_system_injectMouseWheelChange(CEGUI::System * sys, int delta) {
	return sys->injectMouseWheelChange(static_cast<float>(delta));
}

// *************************************************

CEGUI::UDim guessUDim(float value) {
	if ((value >=0) && (value <= 1.0)) {
		return CEGUI::UDim(value, value);
	} else {
		return CEGUI::UDim(0, value);
	}
}


void bmx_cegui_window_settext(CEGUI::Window * window, const CEGUI::utf8 * text) {
	window->setText(text);
}

void bmx_cegui_window_deactivate(CEGUI::Window * window) {
	window->deactivate();
}

void bmx_cegui_window_setposition(CEGUI::Window * window, float x, float y) {
	window->setPosition(CEGUI::UVector2(guessUDim(x), guessUDim(y)));
}

void bmx_cegui_window_setxposition(CEGUI::Window * window, float x) {
	window->setXPosition(guessUDim(x));
}

void bmx_cegui_window_setyposition(CEGUI::Window * window, float y) {
	window->setYPosition(guessUDim(y));
}

void bmx_cegui_window_setsize(CEGUI::Window * window, float width, float height) {
	window->setSize(CEGUI::UVector2(guessUDim(width), guessUDim(height)));
}

void bmx_cegui_window_setwidth(CEGUI::Window * window, float width) {
	window->setWidth(guessUDim(width));
}

void bmx_cegui_window_setheight(CEGUI::Window * window, float height) {
	window->setHeight(guessUDim(height));
}

void bmx_cegui_window_setmaxsize(CEGUI::Window * window, float width, float height) {
	window->setMaxSize(CEGUI::UVector2(guessUDim(width), guessUDim(height)));
}

void bmx_cegui_window_setminsize(CEGUI::Window * window, float width, float height) {
	window->setMinSize(CEGUI::UVector2(guessUDim(width), guessUDim(height)));
}

void bmx_cegui_window_addchildwindowwindow(CEGUI::Window * window, CEGUI::Window * win) {
	window->addChildWindow(win);
}

void bmx_cegui_window_addchildwindow(CEGUI::Window * window, const CEGUI::utf8 * name) {
	window->addChildWindow(name);
}

void bmx_cegui_window_setproperty(CEGUI::Window * window, const CEGUI::utf8 * name, const CEGUI::utf8 * value) {
	window->setProperty(name, value);
}

void bmx_cegui_window_hide(CEGUI::Window * window) {
	window->hide();
}

void bmx_cegui_window_show(CEGUI::Window * window) {
	window->show();
}

const CEGUI::utf8 * bmx_cegui_window_gettype(CEGUI::Window * window) {
	return window->getType().data();
}

void bmx_cegui_window_activate(CEGUI::Window * window) {
	window->activate();
}

// *************************************************

void bmx_cegui_connection_delete(MaxConnection * conn) {
	delete conn;
}

// *************************************************

bool bmx_cegui_checkbox_isselected(CEGUI::Checkbox * cb) {
	return cb->isSelected();
}

void bmx_cegui_checkbox_setselected(CEGUI::Checkbox * cb, bool selected) {
	cb->setSelected(selected);
}

// *************************************************

float bmx_cegui_progressbar_getprogress(CEGUI::ProgressBar * pb) {
	return pb->getProgress();
}

float bmx_cegui_progressbar_getstep(CEGUI::ProgressBar * pb) {
	return pb->getStep();
}

void bmx_cegui_progressbar_setprogress(CEGUI::ProgressBar * pb, float progress) {
	pb->setProgress(progress);
}

void bmx_cegui_progressbar_setstepsize(CEGUI::ProgressBar * pb, float stepVal) {
	pb->setStepSize(stepVal);
}

void bmx_cegui_progressbar_dostep(CEGUI::ProgressBar * pb) {
	pb->step();
}

void bmx_cegui_progressbar_adjustprogress(CEGUI::ProgressBar * pb, float delta) {
	pb->adjustProgress(delta);
}

// *************************************************

CEGUI::uint bmx_cegui_mapmaxtocekey(int key) {
	switch (key) {
		case KEY_BACKSPACE:    return CEGUI::Key::Backspace;
		case KEY_TAB:          return CEGUI::Key::Tab;
		case KEY_ENTER:        return CEGUI::Key::Return;
		case 19:               return CEGUI::Key::Pause;
		case KEY_ESC:          return CEGUI::Key::Escape;
		case KEY_SPACE:        return CEGUI::Key::Space;
		case KEY_COMMA:        return CEGUI::Key::Comma;
		case KEY_MINUS:        return CEGUI::Key::Minus;
		case KEY_PERIOD:       return CEGUI::Key::Period;
		case KEY_SLASH:        return CEGUI::Key::Slash;
		case KEY_0:            return CEGUI::Key::Zero;
		case KEY_1:            return CEGUI::Key::One;
		case KEY_2:            return CEGUI::Key::Two;
		case KEY_3:            return CEGUI::Key::Three;
		case KEY_4:            return CEGUI::Key::Four;
		case KEY_5:            return CEGUI::Key::Five;
		case KEY_6:            return CEGUI::Key::Six;
		case KEY_7:            return CEGUI::Key::Seven;
		case KEY_8:            return CEGUI::Key::Eight;
		case KEY_9:            return CEGUI::Key::Nine;
		case 146:              return CEGUI::Key::Colon;
		case KEY_SEMICOLON:    return CEGUI::Key::Semicolon;
		case KEY_EQUALS:       return CEGUI::Key::Equals;
		case KEY_OPENBRACKET:  return CEGUI::Key::LeftBracket;
		case KEY_BACKSLASH:    return CEGUI::Key::Backslash;
		case KEY_CLOSEBRACKET: return CEGUI::Key::RightBracket;
		case KEY_A:            return CEGUI::Key::A;
		case KEY_B:            return CEGUI::Key::B;
		case KEY_C:            return CEGUI::Key::C;
		case KEY_D:            return CEGUI::Key::D;
		case KEY_E:            return CEGUI::Key::E;
		case KEY_F:            return CEGUI::Key::F;
		case KEY_G:            return CEGUI::Key::G;
		case KEY_H:            return CEGUI::Key::H;
		case KEY_I:            return CEGUI::Key::I;
		case KEY_J:            return CEGUI::Key::J;
		case KEY_K:            return CEGUI::Key::K;
		case KEY_L:            return CEGUI::Key::L;
		case KEY_M:            return CEGUI::Key::M;
		case KEY_N:            return CEGUI::Key::N;
		case KEY_O:            return CEGUI::Key::O;
		case KEY_P:            return CEGUI::Key::P;
		case KEY_Q:            return CEGUI::Key::Q;
		case KEY_R:            return CEGUI::Key::R;
		case KEY_S:            return CEGUI::Key::S;
		case KEY_T:            return CEGUI::Key::T;
		case KEY_U:            return CEGUI::Key::U;
		case KEY_V:            return CEGUI::Key::V;
		case KEY_W:            return CEGUI::Key::W;
		case KEY_X:            return CEGUI::Key::X;
		case KEY_Y:            return CEGUI::Key::Y;
		case KEY_Z:            return CEGUI::Key::Z;
		case KEY_DELETE:       return CEGUI::Key::Delete;
		case KEY_NUM0:          return CEGUI::Key::Numpad0;
		case KEY_NUM1:          return CEGUI::Key::Numpad1;
		case KEY_NUM2:          return CEGUI::Key::Numpad2;
		case KEY_NUM3:          return CEGUI::Key::Numpad3;
		case KEY_NUM4:          return CEGUI::Key::Numpad4;
		case KEY_NUM5:          return CEGUI::Key::Numpad5;
		case KEY_NUM6:          return CEGUI::Key::Numpad6;
		case KEY_NUM7:          return CEGUI::Key::Numpad7;
		case KEY_NUM8:          return CEGUI::Key::Numpad8;
		case KEY_NUM9:          return CEGUI::Key::Numpad9;
		case KEY_NUMDECIMAL:    return CEGUI::Key::Decimal;
		case KEY_NUMDIVIDE:    return CEGUI::Key::Divide;
		case KEY_NUMMULTIPLY:  return CEGUI::Key::Multiply;
		case KEY_NUMSUBTRACT:  return CEGUI::Key::Subtract;
		case KEY_NUMADD:       return CEGUI::Key::Add;
		case 156:              return CEGUI::Key::NumpadEnter;
		case 141:              return CEGUI::Key::NumpadEquals;
		case KEY_UP:           return CEGUI::Key::ArrowUp;
		case KEY_DOWN:         return CEGUI::Key::ArrowDown;
		case KEY_RIGHT:        return CEGUI::Key::ArrowRight;
		case KEY_LEFT:         return CEGUI::Key::ArrowLeft;
		case KEY_INSERT:       return CEGUI::Key::Insert;
		case KEY_HOME:         return CEGUI::Key::Home;
		case KEY_END:          return CEGUI::Key::End;
		case KEY_PAGEUP:       return CEGUI::Key::PageUp;
		case KEY_PAGEDOWN:     return CEGUI::Key::PageDown;
		case KEY_F1:           return CEGUI::Key::F1;
		case KEY_F2:           return CEGUI::Key::F2;
		case KEY_F3:           return CEGUI::Key::F3;
		case KEY_F4:           return CEGUI::Key::F4;
		case KEY_F5:           return CEGUI::Key::F5;
		case KEY_F6:           return CEGUI::Key::F6;
		case KEY_F7:           return CEGUI::Key::F7;
		case KEY_F8:           return CEGUI::Key::F8;
		case KEY_F9:           return CEGUI::Key::F9;
		case KEY_F10:          return CEGUI::Key::F10;
		case KEY_F11:          return CEGUI::Key::F11;
		case KEY_F12:          return CEGUI::Key::F12;
//		case KEY_F13:          return CEGUI::Key::F13;
//		case KEY_F14:          return CEGUI::Key::F14;
//		case KEY_F15:          return CEGUI::Key::F15;
		case 144:              return CEGUI::Key::NumLock;
		case 145:              return CEGUI::Key::ScrollLock;
		case KEY_RSHIFT:       return CEGUI::Key::RightShift;
		case KEY_LSHIFT:       return CEGUI::Key::LeftShift;
		case KEY_RCONTROL:     return CEGUI::Key::RightControl;
		case KEY_LCONTROL:     return CEGUI::Key::LeftControl;
		case KEY_RALT:         return CEGUI::Key::RightAlt;
		case KEY_LALT:         return CEGUI::Key::LeftAlt;
		case KEY_LSYS:         return CEGUI::Key::LeftWindows;
		case KEY_RSYS:         return CEGUI::Key::RightWindows;
//		case KEY_SYSREQ:       return CEGUI::Key::SysRq;
//		case KEY_MENU:         return CEGUI::Key::AppMenu;
//		case KEY_POWER:        return CEGUI::Key::Power;
		default:                return 0;
	}
	return 0;
}


// *************************************************

bool bmx_cegui_editbox_hasinputfocus(CEGUI::Editbox * eb) {
	return eb->hasInputFocus();
}

bool bmx_cegui_editbox_isreadonly(CEGUI::Editbox * eb) {
	return eb->isReadOnly();
}

bool bmx_cegui_editbox_istextmasked(CEGUI::Editbox * eb) {
	return eb->isTextMasked();
}

bool bmx_cegui_editbox_istextvalid(CEGUI::Editbox * eb) {
	return eb->isTextValid();
}

const CEGUI::utf8 * bmx_cegui_editbox_getvalidationstring(CEGUI::Editbox * eb) {
	return eb->getValidationString().data();
}

int bmx_cegui_editbox_getcaratindex(CEGUI::Editbox * eb) {
	return eb->getCaratIndex();
}

int bmx_cegui_editbox_getselectionstartindex(CEGUI::Editbox * eb) {
	return eb->getSelectionStartIndex();
}

int bmx_cegui_editbox_getselectionendindex(CEGUI::Editbox * eb) {
	return eb->getSelectionEndIndex();
}

int bmx_cegui_editbox_getselectionlength(CEGUI::Editbox * eb) {
	return eb->getSelectionLength();
}

int bmx_cegui_editbox_getmaxtextlength(CEGUI::Editbox * eb) {
	return eb->getMaxTextLength();
}

void bmx_cegui_editbox_setreadonly(CEGUI::Editbox * eb, bool setting) {
	eb->setReadOnly(setting);
}

void bmx_cegui_editbox_settextmasked(CEGUI::Editbox * eb, bool setting) {
	eb->setTextMasked(setting);
}

void bmx_cegui_editbox_setvalidationstring(CEGUI::Editbox * eb, const CEGUI::utf8 * validationString) {
	eb->setValidationString(validationString);
}

void bmx_cegui_editbox_setcaratindex(CEGUI::Editbox * eb, int caratPos) {
	eb->setCaratIndex(caratPos);
}

void bmx_cegui_editbox_setselection(CEGUI::Editbox * eb, int startPos, int endPos) {
	eb->setSelection(startPos, endPos);
}

void bmx_cegui_editbox_setmaxtextlength(CEGUI::Editbox * eb, int maxLen) {
	eb->setMaxTextLength(maxLen);
}

// *************************************************

void bmx_cegui_logger_setlogginglevel(CEGUI::LoggingLevel level) {
	CEGUI::Logger::getSingleton().setLoggingLevel(level);
}

void bmx_cegui_logger_setlogfilename(const CEGUI::utf8 * filename, bool append) {
	CEGUI::Logger::getSingleton().setLogFilename(filename, append);
}

void bmx_cegui_logger_logevent(const CEGUI::utf8 * message, CEGUI::LoggingLevel level) {
	CEGUI::Logger::getSingleton().logEvent(message, level);
}

MaxLogger * bmx_cegui_customlogger_create(BBObject * handle) {
	return new MaxLogger(handle);
}

void bmx_cegui_customlogger_delete(MaxLogger * logger) {
	delete logger;
}

CEGUI::LoggingLevel bmx_cegui_logger_getlogginglevel() {
	return CEGUI::Logger::getSingleton().getLoggingLevel();
}

// *************************************************

bool bmx_cegui_eventargs_gethandled(MaxEventArgs * args) {
	return args->Args().handled;
}

BBObject * bmx_cegui_windoweventargs_getwindow(MaxEventArgs * args) {
	return newObjectForWindow(static_cast<const CEGUI::WindowEventArgs&>(args->Args()).window);
}

BBObject * bmx_cegui_activationeventargs_getotherwindow(MaxEventArgs * args) {
	return newObjectForWindow(static_cast<const CEGUI::ActivationEventArgs&>(args->Args()).otherWindow);
}

float bmx_cegui_updateeventargs_gettimesincelastframe(MaxEventArgs * args) {
	return static_cast<const CEGUI::UpdateEventArgs&>(args->Args()).d_timeSinceLastFrame;
}

// *************************************************

bool bmx_cegui_framewindow_issizingenabled(CEGUI::FrameWindow * win) {
	return win->isSizingEnabled();
}

bool bmx_cegui_framewindow_isframeenabled(CEGUI::FrameWindow * win) {
	return win->isFrameEnabled();
}

bool bmx_cegui_framewindow_istitlebarenabled(CEGUI::FrameWindow * win) {
	return win->isTitleBarEnabled();
}

bool bmx_cegui_framewindow_isclosebuttonenabled(CEGUI::FrameWindow * win) {
	return win->isCloseButtonEnabled();
}

bool bmx_cegui_framewindow_isrollupenabled(CEGUI::FrameWindow * win) {
	return win->isRollupEnabled();
}

bool bmx_cegui_framewindow_isrolledup(CEGUI::FrameWindow * win) {
	return win->isRolledup();
}

float bmx_cegui_framewindow_getsizingborderthickness(CEGUI::FrameWindow * win) {
	return win->getSizingBorderThickness();
}

void bmx_cegui_framewindow_setsizingenabled(CEGUI::FrameWindow * win, bool setting) {
	win->setSizingEnabled(setting);
}

void bmx_cegui_framewindow_setframeenabled(CEGUI::FrameWindow * win, bool setting) {
	win->setFrameEnabled(setting);
}

void bmx_cegui_framewindow_settitlebarenabled(CEGUI::FrameWindow * win, bool setting) {
	win->setTitleBarEnabled(setting);
}

void bmx_cegui_framewindow_setclosebuttonenabled(CEGUI::FrameWindow * win, bool setting) {
	win->setCloseButtonEnabled(setting);
}

void bmx_cegui_framewindow_rollupenabled(CEGUI::FrameWindow * win, bool setting) {
	win->setRollupEnabled(setting);
}

void bmx_cegui_framewindow_togglerollup(CEGUI::FrameWindow * win) {
	win->toggleRollup();
}

void bmx_cegui_framewindow_setsizingborderthickness(CEGUI::FrameWindow * win, float pixels) {
	win->setSizingBorderThickness(pixels);
}

bool bmx_cegui_framewindow_isdragmovingenabled(CEGUI::FrameWindow * win) {
	return win->isDragMovingEnabled();
}

void bmx_cegui_framewindow_setdragmovingenabled(CEGUI::FrameWindow * win, bool setting) {
	win->setDragMovingEnabled(setting);
}

bool bmx_cegui_framewindow_ishit(CEGUI::FrameWindow * win, float x, float y) {
	return win->isHit(CEGUI::Point(x, y));
}

BBObject * bmx_cegui_framewindow_gettitlebar(CEGUI::FrameWindow * win) {
	return _bah_cegui_TCETitlebar__create(win->getTitlebar());
}

BBObject * bmx_cegui_framewindow_getclosebutton(CEGUI::FrameWindow * win) {
		return _bah_cegui_TCEPushButton__create(win->getCloseButton());
}

// *************************************************


bool bmx_cegui_titlebar_isdraggingenabled(CEGUI::Titlebar * tb) {
	return tb->isDraggingEnabled();
}

void bmx_cegui_titlebar_setdraggingenabled(CEGUI::Titlebar * tb, bool setting) {
	tb->setDraggingEnabled(setting);
}

// *************************************************


bool bmx_cegui_buttonbase_ishovering(CEGUI::ButtonBase * base) {
	return base->isHovering();
}

bool bmx_cegui_buttonbase_ispushed(CEGUI::ButtonBase * base) {
	return base->isPushed();
}

// *************************************************

bool bmx_cegui_radiobutton_isselected(CEGUI::RadioButton * rb) {
	return rb->isSelected();
}

CEGUI::ulong bmx_cegui_radiobutton_getgroupid(CEGUI::RadioButton * rb) {
	return rb->getGroupID();
}

BBObject * bmx_cegui_radiobutton_getselectedbuttoningroup(CEGUI::RadioButton * rb) {
	return _bah_cegui_TCERadioButton__create(rb->getSelectedButtonInGroup());
}

void bmx_cegui_radiobutton_setselected(CEGUI::RadioButton * rb, bool selected) {
	rb->setSelected(selected);
}

void bmx_cegui_radiobutton_setgroupid(CEGUI::RadioButton * rb, CEGUI::ulong group) {
	rb->setGroupID(group);
}

// *************************************************

void bmx_cegui_combodroplist_setarmed(CEGUI::ComboDropList * list, bool setting) {
	list->setArmed(setting);
}

bool bmx_cegui_combodroplist_isarmed(CEGUI::ComboDropList * list) {
	return list->isArmed();
}

void bmx_cegui_combodroplist_setautoarmenabled(CEGUI::ComboDropList * list, bool setting) {
	list->setAutoArmEnabled(setting);
}

bool bmx_cegui_combodroplist_isautoarmenabled(CEGUI::ComboDropList * list) {
	return list->isAutoArmEnabled();
}

// *************************************************

bool bmx_cegui_multilineeditbox_hasinputfocus(CEGUI::MultiLineEditbox * eb) {
	return eb->hasInputFocus();
}

bool bmx_cegui_multilineeditbox_isreadonly(CEGUI::MultiLineEditbox * eb) {
	return eb->isReadOnly();
}

int bmx_cegui_multilineeditbox_getcaratindex(CEGUI::MultiLineEditbox * eb) {
	return eb->getCaratIndex();
}

int bmx_cegui_multilineeditbox_getselectionstartindex(CEGUI::MultiLineEditbox * eb) {
	return eb->getSelectionStartIndex();
}

int bmx_cegui_multilineeditbox_getselectionendindex(CEGUI::MultiLineEditbox * eb) {
	return eb->getSelectionEndIndex();
}

int bmx_cegui_multilineeditbox_getselectionlength(CEGUI::MultiLineEditbox * eb) {
	return eb->getSelectionLength();
}

int bmx_cegui_multilineeditbox_getmaxtextlength(CEGUI::MultiLineEditbox * eb) {
	return eb->getMaxTextLength();
}

bool bmx_cegui_multilineeditbox_iswordwrapped(CEGUI::MultiLineEditbox * eb) {
	return eb->isWordWrapped();
}

BBObject * bmx_cegui_multilineeditbox_getvertscrollbar(CEGUI::MultiLineEditbox * eb) {
	return _bah_cegui_TCEScrollbar__create(eb->getVertScrollbar());
}

bool bmx_cegui_multilineeditbox_isvertscrollbaralwaysshown(CEGUI::MultiLineEditbox * eb) {
	return eb->isVertScrollbarAlwaysShown();
}

BBObject * bmx_cegui_multilineeditbox_gethorzscrollbar(CEGUI::MultiLineEditbox * eb) {
	return _bah_cegui_TCEScrollbar__create(eb->getHorzScrollbar());
}

void bmx_cegui_multilineeditbox_gettextrenderarea(CEGUI::MultiLineEditbox * eb, float * x, float * y, float * w, float * h) {
	CEGUI::Rect r = eb->getTextRenderArea();
	*x = r.d_left;
	*y = r.d_top;
	*w = r.getWidth();
	*h = r.getHeight();
}

int bmx_cegui_multilineeditbox_getlinenumberfromindex(CEGUI::MultiLineEditbox * eb, int index) {
	return eb->getLineNumberFromIndex(index);
}

void bmx_cegui_multilineeditbox_setreadonly(CEGUI::MultiLineEditbox * eb, bool setting) {
	eb->setReadOnly(setting);
}

void bmx_cegui_multilineeditbox_setcaratindex(CEGUI::MultiLineEditbox * eb, int caratPos) {
	eb->setCaratIndex(caratPos);
}

void bmx_cegui_multilineeditbox_setselection(CEGUI::MultiLineEditbox * eb, int startPos, int endPos) {
	eb->setSelection(startPos, endPos);
}

void bmx_cegui_multilineeditbox_setmaxtextlength(CEGUI::MultiLineEditbox * eb, int maxLen) {
	eb->setMaxTextLength(maxLen);
}

void bmx_cegui_multilineeditbox_ensurecaratisvisible(CEGUI::MultiLineEditbox * eb) {
	eb->ensureCaratIsVisible();
}

void bmx_cegui_multilineeditbox_setwordwrapping(CEGUI::MultiLineEditbox * eb, bool setting) {
	eb->setWordWrapping(setting);
}

void bmx_cegui_multilineeditbox_setshowvertscrollbar(CEGUI::MultiLineEditbox * eb, bool setting) {
	eb->setShowVertScrollbar(setting);
}


// *************************************************

bool bmx_cegui_scrolleditemlistbase_isvertscrollbaralwaysshown(CEGUI::ScrolledItemListBase * lb) {
	return lb->isVertScrollbarAlwaysShown();
}

bool bmx_cegui_scrolleditemlistbase_ishorzscrollbaralwaysshown(CEGUI::ScrolledItemListBase * lb) {
	return lb->isHorzScrollbarAlwaysShown();
}

BBObject * bmx_cegui_scrolleditemlistbase_getvertscrollbar(CEGUI::ScrolledItemListBase * lb) {

}

BBObject * bmx_cegui_scrolleditemlistbase_gethorzscrollbar(CEGUI::ScrolledItemListBase * lb) {

}

void bmx_cegui_scrolleditemlistbase_setshowvertscrollbar(CEGUI::ScrolledItemListBase * lb, bool mode) {
	lb->setShowVertScrollbar(mode);
}

void bmx_cegui_scrolleditemlistbase_setshowhorzscrollbar(CEGUI::ScrolledItemListBase * lb, bool mode) {
	lb->setShowHorzScrollbar(mode);
}


// *************************************************

int bmx_cegui_itemlistbox_getselectedcount(CEGUI::ItemListbox * lb) {
	return lb->getSelectedCount();
}

BBObject * bmx_cegui_itemlistbox_getlastselecteditem(CEGUI::ItemListbox * lb) {
	return _bah_cegui_TCEItemEntry__create(lb->getLastSelectedItem());
}

BBObject * bmx_cegui_itemlistbox_getfirstselecteditem(CEGUI::ItemListbox * lb, int startIndex) {
	return _bah_cegui_TCEItemEntry__create(lb->getFirstSelectedItem(startIndex));
}

BBObject * bmx_cegui_itemlistbox_getnextselecteditem(CEGUI::ItemListbox * lb) {
	return _bah_cegui_TCEItemEntry__create(lb->getNextSelectedItem());
}

BBObject * bmx_cegui_itemlistbox_getnextselecteditemafter(CEGUI::ItemListbox * lb, const CEGUI::ItemEntry * startItem) {
	return _bah_cegui_TCEItemEntry__create(lb->getNextSelectedItemAfter(startItem));
}

bool bmx_cegui_itemlistbox_ismultiselectenabled(CEGUI::ItemListbox * lb) {
	return lb->isMultiSelectEnabled();
}

bool bmx_cegui_itemlistbox_isitemselected(CEGUI::ItemListbox * lb, int index) {
	return lb->isItemSelected(index);
}

void bmx_cegui_itemlistbox_setmultiselectenabled(CEGUI::ItemListbox * lb, bool state) {
	lb->setMultiSelectEnabled(state);
}

void bmx_cegui_itemlistbox_clearallselections(CEGUI::ItemListbox * lb) {
	lb->clearAllSelections();
}

void bmx_cegui_itemlistbox_selectrange(CEGUI::ItemListbox * lb, int a, int z) {
	lb->selectRange(a, z);
}

void bmx_cegui_itemlistbox_selectallitems(CEGUI::ItemListbox * lb) {
	lb->selectAllItems();
}

// *************************************************

CEGUI::Imageset * bmx_cegui_imagesetmanager_createimagesetfromimagefile(const CEGUI::utf8 * name, const CEGUI::utf8 * filename, const CEGUI::utf8 * resourceGroup) {
	return CEGUI::ImagesetManager::getSingleton().createImagesetFromImageFile(name, filename, resourceGroup);
}

// *************************************************

CEGUI::Font * bmx_cegui_fontmanager_createfont(const CEGUI::utf8 * filename, const CEGUI::utf8 * resourceGroup) {
	return CEGUI::FontManager::getSingleton().createFont(filename, resourceGroup);
}

bool bmx_cegui_fontmanager_isfontpresent(const CEGUI::utf8 * name) {
	return CEGUI::FontManager::getSingleton().isFontPresent(name);
}

// *************************************************

float bmx_cegui_spinner_getcurrentvalue(CEGUI::Spinner * spinner) {
	return spinner->getCurrentValue();
}

float bmx_cegui_spinner_getstepsize(CEGUI::Spinner * spinner) {
	return spinner->getStepSize();
}

float bmx_cegui_spinner_getmaximumvalue(CEGUI::Spinner * spinner) {
	return spinner->getMaximumValue();
}

float bmx_cegui_spinner_getminimumvalue(CEGUI::Spinner * spinner) {
	return spinner->getMinimumValue();
}

CEGUI::Spinner::TextInputMode bmx_cegui_spinner_gettextinputmode(CEGUI::Spinner * spinner) {
	return spinner->getTextInputMode();
}

void bmx_cegui_spinner_setcurrentvalue(CEGUI::Spinner * spinner, float value) {
	spinner->setCurrentValue(value);
}

void bmx_cegui_spinner_setstepsize(CEGUI::Spinner * spinner, float stepSize) {
	spinner->setStepSize(stepSize);
}

void bmx_cegui_spinner_setmaximumvalue(CEGUI::Spinner * spinner, float maxValue) {
	spinner->setMaximumValue(maxValue);
}

void bmx_cegui_spinner_setminimumvalue(CEGUI::Spinner * spinner, float minValue) {
	spinner->setMinimumValue(minValue);
}

void bmx_cegui_spinner_settextinputmode(CEGUI::Spinner * spinner, CEGUI::Spinner::TextInputMode mode) {
	spinner->setTextInputMode(mode);
}


