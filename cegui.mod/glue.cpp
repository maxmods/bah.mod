/*
  Copyright (c) 2008-2009 Bruce A Henderson
 
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

void bmx_cegui_throw_exception(CEGUI::Exception &e) {
	bbExThrow(_bah_cegui_TCEException__create(bah_cegui__convertUTF8ToMax(e.getMessage().data()),
		bah_cegui__convertUTF8ToMax(e.getName().data()),
		bah_cegui__convertUTF8ToMax(e.getFileName().data()),
		e.getLine()));
}

void bmx_cegui_throw_alreadyexistsexception(CEGUI::AlreadyExistsException &e) {
	bbExThrow(_bah_cegui_TCEAlreadyExistsException__create(bah_cegui__convertUTF8ToMax(e.getMessage().data()),
		bah_cegui__convertUTF8ToMax(e.getName().data()),
		bah_cegui__convertUTF8ToMax(e.getFileName().data()),
		e.getLine()));
}

void bmx_cegui_throw_fileioexception(CEGUI::FileIOException &e) {
	bbExThrow(_bah_cegui_TCEFileIOException__create(bah_cegui__convertUTF8ToMax(e.getMessage().data()),
		bah_cegui__convertUTF8ToMax(e.getName().data()),
		bah_cegui__convertUTF8ToMax(e.getFileName().data()),
		e.getLine()));
}

void bmx_cegui_throw_genericexception(CEGUI::GenericException &e) {
	bbExThrow(_bah_cegui_TCEGenericException__create(bah_cegui__convertUTF8ToMax(e.getMessage().data()),
		bah_cegui__convertUTF8ToMax(e.getName().data()),
		bah_cegui__convertUTF8ToMax(e.getFileName().data()),
		e.getLine()));
}

void bmx_cegui_throw_invalidrequestexception(CEGUI::InvalidRequestException &e) {
	bbExThrow(_bah_cegui_TCEInvalidRequestException__create(bah_cegui__convertUTF8ToMax(e.getMessage().data()),
		bah_cegui__convertUTF8ToMax(e.getName().data()),
		bah_cegui__convertUTF8ToMax(e.getFileName().data()),
		e.getLine()));
}

void bmx_cegui_throw_memoryexception(CEGUI::MemoryException &e) {
	bbExThrow(_bah_cegui_TCEMemoryException__create(bah_cegui__convertUTF8ToMax(e.getMessage().data()),
		bah_cegui__convertUTF8ToMax(e.getName().data()),
		bah_cegui__convertUTF8ToMax(e.getFileName().data()),
		e.getLine()));
}

void bmx_cegui_throw_nullobjectexception(CEGUI::NullObjectException &e) {
	bbExThrow(_bah_cegui_TCENullObjectException__create(bah_cegui__convertUTF8ToMax(e.getMessage().data()),
		bah_cegui__convertUTF8ToMax(e.getName().data()),
		bah_cegui__convertUTF8ToMax(e.getFileName().data()),
		e.getLine()));
}

void bmx_cegui_throw_objectinuseexception(CEGUI::ObjectInUseException &e) {
	bbExThrow(_bah_cegui_TCEObjectInUseException__create(bah_cegui__convertUTF8ToMax(e.getMessage().data()),
		bah_cegui__convertUTF8ToMax(e.getName().data()),
		bah_cegui__convertUTF8ToMax(e.getFileName().data()),
		e.getLine()));
}

void bmx_cegui_throw_rendererexception(CEGUI::RendererException &e) {
	bbExThrow(_bah_cegui_TCERendererException__create(bah_cegui__convertUTF8ToMax(e.getMessage().data()),
		bah_cegui__convertUTF8ToMax(e.getName().data()),
		bah_cegui__convertUTF8ToMax(e.getFileName().data()),
		e.getLine()));
}

void bmx_cegui_throw_scriptexception(CEGUI::ScriptException &e) {
	bbExThrow(_bah_cegui_TCEScriptException__create(bah_cegui__convertUTF8ToMax(e.getMessage().data()),
		bah_cegui__convertUTF8ToMax(e.getName().data()),
		bah_cegui__convertUTF8ToMax(e.getFileName().data()),
		e.getLine()));
}

void bmx_cegui_throw_unknownobjectexception(CEGUI::UnknownObjectException &e) {
	bbExThrow(_bah_cegui_TCEUnknownObjectException__create(bah_cegui__convertUTF8ToMax(e.getMessage().data()),
		bah_cegui__convertUTF8ToMax(e.getName().data()),
		bah_cegui__convertUTF8ToMax(e.getFileName().data()),
		e.getLine()));
}


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
	return static_cast<bool>(_bah_cegui_TCEEventCallback__callback(callbackHandle, 
		newObjectForEventArgs((CEGUI::EventArgs *)&args, new MaxEventArgs(args))));
}

MaxConnection * MaxCEEventCallback::subscribeEvent(CEGUI::Window * handle, const CEGUI::utf8 * name) {
	return new MaxConnection(handle->subscribeEvent(name, CEGUI::Event::Subscriber(&MaxCEEventCallback::EventCallback, this)));
}

MaxConnection * MaxCEEventCallback::subscribeEvent(CEGUI::GlobalEventSet * handle, const CEGUI::utf8 * name) {
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

MaxCEColour::MaxCEColour(CEGUI::colour col)
	: colour(col)
{
}

MaxCEColour::~MaxCEColour()
{
}

CEGUI::colour MaxCEColour::Colour() {
	return colour;
}

// *************************************************

MaxCEColourRect::MaxCEColourRect(CEGUI::ColourRect col)
	: rect(col)
{
}

MaxCEColourRect::~MaxCEColourRect()
{
}

CEGUI::ColourRect MaxCEColourRect::Rect() {
	return rect;
}

// *************************************************

MaxRenderer::MaxRenderer()
{
}

// *************************************************

MaxResourceProvider::MaxResourceProvider(BBObject * handle)
	: maxHandle(handle)
{
}

MaxResourceProvider::~MaxResourceProvider(void)
{
}

void MaxResourceProvider::loadRawDataContainer(const CEGUI::String &filename, CEGUI::RawDataContainer &output, const CEGUI::String &resourceGroup) {
	_bah_cegui_TCEResourceProvider__loadRawDataContainer(maxHandle, bah_cegui__convertUTF8ToMax(filename.data()), new MaxRawDataContainer(output), bah_cegui__convertUTF8ToMax(resourceGroup.data()));
}

void MaxResourceProvider::unloadRawDataContainer(CEGUI::RawDataContainer &data) {
	_bah_cegui_TCEResourceProvider__unloadRawDataContainer(maxHandle, new MaxRawDataContainer(data));
}

size_t MaxResourceProvider::getResourceGroupFileNames(std::vector<CEGUI::String>& out_vec, const CEGUI::String& file_pattern, const CEGUI::String& resource_group) {
	// TODO
}

// *************************************************

MaxRawDataContainer::MaxRawDataContainer(CEGUI::RawDataContainer & container)
	: container(container)
{
}
	
CEGUI::RawDataContainer & MaxRawDataContainer::Container() {
	return container;
}


// *************************************************

MaxWindowFactory::MaxWindowFactory(BBObject * handle, const CEGUI::utf8 * typeName)
	: maxHandle(handle), CEGUI::WindowFactory(typeName)
{
}

MaxWindowFactory::~MaxWindowFactory()
{
}

CEGUI::Window * MaxWindowFactory::createWindow(const CEGUI::String &name) {
	return _bah_cegui_TCEWindowFactory__createWindow(maxHandle, bah_cegui__convertUTF8ToMax(name.data()));
}

void MaxWindowFactory::destroyWindow(CEGUI::Window *window) {
	_bah_cegui_TCEWindowFactory__destroyWindow(maxHandle, newObjectForWindow(window));
}

const CEGUI::String & MaxWindowFactory::getTypeName() {
	return _bah_cegui_TCEWindowFactory__getTypeName(maxHandle);
}


// *************************************************

BBObject * newObjectForWindow(CEGUI::Window * window) {

	if (!window) {
		return &bbNullObject;
	}

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

CEGUI::System * bmx_cegui_new_system(CEGUI::Renderer * r, MaxResourceProvider * provider) {
	// we pass in the renderer otherwise it will attepmt to load it from the shared library
	CEGUI::System * sys =  &CEGUI::System::create(*r, provider);
	
	return sys;
}

void bmx_cegui_system_destroy() {
	CEGUI::System::destroy();
}

void bmx_cegui_delete_renderer(CEGUI::Renderer * r) {
	delete r;
}

// *************************************************

void bmx_cegui_system_setdefaultfont(CEGUI::System * sys, CEGUI::Font * font) {
	sys->setDefaultFont(font);
}

void bmx_cegui_system_setdefaultfonttxt(CEGUI::System * sys, const CEGUI::utf8 * font) {
	try {
		sys->setDefaultFont(font);
	} catch (CEGUI::UnknownObjectException &e) {
		bmx_cegui_throw_unknownobjectexception(e);
	} catch (CEGUI::Exception &e) {
		bmx_cegui_throw_exception(e);
	}
}

void bmx_cegui_system_setDefaultMouseCursor(CEGUI::System * sys, const CEGUI::utf8 * l, const CEGUI::utf8 * kind) {
	try {
		sys->setDefaultMouseCursor(l, kind);
	} catch (CEGUI::UnknownObjectException &e) {
		bmx_cegui_throw_unknownobjectexception(e);
	} catch (CEGUI::Exception &e) {
		bmx_cegui_throw_exception(e);
	}
}

void bmx_cegui_system_renderGUI(CEGUI::System * sys) {
	sys->renderGUI();
}

BBObject * bmx_cegui_system_setGUISheet(CEGUI::System * sys, CEGUI::Window * window) {
	return newObjectForWindow(sys->setGUISheet(window));
}

CEGUI::Font * bmx_cegui_system_getDefaultFont(CEGUI::System * sys) {
	return sys->getDefaultFont();
}

void bmx_cegui_system_signalredraw(CEGUI::System * sys) {
	sys->signalRedraw();
}

int bmx_cegui_system_isredrawrequested(CEGUI::System * sys) {
	return sys->isRedrawRequested();
}

double bmx_cegui_system_getsingleclicktimeout(CEGUI::System * sys) {
	return sys->getSingleClickTimeout();
}

double bmx_cegui_system_getmulticlicktimeout(CEGUI::System * sys) {
	return sys->getMultiClickTimeout();
}

void bmx_cegui_system_getmulticlicktoleranceareasize(CEGUI::System * sys, float * width, float * height) {
	CEGUI::Size s(sys->getMultiClickToleranceAreaSize());
	*width = s.d_width;
	*height = s.d_height;
}

void bmx_cegui_system_setsingleclicktimeout(CEGUI::System * sys, double timeout) {
	sys->setSingleClickTimeout(timeout);
}

void bmx_cegui_system_setmulticlicktimeout(CEGUI::System * sys, double timeout) {
	sys->setMultiClickTimeout(timeout);
}

void bmx_cegui_system_setmulticlicktoleranceareasize(CEGUI::System * sys, float width, float height) {
	sys->setMultiClickToleranceAreaSize(CEGUI::Size(width, height));
}

void bmx_cegui_system_setdefaulttooltip(CEGUI::System * sys, CEGUI::Tooltip * tooltip) {
	sys->setDefaultTooltip(tooltip);
}

void bmx_cegui_system_setdefaulttooltiptext(CEGUI::System * sys, CEGUI::utf8 * tooltip) {
	sys->setDefaultTooltip(tooltip);
}

BBObject * bmx_cegui_system_getdefaulttooltip(CEGUI::System * sys) {
	return _bah_cegui_TCETooltip__create(sys->getDefaultTooltip());
}

float bmx_cegui_system_getmousemovescaling(CEGUI::System * sys) {
	return sys->getMouseMoveScaling();
}

void bmx_cegui_system_setmousemovescaling(CEGUI::System * sys, float scaling) {
	sys->setMouseMoveScaling(scaling);
}

// *************************************************

CEGUI::WindowManager * bmx_cegui_windowmanager_getsingleton() {
	return &CEGUI::WindowManager::getSingleton();
}

BBObject * bmx_cegui_windowmanager_loadWindowLayout(CEGUI::WindowManager * mgr, const CEGUI::utf8 * layout, const CEGUI::utf8 * namePrefix, const CEGUI::utf8 * resourceGroup) {
	try {
		return newObjectForWindow(mgr->loadWindowLayout(layout, namePrefix, resourceGroup));
	} catch (CEGUI::FileIOException &e) {
		bmx_cegui_throw_fileioexception(e);
	} catch (CEGUI::InvalidRequestException &e) {
		bmx_cegui_throw_invalidrequestexception(e);
	} catch (CEGUI::Exception &e) {
		bmx_cegui_throw_exception(e);
	}
}

BBObject * bmx_cegui_windowmanager_getwindow(CEGUI::WindowManager * mgr, const CEGUI::utf8 * name) {
	try {
		return newObjectForWindow(mgr->getWindow(name));
	} catch (CEGUI::UnknownObjectException &e) {
		bmx_cegui_throw_unknownobjectexception(e);
	} catch (CEGUI::Exception &e) {
		bmx_cegui_throw_exception(e);
	}
}

BBObject * bmx_cegui_windowmanager_createwindow(CEGUI::WindowManager * mgr, const CEGUI::utf8 * windowType, const CEGUI::utf8 * name) {
	try {
		return newObjectForWindow(mgr->createWindow(windowType, name));
	} catch (CEGUI::InvalidRequestException &e) {
		bmx_cegui_throw_invalidrequestexception(e);
	} catch (CEGUI::AlreadyExistsException &e) {
		bmx_cegui_throw_alreadyexistsexception(e);
	} catch (CEGUI::UnknownObjectException &e) {
		bmx_cegui_throw_unknownobjectexception(e);
	} catch (CEGUI::GenericException &e) {
		bmx_cegui_throw_genericexception(e);
	} catch (CEGUI::Exception &e) {
		bmx_cegui_throw_exception(e);
	}
}

int bmx_cegui_windowmanager_iswindowpresent(CEGUI::WindowManager * mgr, const CEGUI::utf8 * name) {
	return mgr->isWindowPresent(name);
}

void bmx_cegui_windowmanager_destroyallwindows(CEGUI::WindowManager * mgr) {
	try {
		mgr->destroyAllWindows();
	} catch (CEGUI::InvalidRequestException &e) {
		bmx_cegui_throw_invalidrequestexception(e);
	} catch (CEGUI::Exception &e) {
		bmx_cegui_throw_exception(e);
	}
}

void bmx_cegui_windowmanager_destroywindowwindow(CEGUI::WindowManager * mgr, CEGUI::Window * window) {
	try {
		mgr->destroyWindow(window);
	} catch (CEGUI::InvalidRequestException &e) {
		bmx_cegui_throw_invalidrequestexception(e);
	} catch (CEGUI::Exception &e) {
		bmx_cegui_throw_exception(e);
	}
}

void bmx_cegui_windowmanager_destroywindowname(CEGUI::WindowManager * mgr, const CEGUI::utf8 * window) {
	try {
		mgr->destroyWindow(window);
	} catch (CEGUI::InvalidRequestException &e) {
		bmx_cegui_throw_invalidrequestexception(e);
	} catch (CEGUI::Exception &e) {
		bmx_cegui_throw_exception(e);
	}
}

void bmx_cegui_windowmanager_renamewindowwindow(CEGUI::WindowManager * mgr, CEGUI::Window * window, const CEGUI::utf8 * newName) {
	try {
		mgr->renameWindow(window, newName);
	} catch (CEGUI::AlreadyExistsException &e) {
		bmx_cegui_throw_alreadyexistsexception(e);
	} catch (CEGUI::Exception &e) {
		bmx_cegui_throw_exception(e);
	}
}

void bmx_cegui_windowmanager_renamewindowname(CEGUI::WindowManager * mgr, const CEGUI::utf8 * window, const CEGUI::utf8 * newName) {
	try {
		mgr->renameWindow(window, newName);
	} catch (CEGUI::UnknownObjectException &e) {
		bmx_cegui_throw_unknownobjectexception(e);
	} catch (CEGUI::AlreadyExistsException &e) {
		bmx_cegui_throw_alreadyexistsexception(e);
	} catch (CEGUI::Exception &e) {
		bmx_cegui_throw_exception(e);
	}
}


// *************************************************

int bmx_cegui_system_injectTimePulse(CEGUI::System * sys, float t) {
	return sys->injectTimePulse(t);
}

int bmx_cegui_system_injectMousePosition(CEGUI::System * sys, int x, int y) {
	return sys->injectMousePosition(static_cast<float>(x),static_cast<float>(y)); 
}

int bmx_cegui_system_injectMouseButtonDown(CEGUI::System * sys, int button) {
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

int bmx_cegui_system_injectMouseButtonUp(CEGUI::System * sys, int button) {
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

int bmx_cegui_system_injectkeydown(CEGUI::System * sys, int key) {
	return sys->injectKeyDown(bmx_cegui_mapmaxtocekey(key));
}

int bmx_cegui_system_injectkeyup(CEGUI::System * sys, int key) {
	return sys->injectKeyUp(bmx_cegui_mapmaxtocekey(key));
}

int bmx_cegui_system_injectchar(CEGUI::System * sys, CEGUI::utf32 key) {
	return sys->injectChar(key);
}

int bmx_cegui_system_injectMouseWheelChange(CEGUI::System * sys, int delta) {
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
	try {
		window->addChildWindow(win);
	} catch (CEGUI::InvalidRequestException &e) {
		bmx_cegui_throw_invalidrequestexception(e);
	} catch (CEGUI::Exception &e) {
		bmx_cegui_throw_exception(e);
	}
}

void bmx_cegui_window_addchildwindow(CEGUI::Window * window, const CEGUI::utf8 * name) {
	try {
		window->addChildWindow(name);
	} catch (CEGUI::UnknownObjectException &e) {
		bmx_cegui_throw_unknownobjectexception(e);
	} catch (CEGUI::InvalidRequestException &e) {
		bmx_cegui_throw_invalidrequestexception(e);
	} catch (CEGUI::Exception &e) {
		bmx_cegui_throw_exception(e);
	}
}

void bmx_cegui_window_setproperty(CEGUI::Window * window, const CEGUI::utf8 * name, const CEGUI::utf8 * value) {
	try {
		window->setProperty(name, value);
	} catch (CEGUI::UnknownObjectException &e) {
		bmx_cegui_throw_unknownobjectexception(e);
	} catch (CEGUI::InvalidRequestException &e) {
		bmx_cegui_throw_invalidrequestexception(e);
	} catch (CEGUI::Exception &e) {
		bmx_cegui_throw_exception(e);
	}
}

void bmx_cegui_window_removeproperty(CEGUI::Window * window, const CEGUI::utf8 * name) {
	window->removeProperty(name);
}

void bmx_cegui_window_clearproperties(CEGUI::Window * window) {
	window->clearProperties();
}

int bmx_cegui_window_ispropertypresent(CEGUI::Window * window, const CEGUI::utf8 * name) {
	return window->isPropertyPresent(name);
}

BBString * bmx_cegui_window_getpropertyhelp(CEGUI::Window * window, const CEGUI::utf8 * name) {
	try {
		return bah_cegui__convertUTF8ToMax(window->getPropertyHelp(name).data());
	} catch (CEGUI::UnknownObjectException &e) {
		bmx_cegui_throw_unknownobjectexception(e);
	} catch (CEGUI::Exception &e) {
		bmx_cegui_throw_exception(e);
	}
}

BBString * bmx_cegui_window_getproperty(CEGUI::Window * window, const CEGUI::utf8 * name) {
	try {
		return bah_cegui__convertUTF8ToMax(window->getProperty(name).data());
	} catch (CEGUI::UnknownObjectException &e) {
		bmx_cegui_throw_unknownobjectexception(e);
	} catch (CEGUI::Exception &e) {
		bmx_cegui_throw_exception(e);
	}
}

int bmx_cegui_window_ispropertydefault(CEGUI::Window * window, const CEGUI::utf8 * name) {
	return window->isPropertyDefault(name);
}

BBString * bmx_cegui_window_getpropertydefault(CEGUI::Window * window, const CEGUI::utf8 * name) {
	return bah_cegui__convertUTF8ToMax(window->getPropertyDefault(name).data());
}

void bmx_cegui_window_hide(CEGUI::Window * window) {
	window->hide();
}

void bmx_cegui_window_show(CEGUI::Window * window) {
	window->show();
}

BBString * bmx_cegui_window_gettype(CEGUI::Window * window) {
	return bah_cegui__convertUTF8ToMax(window->getType().data());
}

void bmx_cegui_window_activate(CEGUI::Window * window) {
	window->activate();
}

BBString * bmx_cegui_window_getname(CEGUI::Window * window) {
	return bah_cegui__convertUTF8ToMax(window->getName().data());
}

int bmx_cegui_window_isdestroyedbyparent(CEGUI::Window * window) {
	return window->isDestroyedByParent();
}

int bmx_cegui_window_isalwaysontop(CEGUI::Window * window) {
	return window->isAlwaysOnTop();
}

int bmx_cegui_window_isdisabled(CEGUI::Window * window, int localOnly) {
	return window->isDisabled(localOnly);
}

int bmx_cegui_window_isvisible(CEGUI::Window * window, int localOnly) {
	return window->isVisible(localOnly);
}

int bmx_cegui_window_isactive(CEGUI::Window * window) {
	return window->isActive();
}

int bmx_cegui_window_isclippedbyparent(CEGUI::Window * window) {
	return window->isClippedByParent();
}

CEGUI::uint bmx_cegui_window_getid(CEGUI::Window * window) {
	return window->getID();
}

int bmx_cegui_window_getchildcount(CEGUI::Window * window) {
	return window->getChildCount();
}

int bmx_cegui_window_ischild(CEGUI::Window * window, const CEGUI::utf8 * name) {
	return window->isChild(name);
}

int bmx_cegui_window_ischildid(CEGUI::Window * window, CEGUI::uint ID) {
	return window->isChild(ID);
}

int bmx_cegui_window_ischildrecursive(CEGUI::Window * window, CEGUI::uint ID) {
	return window->isChildRecursive(ID);
}

int bmx_cegui_window_ischildwindow(CEGUI::Window * window, CEGUI::Window * win) {
	return window->isChild(win);
}

BBObject * bmx_cegui_window_getchild(CEGUI::Window * window, const CEGUI::utf8 * name) {
	try {
		return newObjectForWindow(window->getChild(name));
	} catch (CEGUI::UnknownObjectException &e) {
		bmx_cegui_throw_unknownobjectexception(e);
	} catch (CEGUI::Exception &e) {
		bmx_cegui_throw_exception(e);
	}
}

BBObject * bmx_cegui_window_getchildid(CEGUI::Window * window, CEGUI::uint ID) {
	try {
		return newObjectForWindow(window->getChild(ID));
	} catch (CEGUI::UnknownObjectException &e) {
		bmx_cegui_throw_unknownobjectexception(e);
	} catch (CEGUI::Exception &e) {
		bmx_cegui_throw_exception(e);
	}
}

BBObject * bmx_cegui_window_getchildrecursive(CEGUI::Window * window, CEGUI::uint ID) {
	return newObjectForWindow(window->getChildRecursive(ID));
}

BBObject * bmx_cegui_window_getchildatidx(CEGUI::Window * window, int idx) {
	return newObjectForWindow(window->getChildAtIdx(idx));
}

BBObject * bmx_cegui_window_getactivechild(CEGUI::Window * window) {
	return newObjectForWindow(window->getActiveChild());
}

int bmx_cegui_window_isancestor(CEGUI::Window * window, const CEGUI::utf8 * name) {
	return window->isAncestor(name);
}

int bmx_cegui_window_isancestorid(CEGUI::Window * window, CEGUI::uint ID) {
	return window->isAncestor(ID);
}

int bmx_cegui_window_isancestorwindow(CEGUI::Window * window, CEGUI::Window * win) {
	return window->isAncestor(win);
}

BBString * bmx_cegui_window_gettext(CEGUI::Window * window) {
	return bah_cegui__convertUTF8ToMax(window->getText().data());
}

int bmx_cegui_window_inheritsalpha(CEGUI::Window * window) {
	return window->inheritsAlpha();
}

float bmx_cegui_window_getalpha(CEGUI::Window * window) {
	return window->getAlpha();
}

float bmx_cegui_window_geteffectivealpha(CEGUI::Window * window) {
	return window->getEffectiveAlpha();
}

void bmx_cegui_window_getunclippedinnerrect(CEGUI::Window * window, float * x, float * y, float * w, float * h) {
	CEGUI::Rect r = window->getUnclippedInnerRect();
	*x = r.d_left;
	*y = r.d_top;
	*w = r.getWidth();
	*h = r.getHeight();
}

int bmx_cegui_window_iscapturedbythis(CEGUI::Window * window) {
	return window->isCapturedByThis();
}

int bmx_cegui_window_iscapturedbyancestor(CEGUI::Window * window) {
	return window->isCapturedByAncestor();
}

int bmx_cegui_window_iscapturedbychild(CEGUI::Window * window) {
	return window->isCapturedByChild();
}

int bmx_cegui_window_ishit(CEGUI::Window * window, float x, float y, int allowDisabled) {
	return window->isHit(CEGUI::Vector2(x, y), static_cast<bool>(allowDisabled));
}

BBObject * bmx_cegui_window_getchildatposition(CEGUI::Window * window, float x, float y) {
	return newObjectForWindow(window->getChildAtPosition(CEGUI::Vector2(x, y)));
}

BBObject * bmx_cegui_window_gettargetchildatposition(CEGUI::Window * window, float x, float y) {
	return newObjectForWindow(window->getTargetChildAtPosition(CEGUI::Vector2(x, y)));
}

BBObject * bmx_cegui_window_getparent(CEGUI::Window * window) {
	return newObjectForWindow(window->getParent());
}

void bmx_cegui_window_getpixelsize(CEGUI::Window * window, float * width, float * height) {
	CEGUI::Size s = window->getPixelSize();
	*width = s.d_width;
	*height = s.d_height;
}

int bmx_cegui_window_restoresoldcapture(CEGUI::Window * window) {
	return window->restoresOldCapture();
}

int bmx_cegui_window_iszorderingenabled(CEGUI::Window * window) {
	return window->isZOrderingEnabled();
}

int bmx_cegui_window_wantsmulticlickevents(CEGUI::Window * window) {
	return window->wantsMultiClickEvents();
}

int bmx_cegui_window_ismouseautorepeatenabled(CEGUI::Window * window) {
	return window->isMouseAutoRepeatEnabled();
}

float bmx_cegui_window_getautorepeatdelay(CEGUI::Window * window) {
	return window->getAutoRepeatDelay();
}

float bmx_cegui_window_getautorepeatrate(CEGUI::Window * window) {
	return window->getAutoRepeatRate();
}

int bmx_cegui_window_distributescapturedinputs(CEGUI::Window * window) {
	return window->distributesCapturedInputs();
}

int bmx_cegui_window_isusingdefaulttooltip(CEGUI::Window * window) {
	return window->isUsingDefaultTooltip();
}

BBString * bmx_cegui_window_gettooltiptype(CEGUI::Window * window) {
	return bah_cegui__convertUTF8ToMax(window->getTooltipType().data());
}

BBString * bmx_cegui_window_gettooltiptext(CEGUI::Window * window) {
	return bah_cegui__convertUTF8ToMax(window->getTooltipText().data());
}

int bmx_cegui_window_inheritstooltiptext(CEGUI::Window * window) {
	return window->inheritsTooltipText();
}

int bmx_cegui_window_isriseonclickenabled(CEGUI::Window * window) {
	return window->isRiseOnClickEnabled();
}

int bmx_cegui_window_testclassname(CEGUI::Window * window, const CEGUI::utf8 * className) {
	return window->testClassName(className);
}

CEGUI::VerticalAlignment bmx_cegui_window_getverticalalignment(CEGUI::Window * window) {
	return window->getVerticalAlignment();
}

CEGUI::HorizontalAlignment bmx_cegui_window_gethorizontalalignment(CEGUI::Window * window) {
	return window->getHorizontalAlignment();
}

BBString * bmx_cegui_window_getlooknfeel(CEGUI::Window * window) {
	return bah_cegui__convertUTF8ToMax(window->getLookNFeel().data());
}

int bmx_cegui_window_getmodalstate(CEGUI::Window * window) {
	return window->getModalState();
}

BBString * bmx_cegui_window_getuserstring(CEGUI::Window * window, const CEGUI::utf8 * name) {
	try {
		return bah_cegui__convertUTF8ToMax(window->getUserString(name).data());
	} catch (CEGUI::UnknownObjectException &e) {
		bmx_cegui_throw_unknownobjectexception(e);
	} catch (CEGUI::Exception &e) {
		bmx_cegui_throw_exception(e);
	}
}

int bmx_cegui_window_isuserstringdefined(CEGUI::Window * window, const CEGUI::utf8 * name) {
	return window->isUserStringDefined(name);
}

float bmx_cegui_window_getparentpixelwidth(CEGUI::Window * window) {
	return window->getParentPixelWidth();
}

float bmx_cegui_window_getparentpixelheight(CEGUI::Window * window) {
	return window->getParentPixelHeight();
}

int bmx_cegui_window_ismousepassthroughenabled(CEGUI::Window * window) {
	return window->isMousePassThroughEnabled();
}

int bmx_cegui_window_isautowindow(CEGUI::Window * window) {
	return window->isAutoWindow();
}

int bmx_cegui_window_iswritingxmlallowed(CEGUI::Window * window) {
	return window->isWritingXMLAllowed();
}

int bmx_cegui_window_isdragdroptarget(CEGUI::Window * window) {
	return window->isDragDropTarget();
}

void bmx_cegui_window_setclippedbyparent(CEGUI::Window * window, int setting) {
	window->setClippedByParent(setting);
}

void bmx_cegui_window_setid(CEGUI::Window * window, CEGUI::uint ID) {
	window->setID(ID);
}

/*void bmx_cegui_window_setprefix(CEGUI::Window * window, const CEGUI::utf8 * prefix) {
	window->setPrefix(prefix);
}*/

void bmx_cegui_window_inserttext(CEGUI::Window * window, const CEGUI::utf8 * text, int position) {
	window->insertText(text, position);
}

void bmx_cegui_window_appendtext(CEGUI::Window * window, const CEGUI::utf8 * text) {
	window->appendText(text);
}

void bmx_cegui_window_setfont(CEGUI::Window * window, CEGUI::Font * font) {
	window->setFont(font);
}

void bmx_cegui_window_setfontbyname(CEGUI::Window * window, const CEGUI::utf8 * name) {
	window->setFont(name);
}

void bmx_cegui_window_removechildwindow(CEGUI::Window * window, CEGUI::Window * win) {
	window->removeChildWindow(win);
}

void bmx_cegui_window_removechildwindowname(CEGUI::Window * window, const CEGUI::utf8 * name) {
	window->removeChildWindow(name);
}

void bmx_cegui_window_removechildwindowforid(CEGUI::Window * window, CEGUI::uint ID) {
	window->removeChildWindow(ID);
}

void bmx_cegui_window_movetofront(CEGUI::Window * window) {
	window->moveToFront();
}

void bmx_cegui_window_movetoback(CEGUI::Window * window) {
	window->moveToBack();
}

int bmx_cegui_window_captureinput(CEGUI::Window * window) {
	return window->captureInput();
}

void bmx_cegui_window_releaseinput(CEGUI::Window * window) {
	window->releaseInput();
}

void bmx_cegui_window_setrestorecapture(CEGUI::Window * window, int setting) {
	window->setRestoreCapture(setting);
}

void bmx_cegui_window_setalpha(CEGUI::Window * window, float alpha) {
	window->setAlpha(alpha);
}

void bmx_cegui_window_setinheritsalpha(CEGUI::Window * window, int setting) {
	window->setInheritsAlpha(setting);
}

void bmx_cegui_window_invalidate(CEGUI::Window * window) {
	window->invalidate();
}

void bmx_cegui_window_invalidaterenderingsurface(CEGUI::Window * window) {
	window->invalidateRenderingSurface();
}

void bmx_cegui_window_setmousecursor(CEGUI::Window * window, CEGUI::Image * image) {
	window->setMouseCursor(image);
}

void bmx_cegui_window_setmousecursormode(CEGUI::Window * window, CEGUI::MouseCursorImage image) {
	window->setMouseCursor(image);
}

void bmx_cegui_window_setmousecursorbyname(CEGUI::Window * window, const CEGUI::utf8 * imageset, const CEGUI::utf8 * imageName) {
	try {
		window->setMouseCursor(imageset, imageName);
	} catch (CEGUI::UnknownObjectException &e) {
		bmx_cegui_throw_unknownobjectexception(e);
	} catch (CEGUI::Exception &e) {
		bmx_cegui_throw_exception(e);
	}
}

void bmx_cegui_window_setzorderingenabled(CEGUI::Window * window, int setting) {
	window->setZOrderingEnabled(setting);
}

void bmx_cegui_window_setwantsmulticlickevents(CEGUI::Window * window, int setting) {
	window->setWantsMultiClickEvents(setting);
}

void bmx_cegui_window_setmouseautorepeateenabled(CEGUI::Window * window, int setting) {
	window->setMouseAutoRepeatEnabled(setting);
}

void bmx_cegui_window_setautorepeatdelay(CEGUI::Window * window, float delay) {
	window->setAutoRepeatDelay(delay);
}

void bmx_cegui_window_setautorepeatrate(CEGUI::Window * window, float rate) {
	window->setAutoRepeatRate(rate);
}

void bmx_cegui_window_setdistributescapturedinputs(CEGUI::Window * window, int setting) {
	window->setDistributesCapturedInputs(setting);
}

void bmx_cegui_window_notifydragdropitementers(CEGUI::Window * window, CEGUI::DragContainer * item) {
	window->notifyDragDropItemEnters(item);
}

void bmx_cegui_window_notifydragdropitemleaves(CEGUI::Window * window, CEGUI::DragContainer * item) {
	window->notifyDragDropItemLeaves(item);
}

void bmx_cegui_window_notifydragdropitemdropped(CEGUI::Window * window, CEGUI::DragContainer * item) {
	window->notifyDragDropItemDropped(item);
}

void bmx_cegui_window_destroy(CEGUI::Window * window) {
	window->destroy();
}

void bmx_cegui_window_settooltip(CEGUI::Window * window, CEGUI::Tooltip * tooltip) {
	window->setTooltip(tooltip);
}

void bmx_cegui_window_settooltiptype(CEGUI::Window * window, const CEGUI::utf8 * tooltipType) {
	window->setTooltipType(tooltipType);
}

void bmx_cegui_window_settooltiptext(CEGUI::Window * window, const CEGUI::utf8 * tip) {
	window->setTooltipText(tip);
}

void bmx_cegui_window_setinheritstooltiptext(CEGUI::Window * window, int setting) {
	window->setInheritsTooltipText(setting);
}

void bmx_cegui_window_setriseonclickenabled(CEGUI::Window * window, int setting) {
	window->setRiseOnClickEnabled(setting);
}

void bmx_cegui_window_setverticalaligmnent(CEGUI::Window * window, CEGUI::VerticalAlignment alignment) {
	window->setVerticalAlignment(alignment);
}

void bmx_cegui_window_sethorizontalalignment(CEGUI::Window * window, CEGUI::HorizontalAlignment alignment) {
	window->setHorizontalAlignment(alignment);
}

void bmx_cegui_window_setlooknfeel(CEGUI::Window * window, const CEGUI::utf8 * look) {
	try {
		window->setLookNFeel(look);
	} catch (CEGUI::UnknownObjectException &e) {
		bmx_cegui_throw_unknownobjectexception(e);
	} catch (CEGUI::Exception &e) {
		bmx_cegui_throw_exception(e);
	}
}

void bmx_cegui_window_setmodalstate(CEGUI::Window * window, int state) {
	window->setModalState(state);
}

void bmx_cegui_window_performchildwindowlayout(CEGUI::Window * window) {
	window->performChildWindowLayout();
}

void bmx_cegui_window_setuserstring(CEGUI::Window * window, const CEGUI::utf8 * name, const CEGUI::utf8 * value) {
	window->setUserString(name, value);
}

void bmx_cegui_window_setpositionu(CEGUI::Window * window, const CEGUI::UDim * x, const CEGUI::UDim * y) {
	window->setPosition(CEGUI::UVector2(*x, *y));
}

void bmx_cegui_window_setxpositionu(CEGUI::Window * window, const CEGUI::UDim * x) {
	window->setXPosition(*x);
}

void bmx_cegui_window_setypositionu(CEGUI::Window * window, const CEGUI::UDim * y) {
	window->setYPosition(*y);
}

void bmx_cegui_window_setsizeu(CEGUI::Window * window, const CEGUI::UDim * width, const CEGUI::UDim * height) {
	window->setSize(CEGUI::UVector2(*width, *height));
}

void bmx_cegui_window_setwidthu(CEGUI::Window * window, const CEGUI::UDim * width) {
	window->setWidth(*width);
}

void bmx_cegui_window_setheightu(CEGUI::Window * window, const CEGUI::UDim * height) {
	window->setHeight(*height);
}

BBObject * bmx_cegui_window_getmousecursor(CEGUI::Window * window, int useDefault) {
	return _bah_cegui_TCEImage__create(const_cast<CEGUI::Image*>(window->getMouseCursor(useDefault)));
}

void bmx_cegui_window_rename(CEGUI::Window * window, const CEGUI::utf8 * newName) {
	try {
		window->rename(newName);
	} catch (CEGUI::AlreadyExistsException &e) {
		bmx_cegui_throw_alreadyexistsexception(e);
	} catch (CEGUI::Exception &e) {
		bmx_cegui_throw_exception(e);
	}
}

void bmx_cegui_window_setdestroyedbyparent(CEGUI::Window * window, int setting) {
	window->setDestroyedByParent(setting);
}

void bmx_cegui_window_setalwaysontop(CEGUI::Window * window, int setting) {
	window->setAlwaysOnTop(setting);
}

void bmx_cegui_window_setenabled(CEGUI::Window * window, int setting) {
	window->setEnabled(setting);
}

void bmx_cegui_window_enable(CEGUI::Window * window) {
	window->enable();
}

void bmx_cegui_window_disable(CEGUI::Window * window) {
	window->disable();
}

void bmx_cegui_window_setvisible(CEGUI::Window * window, int setting) {
	window->setVisible(setting);
}

float bmx_cegui_window_getxposition(CEGUI::Window * window) {
	return window->getXPosition().d_offset;
}

float bmx_cegui_window_getyposition(CEGUI::Window * window) {
	return window->getYPosition().d_offset;
}

float bmx_cegui_window_getwidth(CEGUI::Window * window) {
	return window->getWidth().d_offset;
}

float bmx_cegui_window_getheight(CEGUI::Window * window) {
	return window->getHeight().d_offset;
}

void bmx_cegui_window_render(CEGUI::Window * window) {
	window->render();
}

void bmx_cegui_window_getsize(CEGUI::Window * window, float * w, float * h) {
	CEGUI::UVector2 v(window->getSize());
	*w = v.d_x.d_offset;
	*h = v.d_y.d_offset;
}

void bmx_cegui_window_getmaxsize(CEGUI::Window * window, float * w, float * h) {
	CEGUI::UVector2 v(window->getMinSize());
	*w = v.d_x.d_offset;
	*h = v.d_y.d_offset;
}

void bmx_cegui_window_getminsize(CEGUI::Window * window, float * w, float * h) {
	CEGUI::UVector2 v(window->getMaxSize());
	*w = v.d_x.d_offset;
	*h = v.d_y.d_offset;
}

void bmx_cegui_window_setmousepassthroughenabled(CEGUI::Window * window, int setting) {
	window->setMousePassThroughEnabled(setting);
}

void bmx_cegui_window_setwindowrenderer(CEGUI::Window * window, CEGUI::utf8 * name) {
	window->setWindowRenderer(name);
}

BBString * bmx_cegui_window_getwindowrenderername(CEGUI::Window * window) {
	return bah_cegui__convertUTF8ToMax(window->getWindowRendererName().data());
}

void bmx_cegui_window_setwritingxmlallowed(CEGUI::Window * window, int allow) {
	window->setWritingXMLAllowed(allow);
}

void bmx_cegui_window_notifyscreenareachanged(CEGUI::Window * window) {
	window->notifyScreenAreaChanged();
}

void bmx_cegui_window_setfalagardtype(CEGUI::Window * window, CEGUI::utf8 * type, CEGUI::utf8 * rendererType) {
	window->setFalagardType(type, rendererType);
}

void bmx_cegui_window_setdragdroptarget(CEGUI::Window * window, int setting) {
	window->setDragDropTarget(setting);
}

CEGUI::Font * bmx_cegui_window_getfont(CEGUI::Window * window, int useDefault) {
	return window->getFont(useDefault);
}

BBObject * bmx_cegui_window_gettooltip(CEGUI::Window * window) {
	return _bah_cegui_TCETooltip__create(window->getTooltip());
}

BBObject * bmx_cegui_window_getactivesibling(CEGUI::Window * window) {
	return newObjectForWindow(window->getActiveSibling());
}

void bmx_cegui_window_getparentpixelsize(CEGUI::Window * window, float * width, float * height) {
	CEGUI::Size s(window->getParentPixelSize());
	*width = s.d_width;
	*height = s.d_height;
}

// *************************************************

void bmx_cegui_connection_delete(MaxConnection * conn) {
	delete conn;
}

// *************************************************

int bmx_cegui_checkbox_isselected(CEGUI::Checkbox * cb) {
	return cb->isSelected();
}

void bmx_cegui_checkbox_setselected(CEGUI::Checkbox * cb, int selected) {
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

int bmx_cegui_editbox_hasinputfocus(CEGUI::Editbox * eb) {
	return eb->hasInputFocus();
}

int bmx_cegui_editbox_isreadonly(CEGUI::Editbox * eb) {
	return eb->isReadOnly();
}

int bmx_cegui_editbox_istextmasked(CEGUI::Editbox * eb) {
	return eb->isTextMasked();
}

int bmx_cegui_editbox_istextvalid(CEGUI::Editbox * eb) {
	return eb->isTextValid();
}

BBString * bmx_cegui_editbox_getvalidationstring(CEGUI::Editbox * eb) {
	return bah_cegui__convertUTF8ToMax(eb->getValidationString().data());
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

void bmx_cegui_editbox_setreadonly(CEGUI::Editbox * eb, int setting) {
	eb->setReadOnly(setting);
}

void bmx_cegui_editbox_settextmasked(CEGUI::Editbox * eb, int setting) {
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

void bmx_cegui_editbox_setmaskcodepoint(CEGUI::Editbox * eb, CEGUI::utf32 codePoint) {
	eb->setMaskCodePoint(codePoint);
}

// *************************************************

void bmx_cegui_logger_setlogginglevel(CEGUI::LoggingLevel level) {
	CEGUI::Logger::getSingleton().setLoggingLevel(level);
}

void bmx_cegui_logger_setlogfilename(const CEGUI::utf8 * filename, int append) {
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

int bmx_cegui_eventargs_gethandled(MaxEventArgs * args) {
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

int bmx_cegui_framewindow_issizingenabled(CEGUI::FrameWindow * win) {
	return win->isSizingEnabled();
}

int bmx_cegui_framewindow_isframeenabled(CEGUI::FrameWindow * win) {
	return win->isFrameEnabled();
}

int bmx_cegui_framewindow_istitlebarenabled(CEGUI::FrameWindow * win) {
	return win->isTitleBarEnabled();
}

int bmx_cegui_framewindow_isclosebuttonenabled(CEGUI::FrameWindow * win) {
	return win->isCloseButtonEnabled();
}

int bmx_cegui_framewindow_isrollupenabled(CEGUI::FrameWindow * win) {
	return win->isRollupEnabled();
}

int bmx_cegui_framewindow_isrolledup(CEGUI::FrameWindow * win) {
	return win->isRolledup();
}

float bmx_cegui_framewindow_getsizingborderthickness(CEGUI::FrameWindow * win) {
	return win->getSizingBorderThickness();
}

void bmx_cegui_framewindow_setsizingenabled(CEGUI::FrameWindow * win, int setting) {
	win->setSizingEnabled(setting);
}

void bmx_cegui_framewindow_setframeenabled(CEGUI::FrameWindow * win, int setting) {
	win->setFrameEnabled(setting);
}

void bmx_cegui_framewindow_settitlebarenabled(CEGUI::FrameWindow * win, int setting) {
	win->setTitleBarEnabled(setting);
}

void bmx_cegui_framewindow_setclosebuttonenabled(CEGUI::FrameWindow * win, int setting) {
	win->setCloseButtonEnabled(setting);
}

void bmx_cegui_framewindow_rollupenabled(CEGUI::FrameWindow * win, int setting) {
	win->setRollupEnabled(setting);
}

void bmx_cegui_framewindow_togglerollup(CEGUI::FrameWindow * win) {
	win->toggleRollup();
}

void bmx_cegui_framewindow_setsizingborderthickness(CEGUI::FrameWindow * win, float pixels) {
	win->setSizingBorderThickness(pixels);
}

int bmx_cegui_framewindow_isdragmovingenabled(CEGUI::FrameWindow * win) {
	return win->isDragMovingEnabled();
}

void bmx_cegui_framewindow_setdragmovingenabled(CEGUI::FrameWindow * win, int setting) {
	win->setDragMovingEnabled(setting);
}

int bmx_cegui_framewindow_ishit(CEGUI::FrameWindow * win, float x, float y, int allowDisabled) {
	return win->isHit(CEGUI::Point(x, y), static_cast<bool>(allowDisabled));
}

BBObject * bmx_cegui_framewindow_gettitlebar(CEGUI::FrameWindow * win) {
	return _bah_cegui_TCETitlebar__create(win->getTitlebar());
}

BBObject * bmx_cegui_framewindow_getclosebutton(CEGUI::FrameWindow * win) {
		return _bah_cegui_TCEPushButton__create(win->getCloseButton());
}

// *************************************************


int bmx_cegui_titlebar_isdraggingenabled(CEGUI::Titlebar * tb) {
	return tb->isDraggingEnabled();
}

void bmx_cegui_titlebar_setdraggingenabled(CEGUI::Titlebar * tb, int setting) {
	tb->setDraggingEnabled(setting);
}

// *************************************************


int bmx_cegui_buttonbase_ishovering(CEGUI::ButtonBase * base) {
	return base->isHovering();
}

int bmx_cegui_buttonbase_ispushed(CEGUI::ButtonBase * base) {
	return base->isPushed();
}

// *************************************************

int bmx_cegui_radiobutton_isselected(CEGUI::RadioButton * rb) {
	return rb->isSelected();
}

CEGUI::ulong bmx_cegui_radiobutton_getgroupid(CEGUI::RadioButton * rb) {
	return rb->getGroupID();
}

BBObject * bmx_cegui_radiobutton_getselectedbuttoningroup(CEGUI::RadioButton * rb) {
	return _bah_cegui_TCERadioButton__create(rb->getSelectedButtonInGroup());
}

void bmx_cegui_radiobutton_setselected(CEGUI::RadioButton * rb, int selected) {
	rb->setSelected(selected);
}

void bmx_cegui_radiobutton_setgroupid(CEGUI::RadioButton * rb, CEGUI::ulong group) {
	rb->setGroupID(group);
}

// *************************************************

void bmx_cegui_combodroplist_setarmed(CEGUI::ComboDropList * list, int setting) {
	list->setArmed(setting);
}

int bmx_cegui_combodroplist_isarmed(CEGUI::ComboDropList * list) {
	return list->isArmed();
}

void bmx_cegui_combodroplist_setautoarmenabled(CEGUI::ComboDropList * list, int setting) {
	list->setAutoArmEnabled(setting);
}

int bmx_cegui_combodroplist_isautoarmenabled(CEGUI::ComboDropList * list) {
	return list->isAutoArmEnabled();
}

// *************************************************

int bmx_cegui_multilineeditbox_hasinputfocus(CEGUI::MultiLineEditbox * eb) {
	return eb->hasInputFocus();
}

int bmx_cegui_multilineeditbox_isreadonly(CEGUI::MultiLineEditbox * eb) {
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

int bmx_cegui_multilineeditbox_iswordwrapped(CEGUI::MultiLineEditbox * eb) {
	return eb->isWordWrapped();
}

BBObject * bmx_cegui_multilineeditbox_getvertscrollbar(CEGUI::MultiLineEditbox * eb) {
	return _bah_cegui_TCEScrollbar__create(eb->getVertScrollbar());
}

int bmx_cegui_multilineeditbox_isvertscrollbaralwaysshown(CEGUI::MultiLineEditbox * eb) {
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

void bmx_cegui_multilineeditbox_setreadonly(CEGUI::MultiLineEditbox * eb, int setting) {
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

void bmx_cegui_multilineeditbox_setwordwrapping(CEGUI::MultiLineEditbox * eb, int setting) {
	eb->setWordWrapping(setting);
}

void bmx_cegui_multilineeditbox_setshowvertscrollbar(CEGUI::MultiLineEditbox * eb, int setting) {
	eb->setShowVertScrollbar(setting);
}


// *************************************************

int bmx_cegui_scrolleditemlistbase_isvertscrollbaralwaysshown(CEGUI::ScrolledItemListBase * lb) {
	return lb->isVertScrollbarAlwaysShown();
}

int bmx_cegui_scrolleditemlistbase_ishorzscrollbaralwaysshown(CEGUI::ScrolledItemListBase * lb) {
	return lb->isHorzScrollbarAlwaysShown();
}

BBObject * bmx_cegui_scrolleditemlistbase_getvertscrollbar(CEGUI::ScrolledItemListBase * lb) {

}

BBObject * bmx_cegui_scrolleditemlistbase_gethorzscrollbar(CEGUI::ScrolledItemListBase * lb) {

}

void bmx_cegui_scrolleditemlistbase_setshowvertscrollbar(CEGUI::ScrolledItemListBase * lb, int mode) {
	lb->setShowVertScrollbar(mode);
}

void bmx_cegui_scrolleditemlistbase_setshowhorzscrollbar(CEGUI::ScrolledItemListBase * lb, int mode) {
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

int bmx_cegui_itemlistbox_ismultiselectenabled(CEGUI::ItemListbox * lb) {
	return lb->isMultiSelectEnabled();
}

int bmx_cegui_itemlistbox_isitemselected(CEGUI::ItemListbox * lb, int index) {
	return lb->isItemSelected(index);
}

void bmx_cegui_itemlistbox_setmultiselectenabled(CEGUI::ItemListbox * lb, int state) {
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
/*
CEGUI::Imageset * bmx_cegui_imagesetmanager_createimagesetfromimagefile(const CEGUI::utf8 * name, const CEGUI::utf8 * filename, const CEGUI::utf8 * resourceGroup) {
	try {
		return CEGUI::ImagesetManager::getSingleton().createImagesetFromImageFile(name, filename, resourceGroup);

	} catch (CEGUI::FileIOException &e) {
		bmx_cegui_throw_fileioexception(e);
	} catch (CEGUI::AlreadyExistsException &e) {
		bmx_cegui_throw_alreadyexistsexception(e);
	} catch (CEGUI::InvalidRequestException &e) {
		bmx_cegui_throw_invalidrequestexception(e);
	} catch (CEGUI::Exception &e) {
		bmx_cegui_throw_exception(e);
	}
}

CEGUI::Imageset * bmx_cegui_imagesetmanager_createimageset(const CEGUI::utf8 * filename, const CEGUI::utf8 * resourceGroup) {
	try {
		return CEGUI::ImagesetManager::getSingleton().createImageset(filename, resourceGroup);
		
	} catch (CEGUI::FileIOException &e) {
		bmx_cegui_throw_fileioexception(e);
	} catch (CEGUI::AlreadyExistsException &e) {
		bmx_cegui_throw_alreadyexistsexception(e);
	} catch (CEGUI::Exception &e) {
		bmx_cegui_throw_exception(e);
	}
}

CEGUI::Imageset * bmx_cegui_imagesetmanager_createimagesetfromtexture(const CEGUI::utf8 * name, CEGUI::Texture * texture) {
	try {
		return CEGUI::ImagesetManager::getSingleton().createImageset(name, texture);
		
	} catch (CEGUI::AlreadyExistsException &e) {
		bmx_cegui_throw_alreadyexistsexception(e);
	} catch (CEGUI::Exception &e) {
		bmx_cegui_throw_exception(e);
	}
}

void bmx_cegui_imagesetmanager_destroyimageset(CEGUI::Imageset * imageset) {
	CEGUI::ImagesetManager::getSingleton().destroyImageset(imageset);
}

void bmx_cegui_imagesetmanager_destroyimagesetname(const CEGUI::utf8 * name) {
	CEGUI::ImagesetManager::getSingleton().destroyImageset(name);
}

void bmx_cegui_imagesetmanager_destroyallimagesets() {
	CEGUI::ImagesetManager::getSingleton().destroyAllImagesets();
}

CEGUI::Imageset * bmx_cegui_imagesetmanager_getimageset(const CEGUI::utf8 * name) {
	try {
		return CEGUI::ImagesetManager::getSingleton().getImageset(name);
	} catch (CEGUI::UnknownObjectException &e) {
		bmx_cegui_throw_unknownobjectexception(e);
	}
}

int bmx_cegui_imagesetmanager_isimagesetpresent(const CEGUI::utf8 * name) {
	return CEGUI::ImagesetManager::getSingleton().isImagesetPresent(name);
}

void bmx_cegui_imagesetmanager_notifyscreenresolution(float width, float height) {
	CEGUI::ImagesetManager::getSingleton().notifyScreenResolution(CEGUI::Size(width, height));
}
*/

CEGUI::Imageset * bmx_cegui_imagesetmanager_createimagesetfromtexture(const CEGUI::utf8 * name, CEGUI::Texture * texture, CEGUI::XMLResourceExistsAction action) {
	try {
		return & CEGUI::ImagesetManager::getSingleton().create(name, *texture, action);
	} catch (CEGUI::AlreadyExistsException &e) {
		bmx_cegui_throw_alreadyexistsexception(e);
	} catch (CEGUI::Exception &e) {
		bmx_cegui_throw_exception(e);
	}
}

CEGUI::Imageset * bmx_cegui_imagesetmanager_createimagesetfromimagefile(const CEGUI::utf8 * name, const CEGUI::utf8 * filename, const CEGUI::utf8 * resourceGroup, CEGUI::XMLResourceExistsAction action) {
	try {
		return & CEGUI::ImagesetManager::getSingleton().createFromImageFile(name, filename, resourceGroup, action);
	} catch (CEGUI::FileIOException &e) {
		bmx_cegui_throw_fileioexception(e);
	} catch (CEGUI::AlreadyExistsException &e) {
		bmx_cegui_throw_alreadyexistsexception(e);
	} catch (CEGUI::InvalidRequestException &e) {
		bmx_cegui_throw_invalidrequestexception(e);
	} catch (CEGUI::Exception &e) {
		bmx_cegui_throw_exception(e);
	}
}

void bmx_cegui_imagesetmanager_notifydisplaysizechanged(float width, float height) {
	CEGUI::ImagesetManager::getSingleton().notifyDisplaySizeChanged(CEGUI::Size(width, height));
}

BBObject * bmx_cegui_imagesetmanager_createimageset(const CEGUI::utf8 * filename, const CEGUI::utf8 * resourceGroup, CEGUI::XMLResourceExistsAction action) {
	return _bah_cegui_TCEImageset__create(&CEGUI::ImagesetManager::getSingleton().create(filename, resourceGroup, action));
}

void bmx_cegui_imagesetmanager_destroyname(const CEGUI::utf8 * name) {
	CEGUI::ImagesetManager::getSingleton().destroy(name);	
}

void bmx_cegui_imagesetmanager_destroyobj(CEGUI::Imageset * imageset) {
	CEGUI::ImagesetManager::getSingleton().destroy(*imageset);
}

void bmx_cegui_imagesetmanager_destroyall() {
	CEGUI::ImagesetManager::getSingleton().destroyAll();
}

CEGUI::Imageset * bmx_cegui_imagesetmanager_get(const CEGUI::utf8 * name) {
	return & CEGUI::ImagesetManager::getSingleton().get(name);
}

int bmx_cegui_imagesetmanager_isdefined(const CEGUI::utf8 * name) {
	return static_cast<int>(CEGUI::ImagesetManager::getSingleton().isDefined(name));
}

void bmx_cegui_imagesetmanager_createall(const CEGUI::utf8 * pattern, const CEGUI::utf8 * resourceGroup) {
	CEGUI::ImagesetManager::getSingleton().createAll(pattern, resourceGroup);
}


// *************************************************

CEGUI::Font * bmx_cegui_fontmanager_createfreetypefont(const CEGUI::utf8 * font_name, float pointSize, int antialiased, const CEGUI::utf8 * fontFilename, 
		const CEGUI::utf8 * resourceGroup, int autoScaled, float nativeHorzRes, float nativeVertRes, CEGUI::XMLResourceExistsAction action) {
	return &CEGUI::FontManager::getSingleton().createFreeTypeFont(font_name, pointSize, antialiased, fontFilename, resourceGroup, 
		autoScaled, nativeHorzRes, nativeVertRes, action);
}

CEGUI::Font * bmx_cegui_fontmanager_createpixmapfont(const CEGUI::utf8 * font_name, const CEGUI::utf8 * imagesetFilename, const CEGUI::utf8 * resourceGroup,
		int autoScaled, float nativeHorzRes, float nativeVertRes, CEGUI::XMLResourceExistsAction action) {
	return & CEGUI::FontManager::getSingleton().createPixmapFont(font_name, imagesetFilename, resourceGroup, 
		autoScaled, nativeHorzRes, nativeVertRes, action);
}

void bmx_cegui_fontmanager_notifydislaysizechanged(float width, float height) {
	CEGUI::FontManager::getSingleton().notifyDisplaySizeChanged(CEGUI::Size(width, height));
}

CEGUI::Font * bmx_cegui_fontmanager_createfont(const CEGUI::utf8 * filename, const CEGUI::utf8 * resourceGroup, CEGUI::XMLResourceExistsAction action) {
	try {
		return &CEGUI::FontManager::getSingleton().create(filename, resourceGroup, action);
	} catch (CEGUI::FileIOException &e) {
		bmx_cegui_throw_fileioexception(e);
	} catch (CEGUI::InvalidRequestException &e) {
		bmx_cegui_throw_invalidrequestexception(e);
	} catch (CEGUI::AlreadyExistsException &e) {
		bmx_cegui_throw_alreadyexistsexception(e);
	} catch (CEGUI::GenericException &e) {
		bmx_cegui_throw_genericexception(e);
	} catch (CEGUI::RendererException &e) {
		bmx_cegui_throw_rendererexception(e);
	} catch (CEGUI::MemoryException &e) {
		bmx_cegui_throw_memoryexception(e);
	} catch (CEGUI::Exception &e) {
		bmx_cegui_throw_exception(e);
	}
}

void bmx_cegui_fontmanager_destroyname(const CEGUI::utf8 * name) {
	CEGUI::FontManager::getSingleton().destroy(name);
}

void bmx_cegui_fontmanager_destroyobj(CEGUI::Font * font) {
	CEGUI::FontManager::getSingleton().destroy(*font);
}

void bmx_cegui_fontmanager_destroyall() {
	CEGUI::FontManager::getSingleton().destroyAll();
}

BBObject * bmx_cegui_fontmanager_get(const CEGUI::utf8 * name) {
	try{ 
		// TODO
	} catch (CEGUI::UnknownObjectException &e) {
		bmx_cegui_throw_unknownobjectexception(e);
	} catch (CEGUI::Exception &e) {
		bmx_cegui_throw_exception(e);
	}
}

int bmx_cegui_fontmanager_isdefined(const CEGUI::utf8 * name) {
	return static_cast<int>(CEGUI::FontManager::getSingleton().isDefined(name));
}

void bmx_cegui_fontmanager_createall(const CEGUI::utf8 * pattern, const CEGUI::utf8 * resourceGroup) {
	CEGUI::FontManager::getSingleton().createAll(pattern, resourceGroup);
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

// *************************************************

float bmx_cegui_scrollbar_getdocumentsize(CEGUI::Scrollbar * sb) {
	return sb->getDocumentSize();
}

float bmx_cegui_scrollbar_getpagesize(CEGUI::Scrollbar * sb) {
	return sb->getPageSize();
}

float bmx_cegui_scrollbar_getstepsize(CEGUI::Scrollbar * sb) {
	return sb->getStepSize();
}

float bmx_cegui_scrollbar_getoverlapsize(CEGUI::Scrollbar * sb) {
	return sb->getOverlapSize();
}

float bmx_cegui_scrollbar_getscrollposition(CEGUI::Scrollbar * sb) {
	return sb->getScrollPosition();
}

BBObject * bmx_cegui_scrollbar_getincreasebutton(CEGUI::Scrollbar * sb) {
	return _bah_cegui_TCEPushButton__create(sb->getIncreaseButton());
}

BBObject * bmx_cegui_scrollbar_getdecreasebutton(CEGUI::Scrollbar * sb) {
	return _bah_cegui_TCEPushButton__create(sb->getDecreaseButton());
}

void bmx_cegui_scrollbar_setdocumentsize(CEGUI::Scrollbar * sb, float documentSize) {
	sb->setDocumentSize(documentSize);
}

void bmx_cegui_scrollbar_setpagesize(CEGUI::Scrollbar * sb, float pageSize) {
	sb->setPageSize(pageSize);
}

void bmx_cegui_scrollbar_setstepsize(CEGUI::Scrollbar * sb, float stepSize) {
	sb->setStepSize(stepSize);
}

void bmx_cegui_scrollbar_setoverlapsize(CEGUI::Scrollbar * sb, float overlapSize) {
	sb->setOverlapSize(overlapSize);
}

void bmx_cegui_scrollbar_setscrollposition(CEGUI::Scrollbar * sb, float position) {
	sb->setScrollPosition(position);
}


// *************************************************

float bmx_cegui_slider_getcurrentvalue(CEGUI::Slider * slider) {
	return slider->getCurrentValue();
}

float bmx_cegui_slider_getmaxvalue(CEGUI::Slider * slider) {
	return slider->getMaxValue();
}

float bmx_cegui_slider_getclickstep(CEGUI::Slider * slider) {
	return slider->getClickStep();
}

void bmx_cegui_slider_setmaxvalue(CEGUI::Slider * slider, float maxVal) {
	slider->setMaxValue(maxVal);
}

void bmx_cegui_slider_setcurrentvalue(CEGUI::Slider * slider, float value) {
	slider->setCurrentValue(value);
}

void bmx_cegui_slider_setclickstep(CEGUI::Slider * slider, float clickStep) {
	slider->setClickStep(clickStep);
}


// *************************************************

BBObject * bmx_cegui_tooltip_gettargetwindow(CEGUI::Tooltip * tt) {
	return newObjectForWindow(const_cast<CEGUI::Window *>(tt->getTargetWindow()));
}

float bmx_cegui_tooltip_gethovertime(CEGUI::Tooltip * tt) {
	return tt->getHoverTime();
}

void bmx_cegui_tooltip_setdisplaytime(CEGUI::Tooltip * tt, float seconds) {
	tt->setDisplayTime(seconds);
}

float bmx_cegui_tooltip_getfadetime(CEGUI::Tooltip * tt) {
	return tt->getFadeTime();
}

void bmx_cegui_tooltip_sethovertime(CEGUI::Tooltip * tt, float seconds) {
	tt->setHoverTime(seconds);
}

float bmx_cegui_tooltip_getdisplaytime(CEGUI::Tooltip * tt) {
	return tt->getDisplayTime();
}

void bmx_cegui_tooltip_setfadetime(CEGUI::Tooltip * tt, float seconds) {
	tt->setFadeTime(seconds);
}

void bmx_cegui_tooltip_positionself(CEGUI::Tooltip * tt) {
	tt->positionSelf();
}

void bmx_cegui_tooltip_sizeself(CEGUI::Tooltip * tt) {
	tt->sizeSelf();
}

void bmx_cegui_tooltip_gettextsize(CEGUI::Tooltip * tt, float * width, float * height) {
	CEGUI::Size s(tt->getTextSize());
	*width = s.d_width;
	*height = s.d_height;
}


// *************************************************

int bmx_cegui_tabcontrol_gettabcount(CEGUI::TabControl * tc) {
	return tc->getTabCount();
}

CEGUI::TabControl::TabPanePosition bmx_cegui_tabcontrol_gettabpaneposition(CEGUI::TabControl * tc) {
	return tc->getTabPanePosition();
}

void bmx_cegui_tabcontrol_settabpaneposition(CEGUI::TabControl * tc, CEGUI::TabControl::TabPanePosition pos) {
	tc->setTabPanePosition(pos);
}

void bmx_cegui_tabcontrol_setselectedtab(CEGUI::TabControl * tc, const CEGUI::utf8 * name) {
	tc->setSelectedTab(name);
}

void bmx_cegui_tabcontrol_setselectedtabforid(CEGUI::TabControl * tc, CEGUI::uint ID) {
	tc->setSelectedTab(ID);
}

void bmx_cegui_tabcontrol_setselectedtabatindex(CEGUI::TabControl * tc, int index) {
	tc->setSelectedTabAtIndex(index);
}

void bmx_cegui_tabcontrol_maketabvisible(CEGUI::TabControl * tc, const CEGUI::utf8 * name) {
	tc->makeTabVisible(name);
}

void bmx_cegui_tabcontrol_maketabvisibleforid(CEGUI::TabControl * tc, CEGUI::uint ID) {
	tc->makeTabVisible(ID);
}

void bmx_cegui_tabcontrol_maketabvisibleatindex(CEGUI::TabControl * tc, int index) {
	tc->makeTabVisibleAtIndex(index);
}

BBObject * bmx_cegui_tabcontrol_gettabcontentsatindex(CEGUI::TabControl * tc, int index) {
	return newObjectForWindow(tc->getTabContentsAtIndex(index));
}

BBObject * bmx_cegui_tabcontrol_gettabcontents(CEGUI::TabControl * tc, const CEGUI::utf8 * name) {
	return newObjectForWindow(tc->getTabContents(name));
}

BBObject * bmx_cegui_tabcontrol_gettabcontentsforid(CEGUI::TabControl * tc, CEGUI::uint ID) {
	return newObjectForWindow(tc->getTabContents(ID));
}

int bmx_cegui_tabcontrol_istabcontentsselected(CEGUI::TabControl * tc, CEGUI::Window * wnd) {
	return tc->isTabContentsSelected(wnd);
}

int bmx_cegui_tabcontrol_getselectedtabindex(CEGUI::TabControl * tc) {
	return tc->getSelectedTabIndex();
}

float bmx_cegui_tabcontrol_gettabheight(CEGUI::TabControl * tc) {
	return tc->getTabHeight().d_offset;
}

float bmx_cegui_tabcontrol_gettabtextpadding(CEGUI::TabControl * tc) {
	return tc->getTabTextPadding().d_offset;
}

void bmx_cegui_tabcontrol_settabheight(CEGUI::TabControl * tc, float height) {
	tc->setTabHeight(guessUDim(height));
}

void bmx_cegui_tabcontrol_settabtextpadding(CEGUI::TabControl * tc, float padding) {
	tc->setTabTextPadding(guessUDim(padding));
}

void bmx_cegui_tabcontrol_addtab(CEGUI::TabControl * tc, CEGUI::Window * wnd) {
	tc->addTab(wnd);
}

void bmx_cegui_tabcontrol_removetab(CEGUI::TabControl * tc, const CEGUI::utf8 * name) {
	tc->removeTab(name);
}

void bmx_cegui_tabcontrol_removetabforid(CEGUI::TabControl * tc, CEGUI::uint ID) {
	tc->removeTab(ID);
}

void bmx_cegui_tabcontrol_settabheightu(CEGUI::TabControl * tc, const CEGUI::UDim * height) {
	tc->setTabHeight(*height);
}

// *************************************************

/*
BBObject * bmx_cegui_renderer_createtexture(CEGUI::Renderer * rend, const CEGUI::utf8 * filename, const CEGUI::utf8 * resourceGroup) {
	return _bah_cegui_TCETexture__create(rend->createTexture(filename, resourceGroup));
}

BBObject * bmx_cegui_renderer_createtexturewithsize(CEGUI::Renderer * rend, float size) {
	return _bah_cegui_TCETexture__create(rend->createTexture(size));
}

void bmx_cegui_renderer_destroytexture(CEGUI::Renderer * rend, CEGUI::Texture * texture) {
	rend->destroyTexture(texture);
}

void bmx_cegui_renderer_destroyalltextures(CEGUI::Renderer * rend) {
	rend->destroyAllTextures();
}

int bmx_cegui_renderer_isqueueingenabled(CEGUI::Renderer * rend) {
	return rend->isQueueingEnabled();
}

float bmx_cegui_renderer_getwidth(CEGUI::Renderer * rend) {
	return rend->getWidth();
}

float bmx_cegui_renderer_getheight(CEGUI::Renderer * rend) {
	return rend->getHeight();
}

void bmx_cegui_renderer_getsize(CEGUI::Renderer * rend, float * width, float * height) {
	CEGUI::Size s(rend->getSize());
	*width = s.d_width;
	*height = s.d_height;
}

CEGUI::uint bmx_cegui_renderer_getmaxtexturesize(CEGUI::Renderer * rend) {
	return rend->getMaxTextureSize();
}

CEGUI::uint bmx_cegui_renderer_gethorzscreendpi(CEGUI::Renderer * rend) {
	return rend->getHorzScreenDPI();
}

CEGUI::uint bmx_cegui_renderer_getvertscreendpi(CEGUI::Renderer * rend) {
	return rend->getVertScreenDPI();
}

void bmx_cegui_renderer_resetzvalue(CEGUI::Renderer * rend) {
	rend->resetZValue();
}

void bmx_cegui_renderer_advancezvalue(CEGUI::Renderer * rend) {
	rend->advanceZValue();
}

float bmx_cegui_renderer_getcurrentz(CEGUI::Renderer * rend) {
	return rend->getCurrentZ();
}

float bmx_cegui_renderer_getzlayer(CEGUI::Renderer * rend, CEGUI::uint layer) {
	return rend->getZLayer(layer);
}

BBString * bmx_cegui_renderer_getidentifierstring(CEGUI::Renderer * rend) {
	return bah_cegui__convertUTF8ToMax(rend->getIdentifierString().data());
}
*/

// *************************************************

void bmx_cegui_texture_getsize(CEGUI::Texture * texture, int * w, int * h) {
	CEGUI::Size s(texture->getSize());
	*w = s.d_width;
	*h = s.d_height;
}

void bmx_cegui_texture_getoriginaldatasize(CEGUI::Texture * texture, int * w, int * h) {
	CEGUI::Size s(texture->getOriginalDataSize());
	*w = s.d_width;
	*h = s.d_height;
}

void bmx_cegui_texture_loadfromfile(CEGUI::Texture * texture, const CEGUI::utf8 * filename, const CEGUI::utf8 * resourceGroup) {
	texture->loadFromFile(filename, resourceGroup);
}

void bmx_cegui_texture_loadfrommemory(CEGUI::Texture * texture, const void * buffer, CEGUI::uint width, CEGUI::uint height, CEGUI::Texture::PixelFormat pixelFormat) {
	texture->loadFromMemory(buffer, CEGUI::Size(width, height), pixelFormat);
}

void bmx_cegui_texture_savetomemory(CEGUI::Texture * texture, void * buffer) {
	texture->saveToMemory(buffer);
}

// *************************************************


void bmx_cegui_scheme_loadresources(CEGUI::Scheme * scheme) {
	scheme->loadResources();
}

void bmx_cegui_scheme_unloadresources(CEGUI::Scheme * scheme) {
	scheme->unloadResources();
}

int bmx_cegui_scheme_resourcesloaded(CEGUI::Scheme * scheme) {
	return scheme->resourcesLoaded();
}

BBString * bmx_cegui_scheme_getname(CEGUI::Scheme * scheme) {
	return bah_cegui__convertUTF8ToMax(scheme->getName().data());
}

// *************************************************

BBString * bmx_cegui_listboxitem_gettext(CEGUI::ListboxItem * item) {
	return bah_cegui__convertUTF8ToMax(item->getText().data());
}

BBString * bmx_cegui_listboxitem_gettooltiptext(CEGUI::ListboxItem * item) {
	return bah_cegui__convertUTF8ToMax(item->getTooltipText().data());
}

CEGUI::uint bmx_cegui_listboxitem_getid(CEGUI::ListboxItem * item) {
	return item->getID();
}

int bmx_cegui_listboxitem_isselected(CEGUI::ListboxItem * item) {
	return item->isSelected();
}

int bmx_cegui_listboxitem_isdisabled(CEGUI::ListboxItem * item) {
	return item->isDisabled();
}

int bmx_cegui_listboxitem_isautodeleted(CEGUI::ListboxItem * item) {
	return item->isAutoDeleted();
}

BBObject * bmx_cegui_listboxitem_getownerwindow(CEGUI::ListboxItem * item) {
	return newObjectForWindow(const_cast<CEGUI::Window *>(item->getOwnerWindow()));
}

void bmx_cegui_listboxitem_settext(CEGUI::ListboxItem * item, const CEGUI::utf8 * text) {
	item->setText(text);
}

void bmx_cegui_listboxitem_settooltiptext(CEGUI::ListboxItem * item, const CEGUI::utf8 * text) {
	item->setTooltipText(text);
}

void bmx_cegui_listboxitem_setid(CEGUI::ListboxItem * item, CEGUI::uint itemId) {
	item->setID(itemId);
}

void bmx_cegui_listboxitem_setselected(CEGUI::ListboxItem * item, int setting) {
	item->setSelected(setting);
}

void bmx_cegui_listboxitem_setdisabled(CEGUI::ListboxItem * item, int setting) {
	item->setDisabled(setting);
}

void bmx_cegui_listboxitem_setautodeleted(CEGUI::ListboxItem * item, int setting) {
	item->setAutoDeleted(setting);
}

void bmx_cegui_listboxitem_setownerwindow(CEGUI::ListboxItem * item, CEGUI::Window * owner) {
	item->setOwnerWindow(owner);
}

void bmx_cegui_listboxitem_setuserdata(CEGUI::ListboxItem * item, BBObject * itemData) {
	item->setUserData(itemData);
}

MaxCEColourRect * bmx_cegui_listboxitem_getselectioncolours(CEGUI::ListboxItem * item) {
	CEGUI::ColourRect r = item->getSelectionColours();
	return new MaxCEColourRect(r);
}

BBObject * bmx_cegui_listboxitem_getselectionbrushimage(CEGUI::ListboxItem * item) {
	return _bah_cegui_TCEImage__create(const_cast<CEGUI::Image*>(item->getSelectionBrushImage()));
}

void bmx_cegui_listboxitem_setselectioncolours(CEGUI::ListboxItem * item, MaxCEColourRect * cols) {
	item->setSelectionColours(cols->Rect());
}

void bmx_cegui_listboxitem_setselectioncoloursforcorners(CEGUI::ListboxItem * item, MaxCEColour * topLeftColour, MaxCEColour * topRightColour, MaxCEColour * bottomLeftColour, MaxCEColour * bottomRightColour) {
	item->setSelectionColours(topLeftColour->Colour(), topRightColour->Colour(), bottomLeftColour->Colour(), bottomRightColour->Colour());
}

void bmx_cegui_listboxitem_setselectioncoloursforcolour(CEGUI::ListboxItem * item, MaxCEColour * col) {
	item->setSelectionColours(col->Colour());
}

void bmx_cegui_listboxitem_setselectionbrushimage(CEGUI::ListboxItem * item, CEGUI::Image * image) {
	item->setSelectionBrushImage(image);
}

void bmx_cegui_listboxitem_setselectionbrushimagebyname(CEGUI::ListboxItem * item, const CEGUI::utf8 * imageset, const CEGUI::utf8 * image) {
	item->setSelectionBrushImage(imageset, image);
}

void bmx_cegui_listboxitem_getpixelsize(CEGUI::ListboxItem * item, float * width, float * height) {
	CEGUI::Size s = item->getPixelSize();
	*width = s.d_width;
	*height = s.d_height;
}

BBObject * bmx_cegui_listboxitem_getuserdata(CEGUI::ListboxItem * item) {
	void * data = item->getUserData();
	if (data) {
		return (BBObject*)data;
	} else {
		return &bbNullObject;
	}
}


// *************************************************


int bmx_cegui_multicolumnlist_isusersortcontrolenabled(CEGUI::MultiColumnList * mc) {
	return mc->isUserSortControlEnabled();
}

int bmx_cegui_multicolumnlist_isusercolumnsizingenabled(CEGUI::MultiColumnList * mc) {
	return mc->isUserColumnSizingEnabled();
}

int bmx_cegui_multicolumnlist_isusercolumndraggingenabled(CEGUI::MultiColumnList * mc) {
	return mc->isUserColumnDraggingEnabled();
}

CEGUI::uint bmx_cegui_multicolumnlist_getcolumncount(CEGUI::MultiColumnList * mc) {
	return mc->getColumnCount();
}

CEGUI::uint bmx_cegui_multicolumnlist_getrowcount(CEGUI::MultiColumnList * mc) {
	return mc->getRowCount();
}

CEGUI::uint bmx_cegui_multicolumnlist_getsortcolumn(CEGUI::MultiColumnList * mc) {
	return mc->getSortColumn();
}

CEGUI::uint bmx_cegui_multicolumnlist_getcolumnwithid(CEGUI::MultiColumnList * mc, CEGUI::uint colId) {
	return mc->getColumnWithID(colId);
}

CEGUI::uint bmx_cegui_multicolumnlist_getcolumnwithheadertext(CEGUI::MultiColumnList * mc, const CEGUI::utf8 * text) {
	return mc->getColumnWithHeaderText(text);
}

float bmx_cegui_multicolumnlist_gettotalcolumnheaderswidth(CEGUI::MultiColumnList * mc) {
	return mc->getTotalColumnHeadersWidth().d_offset;
}

float bmx_cegui_multicolumnlist_getcolumnheaderwidth(CEGUI::MultiColumnList * mc, CEGUI::uint colIdx) {
	return mc->getColumnHeaderWidth(colIdx).d_offset;
}

CEGUI::ListHeaderSegment::SortDirection bmx_cegui_multicolumnlist_getsortdirection(CEGUI::MultiColumnList * mc) {
	return mc->getSortDirection();
}

MaxListHeaderSegment * bmx_cegui_multicolumnlist_getheadersegmentforcolumn(CEGUI::MultiColumnList * mc, CEGUI::uint colIdx) {
 // TODO
	return NULL;
}

CEGUI::uint bmx_cegui_multicolumnlist_getitemrowindex(CEGUI::MultiColumnList * mc, const CEGUI::ListboxItem * item) {
	return mc->getItemRowIndex(item);
}

CEGUI::uint bmx_cegui_multicolumnlist_getitemcolumnindex(CEGUI::MultiColumnList * mc, const CEGUI::ListboxItem * item) {
	return mc->getItemColumnIndex(item);
}

void bmx_cegui_multicolumnlist_getitemgridreference(CEGUI::MultiColumnList * mc, const CEGUI::ListboxItem * item, int * row, int * col) {
	CEGUI::MCLGridRef ref = mc->getItemGridReference(item);
	*row = ref.row;
	*col = ref.column;
}

BBObject * bmx_cegui_multicolumnlist_getitematgridreference(CEGUI::MultiColumnList * mc, int row, int col) {
	return _bah_cegui_TCEListboxItem__create(mc->getItemAtGridReference(CEGUI::MCLGridRef(row, col)));
}

int bmx_cegui_multicolumnlist_islistboxitemincolumn(CEGUI::MultiColumnList * mc, const CEGUI::ListboxItem * item, CEGUI::uint colIdx) {
	return mc->isListboxItemInColumn(item, colIdx);
}

int bmx_cegui_multicolumnlist_islistboxiteminrow(CEGUI::MultiColumnList * mc, const CEGUI::ListboxItem * item, CEGUI::uint rowIdx) {
	return mc->isListboxItemInRow(item, rowIdx);
}

int bmx_cegui_multicolumnlist_islistboxiteminlist(CEGUI::MultiColumnList * mc, const CEGUI::ListboxItem * item) {
	return mc->isListboxItemInList(item);
}

BBObject * bmx_cegui_multicolumnlist_findcolumnitemwithtext(CEGUI::MultiColumnList * mc, const CEGUI::utf8 * text, CEGUI::uint colIdx, const CEGUI::ListboxItem * startItem) {
	return _bah_cegui_TCEListboxItem__create(mc->findColumnItemWithText(text, colIdx, startItem));
}

BBObject * bmx_cegui_multicolumnlist_findrowitemwithtext(CEGUI::MultiColumnList * mc, const CEGUI::utf8 * text, CEGUI::uint rowIdx, const CEGUI::ListboxItem * startItem) {
	return _bah_cegui_TCEListboxItem__create(mc->findRowItemWithText(text, rowIdx, startItem));
}

BBObject * bmx_cegui_multicolumnlist_findlistitemwithtext(CEGUI::MultiColumnList * mc, const CEGUI::utf8 * text, const CEGUI::ListboxItem * startItem) {
	return _bah_cegui_TCEListboxItem__create(mc->findListItemWithText(text, startItem));
}

BBObject * bmx_cegui_multicolumnlist_getfirstselecteditem(CEGUI::MultiColumnList * mc) {
	return _bah_cegui_TCEListboxItem__create(mc->getFirstSelectedItem());
}

BBObject * bmx_cegui_multicolumnlist_getnextselected(CEGUI::MultiColumnList * mc, const CEGUI::ListboxItem * startItem) {
	return _bah_cegui_TCEListboxItem__create(mc->getNextSelected(startItem));
}

CEGUI::uint bmx_cegui_multicolumnlist_getselectedcount(CEGUI::MultiColumnList * mc) {
	return mc->getSelectedCount();
}

int bmx_cegui_multicolumnlist_isitemselected(CEGUI::MultiColumnList * mc, int row, int col) {
	return mc->isItemSelected(CEGUI::MCLGridRef(row, col));
}

CEGUI::uint bmx_cegui_multicolumnlist_getnominatedselectioncolumnid(CEGUI::MultiColumnList * mc) {
	return mc->getNominatedSelectionColumnID();
}

CEGUI::uint bmx_cegui_multicolumnlist_getnominatedselectioncolum(CEGUI::MultiColumnList * mc) {
	return mc->getNominatedSelectionColumn();
}

CEGUI::uint bmx_cegui_multicolumnlist_getnominatedselectionrow(CEGUI::MultiColumnList * mc) {
	return mc->getNominatedSelectionRow();
}

CEGUI::MultiColumnList::SelectionMode bmx_cegui_multicolumnlist_getselectionmode(CEGUI::MultiColumnList * mc) {
	return mc->getSelectionMode();
}

int bmx_cegui_multicolumnlist_isvertscrollbaralwaysshown(CEGUI::MultiColumnList * mc) {
	return mc->isVertScrollbarAlwaysShown();
}

int bmx_cegui_multicolumnlist_ishorzscrollbaralwaysshown(CEGUI::MultiColumnList * mc) {
	return mc->isHorzScrollbarAlwaysShown();
}

CEGUI::uint bmx_cegui_multicolumnlist_getcolumnid(CEGUI::MultiColumnList * mc, CEGUI::uint colIdx) {
	return mc->getColumnID(colIdx);
}

CEGUI::uint bmx_cegui_multicolumnlist_getrowid(CEGUI::MultiColumnList * mc, CEGUI::uint rowIdx) {
	return mc->getRowID(rowIdx);
}

CEGUI::uint bmx_cegui_multicolumnlist_getrowwithid(CEGUI::MultiColumnList * mc, CEGUI::uint rowId) {
	return mc->getRowWithID(rowId);
}

void bmx_cegui_multicolumnlist_getlistrenderarea(CEGUI::MultiColumnList * mc, float * x, float * y, float * w, float * h) {
	CEGUI::Rect r = mc->getListRenderArea();
	*x = r.d_left;
	*y = r.d_top;
	*w = r.getWidth();
	*h = r.getHeight();
}

BBObject * bmx_cegui_multicolumnlist_getvertscrollbar(CEGUI::MultiColumnList * mc) {
	return _bah_cegui_TCEScrollbar__create(mc->getVertScrollbar());
}

BBObject * bmx_cegui_multicolumnlist_gethorzscrollbar(CEGUI::MultiColumnList * mc) {
	return _bah_cegui_TCEScrollbar__create(mc->getHorzScrollbar());
}

BBObject * bmx_cegui_multicolumnlist_getlistheader(CEGUI::MultiColumnList * mc) {
	return _bah_cegui_TCEListHeader__create(mc->getListHeader());
}

float bmx_cegui_multicolumnlist_gettotalrowsheight(CEGUI::MultiColumnList * mc) {
	return mc->getTotalRowsHeight();
}

float bmx_cegui_multicolumnlist_getwidestcolumnitemwidth(CEGUI::MultiColumnList * mc, CEGUI::uint colIdx) {
	return mc->getWidestColumnItemWidth(colIdx);
}

float bmx_cegui_multicolumnlist_gethighestrowitemheight(CEGUI::MultiColumnList * mc, CEGUI::uint rowIdx) {
	return mc->getHighestRowItemHeight(rowIdx);
}

void bmx_cegui_multicolumnlist_resetlist(CEGUI::MultiColumnList * mc) {
	mc->resetList();
}

void bmx_cegui_multicolumnlist_addcolumn(CEGUI::MultiColumnList * mc, const CEGUI::utf8 * text, CEGUI::uint colId, float width) {
	mc->addColumn(text, colId, guessUDim(width));
}

void bmx_cegui_multicolumnlist_addcolumnu(CEGUI::MultiColumnList * mc, const CEGUI::utf8 * text, CEGUI::uint colId, const CEGUI::UDim * width) {
	mc->addColumn(text, colId, *width);
}

void bmx_cegui_multicolumnlist_insertcolumn(CEGUI::MultiColumnList * mc, const CEGUI::utf8 * text, CEGUI::uint colId, float width, CEGUI::uint position) {
	mc->insertColumn(text, colId, guessUDim(width), position);
}

void bmx_cegui_multicolumnlist_insertcolumnu(CEGUI::MultiColumnList * mc, const CEGUI::utf8 * text, CEGUI::uint colId, const CEGUI::UDim * width, CEGUI::uint position) {
	mc->insertColumn(text, colId, *width, position);
}

void bmx_cegui_multicolumnlist_removecolumn(CEGUI::MultiColumnList * mc, CEGUI::uint colIdx) {
	mc->removeColumn(colIdx);
}

void bmx_cegui_multicolumnlist_removecolumnwithid(CEGUI::MultiColumnList * mc, CEGUI::uint colId) {
	mc->removeColumnWithID(colId);
}

void bmx_cegui_multicolumnlist_movecolumn(CEGUI::MultiColumnList * mc, CEGUI::uint colIdx, CEGUI::uint position) {
	mc->moveColumn(colIdx, position);
}

void bmx_cegui_multicolumnlist_movecolumnwithid(CEGUI::MultiColumnList * mc, CEGUI::uint colId, CEGUI::uint position) {
	mc->moveColumnWithID(colId, position);
}

CEGUI::uint bmx_cegui_multicolumnlist_addrow(CEGUI::MultiColumnList * mc, CEGUI::uint rowId) {
	return mc->addRow(rowId);
}

CEGUI::uint bmx_cegui_multicolumnlist_addrowitem(CEGUI::MultiColumnList * mc, CEGUI::ListboxItem * item, CEGUI::uint colId, CEGUI::uint rowId) {
	return mc->addRow(item, colId, rowId);
}

CEGUI::uint bmx_cegui_multicolumnlist_insertrow(CEGUI::MultiColumnList * mc, CEGUI::uint rowIdx, CEGUI::uint rowId) {
	return mc->insertRow(rowIdx, rowId);
}

CEGUI::uint bmx_cegui_multicolumnlist_insertrowitem(CEGUI::MultiColumnList * mc, CEGUI::ListboxItem * item, CEGUI::uint colId, CEGUI::uint rowIdx, CEGUI::uint rowId) {
	return mc->insertRow(item, colId, rowIdx, rowId);
}

void bmx_cegui_multicolumnlist_removerow(CEGUI::MultiColumnList * mc, CEGUI::uint rowIdx) {
	mc->removeRow(rowIdx);
}

void bmx_cegui_multicolumnlist_setitemforgridref(CEGUI::MultiColumnList * mc, CEGUI::ListboxItem * item, int row, int col) {
	mc->setItem(item, CEGUI::MCLGridRef(row, col));
}

void bmx_cegui_multicolumnlist_setitem(CEGUI::MultiColumnList * mc, CEGUI::ListboxItem * item, CEGUI::uint colId, CEGUI::uint rowIdx) {
	mc->setItem(item, colId, rowIdx);
}

void bmx_cegui_multicolumnlist_setselectionmode(CEGUI::MultiColumnList * mc, CEGUI::MultiColumnList::SelectionMode selMode) {
	mc->setSelectionMode(selMode);
}

void bmx_cegui_multicolumnlist_setnominatedselectioncolumnid(CEGUI::MultiColumnList * mc, CEGUI::uint colId) {
	mc->setNominatedSelectionColumnID(colId);
}

void bmx_cegui_multicolumnlist_setnominatedselectioncolumn(CEGUI::MultiColumnList * mc, CEGUI::uint colIdx) {
	mc->setNominatedSelectionColumn(colIdx);
}

void bmx_cegui_multicolumnlist_setnominatedselectionrow(CEGUI::MultiColumnList * mc, CEGUI::uint rowIdx) {
	mc->setNominatedSelectionRow(rowIdx);
}

void bmx_cegui_multicolumnlist_setsortdirection(CEGUI::MultiColumnList * mc, CEGUI::ListHeaderSegment::SortDirection direction) {
	mc->setSortDirection(direction);
}

void bmx_cegui_multicolumnlist_setsortcolumn(CEGUI::MultiColumnList * mc, CEGUI::uint colIdx) {
	mc->setSortColumn(colIdx);
}

void bmx_cegui_multicolumnlist_setsortcolumnbyid(CEGUI::MultiColumnList * mc, CEGUI::uint colId) {
	mc->setSortColumnByID(colId);
}

void bmx_cegui_multicolumnlist_setshowvertscrollbar(CEGUI::MultiColumnList * mc, int setting) {
	mc->setShowVertScrollbar(setting);
}

void bmx_cegui_multicolumnlist_setshowhorzscrollbar(CEGUI::MultiColumnList * mc, int setting) {
	mc->setShowHorzScrollbar(setting);
}

void bmx_cegui_multicolumnlist_clearallselections(CEGUI::MultiColumnList * mc) {
	mc->clearAllSelections();
}

void bmx_cegui_multicolumnlist_setitemselectstate(CEGUI::MultiColumnList * mc, CEGUI::ListboxItem * item, int state) {
	mc->setItemSelectState(item, state);
}

void bmx_cegui_multicolumnlist_setitemselectstateforgridref(CEGUI::MultiColumnList * mc, int row, int col, int state) {
	mc->setItemSelectState(CEGUI::MCLGridRef(row, col), state);
}

void bmx_cegui_multicolumnlist_handleupdateditemdata(CEGUI::MultiColumnList * mc) {
	mc->handleUpdatedItemData();
}

void bmx_cegui_multicolumnlist_setcolumnheaderwidth(CEGUI::MultiColumnList * mc, CEGUI::uint colIdx, float width) {
	mc->setColumnHeaderWidth(colIdx, guessUDim(width));
}

void bmx_cegui_multicolumnlist_setusersortcontrolenabled(CEGUI::MultiColumnList * mc, int setting) {
	mc->setUserSortControlEnabled(setting);
}

void bmx_cegui_multicolumnlist_setusercolumnsizingenabled(CEGUI::MultiColumnList * mc, int setting) {
	mc->setUserColumnSizingEnabled(setting);
}

void bmx_cegui_multicolumnlist_setusercolumndraggingenabled(CEGUI::MultiColumnList * mc, int setting) {
	mc->setUserColumnDraggingEnabled(setting);
}

void bmx_cegui_multicolumnlist_autosizecolumnheader(CEGUI::MultiColumnList * mc, CEGUI::uint colIdx) {
	mc->autoSizeColumnHeader(colIdx);
}

void bmx_cegui_multicolumnlist_setrowid(CEGUI::MultiColumnList * mc, CEGUI::uint rowIdx, CEGUI::uint rowId) {
	mc->setRowID(rowIdx, rowId);
}


// *************************************************

int bmx_cegui_dragcontainer_isdraggingenabled(CEGUI::DragContainer * dc) {
	return dc->isDraggingEnabled();
}

void bmx_cegui_dragcontainer_setdraggingenabled(CEGUI::DragContainer * dc, int setting) {
	dc->setDraggingEnabled(setting);
}

int bmx_cegui_dragcontainer_isbeingdragged(CEGUI::DragContainer * dc) {
	return dc->isBeingDragged();
}

float bmx_cegui_dragcontainer_getpixeldragthreshold(CEGUI::DragContainer * dc) {
	return dc->getPixelDragThreshold();
}

void bmx_cegui_dragcontainer_setpixeldragthreshold(CEGUI::DragContainer * dc, float pixels) {
	dc->setPixelDragThreshold(pixels);
}

float bmx_cegui_dragcontainer_getdragalpha(CEGUI::DragContainer * dc) {
	return dc->getDragAlpha();
}

void bmx_cegui_dragcontainer_setdragalpha(CEGUI::DragContainer * dc, float alpha) {
	dc->setDragAlpha(alpha);
}

BBObject * bmx_cegui_dragcontainer_getdragcursorimage(CEGUI::DragContainer * dc) {
	return _bah_cegui_TCEImage__create(const_cast<CEGUI::Image*>(dc->getDragCursorImage()));
}

void bmx_cegui_dragcontainer_setdragcursorimage(CEGUI::DragContainer * dc, CEGUI::Image * image) {
	dc->setDragCursorImage(image);
}

void bmx_cegui_dragcontainer_setdragcursorimagemode(CEGUI::DragContainer * dc, CEGUI::MouseCursorImage image) {
	dc->setDragCursorImage(image);
}

void bmx_cegui_dragcontainer_setdragcursorimagebyname(CEGUI::DragContainer * dc, const CEGUI::utf8 * imageset, const CEGUI::utf8 * image) {
	dc->setDragCursorImage(imageset, image);
}

BBObject * bmx_cegui_dragcontainer_getcurrentdroptarget(CEGUI::DragContainer * dc) {
	return newObjectForWindow(dc->getCurrentDropTarget());
}

// *************************************************

MaxCEColour * bmx_cegui_colour_create(float red, float green, float blue, float alpha) {
	return new MaxCEColour(CEGUI::colour(red, green, blue, alpha));
}

void bmx_cegui_colour_delete(MaxCEColour * col) {
	delete col;
}

CEGUI::argb_t bmx_cegui_colour_getargb(MaxCEColour * col) {
	return col->Colour().getARGB();
}

float bmx_cegui_colour_getalpha(MaxCEColour * col) {
	return col->Colour().getAlpha();
}

float bmx_cegui_colour_getred(MaxCEColour * col) {
	return col->Colour().getRed();
}

float bmx_cegui_colour_getgreen(MaxCEColour * col) {
	return col->Colour().getGreen();
}

float bmx_cegui_colour_getblue(MaxCEColour * col) {
	return col->Colour().getBlue();
}

float bmx_cegui_colour_gethue(MaxCEColour * col) {
	return col->Colour().getHue();
}

float bmx_cegui_colour_getsaturation(MaxCEColour * col) {
	return col->Colour().getSaturation();
}

float bmx_cegui_colour_getlumination(MaxCEColour * col) {
	return col->Colour().getLumination();
}

void bmx_cegui_colour_setargb(MaxCEColour * col, CEGUI::argb_t argb) {
	col->Colour().setARGB(argb);
}

void bmx_cegui_colour_setalpha(MaxCEColour * col, float alpha) {
	col->Colour().setAlpha(alpha);
}

void bmx_cegui_colour_setred(MaxCEColour * col, float red) {
	col->Colour().setRed(red);
}

void bmx_cegui_colour_setgreen(MaxCEColour * col, float green) {
	col->Colour().setGreen(green);
}

void bmx_cegui_colour_setblue(MaxCEColour * col, float blue) {
	col->Colour().setBlue(blue);
}

void bmx_cegui_colour_set(MaxCEColour * col, float red, float green, float blue, float alpha) {
	col->Colour().set(red, green, blue, alpha);
}

void bmx_cegui_colour_setrgb(MaxCEColour * col, float red, float green, float blue) {
	col->Colour().setRGB(red, green, blue);
}

void bmx_cegui_colour_sethsl(MaxCEColour * col, float hue, float saturation, float luminance, float alpha) {
	col->Colour().setHSL(hue, saturation, luminance, alpha);
}

void bmx_cegui_colour_inverColour(MaxCEColour * col) {
	col->Colour().invertColour();
}

void bmx_cegui_colour_invertColourWithAlpha(MaxCEColour * col) {
	col->Colour().invertColourWithAlpha();
}

// *************************************************

MaxCEColourRect * bmx_cegui_colourrect_create(MaxCEColour * col) {
	return new MaxCEColourRect(CEGUI::ColourRect(col->Colour()));
}

void bmx_cegui_colourrect_setalpha(MaxCEColourRect * rect, float alpha) {
	rect->Rect().setAlpha(alpha);
}

void bmx_cegui_colourrect_settopalpha(MaxCEColourRect * rect, float alpha) {
	rect->Rect().setTopAlpha(alpha);
}

void bmx_cegui_colourrect_setbottomalpha(MaxCEColourRect * rect, float alpha) {
	rect->Rect().setBottomAlpha(alpha);
}

void bmx_cegui_colourrect_setleftalpha(MaxCEColourRect * rect, float alpha) {
	rect->Rect().setLeftAlpha(alpha);
}

void bmx_cegui_colourrect_setrightalpha(MaxCEColourRect * rect, float alpha) {
	rect->Rect().setRightAlpha(alpha);
}

int bmx_cegui_colourrect_ismonochromatic(MaxCEColourRect * rect) {
	return rect->Rect().isMonochromatic();
}

MaxCEColourRect * bmx_cegui_colourrect_getsubrectangle(MaxCEColourRect * rect, float left, float right, float top, float bottom) {
	return new MaxCEColourRect(rect->Rect().getSubRectangle(left, right, top, bottom));
}

MaxCEColour * bmx_cegui_colourrect_getcolouratpoint(MaxCEColourRect * rect, float x, float y) {
	return new MaxCEColour(rect->Rect().getColourAtPoint(x, y));
}

void bmx_cegui_colourrect_setcolours(MaxCEColourRect * rect, MaxCEColour * col) {
	rect->Rect().setColours(col->Colour());
}

void bmx_cegui_colourrect_modulatealpha(MaxCEColourRect * rect, float alpha) {
	rect->Rect().modulateAlpha(alpha);
}

void bmx_cegui_colourrect_settopleft(MaxCEColourRect * rect, MaxCEColour * col) {
	rect->Rect().d_top_left = col->Colour();
}

void bmx_cegui_colourrect_settopright(MaxCEColourRect * rect, MaxCEColour * col) {
	rect->Rect().d_top_right = col->Colour();
}

void bmx_cegui_colourrect_setbottomleft(MaxCEColourRect * rect, MaxCEColour * col) {
	rect->Rect().d_bottom_left = col->Colour();
}

void bmx_cegui_colourrect_setbottomright(MaxCEColourRect * rect, MaxCEColour * col) {
	rect->Rect().d_bottom_right = col->Colour();
}

MaxCEColour * bmx_cegui_colourrect_gettopleft(MaxCEColourRect * rect) {
	return new MaxCEColour(rect->Rect().d_top_left);
}

MaxCEColour * bmx_cegui_colourrect_gettopright(MaxCEColourRect * rect) {
	return new MaxCEColour(rect->Rect().d_top_right);
}

MaxCEColour * bmx_cegui_colourrect_getbottomleft(MaxCEColourRect * rect) {
	return new MaxCEColour(rect->Rect().d_bottom_left);
}

MaxCEColour * bmx_cegui_colourrect_getbottomright(MaxCEColourRect * rect) {
	return new MaxCEColour(rect->Rect().d_bottom_right);
}

void bmx_cegui_colourrect_delete(MaxCEColourRect * rect) {
	delete rect;
}

// *************************************************


float bmx_cegui_menubase_getitemspacing(CEGUI::MenuBase * base) {
	return base->getItemSpacing();
}

int bmx_cegui_menubase_ismultiplepopupsallowed(CEGUI::MenuBase * base) {
	return base->isMultiplePopupsAllowed();
}

BBObject * bmx_cegui_menubase_getpopupmenuitem(CEGUI::MenuBase * base) {
	return _bah_cegui_TCEMenuItem__create(base->getPopupMenuItem());
}

void bmx_cegui_menubase_setitemspacing(CEGUI::MenuBase * base, float spacing) {
	base->setItemSpacing(spacing);
}

void bmx_cegui_menubase_changepopupmenuitem(CEGUI::MenuBase * base, CEGUI::MenuItem * item) {
	base->changePopupMenuItem(item);
}

void bmx_cegui_menubase_setallowmultiplepopups(CEGUI::MenuBase * base, int setting) {
	base->setAllowMultiplePopups(setting);
}

// *************************************************


float bmx_cegui_popupmenu_getfadeintime(CEGUI::PopupMenu * menu) {
	return menu->getFadeInTime();
}

float bmx_cegui_popupmenu_getfadeouttime(CEGUI::PopupMenu * menu) {
	return menu->getFadeOutTime();
}

int bmx_cegui_popupmenu_ispopupmenuopen(CEGUI::PopupMenu * menu) {
	return menu->isPopupMenuOpen();
}

void bmx_cegui_popupmenu_setfadeintime(CEGUI::PopupMenu * menu, float fadetime) {
	menu->setFadeInTime(fadetime);
}

void bmx_cegui_popupmenu_setfadeouttime(CEGUI::PopupMenu * menu, float fadetime) {
	menu->setFadeOutTime(fadetime);
}

void bmx_cegui_popupmenu_openpopupmenu(CEGUI::PopupMenu * menu, int _notify) {
	menu->openPopupMenu(_notify);
}

void bmx_cegui_popupmenu_closepopupmenu(CEGUI::PopupMenu * menu, int _notify) {
	menu->closePopupMenu(_notify);
}

// *************************************************

void bmx_cegui_itementry_getitempixelsize(CEGUI::ItemEntry * entry, float * w, float * h) {
	CEGUI::Size s(entry->getItemPixelSize());
	*w = s.d_width;
	*h = s.d_height;
}

BBObject * bmx_cegui_itementry_getownerlist(CEGUI::ItemEntry * entry) {
	return newObjectForWindow(entry->getOwnerList());
}

int bmx_cegui_itementry_isselected(CEGUI::ItemEntry * entry) {
	return entry->isSelected();
}

int bmx_cegui_itementry_isselectable(CEGUI::ItemEntry * entry) {
	return entry->isSelectable();
}

void bmx_cegui_itementry_setselected(CEGUI::ItemEntry * entry, int setting) {
	entry->setSelected(setting);
}

void bmx_cegui_itementry_selectentry(CEGUI::ItemEntry * entry) {
	entry->select();
}

void bmx_cegui_itementry_deselect(CEGUI::ItemEntry * entry) {
	entry->deselect();
}

void bmx_cegui_itementry_setselectable(CEGUI::ItemEntry * entry, int setting) {
	entry->setSelectable(setting);
}

// *************************************************


int bmx_cegui_menuitem_ishovering(CEGUI::MenuItem * item) {
	return item->isHovering();
}

int bmx_cegui_menuitem_ispushed(CEGUI::MenuItem * item) {
	return item->isPushed();
}

int bmx_cegui_menuitem_isopened(CEGUI::MenuItem * item) {
	return item->isOpened();
}

BBObject * bmx_cegui_menuitem_getpopupmenu(CEGUI::MenuItem * item) {
	return newObjectForWindow(item->getPopupMenu());
}

void bmx_cegui_menuitem_setpopupmenu(CEGUI::MenuItem * item, CEGUI::PopupMenu * popup) {
	item->setPopupMenu(popup);
}

void bmx_cegui_menuitem_openpopupmenu(CEGUI::MenuItem * item, int _notify) {
	item->openPopupMenu(_notify);
}

void bmx_cegui_menuitem_closepopupmenu(CEGUI::MenuItem * item, int _notify) {
	item->closePopupMenu(_notify);
}

int bmx_cegui_menuitem_togglepopupmenu(CEGUI::MenuItem * item) {
	return item->togglePopupMenu();
}

// *************************************************

void bmx_cegui_treeitem_settextcolours(CEGUI::TreeItem * ti, MaxCEColourRect * cols) {
	ti->setTextColours(cols->Rect());
}

void bmx_cegui_treeitem_settextcoloursforcorners(CEGUI::TreeItem * ti, MaxCEColour * topLeftColour, MaxCEColour * topRightColour, MaxCEColour * bottomLeftColour, MaxCEColour * bottomRightColour) {
	ti->setTextColours(topLeftColour->Colour(), topRightColour->Colour(), bottomLeftColour->Colour(), bottomRightColour->Colour());
}

void bmx_cegui_treeitem_settextcoloursforcolour(CEGUI::TreeItem * ti, MaxCEColour * col) {
	ti->setTextColours(col->Colour());
}

BBString * bmx_cegui_treeitem_gettext(CEGUI::TreeItem * ti) {
	return bah_cegui__convertUTF8ToMax(ti->getText().data());
}

BBString * bmx_cegui_treeitem_gettooltiptext(CEGUI::TreeItem * ti) {
	return bah_cegui__convertUTF8ToMax(ti->getTooltipText().data());
}

CEGUI::uint bmx_cegui_treeitem_getid(CEGUI::TreeItem * ti) {
	return ti->getID();
}

BBObject * bmx_cegui_treeitem_getuserdata(CEGUI::TreeItem * ti) {
	void * data = ti->getUserData();
	if (data) {
		return (BBObject*)data;
	} else {
		return &bbNullObject;
	}
}

int bmx_cegui_treeitem_isselected(CEGUI::TreeItem * ti) {
	return ti->isSelected();
}

int bmx_cegui_treeitem_isdisabled(CEGUI::TreeItem * ti) {
	return ti->isDisabled();
}

int bmx_cegui_treeitem_isautodeleted(CEGUI::TreeItem * ti) {
	return ti->isAutoDeleted();
}

BBObject * bmx_cegui_treeitem_getownerwindow(CEGUI::TreeItem * ti) {
	return newObjectForWindow(const_cast<CEGUI::Window *>(ti->getOwnerWindow()));
}

MaxCEColourRect * bmx_cegui_treeitem_getselectioncolours(CEGUI::TreeItem * ti) {
	return new MaxCEColourRect(ti->getSelectionColours());
}

BBObject * bmx_cegui_treeitem_getselectionbrushimage(CEGUI::TreeItem * ti) {
	return _bah_cegui_TCEImage__create(const_cast<CEGUI::Image *>(ti->getSelectionBrushImage()));
}

void bmx_cegui_treeitem_settext(CEGUI::TreeItem * ti, const CEGUI::utf8 * text) {
	ti->setText(text);
}

void bmx_cegui_treeitem_settooltiptext(CEGUI::TreeItem * ti, const CEGUI::utf8 * text) {
	ti->setTooltipText(text);
}

void bmx_cegui_treeitem_setid(CEGUI::TreeItem * ti, CEGUI::uint itemId) {
	ti->setID(itemId);
}

void bmx_cegui_treeitem_setuserdata(CEGUI::TreeItem * ti, BBObject * itemData) {
	ti->setUserData(itemData);
}

void bmx_cegui_treeitem_setselected(CEGUI::TreeItem * ti, int setting) {
	ti->setSelected(setting);
}

void bmx_cegui_treeitem_setdisabled(CEGUI::TreeItem * ti, int setting) {
	ti->setDisabled(setting);
}

void bmx_cegui_treeitem_setautodeleted(CEGUI::TreeItem * ti, int setting) {
	ti->setAutoDeleted(setting);
}

void bmx_cegui_treeitem_setownerwindow(CEGUI::TreeItem * ti, CEGUI::Window * owner) {
	ti->setOwnerWindow(owner);
}

void bmx_cegui_treeitem_setselectioncolours(CEGUI::TreeItem * ti, MaxCEColourRect * cols) {
	ti->setSelectionColours(cols->Rect());
}

void bmx_cegui_treeitem_setselectioncoloursforcorners(CEGUI::TreeItem * ti, MaxCEColour * topLeftColour, MaxCEColour * topRightColour, MaxCEColour * bottomLeftColour, MaxCEColour * bottomRightColour) {
	ti->setSelectionColours(topLeftColour->Colour(), topRightColour->Colour(), bottomLeftColour->Colour(), bottomRightColour->Colour());
}

void bmx_cegui_treeitem_setselectioncoloursforcolour(CEGUI::TreeItem * ti, MaxCEColour * col) {
	ti->setSelectionColours(col->Colour());
}

void bmx_cegui_treeitem_setselectionbrushimage(CEGUI::TreeItem * ti, CEGUI::Image * image) {
	ti->setSelectionBrushImage(image);
}

void bmx_cegui_treeitem_setselectionbrushimagebyname(CEGUI::TreeItem * ti, const CEGUI::utf8 * imageset, const CEGUI::utf8 * image) {
	ti->setSelectionBrushImage(imageset, image);
}

void bmx_cegui_treeitem_setbuttonlocation(CEGUI::TreeItem * ti, float x, float y, float w, float h) {
	CEGUI::Rect r(x, y, w, h);
	ti->setButtonLocation(r);
}

void bmx_cegui_treeitem_getbuttonlocation(CEGUI::TreeItem * ti, float * x, float * y, float * w, float * h) {
	CEGUI::Rect r = ti->getButtonLocation();
	*x = r.d_left;
	*y = r.d_top;
	*w = r.getWidth();
	*h = r.getHeight();
}

int bmx_cegui_treeitem_getisopen(CEGUI::TreeItem * ti) {
	return ti->getIsOpen();
}

void bmx_cegui_treeitem_toggleisopen(CEGUI::TreeItem * ti) {
	ti->toggleIsOpen();
}

BBObject * bmx_cegui_treeitem_gettreeitemfromindex(CEGUI::TreeItem * ti, int itemIndex) {
	return _bah_cegui_TCETreeItem__create(ti->getTreeItemFromIndex(itemIndex));
}

int bmx_cegui_treeitem_getitemcount(CEGUI::TreeItem * ti) {
	return ti->getItemCount();
}

void bmx_cegui_treeitem_additem(CEGUI::TreeItem * ti, CEGUI::TreeItem * item) {
	ti->addItem(item);
}

void bmx_cegui_treeitem_seticon(CEGUI::TreeItem * ti, CEGUI::Image * theIcon) {
	ti->setIcon(*theIcon);
}

void bmx_cegui_treeitem_getpixelsize(CEGUI::TreeItem * ti, float * width, float * height) {
	CEGUI::Size s = ti->getPixelSize();
	*width = s.d_width;
	*height = s.d_height;
}

// *************************************************

CEGUI::Font * bmx_cegui_listboxtextitem_getfont(CEGUI::ListboxTextItem * item) {
	return item->getFont();
}

MaxCEColourRect * bmx_cegui_listboxtextitem_gettextcolours(CEGUI::ListboxTextItem * item) {
	CEGUI::ColourRect r = item->getTextColours();
	return new MaxCEColourRect(r);
}

void bmx_cegui_listboxtextitem_setfont(CEGUI::ListboxTextItem * item, CEGUI::Font * font) {
	item->setFont(font);
}

void bmx_cegui_listboxtextitem_setfontbyname(CEGUI::ListboxTextItem * item, const CEGUI::utf8 * fontName) {
	item->setFont(fontName);
}

void bmx_cegui_listboxtextitem_settextcolours(CEGUI::ListboxTextItem * item, MaxCEColourRect * cols) {
	item->setTextColours(cols->Rect());
}

void bmx_cegui_listboxtextitem_settextcoloursforcorner(CEGUI::ListboxTextItem * item, MaxCEColour * topLeftColour, MaxCEColour * topRightColour, MaxCEColour * bottomLeftColour, MaxCEColour * bottomRightColour) {
	item->setTextColours(topLeftColour->Colour(), topRightColour->Colour(), bottomLeftColour->Colour(), bottomRightColour->Colour());
}

void bmx_cegui_listboxtextitem_settextcoloursforcolour(CEGUI::ListboxTextItem * item, MaxCEColour * col) {
	item->setTextColours(col->Colour());
}

void bmx_cegui_listboxtextitem_getpixelsize(CEGUI::ListboxTextItem * item, float * width, float * height) {
	CEGUI::Size s = item->getPixelSize();
	*width = s.d_width;
	*height = s.d_height;
}

CEGUI::ListboxTextItem * bmx_cegui_listboxtextitem_new(const CEGUI::utf8 * text, CEGUI::uint itemId, int disabled, int autoDelete) {
	return new CEGUI::ListboxTextItem(text, itemId, 0, disabled, autoDelete);
}

void bmx_cegui_listboxtextitem_delete(CEGUI::ListboxTextItem * item) {
	delete item;
}

// *************************************************

BBObject * bmx_cegui_scrollablepane_getcontentpane(CEGUI::ScrollablePane * sp) {
	return _bah_cegui_TCEScrolledContainer__create(const_cast<CEGUI::ScrolledContainer *>(sp->getContentPane()));
}

int bmx_cegui_scrollablepane_isvertscrollbaralwaysshown(CEGUI::ScrollablePane * sp) {
	return sp->isVertScrollbarAlwaysShown();
}

void bmx_cegui_scrollablepane_setshowvertscrollbar(CEGUI::ScrollablePane * sp, int setting) {
	sp->setShowVertScrollbar(setting);
}

int bmx_cegui_scrollablepane_ishorzscrollbaralwaysshown(CEGUI::ScrollablePane * sp) {
	return sp->isHorzScrollbarAlwaysShown();
}

void bmx_cegui_scrollablepane_setshowhorzscrollbar(CEGUI::ScrollablePane * sp, int setting) {
	sp->setShowHorzScrollbar(setting);
}

int bmx_cegui_scrollablepane_iscontentpaneautosized(CEGUI::ScrollablePane * sp) {
	return sp->isContentPaneAutoSized();
}

void bmx_cegui_scrollablepane_setcontentpaneautosized(CEGUI::ScrollablePane * sp, int setting) {
	sp->setContentPaneAutoSized(setting);
}

void bmx_cegui_scrollablepane_getcontentpanearea(CEGUI::ScrollablePane * sp, float * x, float * y, float * w, float * h) {
	CEGUI::Rect r = sp->getContentPaneArea();
	*x = r.d_left;
	*y = r.d_top;
	*w = r.getWidth();
	*h = r.getHeight();
}

void bmx_cegui_scrollablepane_setcontentpanearea(CEGUI::ScrollablePane * sp, float x, float y, float w, float h) {
	CEGUI::Rect r(x, y, w, h);
	sp->setContentPaneArea(r);
}

float bmx_cegui_scrollablepane_gethorizontalstepsize(CEGUI::ScrollablePane * sp) {
	return sp->getHorizontalStepSize();
}

void bmx_cegui_scrollablepane_sethorizontalstepsize(CEGUI::ScrollablePane * sp, float stepSize) {
	sp->setHorizontalStepSize(stepSize);
}

float bmx_cegui_scrollablepane_gethorizontaloverlapsize(CEGUI::ScrollablePane * sp) {
	return sp->getHorizontalOverlapSize();
}

void bmx_cegui_scrollablepane_sethorizontaloverlapsize(CEGUI::ScrollablePane * sp, float overlap) {
	sp->setHorizontalOverlapSize(overlap);
}

float bmx_cegui_scrollablepane_gethorizontalscrollposition(CEGUI::ScrollablePane * sp) {
	return sp->getHorizontalScrollPosition();
}

void bmx_cegui_scrollablepane_sethorizontalscrollposition(CEGUI::ScrollablePane * sp, float position) {
	sp->setHorizontalScrollPosition(position);
}

float bmx_cegui_scrollablepane_getverticalstepsize(CEGUI::ScrollablePane * sp) {
	return sp->getVerticalStepSize();
}

void bmx_cegui_scrollablepane_setverticalstepsize(CEGUI::ScrollablePane * sp, float stepSize) {
	sp->setVerticalStepSize(stepSize);
}

float bmx_cegui_scrollablepane_getverticaloverlapsize(CEGUI::ScrollablePane * sp) {
	return sp->getVerticalOverlapSize();
}

void bmx_cegui_scrollablepane_setverticaloverlapsize(CEGUI::ScrollablePane * sp, float overlap) {
	sp->setVerticalOverlapSize(overlap);
}

float bmx_cegui_scrollablepane_getverticalscrollposition(CEGUI::ScrollablePane * sp) {
	return sp->getVerticalScrollPosition();
}

void bmx_cegui_scrollablepane_setverticalscrollposition(CEGUI::ScrollablePane * sp, float position) {
	sp->setVerticalScrollPosition(position);
}

void bmx_cegui_scrollablepane_getviewablearea(CEGUI::ScrollablePane * sp, float * x, float * y, float * w, float * h) {
	CEGUI::Rect r = sp->getViewableArea();
	*x = r.d_left;
	*y = r.d_top;
	*w = r.getWidth();
	*h = r.getHeight();
}

BBObject * bmx_cegui_scrollablepane_getvertscrollbar(CEGUI::ScrollablePane * sp) {
	return _bah_cegui_TCEScrollbar__create(sp->getVertScrollbar());
}

BBObject * bmx_cegui_scrollablepane_gethorzscrollbar(CEGUI::ScrollablePane * sp) {
	return _bah_cegui_TCEScrollbar__create(sp->getHorzScrollbar());
}


// *************************************************


int bmx_cegui_tree_getitemcount(CEGUI::Tree * tree) {
	return tree->getItemCount();
}

int bmx_cegui_tree_getselectedcount(CEGUI::Tree * tree) {
	return tree->getSelectedCount();
}

BBObject * bmx_cegui_tree_getfirstselecteditem(CEGUI::Tree * tree) {
	return _bah_cegui_TCETreeItem__create(tree->getFirstSelectedItem());
}

BBObject * bmx_cegui_tree_getlastselecteditem(CEGUI::Tree * tree) {
	return _bah_cegui_TCETreeItem__create(tree->getLastSelectedItem());
}

BBObject * bmx_cegui_tree_getnextselected(CEGUI::Tree * tree, CEGUI::TreeItem * startItem) {
	return _bah_cegui_TCETreeItem__create(tree->getNextSelected(startItem));
}

int bmx_cegui_tree_issortenabled(CEGUI::Tree * tree) {
	return tree->isSortEnabled();
}

void bmx_cegui_tree_setitemrenderarea(CEGUI::Tree * tree, float x, float y, float w, float h) {
	CEGUI::Rect r(x, y, w, h);
	tree->setItemRenderArea(r);
}

BBObject * bmx_cegui_tree_getvertscrollbar(CEGUI::Tree * tree) {
	return _bah_cegui_TCEScrollbar__create(tree->getVertScrollbar());
}

BBObject * bmx_cegui_tree_gethorzscrollbar(CEGUI::Tree * tree) {
	return _bah_cegui_TCEScrollbar__create(tree->getHorzScrollbar());
}

int bmx_cegui_tree_ismultiselectenabled(CEGUI::Tree * tree) {
	return tree->isMultiselectEnabled();
}

int bmx_cegui_tree_isitemtooltipsenabled(CEGUI::Tree * tree) {
	return tree->isItemTooltipsEnabled();
}

BBObject * bmx_cegui_tree_findfirstitemwithtext(CEGUI::Tree * tree, const CEGUI::utf8 * text) {
	return _bah_cegui_TCETreeItem__create(tree->findFirstItemWithText(text));
}

BBObject * bmx_cegui_tree_findnextitemwithtext(CEGUI::Tree * tree, const CEGUI::utf8 * text, CEGUI::TreeItem * startItem) {
	return _bah_cegui_TCETreeItem__create(tree->findNextItemWithText(text, startItem));
}

BBObject * bmx_cegui_tree_findfirstitemwithid(CEGUI::Tree * tree, CEGUI::uint searchID) {
	return _bah_cegui_TCETreeItem__create(tree->findFirstItemWithID(searchID));
}

BBObject * bmx_cegui_tree_findnextitemwithid(CEGUI::Tree * tree, CEGUI::uint searchID, CEGUI::TreeItem * startItem) {
	return _bah_cegui_TCETreeItem__create(tree->findNextItemWithID(searchID, startItem));
}

int bmx_cegui_tree_istreeiteminlist(CEGUI::Tree * tree, CEGUI::TreeItem * item) {
	return tree->isTreeItemInList(item);
}

int bmx_cegui_tree_isvertscrollbaralwaysshown(CEGUI::Tree * tree) {
	return tree->isVertScrollbarAlwaysShown();
}

int bmx_cegui_tree_ishorzscrollbaralwaysshown(CEGUI::Tree * tree) {
	return tree->isHorzScrollbarAlwaysShown();
}

void bmx_cegui_tree_resetlist(CEGUI::Tree * tree) {
	tree->resetList();
}

void bmx_cegui_tree_additem(CEGUI::Tree * tree, CEGUI::TreeItem * item) {
	tree->addItem(item);
}

void bmx_cegui_tree_insertitem(CEGUI::Tree * tree, CEGUI::TreeItem * item, CEGUI::TreeItem * position) {
	tree->insertItem(item, position);
}

void bmx_cegui_tree_removeitem(CEGUI::Tree * tree, CEGUI::TreeItem * item) {
	tree->removeItem(item);
}

void bmx_cegui_tree_clearallselections(CEGUI::Tree * tree) {
	tree->clearAllSelections();
}

void bmx_cegui_tree_setsortingenabled(CEGUI::Tree * tree, int setting) {
	tree->setSortingEnabled(setting);
}

void bmx_cegui_tree_setmultiselectenabled(CEGUI::Tree * tree, int setting) {
	tree->setMultiselectEnabled(setting);
}

void bmx_cegui_tree_setshowvertscrollbar(CEGUI::Tree * tree, int setting) {
	tree->setShowVertScrollbar(setting);
}

void bmx_cegui_tree_setshowhorzscrollbar(CEGUI::Tree * tree, int setting) {
	tree->setShowHorzScrollbar(setting);
}

void bmx_cegui_tree_setitemtooltipsenabled(CEGUI::Tree * tree, int setting) {
	tree->setItemTooltipsEnabled(setting);
}

void bmx_cegui_tree_setitemselectstate(CEGUI::Tree * tree, CEGUI::TreeItem * item, int state) {
	tree->setItemSelectState(item, state);
}

void bmx_cegui_tree_setitemselectstateindex(CEGUI::Tree * tree, int itemIndex, int state) {
	tree->setItemSelectState(itemIndex, state);
}

void bmx_cegui_tree_setlooknfeel(CEGUI::Tree * tree, const CEGUI::utf8 * look) {
	tree->setLookNFeel(look);
}

void bmx_cegui_tree_handleupdateditemdata(CEGUI::Tree * tree) {
	tree->handleUpdatedItemData();
}

void bmx_cegui_tree_ensureitemisvisible(CEGUI::Tree * tree, CEGUI::TreeItem * item) {
	tree->ensureItemIsVisible(item);
}

// *************************************************

void bmx_cegui_tabbutton_setselected(CEGUI::TabButton * button, int selected) {
	button->setSelected(selected);
}

int bmx_cegui_tabbutton_isselected(CEGUI::TabButton * button) {
	return button->isSelected();
}

void bmx_cegui_tabbutton_settargetwindow(CEGUI::TabButton * button, CEGUI::Window * wnd) {
	button->setTargetWindow(wnd);
}

BBObject * bmx_cegui_tabbutton_gettargetwindow(CEGUI::TabButton * button) {
	return newObjectForWindow(button->getTargetWindow());
}

// *************************************************

MaxResourceProvider * bmx_cegui_resourceprovider_create(BBObject * handle) {
	return new MaxResourceProvider(handle);
}

void bmx_cegui_resourceprovider_delete(MaxResourceProvider * provider) {
	delete provider;
}

// *************************************************

void bmx_cegui_rawdatacontainer_delete(MaxRawDataContainer * container) {
	delete container;
}

// *************************************************

void bmx_cegui_rawdatacontainer_setdata(MaxRawDataContainer * container, CEGUI::uint8 * data) {
	container->Container().setData(data);
}

CEGUI::uint8 * bmx_cegui_rawdatacontainer_getdataptr(MaxRawDataContainer * container) {
	return container->Container().getDataPtr();
}

void bmx_cegui_rawdatacontainer_setsize(MaxRawDataContainer * container, int size) {
	container->Container().setSize(size);
}

int bmx_cegui_rawdatacontainer_getsize(MaxRawDataContainer * container) {
	return container->Container().getSize();
}

// *************************************************

int bmx_cegui_listbox_getitemcount(CEGUI::Listbox * lb) {
	return lb->getItemCount();
}

int bmx_cegui_listbox_getselectedcount(CEGUI::Listbox * lb) {
	return lb->getSelectedCount();
}

BBObject * bmx_cegui_listbox_getfirstselecteditem(CEGUI::Listbox * lb) {
	return _bah_cegui_TCEListboxItem__create(lb->getFirstSelectedItem());
}

BBObject * bmx_cegui_listbox_getnextselected(CEGUI::Listbox * lb, CEGUI::ListboxItem * startItem) {
	return _bah_cegui_TCEListboxItem__create(lb->getNextSelected(startItem));
}

BBObject * bmx_cegui_listbox_getlistboxitemfromindex(CEGUI::Listbox * lb, int index) {
	return _bah_cegui_TCEListboxItem__create(lb->getListboxItemFromIndex(index));
}

int bmx_cegui_listbox_getitemindex(CEGUI::Listbox * lb, CEGUI::ListboxItem * item) {
	return lb->getItemIndex(item);
}

int bmx_cegui_listbox_issortenabled(CEGUI::Listbox * lb) {
	return lb->isSortEnabled();
}

int bmx_cegui_listbox_ismultiselectenabled(CEGUI::Listbox * lb) {
	return lb->isMultiselectEnabled();
}

int bmx_cegui_listbox_isitemtooltipsenabled(CEGUI::Listbox * lb) {
	return lb->isItemTooltipsEnabled();
}

int bmx_cegui_listbox_isitemselected(CEGUI::Listbox * lb, int index) {
	return lb->isItemSelected(index);
}

BBObject * bmx_cegui_listbox_finditemwithtext(CEGUI::Listbox * lb, const CEGUI::utf8 * text, CEGUI::ListboxItem * startItem) {
	return _bah_cegui_TCEListboxItem__create(lb->findItemWithText(text, startItem));
}

int bmx_cegui_listbox_islistboxiteminlist(CEGUI::Listbox * lb, CEGUI::ListboxItem * item) {
	return lb->isListboxItemInList(item);
}

int bmx_cegui_listbox_isvertscrollbaralwaysshown(CEGUI::Listbox * lb) {
	return lb->isVertScrollbarAlwaysShown();
}

int bmx_cegui_listbox_ishorzscrollbaralwaysshown(CEGUI::Listbox * lb) {
	return lb->isHorzScrollbarAlwaysShown();
}

void bmx_cegui_listbox_resetlist(CEGUI::Listbox * lb) {
	lb->resetList();
}

void bmx_cegui_listbox_additem(CEGUI::Listbox * lb, CEGUI::ListboxItem * item) {
	lb->addItem(item);
}

void bmx_cegui_listbox_insertitem(CEGUI::Listbox * lb, CEGUI::ListboxItem * item, CEGUI::ListboxItem * position) {
	lb->insertItem(item, position);
}

void bmx_cegui_listbox_removeitem(CEGUI::Listbox * lb, CEGUI::ListboxItem * item) {
	lb->removeItem(item);
}

void bmx_cegui_listbox_clearallselections(CEGUI::Listbox * lb) {
	lb->clearAllSelections();
}

void bmx_cegui_listbox_setsortingenabled(CEGUI::Listbox * lb, int setting) {
	lb->setSortingEnabled(setting);
}

void bmx_cegui_listbox_setmultiselectenabled(CEGUI::Listbox * lb, int setting) {
	lb->setMultiselectEnabled(setting);
}

void bmx_cegui_listbox_setshowvertscrollbar(CEGUI::Listbox * lb, int setting) {
	lb->setShowVertScrollbar(setting);
}

void bmx_cegui_listbox_setshowhorzscrollbar(CEGUI::Listbox * lb, int setting) {
	lb->setShowHorzScrollbar(setting);
}

void bmx_cegui_listbox_setitemtooltipsenabled(CEGUI::Listbox * lb, int setting) {
	lb->setItemTooltipsEnabled(setting);
}

void bmx_cegui_listbox_setitemselectstate(CEGUI::Listbox * lb, CEGUI::ListboxItem * item, int state) {
	lb->setItemSelectState(item, state);
}

void bmx_cegui_listbox_setitemselectstateforindex(CEGUI::Listbox * lb, int itemIndex, int state) {
	lb->setItemSelectState(itemIndex, state);
}

void bmx_cegui_listbox_handleupdateditemdata(CEGUI::Listbox * lb) {
	lb->handleUpdatedItemData();
}

void bmx_cegui_listbox_ensureitemisvisibleforindex(CEGUI::Listbox * lb, int itemIndex) {
	lb->ensureItemIsVisible(itemIndex);
}

void bmx_cegui_listbox_ensureitemisvisible(CEGUI::Listbox * lb, CEGUI::ListboxItem * item) {
	lb->ensureItemIsVisible(item);
}

void bmx_cegui_listbox_getlistrenderarea(CEGUI::Listbox * lb, float * x, float * y, float * w, float * h) {
	CEGUI::Rect r(lb->getListRenderArea());
	*x = r.d_left;
	*y = r.d_top;
	*w = r.getWidth();
	*h = r.getHeight();
}

BBObject * bmx_cegui_listbox_getvertscrollbar(CEGUI::Listbox * lb) {
	return _bah_cegui_TCEScrollbar__create(lb->getVertScrollbar());
}

BBObject * bmx_cegui_listbox_gethorzscrollbar(CEGUI::Listbox * lb) {
	return _bah_cegui_TCEScrollbar__create(lb->getHorzScrollbar());
}

float bmx_cegui_listbox_gettotalitemsheight(CEGUI::Listbox * lb) {
	return lb->getTotalItemsHeight();
}

float bmx_cegui_listbox_getwidestitemwidth(CEGUI::Listbox * lb) {
	return lb->getWidestItemWidth();
}

// *************************************************

int bmx_cegui_itemlistbase_getitemcount(CEGUI::ItemListBase * lb) {
	return lb->getItemCount();
}

BBObject * bmx_cegui_itemlistbase_getitemfromindex(CEGUI::ItemListBase * lb, int index) {
	return _bah_cegui_TCEItemEntry__create(lb->getItemFromIndex(index));
}

int bmx_cegui_itemlistbase_getitemindex(CEGUI::ItemListBase * lb, CEGUI::ItemEntry * item) {
	return lb->getItemIndex(item);
}

BBObject * bmx_cegui_itemlistbase_finditemwithtext(CEGUI::ItemListBase * lb, CEGUI::uint8 * text, CEGUI::ItemEntry * startItem) {
	return _bah_cegui_TCEItemEntry__create(lb->findItemWithText(text, startItem));
}

int bmx_cegui_itemlistbase_isiteminlist(CEGUI::ItemListBase * lb, CEGUI::ItemEntry * item) {
	return lb->isItemInList(item);
}

int bmx_cegui_itemlistbase_isautoresizeenabled(CEGUI::ItemListBase * lb) {
	return lb->isAutoResizeEnabled();
}

int bmx_cegui_itemlistbase_issortenabled(CEGUI::ItemListBase * lb) {
	return lb->isSortEnabled();
}

CEGUI::ItemListBase::SortMode bmx_cegui_itemlistbase_getsortmode(CEGUI::ItemListBase * lb) {
	return lb->getSortMode();
}

void bmx_cegui_itemlistbase_resetlist(CEGUI::ItemListBase * lb) {
	lb->resetList();
}

void bmx_cegui_itemlistbase_additem(CEGUI::ItemListBase * lb, CEGUI::ItemEntry * item) {
	lb->addItem(item);
}

void bmx_cegui_itemlistbase_insertitem(CEGUI::ItemListBase * lb, CEGUI::ItemEntry * item, CEGUI::ItemEntry * position) {
	lb->insertItem(item, position);
}

void bmx_cegui_itemlistbase_removeitem(CEGUI::ItemListBase * lb, CEGUI::ItemEntry * item) {
	lb->removeItem(item);
}

void bmx_cegui_itemlistbase_handleupdateditemdata(CEGUI::ItemListBase * lb, int resort) {
	lb->handleUpdatedItemData(resort);
}

void bmx_cegui_itemlistbase_setautoresizeenabled(CEGUI::ItemListBase * lb, int setting) {
	lb->setAutoResizeEnabled(setting);
}

void bmx_cegui_itemlistbase_sizetocontent(CEGUI::ItemListBase * lb) {
	lb->sizeToContent();
}

void bmx_cegui_itemlistbase_endinitialisation(CEGUI::ItemListBase * lb) {
	lb->endInitialisation();
}

void bmx_cegui_itemlistbase_performchildwindowlayout(CEGUI::ItemListBase * lb) {
	lb->performChildWindowLayout();
}

void bmx_cegui_itemlistbase_getitemrenderarea(CEGUI::ItemListBase * lb, float * x, float * y, float * w, float * h) {
	CEGUI::Rect r(lb->getItemRenderArea());

}

BBObject * bmx_cegui_itemlistbase_getcontentpane(CEGUI::ItemListBase * lb) {
	return newObjectForWindow(lb->getContentPane());
}

void bmx_cegui_itemlistbase_setsortenabled(CEGUI::ItemListBase * lb, int setting) {
	lb->setSortEnabled(setting);
}

void bmx_cegui_itemlistbase_setsortmode(CEGUI::ItemListBase * lb, CEGUI::ItemListBase::SortMode mode) {
	lb->setSortMode(mode);
}

void bmx_cegui_itemlistbase_sortlist(CEGUI::ItemListBase * lb, int relayout) {
	lb->sortList(relayout);
}


// *************************************************

CEGUI::GlobalEventSet * bmx_cegui_globaleventset_getinstance() {
	return &CEGUI::GlobalEventSet::getSingleton();
}

MaxConnection * bmx_cegui_globaleventset_subscribeevent(CEGUI::GlobalEventSet * handle, const CEGUI::utf8 * name, MaxCEEventCallback * cb) {
	return cb->subscribeEvent(handle, name);
}

// *************************************************

MaxWindowFactory * bmx_cegui_windowfactory_create(BBObject * handle, const CEGUI::utf8 * type) {
	return new MaxWindowFactory(handle, type);
}

int bmx_cegui_scrolledcontainer_iscontentpaneautosized(CEGUI::ScrolledContainer * sc) {
	return sc->isContentPaneAutoSized();
}

void bmx_cegui_scrolledcontainer_setcontentpaneautosized(CEGUI::ScrolledContainer * sc, int setting) {
	sc->setContentPaneAutoSized(setting);
}

void bmx_cegui_scrolledcontainer_getcontentarea(CEGUI::ScrolledContainer * sc, float * x, float * y, float * w, float * h) {
	CEGUI::Rect r = sc->getContentArea();
	*x = r.d_left;
	*y = r.d_top;
	*w = r.getWidth();
	*h = r.getHeight();
}

void bmx_cegui_scrolledcontainer_setcontentarea(CEGUI::ScrolledContainer * sc, float x, float y, float w, float h) {
	sc->setContentArea(CEGUI::Rect(x, y, w, h));
}

void bmx_cegui_scrolledcontainer_getchildextentsarea(CEGUI::ScrolledContainer * sc, float * x, float * y, float * w, float * h) {
	CEGUI::Rect r = sc->getChildExtentsArea();
	*x = r.d_left;
	*y = r.d_top;
	*w = r.getWidth();
	*h = r.getHeight();
}

// *************************************************

void bmx_cegui_clippedcontainer_getcliparea(CEGUI::ClippedContainer * cc, float * x, float * y, float * w, float * h) {
	CEGUI::Rect r = cc->getClipArea();
	*x = r.d_left;
	*y = r.d_top;
	*w = r.getWidth();
	*h = r.getHeight();
}

BBObject * bmx_cegui_clippedcontainer_getclipperwindow(CEGUI::ClippedContainer * cc) {
	return newObjectForWindow(cc->getClipperWindow());
}

void bmx_cegui_clippedcontainer_setcliparea(CEGUI::ClippedContainer * cc, float x, float y, float w, float h) {
	cc->setClipArea(CEGUI::Rect(x, y, w, h));
}

void bmx_cegui_clippedcontainer_setclipperwindow(CEGUI::ClippedContainer * cc, CEGUI::Window * w) {
	cc->setClipperWindow(w);
}

// *************************************************

CEGUI::MouseCursor * bmx_cegui_mousecursor_getsingleton() {
	return CEGUI::MouseCursor::getSingletonPtr();
}

void bmx_cegui_mousecursor_setimagewithname(CEGUI::MouseCursor * mc, const CEGUI::utf8 * imageset, const CEGUI::utf8 * imageName) {
	mc->setImage(imageset, imageName);
}

void bmx_cegui_mousecursor_setimage(CEGUI::MouseCursor * mc, CEGUI::Image * image) {
	mc->setImage(image);
}

BBObject * bmx_cegui_mousecursor_getimage(CEGUI::MouseCursor * mc) {
	return _bah_cegui_TCEImage__create(const_cast<CEGUI::Image*>(mc->getImage()));
}

void bmx_cegui_mousecursor_draw(CEGUI::MouseCursor * mc) {
	mc->draw();
}

void bmx_cegui_mousecursor_setposition(CEGUI::MouseCursor * mc, float x, float y) {
	mc->setPosition(CEGUI::Point(x, y));
}

void bmx_cegui_mousecursor_offsetPosition(CEGUI::MouseCursor * mc, float x, float y) {
	mc->offsetPosition(CEGUI::Point(x, y));
}

void bmx_cegui_mousecursor_setconstraintarea(CEGUI::MouseCursor * mc, float x, float y, float w, float h) {
	CEGUI::Rect r(x, y, w, h);
	mc->setConstraintArea(&r);
}

void bmx_cegui_mousecursor_hide(CEGUI::MouseCursor * mc) {
	mc->hide();
}

void bmx_cegui_mousecursor_show(CEGUI::MouseCursor * mc) {
	mc->show();
}

void bmx_cegui_mousecursor_setvisible(CEGUI::MouseCursor * mc, int visible) {
	mc->setVisible(visible);
}

int bmx_cegui_mousecursor_isvisible(CEGUI::MouseCursor * mc) {
	return mc->isVisible();
}

void bmx_cegui_mousecursor_getposition(CEGUI::MouseCursor * mc, float * x, float * y) {
	CEGUI::Point p = mc->getPosition();
	*x = p.d_x;
	*y = p.d_y;
}

void bmx_cegui_mousecursor_getconstraintarea(CEGUI::MouseCursor * mc, float * x, float * y, float * w, float * h) {
	CEGUI::Rect r = mc->getConstraintArea();
	*x = r.d_left;
	*y = r.d_top;
	*w = r.getWidth();
	*h = r.getHeight();
}

void bmx_cegui_mousecursor_getdisplayindependentposition(CEGUI::MouseCursor * mc, float * x, float * y) {
	CEGUI::Point p = mc->getDisplayIndependantPosition();
	*x = p.d_x;
	*y = p.d_y;
}

// *************************************************

float bmx_cegui_coordconverter_windowtoscreenx(CEGUI::Window * window, float x) {
	return CEGUI::CoordConverter::windowToScreenX(*window, x);
}

float bmx_cegui_coordconverter_windowtoscreeny(CEGUI::Window * window, float y) {
	return CEGUI::CoordConverter::windowToScreenY(*window, y);
}

void bmx_cegui_coordconverter_windowtoscreen(CEGUI::Window * window, float x, float y, float * toX, float * toY) {
	CEGUI::Vector2 v = CEGUI::CoordConverter::windowToScreen(*window, CEGUI::UVector2(guessUDim(x), guessUDim(y)));
	*toX = v.d_x;
	*toY = v.d_y;
}

void bmx_cegui_coordconverter_windowtoscreenrect(CEGUI::Window * window, float x, float y, float w, float h, float * toX, float * toY, float * toW, float * toH) {
	CEGUI::Rect r = CEGUI::CoordConverter::windowToScreen(*window, CEGUI::Rect(x, y, w, h));
	*toX = r.d_left;
	*toY = r.d_top;
	*toW = r.getWidth();
	*toH = r.getHeight();
}

float bmx_cegui_coordconverter_screentowindowx(CEGUI::Window * window, float x) {
	return CEGUI::CoordConverter::screenToWindowX(*window, x);
}

float bmx_cegui_coordconverter_screentowindowy(CEGUI::Window * window, float y) {
	return CEGUI::CoordConverter::screenToWindowY(*window, y);
}

void bmx_cegui_coordconverter_screentowindow(CEGUI::Window * window, float x, float y, float * toX, float * toY) {
	CEGUI::Vector2 v = CEGUI::CoordConverter::screenToWindow(*window, CEGUI::UVector2(guessUDim(x), guessUDim(y)));
	*toX = v.d_x;
	*toY = v.d_y;
}

void bmx_cegui_coordconverter_screentowindowrect(CEGUI::Window * window, float x, float y, float w, float h, float * toX, float * toY, float * toW, float * toH) {
	CEGUI::Rect r = CEGUI::CoordConverter::screenToWindow(*window, CEGUI::Rect(x, y, w, h));
	*toX = r.d_left;
	*toY = r.d_top;
	*toW = r.getWidth();
	*toH = r.getHeight();
}

// *************************************************


int bmx_cegui_combobox_ishit(CEGUI::Combobox * cb, float x, float y, int allowDisabled) {
	return cb->isHit(CEGUI::Point(x, y), allowDisabled);
}

int bmx_cegui_combobox_getsingleclickenabled(CEGUI::Combobox * cb) {
	return cb->getSingleClickEnabled();
}

int bmx_cegui_combobox_isdropdownlistvisible(CEGUI::Combobox * cb) {
	return cb->isDropDownListVisible();
}

BBObject * bmx_cegui_combobox_geteditbox(CEGUI::Combobox * cb) {
	return _bah_cegui_TCEEditbox__create(cb->getEditbox());
}

BBObject * bmx_cegui_combobox_getpushbutton(CEGUI::Combobox * cb) {
	return _bah_cegui_TCEPushButton__create(cb->getPushButton());
}

BBObject * bmx_cegui_combobox_getdroplist(CEGUI::Combobox * cb) {
	return _bah_cegui_TCEComboDropList__create(cb->getDropList());
}

int bmx_cegui_combobox_hasinputfocus(CEGUI::Combobox * cb) {
	return cb->hasInputFocus();
}

int bmx_cegui_combobox_isreadonly(CEGUI::Combobox * cb) {
	return cb->isReadOnly();
}

int bmx_cegui_combobox_istextvalid(CEGUI::Combobox * cb) {
	return cb->isTextValid();
}

BBString * bmx_cegui_combobox_getvalidationstring(CEGUI::Combobox * cb) {
	return bah_cegui__convertUTF8ToMax(cb->getValidationString().data());
}

int bmx_cegui_combobox_getcaratindex(CEGUI::Combobox * cb) {
	return cb->getCaratIndex();
}

int bmx_cegui_combobox_getselectionstartindex(CEGUI::Combobox * cb) {
	return cb->getSelectionStartIndex();
}

int bmx_cegui_combobox_getselectionendindex(CEGUI::Combobox * cb) {
	return cb->getSelectionEndIndex();
}

int bmx_cegui_combobox_getselectionlength(CEGUI::Combobox * cb) {
	return cb->getSelectionLength();
}

int bmx_cegui_combobox_getmaxtextlength(CEGUI::Combobox * cb) {
	return cb->getMaxTextLength();
}

int bmx_cegui_combobox_getitemcount(CEGUI::Combobox * cb) {
	return cb->getItemCount();
}

BBObject * bmx_cegui_combobox_getselecteditem(CEGUI::Combobox * cb) {
	return _bah_cegui_TCEListboxItem__create(cb->getSelectedItem());	
}

BBObject * bmx_cegui_combobox_getlistboxitemfromindex(CEGUI::Combobox * cb, int index) {
	return _bah_cegui_TCEListboxItem__create(cb->getListboxItemFromIndex(index));
}

int bmx_cegui_combobox_getitemindex(CEGUI::Combobox * cb, CEGUI::ListboxItem * item) {
	return cb->getItemIndex(item);
}

int bmx_cegui_combobox_issortenabled(CEGUI::Combobox * cb) {
	return cb->isSortEnabled();
}

int bmx_cegui_combobox_isitemselected(CEGUI::Combobox * cb, int index) {
	return cb->isItemSelected(index);
}

BBObject * bmx_cegui_combobox_finditemwithtext(CEGUI::Combobox * cb, const CEGUI::utf8 * text, CEGUI::ListboxItem * startItem) {
	return _bah_cegui_TCEListboxItem__create(cb->findItemWithText(text, startItem));
}

int bmx_cegui_combobox_islistboxiteminlist(CEGUI::Combobox * cb, CEGUI::ListboxItem * item) {
	return cb->isListboxItemInList(item);
}

int bmx_cegui_combobox_isvertscrollbaralwaysshown(CEGUI::Combobox * cb) {
	return cb->isVertScrollbarAlwaysShown();
}

int bmx_cegui_combobox_ishorzscrollbaralwaysshown(CEGUI::Combobox * cb) {
	return cb->isHorzScrollbarAlwaysShown();
}

void bmx_cegui_combobox_showdroplist(CEGUI::Combobox * cb) {
	cb->showDropList();
}

void bmx_cegui_combobox_hidedroplist(CEGUI::Combobox * cb) {
	cb->hideDropList();
}

void bmx_cegui_combobox_setsingleclickenabled(CEGUI::Combobox * cb, int setting) {
	cb->setSingleClickEnabled(setting);
}

void bmx_cegui_combobox_setreadonly(CEGUI::Combobox * cb, int setting) {
	cb->setReadOnly(setting);
}

void bmx_cegui_combobox_setvalidationstring(CEGUI::Combobox * cb, const CEGUI::utf8 * validationString) {
	cb->setValidationString(validationString);
}

void bmx_cegui_combobox_setcaratindex(CEGUI::Combobox * cb, int caratPos) {
	cb->setCaratIndex(caratPos);
}

void bmx_cegui_combobox_setselection(CEGUI::Combobox * cb, int startPos, int endPos) {
	cb->setSelection(startPos, endPos);
}

void bmx_cegui_combobox_setmaxtextlength(CEGUI::Combobox * cb, int maxLen) {
	cb->setMaxTextLength(maxLen);
}

void bmx_cegui_combobox_activateeditbox(CEGUI::Combobox * cb) {
	cb->activateEditbox();
}

void bmx_cegui_combobox_resetlist(CEGUI::Combobox * cb) {
	cb->resetList();
}

void bmx_cegui_combobox_additem(CEGUI::Combobox * cb, CEGUI::ListboxItem * item) {
	cb->addItem(item);
}

void bmx_cegui_combobox_insertitem(CEGUI::Combobox * cb, CEGUI::ListboxItem * item, CEGUI::ListboxItem * position) {
	cb->insertItem(item, position);
}

void bmx_cegui_combobox_removeitem(CEGUI::Combobox * cb, CEGUI::ListboxItem * item) {
	cb->removeItem(item);
}

void bmx_cegui_combobox_clearallselections(CEGUI::Combobox * cb) {
	cb->clearAllSelections();
}

void bmx_cegui_combobox_setsortingenabled(CEGUI::Combobox * cb, int setting) {
	cb->setSortingEnabled(setting);
}

void bmx_cegui_combobox_setshowvertscrollbar(CEGUI::Combobox * cb, int setting) {
	cb->setShowVertScrollbar(setting);
}

void bmx_cegui_combobox_setshowhorzscrollbar(CEGUI::Combobox * cb, int setting) {
	cb->setShowHorzScrollbar(setting);
}

void bmx_cegui_combobox_setitemselectstate(CEGUI::Combobox * cb, CEGUI::ListboxItem * item, int state) {
	cb->setItemSelectState(item, state);
}

void bmx_cegui_combobox_setitemselectstateindex(CEGUI::Combobox * cb, int itemIndex, int state) {
	cb->setItemSelectState(itemIndex, state);
}

void bmx_cegui_combobox_handleupdatedlistitemdata(CEGUI::Combobox * cb) {
	cb->handleUpdatedListItemData();
}

// *************************************************

CEGUI::UDim * bmx_cegui_udim_create(float scale, float offset) {
	return new CEGUI::UDim(scale, offset);
}

void bmx_cegui_udim_delete(CEGUI::UDim * udim) {
	delete udim;
}

// *************************************************


void bmx_cegui_image_getsize(CEGUI::Image * image, float * width, float * height) {
	CEGUI::Size s(image->getSize());
	*width = s.d_width;
	*height = s.d_height;
}

float bmx_cegui_image_getwidth(CEGUI::Image * image) {
	return image->getWidth();
}

float bmx_cegui_image_getheight(CEGUI::Image * image) {
	return image->getHeight();
}

void bmx_cegui_image_getoffsets(CEGUI::Image * image, float * x, float * y) {
	CEGUI::Point p(image->getOffsets());
	*x = p.d_x;
	*y = p.d_y;
}

float bmx_cegui_image_getoffsetx(CEGUI::Image * image) {
	return image->getOffsetX();
}

float bmx_cegui_image_getoffsety(CEGUI::Image * image) {
	return image->getOffsetY();
}

BBString * bmx_cegui_image_getname(CEGUI::Image * image) {
	return bah_cegui__convertUTF8ToMax(image->getName().data());
}

BBString * bmx_cegui_image_getimagesetname(CEGUI::Image * image) {
	return bah_cegui__convertUTF8ToMax(image->getImagesetName().data());
}

const CEGUI::Imageset * bmx_cegui_image_getimageset(CEGUI::Image * image) {
	return image->getImageset();
}

void bmx_cegui_image_getsourcetexturearea(CEGUI::Image * image, float * x, float * y, float * w, float * h) {
	CEGUI::Rect r(image->getSourceTextureArea());
	*x = r.d_left;
	*y = r.d_top;
	*w = r.getWidth();
	*h = r.getHeight();
}

// *************************************************


int bmx_cegui_font_iscodepointavailable(CEGUI::Font * font, CEGUI::utf32 cp) {
	return font->isCodepointAvailable(cp);
}

float bmx_cegui_font_getlinespacing(CEGUI::Font * font, float yScale) {
	return font->getLineSpacing(yScale);
}

float bmx_cegui_font_getfontheight(CEGUI::Font * font, float yScale) {
	return font->getFontHeight(yScale);
}

float bmx_cegui_font_getbaseline(CEGUI::Font * font, float yScale) {
	return font->getBaseline(yScale);
}

// *************************************************

BBObject * bmx_cegui_dragdropeventargs_getdragdropitem(CEGUI::DragDropEventArgs * args) {
	return _bah_cegui_TCEDragContainer__create(args->dragDropItem);
}

BBObject * bmx_cegui_mousecursoreventargs_getimage(CEGUI::MouseCursorEventArgs * args) {
	return _bah_cegui_TCEImage__create(const_cast<CEGUI::Image*>(args->image));
}

CEGUI::utf32 bmx_cegui_keyeventargs_getcodepoint(CEGUI::KeyEventArgs * args) {
	return args->codepoint;
}

CEGUI::Key::Scan bmx_cegui_keyeventargs_getscancode(CEGUI::KeyEventArgs * args) {
	return args->scancode;
}

CEGUI::uint bmx_cegui_keyeventargs_getsyskeys(CEGUI::KeyEventArgs * args) {
	return args->sysKeys;
}

void bmx_cegui_mouseeventargs_getposition(CEGUI::MouseEventArgs * args, float * x, float * y) {
	*x = args->position.d_x;
	*y = args->position.d_y;
}

void bmx_cegui_mouseeventargs_getmovedelta(CEGUI::MouseEventArgs * args, float * x, float * y) {
	*x = args->moveDelta.d_x;
	*y = args->moveDelta.d_y;
}

CEGUI::MouseButton bmx_cegui_mouseeventargs_getbutton(CEGUI::MouseEventArgs * args) {
	return args->button;
}

CEGUI::uint bmx_cegui_mouseeventargs_getsyskeys(CEGUI::MouseEventArgs * args) {
	return args->sysKeys;
}

float bmx_cegui_mouseeventargs_getwheelchange(CEGUI::MouseEventArgs * args) {
	return args->wheelChange;
}

CEGUI::uint bmx_cegui_mouseeventargs_getclickcount(CEGUI::MouseEventArgs * args) {
	return args->clickCount;
}

CEGUI::uint bmx_cegui_headersequenceeventargs_getoldindex(CEGUI::HeaderSequenceEventArgs * args) {
	return args->d_oldIdx;
}

CEGUI::uint bmx_cegui_headersequenceeventargs_getnewindex(CEGUI::HeaderSequenceEventArgs * args) {
	return args->d_newIdx;
}

BBObject * bmx_cegui_treeeventargs_gettreeitem(CEGUI::TreeEventArgs * args) {
	return _bah_cegui_TCETreeItem__create(args->treeItem);
}

// *************************************************

BBString * bmx_cegui_imageset_getname(CEGUI::Imageset * is) {
	return bah_cegui__convertUTF8ToMax(is->getName().data());
}

CEGUI::uint bmx_cegui_imageset_getimagecount(CEGUI::Imageset * is) {
	return is->getImageCount();
}

int bmx_cegui_imageset_isimagedefined(CEGUI::Imageset * is, const CEGUI::utf8 * name) {
	return is->isImageDefined(name);
}

void bmx_cegui_imageset_undefineimage(CEGUI::Imageset * is, const CEGUI::utf8 * name) {
	is->undefineImage(name);
}

void bmx_cegui_imageset_undefineallimages(CEGUI::Imageset * is) {
	is->undefineAllImages();
}

void bmx_cegui_imageset_getimagesize(CEGUI::Imageset * is, const CEGUI::utf8 * name, float * width, float * height) {
	try {
		CEGUI::Size s(is->getImageSize(name));
		*width = s.d_width;
		*height = s.d_height;
	} catch (CEGUI::UnknownObjectException &e) {
		bmx_cegui_throw_unknownobjectexception(e);
	} catch (CEGUI::Exception &e) {
		bmx_cegui_throw_exception(e);
	}
}

float bmx_cegui_imageset_getimagewidth(CEGUI::Imageset * is, const CEGUI::utf8 * name) {
	try {
		return is->getImageWidth(name);
	} catch (CEGUI::UnknownObjectException &e) {
		bmx_cegui_throw_unknownobjectexception(e);
	} catch (CEGUI::Exception &e) {
		bmx_cegui_throw_exception(e);
	}
}

float bmx_cegui_imageset_getimageheight(CEGUI::Imageset * is, const CEGUI::utf8 * name) {
	try {
		return is->getImageHeight(name);
	} catch (CEGUI::UnknownObjectException &e) {
		bmx_cegui_throw_unknownobjectexception(e);
	} catch (CEGUI::Exception &e) {
		bmx_cegui_throw_exception(e);
	}
}

void bmx_cegui_imageset_getimageoffset(CEGUI::Imageset * is, const CEGUI::utf8 * name, float * x, float * y) {
	try {
		CEGUI::Point p(is->getImageOffset(name));
		*x = p.d_x;
		*y = p.d_y;
	} catch (CEGUI::UnknownObjectException &e) {
		bmx_cegui_throw_unknownobjectexception(e);
	} catch (CEGUI::Exception &e) {
		bmx_cegui_throw_exception(e);
	}
}

float bmx_cegui_imageset_getimageoffsetx(CEGUI::Imageset * is, const CEGUI::utf8 * name) {
	try {
		return is->getImageOffsetX(name);
	} catch (CEGUI::UnknownObjectException &e) {
		bmx_cegui_throw_unknownobjectexception(e);
	} catch (CEGUI::Exception &e) {
		bmx_cegui_throw_exception(e);
	}
}

float bmx_cegui_imageset_getimageoffsety(CEGUI::Imageset * is, const CEGUI::utf8 * name) {
	try {
		return is->getImageOffsetY(name);
	} catch (CEGUI::UnknownObjectException &e) {
		bmx_cegui_throw_unknownobjectexception(e);
	} catch (CEGUI::Exception &e) {
		bmx_cegui_throw_exception(e);
	}
}

void bmx_cegui_imageset_defineimage(CEGUI::Imageset * is, const CEGUI::utf8 * name, float x, float y, float width, float height, float renderOffsetX, float renderOffsetY) {
	try {
		is->defineImage(name, CEGUI::Point(x, y), CEGUI::Size(width, height), CEGUI::Point(renderOffsetX, renderOffsetY));
	} catch (CEGUI::AlreadyExistsException &e) {
		bmx_cegui_throw_alreadyexistsexception(e);
	} catch (CEGUI::Exception &e) {
		bmx_cegui_throw_exception(e);
	}
}

int bmx_cegui_imageset_isautoscaled(CEGUI::Imageset * is) {
	return is->isAutoScaled();
}

void bmx_cegui_imageset_getnativeresoultion(CEGUI::Imageset * is, float * width, float * height) {
	CEGUI::Size s(is->getNativeResolution());
	*width = s.d_width;
	*height = s.d_height;
}

void bmx_cegui_imageset_setautoscalingenabled(CEGUI::Imageset * is, int setting) {
	is->setAutoScalingEnabled(setting);
}

void bmx_cegui_imageset_setnativeresolution(CEGUI::Imageset * is, float width, float height) {
	is->setNativeResolution(CEGUI::Size(width, height));
}

void bmx_cegui_imageset_notifydisplaysizechanged(CEGUI::Imageset * is, float width, float height) {
	is->notifyDisplaySizeChanged(CEGUI::Size(width, height));
}

// *************************************************

BBObject * bmx_cegui_groupbox_getcontentpane(CEGUI::GroupBox * box) {
	return newObjectForWindow(box->getContentPane());
}

// *************************************************

int bmx_cegui_listheadersegment_issizingenabled(CEGUI::ListHeaderSegment * seg) {
	return seg->isSizingEnabled();
}

CEGUI::ListHeaderSegment::SortDirection bmx_cegui_listheadersegment_getsortdirection(CEGUI::ListHeaderSegment * seg) {
	return seg->getSortDirection();
}

int bmx_cegui_listheadersegment_isdragmovingenabled(CEGUI::ListHeaderSegment * seg) {
	return seg->isDragMovingEnabled();
}

void bmx_cegui_listheadersegment_getdragmoveoffset(CEGUI::ListHeaderSegment * seg, float * x, float * y) {
	CEGUI::Point p(seg->getDragMoveOffset());
	*x = p.d_x;
	*y = p.d_y;
}

int bmx_cegui_listheadersegment_isclickable(CEGUI::ListHeaderSegment * seg) {
	return seg->isClickable();
}

int bmx_cegui_listheadersegment_issegmenthovering(CEGUI::ListHeaderSegment * seg) {
	return seg->isSegmentHovering();
}

int bmx_cegui_listheadersegment_issegmentpushed(CEGUI::ListHeaderSegment * seg) {
	return seg->isSegmentPushed();
}

int bmx_cegui_listheadersegment_issplitterhovering(CEGUI::ListHeaderSegment * seg) {
	return seg->isSplitterHovering();
}

int bmx_cegui_listheadersegment_isbeingdragmoved(CEGUI::ListHeaderSegment * seg) {
	return seg->isBeingDragMoved();
}

int bmx_cegui_listheadersegment_isbeingdragsized(CEGUI::ListHeaderSegment * seg) {
	return seg->isBeingDragSized();
}

BBObject * bmx_cegui_listheadersegment_getsizingcursorimage(CEGUI::ListHeaderSegment * seg) {
	return _bah_cegui_TCEImage__create(const_cast<CEGUI::Image*>(seg->getSizingCursorImage()));
}

BBObject * bmx_cegui_listheadersegment_getmovingcursorimage(CEGUI::ListHeaderSegment * seg) {
	return _bah_cegui_TCEImage__create(const_cast<CEGUI::Image*>(seg->getMovingCursorImage()));
}

void bmx_cegui_listheadersegment_setsizingenabled(CEGUI::ListHeaderSegment * seg, int setting) {
	seg->setSizingEnabled(setting);
}

void bmx_cegui_listheadersegment_setsortdirection(CEGUI::ListHeaderSegment * seg, CEGUI::ListHeaderSegment::SortDirection sortDir) {
	seg->setSortDirection(sortDir);
}

void bmx_cegui_listheadersegment_setdragmovingenabled(CEGUI::ListHeaderSegment * seg, int setting) {
	seg->setDragMovingEnabled(setting);
}

void bmx_cegui_listheadersegment_setclickable(CEGUI::ListHeaderSegment * seg, int setting) {
	seg->setClickable(setting);
}


// *************************************************

CEGUI::uint bmx_cegui_listheader_getcolumncount(CEGUI::ListHeader * head) {
	return head->getColumnCount();
}

BBObject * bmx_cegui_listheader_getsegmentfromcolumn(CEGUI::ListHeader * head, CEGUI::uint column) {
	return _bah_cegui_TCEListHeaderSegment__create(&head->getSegmentFromColumn(column));
}

BBObject * bmx_cegui_listheader_getsegmentfromid(CEGUI::ListHeader * head, CEGUI::uint id) {

}

BBObject * bmx_cegui_listheader_getsortsegment(CEGUI::ListHeader * head) {

}

CEGUI::uint bmx_cegui_listheader_getcolumnfromsegment(CEGUI::ListHeader * head, CEGUI::ListHeaderSegment * segment) {
	return head->getColumnFromSegment(*segment);
}

CEGUI::uint bmx_cegui_listheader_getcolumnfromid(CEGUI::ListHeader * head, CEGUI::uint id) {
	return head->getColumnFromID(id);
}

CEGUI::uint bmx_cegui_listheader_getsortcolumn(CEGUI::ListHeader * head) {
	return head->getSortColumn();
}

CEGUI::uint bmx_cegui_listheader_getcolumnwithtext(CEGUI::ListHeader * head, const CEGUI::utf8 * text) {
	return head->getColumnWithText(text);
}

float bmx_cegui_listheader_getpixeloffsettosegment(CEGUI::ListHeader * head, CEGUI::ListHeaderSegment * segment) {
	return head->getPixelOffsetToSegment(*segment);
}

float bmx_cegui_listheader_getpixeloffsettocolumn(CEGUI::ListHeader * head, CEGUI::uint column) {
	return head->getPixelOffsetToColumn(column);
}

float bmx_cegui_listheader_gettotalsegmentspixelextent(CEGUI::ListHeader * head) {
	return head->getTotalSegmentsPixelExtent();
}

float bmx_cegui_listheader_getcolumnwidth(CEGUI::ListHeader * head, CEGUI::uint column) {
	return head->getColumnWidth(column).d_offset;
}

CEGUI::ListHeaderSegment::SortDirection bmx_cegui_listheader_getsortdirection(CEGUI::ListHeader * head) {
	return head->getSortDirection();
}

int bmx_cegui_listheader_issortingenabled(CEGUI::ListHeader * head) {
	return head->isSortingEnabled();
}

int bmx_cegui_listheader_iscolumnsizingenabled(CEGUI::ListHeader * head) {
	return head->isColumnSizingEnabled();
}

int bmx_cegui_listheader_iscolumndraggingenabled(CEGUI::ListHeader * head) {
	return head->isColumnDraggingEnabled();
}

float bmx_cegui_listheader_getsegmentoffset(CEGUI::ListHeader * head) {
	return head->getSegmentOffset();
}

void bmx_cegui_listheader_setsortingenabled(CEGUI::ListHeader * head, int setting) {
	head->setSortingEnabled(setting);
}

void bmx_cegui_listheader_setsortdirection(CEGUI::ListHeader * head, CEGUI::ListHeaderSegment::SortDirection direction) {
	head->setSortDirection(direction);
}

void bmx_cegui_listheader_setsortsegment(CEGUI::ListHeader * head, CEGUI::ListHeaderSegment * segment) {
	head->setSortSegment(*segment);
}

void bmx_cegui_listheader_setsortcolumn(CEGUI::ListHeader * head, CEGUI::uint column) {
	head->setSortColumn(column);
}

void bmx_cegui_listheader_setsortcolumnfromid(CEGUI::ListHeader * head, CEGUI::uint id) {
	head->setSortColumnFromID(id);
}

void bmx_cegui_listheader_setcolumnsizingenabled(CEGUI::ListHeader * head, int setting) {
	head->setColumnSizingEnabled(setting);
}

void bmx_cegui_listheader_setcolumndraggingenabled(CEGUI::ListHeader * head, int setting) {
	head->setColumnDraggingEnabled(setting);
}

void bmx_cegui_listheader_addcolumn(CEGUI::ListHeader * head, const CEGUI::utf8 * text, CEGUI::uint id, float width) {
	head->addColumn(text, id, guessUDim(width));
}

void bmx_cegui_listheader_insertcolumn(CEGUI::ListHeader * head, const CEGUI::utf8 * text, CEGUI::uint id, float width, CEGUI::uint position) {
	head->insertColumn(text, id, guessUDim(width), position);
}

void bmx_cegui_listheader_insertcolumnatsegment(CEGUI::ListHeader * head, const CEGUI::utf8 * text, CEGUI::uint id, float width, CEGUI::ListHeaderSegment * position) {
	head->insertColumn(text, id, guessUDim(width), *position);
}

void bmx_cegui_listheader_removecolumn(CEGUI::ListHeader * head, CEGUI::uint column) {
	head->removeColumn(column);
}

void bmx_cegui_listheader_removesegment(CEGUI::ListHeader * head, CEGUI::ListHeaderSegment * segment) {
	head->removeSegment(*segment);
}

void bmx_cegui_listheader_movecolumn(CEGUI::ListHeader * head, CEGUI::uint column, CEGUI::uint position) {
	head->moveColumn(column, position);
}

void bmx_cegui_listheader_movecolumnatsegment(CEGUI::ListHeader * head, CEGUI::uint column, CEGUI::ListHeaderSegment * position) {
	head->moveColumn(column, *position);
}

void bmx_cegui_listheader_movesegment(CEGUI::ListHeader * head, CEGUI::ListHeaderSegment * segment, CEGUI::uint position) {
	head->moveSegment(*segment, position);
}

void bmx_cegui_listheader_movesegmentatsegment(CEGUI::ListHeader * head, CEGUI::ListHeaderSegment * segment, CEGUI::ListHeaderSegment * position) {
	head->moveSegment(*segment, *position);
}

void bmx_cegui_listheader_setsegmentoffset(CEGUI::ListHeader * head, float offset) {
	head->setSegmentOffset(offset);
}

void bmx_cegui_listheader_setcolumnwidth(CEGUI::ListHeader * head, CEGUI::uint column, float width) {
	head->setColumnWidth(column, guessUDim(width));
}

// *************************************************

BBObject * bmx_cegui_schememanager_createscheme(const CEGUI::utf8 * filename, const CEGUI::utf8 * resourceGroup, CEGUI::XMLResourceExistsAction action) {
	return _bah_cegui_TCEScheme__create(&CEGUI::SchemeManager::getSingleton().create(filename, resourceGroup, action));
}

void bmx_cegui_schememanager_destroyname(const CEGUI::utf8 * scheme) {
	CEGUI::SchemeManager::getSingleton().destroy(scheme);
}

void bmx_cegui_schememanager_destroyobj(CEGUI::Scheme * scheme) {
	CEGUI::SchemeManager::getSingleton().destroy(*scheme);
}

void bmx_cegui_schememanager_destroyall() {
	CEGUI::SchemeManager::getSingleton().destroyAll();
}

BBObject * bmx_cegui_schememanager_get(const CEGUI::utf8 * name) {
	try {
		return _bah_cegui_TCEScheme__create(&CEGUI::SchemeManager::getSingleton().get(name));
	} catch (CEGUI::UnknownObjectException &e) {
		bmx_cegui_throw_unknownobjectexception(e);
	} catch (CEGUI::Exception &e) {
		bmx_cegui_throw_exception(e);
	}
}

int bmx_cegui_schememanager_isdefined(const CEGUI::utf8 * name) {
	return static_cast<int>(CEGUI::SchemeManager::getSingleton().isDefined(name));
}

void bmx_cegui_schememanager_createall(const CEGUI::utf8 * pattern, const CEGUI::utf8 * resourceGroup) {
	CEGUI::SchemeManager::getSingleton().createAll(pattern, resourceGroup);
}

