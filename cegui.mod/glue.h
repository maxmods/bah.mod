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

#include "CEGUI.h"

#include <OpenGLGUIRenderer/openglrenderer.h>

class MaxCEEventCallback;
class MaxEventArgs;
class MaxConnection;

extern "C" {

#include "blitz.h"
#include "keycodes.h"

	bool _bah_cegui_TCEEventCallback__callback(BBObject * cb, BBObject * args);

	BBObject * _bah_cegui_TCEEditbox__create(CEGUI::Window * window);
	BBObject * _bah_cegui_TCEFrameWindow__create(CEGUI::Window * window);
	BBObject * _bah_cegui_TCEGUISheet__create(CEGUI::Window * window);
	BBObject * _bah_cegui_TCECheckbox__create(CEGUI::Window * window);
	BBObject * _bah_cegui_TCERadioButton__create(CEGUI::Window * window);
	BBObject * _bah_cegui_TCEPushButton__create(CEGUI::Window * window);
	BBObject * _bah_cegui_TCETabButton__create(CEGUI::Window * window);
	BBObject * _bah_cegui_TCECombobox__create(CEGUI::Window * window);
	BBObject * _bah_cegui_TCEScrolledItemListBase__create(CEGUI::Window * window);
	BBObject * _bah_cegui_TCEItemListBox__create(CEGUI::Window * window);
	BBObject * _bah_cegui_TCEPopupMenu__create(CEGUI::Window * window);
	BBObject * _bah_cegui_TCEMenubar__create(CEGUI::Window * window);
	BBObject * _bah_cegui_TCEMenuBase__create(CEGUI::Window * window);
	BBObject * _bah_cegui_TCEItemListBase__create(CEGUI::Window * window);
	BBObject * _bah_cegui_TCETree__create(CEGUI::Window * window);
	BBObject * _bah_cegui_TCETooltip__create(CEGUI::Window * window);
	BBObject * _bah_cegui_TCETitlebar__create(CEGUI::Window * window);
	BBObject * _bah_cegui_TCETabControl__create(CEGUI::Window * window);
	BBObject * _bah_cegui_TCESpinner__create(CEGUI::Window * window);
	BBObject * _bah_cegui_TCESlider__create(CEGUI::Window * window);
	BBObject * _bah_cegui_TCEScrolledContainer__create(CEGUI::Window * window);
	BBObject * _bah_cegui_TCEScrollbar__create(CEGUI::Window * window);
	BBObject * _bah_cegui_TCEScrollablePane__create(CEGUI::Window * window);
	BBObject * _bah_cegui_TCEProgressBar__create(CEGUI::Window * window);
	BBObject * _bah_cegui_TCEMultiLineEditbox__create(CEGUI::Window * window);
	BBObject * _bah_cegui_TCEMultiColumnList__create(CEGUI::Window * window);
	BBObject * _bah_cegui_TCEListHeaderSegment__create(CEGUI::Window * window);
	BBObject * _bah_cegui_TCEListHeader__create(CEGUI::Window * window);
	BBObject * _bah_cegui_TCEComboDropList__create(CEGUI::Window * window);
	BBObject * _bah_cegui_TCEListbox__create(CEGUI::Window * window);
	BBObject * _bah_cegui_TCEMenuItem__create(CEGUI::Window * window);
	BBObject * _bah_cegui_TCEItemEntry__create(CEGUI::Window * window);
	BBObject * _bah_cegui_TCEDragContainer__create(CEGUI::Window * window);
	BBObject * _bah_cegui_TCEClippedContainer__create(CEGUI::Window * window);
	BBObject * _bah_cegui_TCEGroupBox__create(CEGUI::Window * window);
	BBObject * _bah_cegui_TCEWindow__create(CEGUI::Window * window);

	
	BBObject * newObjectForWindow(CEGUI::Window * window);
	
	BBObject * _bah_cegui_TCEMouseCursorEventArgs__create(MaxEventArgs * maxArgs);
	BBObject * _bah_cegui_TCEKeyEventArgs__create(MaxEventArgs * maxArgs);
	BBObject * _bah_cegui_TCEActivationEventArgs__create(MaxEventArgs * maxArgs);
	BBObject * _bah_cegui_TCEDragDropEventArgs__create(MaxEventArgs * maxArgs);
	BBObject * _bah_cegui_TCEHeaderSequenceEventArgs__create(MaxEventArgs * maxArgs);
	BBObject * _bah_cegui_TCEMouseEventArgs__create(MaxEventArgs * maxArgs);
	BBObject * _bah_cegui_TCETreeEventArgs__create(MaxEventArgs * maxArgs);
	BBObject * _bah_cegui_TCEUpdateEventArgs__create(MaxEventArgs * maxArgs);
	BBObject * _bah_cegui_TCEWindowEventArgs__create(MaxEventArgs * maxArgs);
	BBObject * _bah_cegui_TCEEventArgs__create(MaxEventArgs * maxArgs);
	
	BBObject * newObjectForEventArgs(CEGUI::EventArgs * args, MaxEventArgs * maxArgs);
	
	CEGUI::Renderer * bmx_cegui_new_oglrenderer();
	CEGUI::System * bmx_cegui_new_system(CEGUI::Renderer * r);
	void bmx_cegui_delete_system(CEGUI::System * s);
	void bmx_cegui_delete_renderer(CEGUI::Renderer * r);

	CEGUI::Scheme * bmx_cegui_schememanager_loadScheme(const CEGUI::utf8 * scheme, const CEGUI::utf8 * resourceGroup);
	
	void bmx_cegui_system_setDefaultFont(const CEGUI::utf8 * font);
	void bmx_cegui_system_setDefaultMouseCursor(const CEGUI::utf8 * l, const CEGUI::utf8 * kind);
	void bmx_cegui_system_renderGUI();
	void bmx_cegui_system_setGUISheet(CEGUI::Window * window);
	bool bmx_cegui_system_injectTimePulse(float t);
	bool bmx_cegui_system_injectMousePosition(int x, int y);
	bool bmx_cegui_system_injectMouseButtonDown(int button);
	bool bmx_cegui_system_injectMouseButtonUp(int button);
	
	BBObject * bmx_cegui_windowmanager_loadWindowLayout(const CEGUI::utf8 * filename, const CEGUI::utf8 * namePrefix, const CEGUI::utf8 * resourceGroup);
	BBObject * bmx_cegui_windowmanager_getwindow(const CEGUI::utf8 * name);
	BBObject * bmx_cegui_windowmanager_createwindow(const CEGUI::utf8 * windowType, const CEGUI::utf8 * name, const CEGUI::utf8 * prefix);

	//MaxCEEventHandler * bmx_cegui_eventhandler_new();
	//void bmx_cegui_eventhandler_delete(MaxCEEventHandler * handle);

	MaxCEEventCallback * bmx_cegui_eventcallback_new(BBObject * handle);

	void bmx_cegui_eventargs_delete(MaxEventArgs * args);

	MaxConnection * bmx_cegui_eventset_subscribeevent(CEGUI::Window * handle, const CEGUI::utf8 * name, MaxCEEventCallback * cb);


	void bmx_cegui_window_settext(CEGUI::Window * window, const CEGUI::utf8 * text);
	void bmx_cegui_window_deactivate(CEGUI::Window * window);
	void bmx_cegui_window_setposition(CEGUI::Window * window, float x, float y);
	void bmx_cegui_window_setxposition(CEGUI::Window * window, float x);
	void bmx_cegui_window_setyposition(CEGUI::Window * window, float y);
	void bmx_cegui_window_setsize(CEGUI::Window * window, float width, float height);
	void bmx_cegui_window_setwidth(CEGUI::Window * window, float width);
	void bmx_cegui_window_setheight(CEGUI::Window * window, float height);
	void bmx_cegui_window_setmaxsize(CEGUI::Window * window, float width, float height);
	void bmx_cegui_window_setminsize(CEGUI::Window * window, float width, float height);
	void bmx_cegui_window_addchildwindowwindow(CEGUI::Window * window, CEGUI::Window * win);
	void bmx_cegui_window_addchildwindow(CEGUI::Window * window, const CEGUI::utf8 * name);
	void bmx_cegui_window_hide(CEGUI::Window * window);
	void bmx_cegui_window_show(CEGUI::Window * window);
	const CEGUI::utf8 * bmx_cegui_window_gettype(CEGUI::Window * window);
	void bmx_cegui_window_activate(CEGUI::Window * window);

	void bmx_cegui_window_setproperty(CEGUI::Window * window, const CEGUI::utf8 * name, const CEGUI::utf8 * value);
	
	void bmx_cegui_connection_delete(MaxConnection * conn);

	bool bmx_cegui_checkbox_isselected(CEGUI::Checkbox * cb);
	void bmx_cegui_checkbox_setselected(CEGUI::Checkbox * cb, bool selected);

	float bmx_cegui_progressbar_getprogress(CEGUI::ProgressBar * pb);
	float bmx_cegui_progressbar_getstep(CEGUI::ProgressBar * pb);
	void bmx_cegui_progressbar_setprogress(CEGUI::ProgressBar * pb, float progress);
	void bmx_cegui_progressbar_setstepsize(CEGUI::ProgressBar * pb, float stepVal);
	void bmx_cegui_progressbar_dostep(CEGUI::ProgressBar * pb);
	void bmx_cegui_progressbar_adjustprogress(CEGUI::ProgressBar * pb, float delta);

	CEGUI::uint bmx_cegui_mapmaxtocekey(int key);
	bool bmx_cegui_system_injectkeydown(int key);
	bool bmx_cegui_system_injectkeyup(int key);
	bool bmx_cegui_system_injectchar(CEGUI::utf32 key);

	bool bmx_cegui_editbox_hasinputfocus(CEGUI::Editbox * eb);
	bool bmx_cegui_editbox_isreadonly(CEGUI::Editbox * eb);
	bool bmx_cegui_editbox_istextmasked(CEGUI::Editbox * eb);
	bool bmx_cegui_editbox_istextvalid(CEGUI::Editbox * eb);
	const CEGUI::utf8 * bmx_cegui_editbox_getvalidationstring(CEGUI::Editbox * eb);
	int bmx_cegui_editbox_getcaratindex(CEGUI::Editbox * eb);
	int bmx_cegui_editbox_getselectionstartindex(CEGUI::Editbox * eb);
	int bmx_cegui_editbox_getselectionendindex(CEGUI::Editbox * eb);
	int bmx_cegui_editbox_getselectionlength(CEGUI::Editbox * eb);
	int bmx_cegui_editbox_getmaxtextlength(CEGUI::Editbox * eb);
	void bmx_cegui_editbox_setreadonly(CEGUI::Editbox * eb, bool setting);
	void bmx_cegui_editbox_settextmasked(CEGUI::Editbox * eb, bool setting);
	void bmx_cegui_editbox_setvalidationstring(CEGUI::Editbox * eb, const CEGUI::utf8 * validationString);
	void bmx_cegui_editbox_setcaratindex(CEGUI::Editbox * eb, int caratPos);
	void bmx_cegui_editbox_setselection(CEGUI::Editbox * eb, int startPos, int endPos);
	void bmx_cegui_editbox_setmaxtextlength(CEGUI::Editbox * eb, int maxLen);


}

class MaxConnection
{
public:
	MaxConnection(CEGUI::Event::Connection conn);
	
private:
	CEGUI::Event::Connection conn;
};

class MaxEventArgs
{
public:
	MaxEventArgs(const CEGUI::EventArgs & args);
	
	const CEGUI::EventArgs & Args();
	
private:
	const CEGUI::EventArgs & eventArgs;
};

class MaxCEEventCallback
{
public:

	MaxCEEventCallback(BBObject * handle);

	bool EventCallback(const CEGUI::EventArgs& args);
	MaxConnection * subscribeEvent(CEGUI::Window * handle, const CEGUI::utf8 * name);

private:
	BBObject *callbackHandle;
	CEGUI::Event::Connection conn;
};



