/*
  Copyright (c) 2008-2011 Bruce A Henderson
 
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

//#include <CEGUIOpenGLRenderer.h>
#include "CEGUIDefaultLogger.h"
#include "CEGUIPixmapFont.h"
#include "CEGUIFreeTypeFont.h"

class MaxCEEventCallback;
class MaxEventArgs;
class MaxConnection;
class MaxLogger;
class MaxListHeaderSegment;
class MaxCEColour;
class MaxCEColourRect;
class MaxResourceProvider;
class MaxRawDataContainer;
class MaxWindowFactory;
class MaxRenderedString;

extern "C" {

#include "blitz.h"
#include "keycodes.h"

	int _bah_cegui_TCEEventCallback__callback(BBObject * cb, BBObject * args);
	void _bah_cegui_TCECustomLogger__logEvent(BBObject * handle, const CEGUI::utf8 * message, CEGUI::LoggingLevel level);
	void _bah_cegui_TCECustomLogger__setLogFilename(BBObject * handle, const CEGUI::utf8 * filename, int append);

	BBString * bah_cegui__convertUTF8ToMax(const CEGUI::utf8 * text);

	BBObject * _bah_cegui_TCEEditbox__create(CEGUI::Window * window);
	BBObject * _bah_cegui_TCEFrameWindow__create(CEGUI::Window * window);
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
	BBObject * _bah_cegui_TCEDefaultWindow__create(CEGUI::Window * window);

	
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
	
	BBObject * _bah_cegui_TCEFont__create(CEGUI::Font * font);
	BBObject * _bah_cegui_TCETexture__create(CEGUI::Texture * texture);
	BBObject * _bah_cegui_TCEListboxItem__create(CEGUI::ListboxItem * item);
	BBObject * _bah_cegui_TCEImage__create(CEGUI::Image * image);
	BBObject * _bah_cegui_TCETreeItem__create(CEGUI::TreeItem * item);
	BBObject * _bah_cegui_TCEScheme__create(CEGUI::Scheme * scheme);
	BBObject * _bah_cegui_TCEImageset__create(CEGUI::Imageset * imageset);
	BBObject * _bah_cegui_TCEDefaultResourceProvider__create(CEGUI::DefaultResourceProvider * provider);
	
	void _bah_cegui_TCEResourceProvider__loadRawDataContainer(BBObject * handle, BBString * filename, MaxRawDataContainer * container, BBString * resourceGroup);
	void _bah_cegui_TCEResourceProvider__unloadRawDataContainer(BBObject * handle, MaxRawDataContainer * data);
	
	CEGUI::Window * _bah_cegui_TCEWindowFactory__createWindow(BBObject * handle, BBString * name);
	void _bah_cegui_TCEWindowFactory__destroyWindow(BBObject * handle, BBObject * window);
	const CEGUI::utf8 * _bah_cegui_TCEWindowFactory__getTypeName(BBObject * handle);
	
	BBObject * _bah_cegui_TCEAlreadyExistsException__create(BBString * message, BBString * name, BBString * fileName, int line);
	BBObject * _bah_cegui_TCEFileIOException__create(BBString * message, BBString * name, BBString * fileName, int line);
	BBObject * _bah_cegui_TCEGenericException__create(BBString * message, BBString * name, BBString * fileName, int line);
	BBObject * _bah_cegui_TCEInvalidRequestException__create(BBString * message, BBString * name, BBString * fileName, int line);
	BBObject * _bah_cegui_TCEMemoryException__create(BBString * message, BBString * name, BBString * fileName, int line);
	BBObject * _bah_cegui_TCENullObjectException__create(BBString * message, BBString * name, BBString * fileName, int line);
	BBObject * _bah_cegui_TCEObjectInUseException__create(BBString * message, BBString * name, BBString * fileName, int line);
	BBObject * _bah_cegui_TCERendererException__create(BBString * message, BBString * name, BBString * fileName, int line);
	BBObject * _bah_cegui_TCEScriptException__create(BBString * message, BBString * name, BBString * fileName, int line);
	BBObject * _bah_cegui_TCEUnknownObjectException__create(BBString * message, BBString * name, BBString * fileName, int line);
	BBObject * _bah_cegui_TCEException__create(BBString * message, BBString * name, BBString * fileName, int line);
	
	BBObject * newObjectForEventArgs(CEGUI::EventArgs * args, MaxEventArgs * maxArgs);
	
	CEGUI::System * bmx_cegui_new_system(CEGUI::Renderer * r, MaxResourceProvider * provider);
	void bmx_cegui_system_destroy();
	void bmx_cegui_delete_renderer(CEGUI::Renderer * r);

	void bmx_cegui_system_setdefaultfont(CEGUI::System * sys, CEGUI::Font * font);
	void bmx_cegui_system_setdefaultfonttxt(CEGUI::System * sys, const CEGUI::utf8 * font);
	void bmx_cegui_system_setDefaultMouseCursor(CEGUI::System * sys, const CEGUI::utf8 * l, const CEGUI::utf8 * kind);
	void bmx_cegui_system_renderGUI(CEGUI::System * sys);
	BBObject * bmx_cegui_system_setGUISheet(CEGUI::System * sys, CEGUI::Window * window);
	int bmx_cegui_system_injectTimePulse(CEGUI::System * sys, float t);
	int bmx_cegui_system_injectMousePosition(CEGUI::System * sys, int x, int y);
	int bmx_cegui_system_injectMouseButtonDown(CEGUI::System * sys, int button);
	int bmx_cegui_system_injectMouseButtonUp(CEGUI::System * sys, int button);
	int bmx_cegui_system_injectMouseWheelChange(CEGUI::System * sys, int delta);
	CEGUI::uint bmx_cegui_mapmaxtocekey(int key);
	int bmx_cegui_system_injectkeydown(CEGUI::System * sys, int key);
	int bmx_cegui_system_injectkeyup(CEGUI::System * sys, int key);
	int bmx_cegui_system_injectchar(CEGUI::System * sys, CEGUI::utf32 key);
	CEGUI::Font * bmx_cegui_system_getDefaultFont(CEGUI::System * sys);
	void bmx_cegui_system_signalredraw(CEGUI::System * sys);
	int bmx_cegui_system_isredrawrequested(CEGUI::System * sys);
	double bmx_cegui_system_getsingleclicktimeout(CEGUI::System * sys);
	double bmx_cegui_system_getmulticlicktimeout(CEGUI::System * sys);
	void bmx_cegui_system_getmulticlicktoleranceareasize(CEGUI::System * sys, float * width, float * height);
	void bmx_cegui_system_setsingleclicktimeout(CEGUI::System * sys, double timeout);
	void bmx_cegui_system_setmulticlicktimeout(CEGUI::System * sys, double timeout);
	void bmx_cegui_system_setmulticlicktoleranceareasize(CEGUI::System * sys, float width, float height);
	void bmx_cegui_system_setdefaulttooltip(CEGUI::System * sys, CEGUI::Tooltip * tooltip);
	void bmx_cegui_system_setdefaulttooltiptext(CEGUI::System * sys, CEGUI::utf8 * tooltip);
	BBObject * bmx_cegui_system_getdefaulttooltip(CEGUI::System * sys);
	float bmx_cegui_system_getmousemovescaling(CEGUI::System * sys);
	void bmx_cegui_system_setmousemovescaling(CEGUI::System * sys, float scaling);
	BBObject * bmx_cegui_system_getresourceprovider(CEGUI::System * sys);
	void bmx_cegui_system_notifydisplaysizechanged(CEGUI::System * sys, float width, float height);

	CEGUI::WindowManager * bmx_cegui_windowmanager_getsingleton();
	BBObject * bmx_cegui_windowmanager_loadWindowLayout(CEGUI::WindowManager * mgr, const CEGUI::utf8 * filename, const CEGUI::utf8 * namePrefix, const CEGUI::utf8 * resourceGroup);
	BBObject * bmx_cegui_windowmanager_getwindow(CEGUI::WindowManager * mgr, const CEGUI::utf8 * name);
	BBObject * bmx_cegui_windowmanager_createwindow(CEGUI::WindowManager * mgr, const CEGUI::utf8 * windowType, const CEGUI::utf8 * name);
	int bmx_cegui_windowmanager_iswindowpresent(CEGUI::WindowManager * mgr, const CEGUI::utf8 * name);
	void bmx_cegui_windowmanager_destroyallwindows(CEGUI::WindowManager * mgr);
	void bmx_cegui_windowmanager_destroywindowwindow(CEGUI::WindowManager * mgr, CEGUI::Window * window);
	void bmx_cegui_windowmanager_destroywindowname(CEGUI::WindowManager * mgr, const CEGUI::utf8 * window);
	void bmx_cegui_windowmanager_renamewindowwindow(CEGUI::WindowManager * mgr, CEGUI::Window * window, const CEGUI::utf8 * newName);
	void bmx_cegui_windowmanager_renamewindowname(CEGUI::WindowManager * mgr, const CEGUI::utf8 * window, const CEGUI::utf8 * newName);
	int bmx_cegui_windowmanager_isdeadpoolempty(CEGUI::WindowManager * mgr);
	void bmx_cegui_windowmanager_cleandeadpool(CEGUI::WindowManager * mgr);
	void bmx_cegui_windowmanager_lock(CEGUI::WindowManager * mgr);
	void bmx_cegui_windowmanager_unlock(CEGUI::WindowManager * mgr);
	int bmx_cegui_windowmanager_islocked(CEGUI::WindowManager * mgr);
	void bmx_cegui_windowmanager_setdefaultresourcegroup(const CEGUI::utf8 * resourceGroup);
	BBString * bmx_cegui_windowmanager_getdefaultresourcegroup();

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
	void bmx_cegui_window_addchildwindow(CEGUI::Window * window, CEGUI::Window * win);
	void bmx_cegui_window_addchild(CEGUI::Window * window, const CEGUI::utf8 * name);
	void bmx_cegui_window_hide(CEGUI::Window * window);
	void bmx_cegui_window_show(CEGUI::Window * window);
	BBString * bmx_cegui_window_gettype(CEGUI::Window * window);
	void bmx_cegui_window_activate(CEGUI::Window * window);
	BBString * bmx_cegui_window_getname(CEGUI::Window * window);
	int bmx_cegui_window_isdestroyedbyparent(CEGUI::Window * window);
	int bmx_cegui_window_isalwaysontop(CEGUI::Window * window);
	int bmx_cegui_window_isdisabled(CEGUI::Window * window, int localOnly);
	int bmx_cegui_window_isvisible(CEGUI::Window * window, int localOnly);
	int bmx_cegui_window_isactive(CEGUI::Window * window);
	int bmx_cegui_window_isclippedbyparent(CEGUI::Window * window);
	CEGUI::uint bmx_cegui_window_getid(CEGUI::Window * window);
	int bmx_cegui_window_getchildcount(CEGUI::Window * window);
	int bmx_cegui_window_ischild(CEGUI::Window * window, const CEGUI::utf8 * name);
	int bmx_cegui_window_ischildid(CEGUI::Window * window, CEGUI::uint ID);
	int bmx_cegui_window_ischildrecursive(CEGUI::Window * window, CEGUI::uint ID);
	int bmx_cegui_window_ischildwindow(CEGUI::Window * window, CEGUI::Window * win);
	BBObject * bmx_cegui_window_getchild(CEGUI::Window * window, const CEGUI::utf8 * name);
	BBObject * bmx_cegui_window_getchildid(CEGUI::Window * window, CEGUI::uint ID);
	BBObject * bmx_cegui_window_getchildrecursive(CEGUI::Window * window, CEGUI::uint ID);
	BBObject * bmx_cegui_window_getchildatidx(CEGUI::Window * window, int idx);
	BBObject * bmx_cegui_window_getactivechild(CEGUI::Window * window);
	int bmx_cegui_window_isancestor(CEGUI::Window * window, const CEGUI::utf8 * name);
	int bmx_cegui_window_isancestorid(CEGUI::Window * window, CEGUI::uint ID);
	int bmx_cegui_window_isancestorwindow(CEGUI::Window * window, CEGUI::Window * win);
	BBString * bmx_cegui_window_gettext(CEGUI::Window * window);
	int bmx_cegui_window_inheritsalpha(CEGUI::Window * window);
	float bmx_cegui_window_getalpha(CEGUI::Window * window);
	float bmx_cegui_window_geteffectivealpha(CEGUI::Window * window);
	void bmx_cegui_window_getunclippedinnerrect(CEGUI::Window * window, float * x, float * y, float * w, float * h);
	int bmx_cegui_window_iscapturedbythis(CEGUI::Window * window);
	int bmx_cegui_window_iscapturedbyancestor(CEGUI::Window * window);
	int bmx_cegui_window_iscapturedbychild(CEGUI::Window * window);
	int bmx_cegui_window_ishit(CEGUI::Window * window, float x, float y, int allowDisabled);
	BBObject * bmx_cegui_window_getchildatposition(CEGUI::Window * window, float x, float y);
	BBObject * bmx_cegui_window_gettargetchildatposition(CEGUI::Window * window, float x, float y);
	BBObject * bmx_cegui_window_getparent(CEGUI::Window * window);
	void bmx_cegui_window_getpixelsize(CEGUI::Window * window, float * width, float * height);
	int bmx_cegui_window_restoresoldcapture(CEGUI::Window * window);
	int bmx_cegui_window_iszorderingenabled(CEGUI::Window * window);
	int bmx_cegui_window_wantsmulticlickevents(CEGUI::Window * window);
	int bmx_cegui_window_ismouseautorepeatenabled(CEGUI::Window * window);
	float bmx_cegui_window_getautorepeatdelay(CEGUI::Window * window);
	float bmx_cegui_window_getautorepeatrate(CEGUI::Window * window);
	int bmx_cegui_window_distributescapturedinputs(CEGUI::Window * window);
	int bmx_cegui_window_isusingdefaulttooltip(CEGUI::Window * window);
	BBString * bmx_cegui_window_gettooltiptype(CEGUI::Window * window);
	BBString * bmx_cegui_window_gettooltiptext(CEGUI::Window * window);
	int bmx_cegui_window_inheritstooltiptext(CEGUI::Window * window);
	int bmx_cegui_window_isriseonclickenabled(CEGUI::Window * window);
	int bmx_cegui_window_testclassname(CEGUI::Window * window, const CEGUI::utf8 * className);
	CEGUI::VerticalAlignment bmx_cegui_window_getverticalalignment(CEGUI::Window * window);
	CEGUI::HorizontalAlignment bmx_cegui_window_gethorizontalalignment(CEGUI::Window * window);
	BBString * bmx_cegui_window_getlooknfeel(CEGUI::Window * window);
	int bmx_cegui_window_getmodalstate(CEGUI::Window * window);
	BBString * bmx_cegui_window_getuserstring(CEGUI::Window * window, const CEGUI::utf8 * name);
	int bmx_cegui_window_isuserstringdefined(CEGUI::Window * window, const CEGUI::utf8 * name);
	float bmx_cegui_window_getparentpixelwidth(CEGUI::Window * window);
	float bmx_cegui_window_getparentpixelheight(CEGUI::Window * window);
	int bmx_cegui_window_ismousepassthroughenabled(CEGUI::Window * window);
	int bmx_cegui_window_isautowindow(CEGUI::Window * window);
	int bmx_cegui_window_iswritingxmlallowed(CEGUI::Window * window);
	int bmx_cegui_window_isdragdroptarget(CEGUI::Window * window);
	void bmx_cegui_window_setclippedbyparent(CEGUI::Window * window, int setting);
	void bmx_cegui_window_setid(CEGUI::Window * window, CEGUI::uint ID);
	void bmx_cegui_window_inserttext(CEGUI::Window * window, const CEGUI::utf8 * text, int position);
	void bmx_cegui_window_appendtext(CEGUI::Window * window, const CEGUI::utf8 * text);
	void bmx_cegui_window_setfont(CEGUI::Window * window, CEGUI::Font * font);
	void bmx_cegui_window_setfontbyname(CEGUI::Window * window, const CEGUI::utf8 * name);
	void bmx_cegui_window_removechild(CEGUI::Window * window, CEGUI::Window * win);
	void bmx_cegui_window_removechildname(CEGUI::Window * window, const CEGUI::utf8 * name);
	void bmx_cegui_window_removechildforid(CEGUI::Window * window, CEGUI::uint ID);
	void bmx_cegui_window_movetofront(CEGUI::Window * window);
	void bmx_cegui_window_movetoback(CEGUI::Window * window);
	int bmx_cegui_window_captureinput(CEGUI::Window * window);
	void bmx_cegui_window_releaseinput(CEGUI::Window * window);
	void bmx_cegui_window_setrestoreoldcapture(CEGUI::Window * window, int setting);
	void bmx_cegui_window_setalpha(CEGUI::Window * window, float alpha);
	void bmx_cegui_window_setinheritsalpha(CEGUI::Window * window, int setting);
	void bmx_cegui_window_invalidate(CEGUI::Window * window);
	void bmx_cegui_window_invalidaterenderingsurface(CEGUI::Window * window);
	void bmx_cegui_window_setmousecursor(CEGUI::Window * window, CEGUI::Image * image);
	void bmx_cegui_window_setmousecursormode(CEGUI::Window * window, CEGUI::MouseCursorImage image);
	void bmx_cegui_window_setmousecursorbyname(CEGUI::Window * window, const CEGUI::utf8 * imageset, const CEGUI::utf8 * imageName);
	void bmx_cegui_window_setzorderingenabled(CEGUI::Window * window, int setting);
	void bmx_cegui_window_setwantsmulticlickevents(CEGUI::Window * window, int setting);
	void bmx_cegui_window_setmouseautorepeateenabled(CEGUI::Window * window, int setting);
	void bmx_cegui_window_setautorepeatdelay(CEGUI::Window * window, float delay);
	void bmx_cegui_window_setautorepeatrate(CEGUI::Window * window, float rate);
	void bmx_cegui_window_setdistributescapturedinputs(CEGUI::Window * window, int setting);
	void bmx_cegui_window_notifydragdropitementers(CEGUI::Window * window, CEGUI::DragContainer * item);
	void bmx_cegui_window_notifydragdropitemleaves(CEGUI::Window * window, CEGUI::DragContainer * item);
	void bmx_cegui_window_notifydragdropitemdropped(CEGUI::Window * window, CEGUI::DragContainer * item);
	void bmx_cegui_window_destroy(CEGUI::Window * window);
	void bmx_cegui_window_settooltip(CEGUI::Window * window, CEGUI::Tooltip * tooltip);
	void bmx_cegui_window_settooltiptype(CEGUI::Window * window, const CEGUI::utf8 * tooltipType);
	void bmx_cegui_window_settooltiptext(CEGUI::Window * window, const CEGUI::utf8 * tip);
	void bmx_cegui_window_setinheritstooltiptext(CEGUI::Window * window, int setting);
	void bmx_cegui_window_setriseonclickenabled(CEGUI::Window * window, int setting);
	void bmx_cegui_window_setverticalaligmnent(CEGUI::Window * window, CEGUI::VerticalAlignment alignment);
	void bmx_cegui_window_sethorizontalalignment(CEGUI::Window * window, CEGUI::HorizontalAlignment alignment);
	void bmx_cegui_window_setlooknfeel(CEGUI::Window * window, const CEGUI::utf8 * look);
	void bmx_cegui_window_setmodalstate(CEGUI::Window * window, int state);
	void bmx_cegui_window_performchildwindowlayout(CEGUI::Window * window);
	void bmx_cegui_window_setuserstring(CEGUI::Window * window, const CEGUI::utf8 * name, const CEGUI::utf8 * value);
	void bmx_cegui_window_setpositionu(CEGUI::Window * window, const CEGUI::UDim * x, const CEGUI::UDim * y);
	void bmx_cegui_window_setxpositionu(CEGUI::Window * window, const CEGUI::UDim * x);
	void bmx_cegui_window_setypositionu(CEGUI::Window * window, const CEGUI::UDim * y);
	void bmx_cegui_window_setsizeu(CEGUI::Window * window, const CEGUI::UDim * width, const CEGUI::UDim * height);
	void bmx_cegui_window_setwidthu(CEGUI::Window * window, const CEGUI::UDim * width);
	void bmx_cegui_window_setheightu(CEGUI::Window * window, const CEGUI::UDim * height);
	BBObject * bmx_cegui_window_getmousecursor(CEGUI::Window * window, int useDefault);
	void bmx_cegui_window_rename(CEGUI::Window * window, const CEGUI::utf8 * newName);
	void bmx_cegui_window_setdestroyedbyparent(CEGUI::Window * window, int setting);
	void bmx_cegui_window_setalwaysontop(CEGUI::Window * window, int setting);
	void bmx_cegui_window_setenabled(CEGUI::Window * window, int setting);
	void bmx_cegui_window_enable(CEGUI::Window * window);
	void bmx_cegui_window_disable(CEGUI::Window * window);
	void bmx_cegui_window_setvisible(CEGUI::Window * window, int setting);
	float bmx_cegui_window_getxposition(CEGUI::Window * window);
	float bmx_cegui_window_getyposition(CEGUI::Window * window);
	float bmx_cegui_window_getwidth(CEGUI::Window * window);
	float bmx_cegui_window_getheight(CEGUI::Window * window);
	void bmx_cegui_window_render(CEGUI::Window * window);
	void bmx_cegui_window_getsize(CEGUI::Window * window, float * w, float * h);
	void bmx_cegui_window_getmaxsize(CEGUI::Window * window, float * w, float * h);
	void bmx_cegui_window_getminsize(CEGUI::Window * window, float * w, float * h);
	void bmx_cegui_window_setmousepassthroughenabled(CEGUI::Window * window, int setting);
	void bmx_cegui_window_setwindowrenderer(CEGUI::Window * window, CEGUI::utf8 * name);
	BBString * bmx_cegui_window_getwindowrenderername(CEGUI::Window * window);
	void bmx_cegui_window_setwritingxmlallowed(CEGUI::Window * window, int allow);
	void bmx_cegui_window_notifyscreenareachanged(CEGUI::Window * window);
	void bmx_cegui_window_setfalagardtype(CEGUI::Window * window, CEGUI::utf8 * type, CEGUI::utf8 * rendererType);
	void bmx_cegui_window_setdragdroptarget(CEGUI::Window * window, int setting);
	CEGUI::Font * bmx_cegui_window_getfont(CEGUI::Window * window, int useDefault);
	BBObject * bmx_cegui_window_gettooltip(CEGUI::Window * window);
	BBObject * bmx_cegui_window_getactivesibling(CEGUI::Window * window);
	void bmx_cegui_window_getparentpixelsize(CEGUI::Window * window, float * width, float * height);
	void bmx_cegui_window_setarearel(CEGUI::Window * window, float xpos, float ypos, float width, float height);
	void bmx_cegui_window_setusingautorenderingsurface(CEGUI::Window * window, int setting);
	void bmx_cegui_window_setrotation(CEGUI::Window * window, float x, float y, float z);
	void bmx_cegui_window_setnonclientwindow(CEGUI::Window * window, int setting);
	MaxRenderedString * bmx_cegui_window_getrenderedstring(CEGUI::Window * window);
	int bmx_cegui_window_istextparsingenabled(CEGUI::Window * window);
	void bmx_cegui_window_settextparsingenabled(CEGUI::Window * window, int setting);

	void bmx_cegui_window_setproperty(CEGUI::Window * window, const CEGUI::utf8 * name, const CEGUI::utf8 * value);
	void bmx_cegui_window_removeproperty(CEGUI::Window * window, const CEGUI::utf8 * name);
	void bmx_cegui_window_clearproperties(CEGUI::Window * window);
	int bmx_cegui_window_ispropertypresent(CEGUI::Window * window, const CEGUI::utf8 * name);
	BBString * bmx_cegui_window_getpropertyhelp(CEGUI::Window * window, const CEGUI::utf8 * name);
	BBString * bmx_cegui_window_getproperty(CEGUI::Window * window, const CEGUI::utf8 * name);
	int bmx_cegui_window_ispropertydefault(CEGUI::Window * window, const CEGUI::utf8 * name);
	BBString * bmx_cegui_window_getpropertydefault(CEGUI::Window * window, const CEGUI::utf8 * name);

	void bmx_cegui_connection_delete(MaxConnection * conn);

	int bmx_cegui_checkbox_isselected(CEGUI::Checkbox * cb);
	void bmx_cegui_checkbox_setselected(CEGUI::Checkbox * cb, int selected);

	float bmx_cegui_progressbar_getprogress(CEGUI::ProgressBar * pb);
	float bmx_cegui_progressbar_getstep(CEGUI::ProgressBar * pb);
	void bmx_cegui_progressbar_setprogress(CEGUI::ProgressBar * pb, float progress);
	void bmx_cegui_progressbar_setstepsize(CEGUI::ProgressBar * pb, float stepVal);
	void bmx_cegui_progressbar_dostep(CEGUI::ProgressBar * pb);
	void bmx_cegui_progressbar_adjustprogress(CEGUI::ProgressBar * pb, float delta);

	int bmx_cegui_editbox_hasinputfocus(CEGUI::Editbox * eb);
	int bmx_cegui_editbox_isreadonly(CEGUI::Editbox * eb);
	int bmx_cegui_editbox_istextmasked(CEGUI::Editbox * eb);
	int bmx_cegui_editbox_istextvalid(CEGUI::Editbox * eb);
	BBString * bmx_cegui_editbox_getvalidationstring(CEGUI::Editbox * eb);
	int bmx_cegui_editbox_getcaretindex(CEGUI::Editbox * eb);
	int bmx_cegui_editbox_getselectionstartindex(CEGUI::Editbox * eb);
	int bmx_cegui_editbox_getselectionendindex(CEGUI::Editbox * eb);
	int bmx_cegui_editbox_getselectionlength(CEGUI::Editbox * eb);
	int bmx_cegui_editbox_getmaxtextlength(CEGUI::Editbox * eb);
	void bmx_cegui_editbox_setreadonly(CEGUI::Editbox * eb, int setting);
	void bmx_cegui_editbox_settextmasked(CEGUI::Editbox * eb, int setting);
	void bmx_cegui_editbox_setvalidationstring(CEGUI::Editbox * eb, const CEGUI::utf8 * validationString);
	void bmx_cegui_editbox_setcaretindex(CEGUI::Editbox * eb, int caretPos);
	void bmx_cegui_editbox_setselection(CEGUI::Editbox * eb, int startPos, int endPos);
	void bmx_cegui_editbox_setmaxtextlength(CEGUI::Editbox * eb, int maxLen);
	void bmx_cegui_editbox_setmaskcodepoint(CEGUI::Editbox * eb, CEGUI::utf32 codePoint);

	void bmx_cegui_logger_setlogginglevel(CEGUI::LoggingLevel level);
	void bmx_cegui_logger_setlogfilename(const CEGUI::utf8 * filename, int append);
	void bmx_cegui_logger_logevent(const CEGUI::utf8 * message, CEGUI::LoggingLevel level);
	MaxLogger * bmx_cegui_customlogger_create(BBObject * handle);
	void bmx_cegui_customlogger_delete(MaxLogger * logger);
	CEGUI::LoggingLevel bmx_cegui_logger_getlogginglevel();

	int bmx_cegui_eventargs_gethandled(MaxEventArgs * args);
	BBObject * bmx_cegui_windoweventargs_getwindow(MaxEventArgs * args);
	BBObject * bmx_cegui_activationeventargs_getotherwindow(MaxEventArgs * args);
	float bmx_cegui_updateeventargs_gettimesincelastframe(MaxEventArgs * args);

	int bmx_cegui_framewindow_issizingenabled(CEGUI::FrameWindow * win);
	int bmx_cegui_framewindow_isframeenabled(CEGUI::FrameWindow * win);
	int bmx_cegui_framewindow_istitlebarenabled(CEGUI::FrameWindow * win);
	int bmx_cegui_framewindow_isclosebuttonenabled(CEGUI::FrameWindow * win);
	int bmx_cegui_framewindow_isrollupenabled(CEGUI::FrameWindow * win);
	int bmx_cegui_framewindow_isrolledup(CEGUI::FrameWindow * win);
	float bmx_cegui_framewindow_getsizingborderthickness(CEGUI::FrameWindow * win);
	void bmx_cegui_framewindow_setsizingenabled(CEGUI::FrameWindow * win, int setting);
	void bmx_cegui_framewindow_setframeenabled(CEGUI::FrameWindow * win, int setting);
	void bmx_cegui_framewindow_settitlebarenabled(CEGUI::FrameWindow * win, int setting);
	void bmx_cegui_framewindow_setclosebuttonenabled(CEGUI::FrameWindow * win, int setting);
	void bmx_cegui_framewindow_rollupenabled(CEGUI::FrameWindow * win, int setting);
	void bmx_cegui_framewindow_togglerollup(CEGUI::FrameWindow * win);
	void bmx_cegui_framewindow_setsizingborderthickness(CEGUI::FrameWindow * win, float pixels);
	int bmx_cegui_framewindow_isdragmovingenabled(CEGUI::FrameWindow * win);
	void bmx_cegui_framewindow_setdragmovingenabled(CEGUI::FrameWindow * win, int setting);
	int bmx_cegui_framewindow_ishit(CEGUI::FrameWindow * win, float x, float y, int allowDisabled);
	BBObject * bmx_cegui_framewindow_gettitlebar(CEGUI::FrameWindow * win);
	BBObject * bmx_cegui_framewindow_getclosebutton(CEGUI::FrameWindow * win);

	int bmx_cegui_titlebar_isdraggingenabled(CEGUI::Titlebar * tb);
	void bmx_cegui_titlebar_setdraggingenabled(CEGUI::Titlebar * tb, int setting);

	int bmx_cegui_buttonbase_ishovering(CEGUI::ButtonBase * base);
	int bmx_cegui_buttonbase_ispushed(CEGUI::ButtonBase * base);

	int bmx_cegui_radiobutton_isselected(CEGUI::RadioButton * rb);
	CEGUI::ulong bmx_cegui_radiobutton_getgroupid(CEGUI::RadioButton * rb);
	BBObject * bmx_cegui_radiobutton_getselectedbuttoningroup(CEGUI::RadioButton * rb);
	void bmx_cegui_radiobutton_setselected(CEGUI::RadioButton * rb, int selected);
	void bmx_cegui_radiobutton_setgroupid(CEGUI::RadioButton * rb, CEGUI::ulong group);

	void bmx_cegui_combodroplist_setarmed(CEGUI::ComboDropList * list, int setting);
	int bmx_cegui_combodroplist_isarmed(CEGUI::ComboDropList * list);
	void bmx_cegui_combodroplist_setautoarmenabled(CEGUI::ComboDropList * list, int setting);
	int bmx_cegui_combodroplist_isautoarmenabled(CEGUI::ComboDropList * list);

	int bmx_cegui_multilineeditbox_hasinputfocus(CEGUI::MultiLineEditbox * eb);
	int bmx_cegui_multilineeditbox_isreadonly(CEGUI::MultiLineEditbox * eb);
	int bmx_cegui_multilineeditbox_getcaretindex(CEGUI::MultiLineEditbox * eb);
	int bmx_cegui_multilineeditbox_getselectionstartindex(CEGUI::MultiLineEditbox * eb);
	int bmx_cegui_multilineeditbox_getselectionendindex(CEGUI::MultiLineEditbox * eb);
	int bmx_cegui_multilineeditbox_getselectionlength(CEGUI::MultiLineEditbox * eb);
	int bmx_cegui_multilineeditbox_getmaxtextlength(CEGUI::MultiLineEditbox * eb);
	int bmx_cegui_multilineeditbox_iswordwrapped(CEGUI::MultiLineEditbox * eb);
	BBObject * bmx_cegui_multilineeditbox_getvertscrollbar(CEGUI::MultiLineEditbox * eb);
	int bmx_cegui_multilineeditbox_isvertscrollbaralwaysshown(CEGUI::MultiLineEditbox * eb);
	BBObject * bmx_cegui_multilineeditbox_gethorzscrollbar(CEGUI::MultiLineEditbox * eb);
	void bmx_cegui_multilineeditbox_gettextrenderarea(CEGUI::MultiLineEditbox * eb, float * x, float * y, float * w, float * h);
	int bmx_cegui_multilineeditbox_getlinenumberfromindex(CEGUI::MultiLineEditbox * eb, int index);
	void bmx_cegui_multilineeditbox_setreadonly(CEGUI::MultiLineEditbox * eb, int setting);
	void bmx_cegui_multilineeditbox_setcaretindex(CEGUI::MultiLineEditbox * eb, int caretPos);
	void bmx_cegui_multilineeditbox_setselection(CEGUI::MultiLineEditbox * eb, int startPos, int endPos);
	void bmx_cegui_multilineeditbox_setmaxtextlength(CEGUI::MultiLineEditbox * eb, int maxLen);
	void bmx_cegui_multilineeditbox_ensurecaretisvisible(CEGUI::MultiLineEditbox * eb);
	void bmx_cegui_multilineeditbox_setwordwrapping(CEGUI::MultiLineEditbox * eb, int setting);
	void bmx_cegui_multilineeditbox_setshowvertscrollbar(CEGUI::MultiLineEditbox * eb, int setting);

	
	int bmx_cegui_scrolleditemlistbase_isvertscrollbaralwaysshown(CEGUI::ScrolledItemListBase * lb);
	int bmx_cegui_scrolleditemlistbase_ishorzscrollbaralwaysshown(CEGUI::ScrolledItemListBase * lb);
	BBObject * bmx_cegui_scrolleditemlistbase_getvertscrollbar(CEGUI::ScrolledItemListBase * lb);
	BBObject * bmx_cegui_scrolleditemlistbase_gethorzscrollbar(CEGUI::ScrolledItemListBase * lb);
	void bmx_cegui_scrolleditemlistbase_setshowvertscrollbar(CEGUI::ScrolledItemListBase * lb, int mode);
	void bmx_cegui_scrolleditemlistbase_setshowhorzscrollbar(CEGUI::ScrolledItemListBase * lb, int mode);

	int bmx_cegui_itemlistbox_getselectedcount(CEGUI::ItemListbox * lb);
	BBObject * bmx_cegui_itemlistbox_getlastselecteditem(CEGUI::ItemListbox * lb);
	BBObject * bmx_cegui_itemlistbox_getfirstselecteditem(CEGUI::ItemListbox * lb, int startIndex);
	BBObject * bmx_cegui_itemlistbox_getnextselecteditem(CEGUI::ItemListbox * lb);
	BBObject * bmx_cegui_itemlistbox_getnextselecteditemafter(CEGUI::ItemListbox * lb, const CEGUI::ItemEntry * startItem);
	int bmx_cegui_itemlistbox_ismultiselectenabled(CEGUI::ItemListbox * lb);
	int bmx_cegui_itemlistbox_isitemselected(CEGUI::ItemListbox * lb, int index);
	void bmx_cegui_itemlistbox_setmultiselectenabled(CEGUI::ItemListbox * lb, int state);
	void bmx_cegui_itemlistbox_clearallselections(CEGUI::ItemListbox * lb);
	void bmx_cegui_itemlistbox_selectrange(CEGUI::ItemListbox * lb, int a, int z);
	void bmx_cegui_itemlistbox_selectallitems(CEGUI::ItemListbox * lb);

	BBObject * bmx_cegui_imagesetmanager_createimagesetfromtexture(const CEGUI::utf8 * name, CEGUI::Texture * texture, CEGUI::XMLResourceExistsAction action);
	BBObject * bmx_cegui_imagesetmanager_createimagesetfromimagefile(const CEGUI::utf8 * name, const CEGUI::utf8 * filename, const CEGUI::utf8 * resourceGroup, CEGUI::XMLResourceExistsAction action);
	void bmx_cegui_imagesetmanager_notifydisplaysizechanged(float width, float height);
	BBObject * bmx_cegui_imagesetmanager_createimageset(const CEGUI::utf8 * filename, const CEGUI::utf8 * resourceGroup, CEGUI::XMLResourceExistsAction action);
	void bmx_cegui_imagesetmanager_destroyname(const CEGUI::utf8 * name);
	void bmx_cegui_imagesetmanager_destroyobj(CEGUI::Imageset * imageset);
	void bmx_cegui_imagesetmanager_destroyall();
	BBObject * bmx_cegui_imagesetmanager_get(const CEGUI::utf8 * name);
	int bmx_cegui_imagesetmanager_isdefined(const CEGUI::utf8 * name);
	void bmx_cegui_imagesetmanager_createall(const CEGUI::utf8 * pattern, const CEGUI::utf8 * resourceGroup);

	BBObject * bmx_cegui_fontmanager_createfreetypefont(const CEGUI::utf8 * font_name, float pointSize, int antialiased, const CEGUI::utf8 * fontFilename, 
		const CEGUI::utf8 * resourceGroup, int autoScaled, float nativeHorzRes, float nativeVertRes, CEGUI::XMLResourceExistsAction action);
	BBObject * bmx_cegui_fontmanager_createpixmapfont(const CEGUI::utf8 * font_name, const CEGUI::utf8 * imagesetFilename, const CEGUI::utf8 * resourceGroup,
		int autoScaled, float nativeHorzRes, float nativeVertRes, CEGUI::XMLResourceExistsAction action);
	void bmx_cegui_fontmanager_notifydislaysizechanged(float width, float height);

	BBObject * bmx_cegui_fontmanager_createfont(const CEGUI::utf8 * filename, const CEGUI::utf8 * resourceGroup, CEGUI::XMLResourceExistsAction action);
	void bmx_cegui_fontmanager_destroyname(const CEGUI::utf8 * name);
	void bmx_cegui_fontmanager_destroyobj(CEGUI::Font * font);
	void bmx_cegui_fontmanager_destroyall();
	BBObject * bmx_cegui_fontmanager_get(const CEGUI::utf8 * name);
	int bmx_cegui_fontmanager_isdefined(const CEGUI::utf8 * name);
	void bmx_cegui_fontmanager_createall(const CEGUI::utf8 * pattern, const CEGUI::utf8 * resourceGroup);

	float bmx_cegui_spinner_getcurrentvalue(CEGUI::Spinner * spinner);
	float bmx_cegui_spinner_getstepsize(CEGUI::Spinner * spinner);
	float bmx_cegui_spinner_getmaximumvalue(CEGUI::Spinner * spinner);
	float bmx_cegui_spinner_getminimumvalue(CEGUI::Spinner * spinner);
	CEGUI::Spinner::TextInputMode bmx_cegui_spinner_gettextinputmode(CEGUI::Spinner * spinner);
	void bmx_cegui_spinner_setcurrentvalue(CEGUI::Spinner * spinner, float value);
	void bmx_cegui_spinner_setstepsize(CEGUI::Spinner * spinner, float stepSize);
	void bmx_cegui_spinner_setmaximumvalue(CEGUI::Spinner * spinner, float maxValue);
	void bmx_cegui_spinner_setminimumvalue(CEGUI::Spinner * spinner, float minValue);
	void bmx_cegui_spinner_settextinputmode(CEGUI::Spinner * spinner, CEGUI::Spinner::TextInputMode mode);

	float bmx_cegui_scrollbar_getdocumentsize(CEGUI::Scrollbar * sb);
	float bmx_cegui_scrollbar_getpagesize(CEGUI::Scrollbar * sb);
	float bmx_cegui_scrollbar_getstepsize(CEGUI::Scrollbar * sb);
	float bmx_cegui_scrollbar_getoverlapsize(CEGUI::Scrollbar * sb);
	float bmx_cegui_scrollbar_getscrollposition(CEGUI::Scrollbar * sb);
	BBObject * bmx_cegui_scrollbar_getincreasebutton(CEGUI::Scrollbar * sb);
	BBObject * bmx_cegui_scrollbar_getdecreasebutton(CEGUI::Scrollbar * sb);
	void bmx_cegui_scrollbar_setdocumentsize(CEGUI::Scrollbar * sb, float documentSize);
	void bmx_cegui_scrollbar_setpagesize(CEGUI::Scrollbar * sb, float pageSize);
	void bmx_cegui_scrollbar_setstepsize(CEGUI::Scrollbar * sb, float stepSize);
	void bmx_cegui_scrollbar_setoverlapsize(CEGUI::Scrollbar * sb, float overlapSize);
	void bmx_cegui_scrollbar_setscrollposition(CEGUI::Scrollbar * sb, float position);

	float bmx_cegui_slider_getcurrentvalue(CEGUI::Slider * slider);
	float bmx_cegui_slider_getmaxvalue(CEGUI::Slider * slider);
	float bmx_cegui_slider_getclickstep(CEGUI::Slider * slider);
	void bmx_cegui_slider_setmaxvalue(CEGUI::Slider * slider, float maxVal);
	void bmx_cegui_slider_setcurrentvalue(CEGUI::Slider * slider, float value);
	void bmx_cegui_slider_setclickstep(CEGUI::Slider * slider, float clickStep);

	BBObject * bmx_cegui_tooltip_gettargetwindow(CEGUI::Tooltip * tt);
	float bmx_cegui_tooltip_gethovertime(CEGUI::Tooltip * tt);
	void bmx_cegui_tooltip_setdisplaytime(CEGUI::Tooltip * tt, float seconds);
	float bmx_cegui_tooltip_getfadetime(CEGUI::Tooltip * tt);
	void bmx_cegui_tooltip_sethovertime(CEGUI::Tooltip * tt, float seconds);
	float bmx_cegui_tooltip_getdisplaytime(CEGUI::Tooltip * tt);
	void bmx_cegui_tooltip_setfadetime(CEGUI::Tooltip * tt, float seconds);
	void bmx_cegui_tooltip_positionself(CEGUI::Tooltip * tt);
	void bmx_cegui_tooltip_sizeself(CEGUI::Tooltip * tt);
	void bmx_cegui_tooltip_gettextsize(CEGUI::Tooltip * tt, float * width, float * height);

	int bmx_cegui_tabcontrol_gettabcount(CEGUI::TabControl * tc);
	CEGUI::TabControl::TabPanePosition bmx_cegui_tabcontrol_gettabpaneposition(CEGUI::TabControl * tc);
	void bmx_cegui_tabcontrol_settabpaneposition(CEGUI::TabControl * tc, CEGUI::TabControl::TabPanePosition pos);
	void bmx_cegui_tabcontrol_setselectedtab(CEGUI::TabControl * tc, const CEGUI::utf8 * name);
	void bmx_cegui_tabcontrol_setselectedtabforid(CEGUI::TabControl * tc, CEGUI::uint ID);
	void bmx_cegui_tabcontrol_setselectedtabatindex(CEGUI::TabControl * tc, int index);
	void bmx_cegui_tabcontrol_maketabvisible(CEGUI::TabControl * tc, const CEGUI::utf8 * name);
	void bmx_cegui_tabcontrol_maketabvisibleforid(CEGUI::TabControl * tc, CEGUI::uint ID);
	void bmx_cegui_tabcontrol_maketabvisibleatindex(CEGUI::TabControl * tc, int index);
	BBObject * bmx_cegui_tabcontrol_gettabcontentsatindex(CEGUI::TabControl * tc, int index);
	BBObject * bmx_cegui_tabcontrol_gettabcontents(CEGUI::TabControl * tc, const CEGUI::utf8 * name);
	BBObject * bmx_cegui_tabcontrol_gettabcontentsforid(CEGUI::TabControl * tc, CEGUI::uint ID);
	int bmx_cegui_tabcontrol_istabcontentsselected(CEGUI::TabControl * tc, CEGUI::Window * wnd);
	int bmx_cegui_tabcontrol_getselectedtabindex(CEGUI::TabControl * tc);
	float bmx_cegui_tabcontrol_gettabheight(CEGUI::TabControl * tc);
	float bmx_cegui_tabcontrol_gettabtextpadding(CEGUI::TabControl * tc);
	void bmx_cegui_tabcontrol_settabheight(CEGUI::TabControl * tc, float height);
	void bmx_cegui_tabcontrol_settabtextpadding(CEGUI::TabControl * tc, float padding);
	void bmx_cegui_tabcontrol_addtab(CEGUI::TabControl * tc, CEGUI::Window * wnd);
	void bmx_cegui_tabcontrol_removetab(CEGUI::TabControl * tc, const CEGUI::utf8 * name);
	void bmx_cegui_tabcontrol_removetabforid(CEGUI::TabControl * tc, CEGUI::uint ID);
	void bmx_cegui_tabcontrol_settabheightu(CEGUI::TabControl * tc, const CEGUI::UDim * height);

	void bmx_cegui_texture_getsize(CEGUI::Texture * texture, int * w, int * h);
	void bmx_cegui_texture_getoriginaldatasize(CEGUI::Texture * texture, int * w, int * h);
	void bmx_cegui_texture_loadfromfile(CEGUI::Texture * texture, const CEGUI::utf8 * filename, const CEGUI::utf8 * resourceGroup);
	void bmx_cegui_texture_loadfrommemory(CEGUI::Texture * texture, const void * buffer, CEGUI::uint width, CEGUI::uint height, CEGUI::Texture::PixelFormat pixelFormat);
	void bmx_cegui_texture_savetomemory(CEGUI::Texture * texture, void * buffer);

	void bmx_cegui_scheme_loadresources(CEGUI::Scheme * scheme);
	void bmx_cegui_scheme_unloadresources(CEGUI::Scheme * scheme);
	int bmx_cegui_scheme_resourcesloaded(CEGUI::Scheme * scheme);
	BBString * bmx_cegui_scheme_getname(CEGUI::Scheme * scheme);
	void bmx_cegui_scheme_setdefaultresourcegroup(const CEGUI::utf8 * resourceGroup);
	BBString * bmx_cegui_scheme_getdefaultresourcegroup();

	BBString * bmx_cegui_listboxitem_gettext(CEGUI::ListboxItem * item);
	BBString * bmx_cegui_listboxitem_gettooltiptext(CEGUI::ListboxItem * item);
	CEGUI::uint bmx_cegui_listboxitem_getid(CEGUI::ListboxItem * item);
	int bmx_cegui_listboxitem_isselected(CEGUI::ListboxItem * item);
	int bmx_cegui_listboxitem_isdisabled(CEGUI::ListboxItem * item);
	int bmx_cegui_listboxitem_isautodeleted(CEGUI::ListboxItem * item);
	BBObject * bmx_cegui_listboxitem_getownerwindow(CEGUI::ListboxItem * item);
	void bmx_cegui_listboxitem_settext(CEGUI::ListboxItem * item, const CEGUI::utf8 * text);
	void bmx_cegui_listboxitem_settooltiptext(CEGUI::ListboxItem * item, const CEGUI::utf8 * text);
	void bmx_cegui_listboxitem_setid(CEGUI::ListboxItem * item, CEGUI::uint itemId);
	void bmx_cegui_listboxitem_setselected(CEGUI::ListboxItem * item, int setting);
	void bmx_cegui_listboxitem_setdisabled(CEGUI::ListboxItem * item, int setting);
	void bmx_cegui_listboxitem_setautodeleted(CEGUI::ListboxItem * item, int setting);
	void bmx_cegui_listboxitem_setownerwindow(CEGUI::ListboxItem * item, CEGUI::Window * owner);
	void bmx_cegui_listboxitem_setuserdata(CEGUI::ListboxItem * item, BBObject * itemData);
	MaxCEColourRect * bmx_cegui_listboxitem_getselectioncolours(CEGUI::ListboxItem * item);
	BBObject * bmx_cegui_listboxitem_getselectionbrushimage(CEGUI::ListboxItem * item);
	void bmx_cegui_listboxitem_setselectioncolours(CEGUI::ListboxItem * item, MaxCEColourRect * cols);
	void bmx_cegui_listboxitem_setselectioncoloursforcorners(CEGUI::ListboxItem * item, MaxCEColour * topLeftColour, MaxCEColour * topRightColour, MaxCEColour * bottomLeftColour, MaxCEColour * bottomRightColour);
	void bmx_cegui_listboxitem_setselectioncoloursforcolour(CEGUI::ListboxItem * item, MaxCEColour * col);
	void bmx_cegui_listboxitem_setselectionbrushimage(CEGUI::ListboxItem * item, CEGUI::Image * image);
	void bmx_cegui_listboxitem_setselectionbrushimagebyname(CEGUI::ListboxItem * item, const CEGUI::utf8 * imageset, const CEGUI::utf8 * image);
	void bmx_cegui_listboxitem_getpixelsize(CEGUI::ListboxItem * item, float * width, float * height);
	BBObject * bmx_cegui_listboxitem_getuserdata(CEGUI::ListboxItem * item);

	int bmx_cegui_multicolumnlist_isusersortcontrolenabled(CEGUI::MultiColumnList * mc);
	int bmx_cegui_multicolumnlist_isusercolumnsizingenabled(CEGUI::MultiColumnList * mc);
	int bmx_cegui_multicolumnlist_isusercolumndraggingenabled(CEGUI::MultiColumnList * mc);
	CEGUI::uint bmx_cegui_multicolumnlist_getcolumncount(CEGUI::MultiColumnList * mc);
	CEGUI::uint bmx_cegui_multicolumnlist_getrowcount(CEGUI::MultiColumnList * mc);
	CEGUI::uint bmx_cegui_multicolumnlist_getsortcolumn(CEGUI::MultiColumnList * mc);
	CEGUI::uint bmx_cegui_multicolumnlist_getcolumnwithid(CEGUI::MultiColumnList * mc, CEGUI::uint colId);
	CEGUI::uint bmx_cegui_multicolumnlist_getcolumnwithheadertext(CEGUI::MultiColumnList * mc, const CEGUI::utf8 * text);
	float bmx_cegui_multicolumnlist_gettotalcolumnheaderswidth(CEGUI::MultiColumnList * mc);
	float bmx_cegui_multicolumnlist_getcolumnheaderwidth(CEGUI::MultiColumnList * mc, CEGUI::uint colIdx);
	CEGUI::ListHeaderSegment::SortDirection bmx_cegui_multicolumnlist_getsortdirection(CEGUI::MultiColumnList * mc);
	MaxListHeaderSegment * bmx_cegui_multicolumnlist_getheadersegmentforcolumn(CEGUI::MultiColumnList * mc, CEGUI::uint colIdx);
	CEGUI::uint bmx_cegui_multicolumnlist_getitemrowindex(CEGUI::MultiColumnList * mc, const CEGUI::ListboxItem * item);
	CEGUI::uint bmx_cegui_multicolumnlist_getitemcolumnindex(CEGUI::MultiColumnList * mc, const CEGUI::ListboxItem * item);
	void bmx_cegui_multicolumnlist_getitemgridreference(CEGUI::MultiColumnList * mc, const CEGUI::ListboxItem * item, int * row, int * col);
	BBObject * bmx_cegui_multicolumnlist_getitematgridreference(CEGUI::MultiColumnList * mc, int row, int col);
	int bmx_cegui_multicolumnlist_islistboxitemincolumn(CEGUI::MultiColumnList * mc, const CEGUI::ListboxItem * item, CEGUI::uint colIdx);
	int bmx_cegui_multicolumnlist_islistboxiteminrow(CEGUI::MultiColumnList * mc, const CEGUI::ListboxItem * item, CEGUI::uint rowIdx);
	int bmx_cegui_multicolumnlist_islistboxiteminlist(CEGUI::MultiColumnList * mc, const CEGUI::ListboxItem * item);
	BBObject * bmx_cegui_multicolumnlist_findcolumnitemwithtext(CEGUI::MultiColumnList * mc, const CEGUI::utf8 * text, CEGUI::uint colIdx, const CEGUI::ListboxItem * startItem);
	BBObject * bmx_cegui_multicolumnlist_findrowitemwithtext(CEGUI::MultiColumnList * mc, const CEGUI::utf8 * text, CEGUI::uint rowIdx, const CEGUI::ListboxItem * startItem);
	BBObject * bmx_cegui_multicolumnlist_findlistitemwithtext(CEGUI::MultiColumnList * mc, const CEGUI::utf8 * text, const CEGUI::ListboxItem * startItem);
	BBObject * bmx_cegui_multicolumnlist_getfirstselecteditem(CEGUI::MultiColumnList * mc);
	BBObject * bmx_cegui_multicolumnlist_getnextselected(CEGUI::MultiColumnList * mc, const CEGUI::ListboxItem * startItem);
	CEGUI::uint bmx_cegui_multicolumnlist_getselectedcount(CEGUI::MultiColumnList * mc);
	int bmx_cegui_multicolumnlist_isitemselected(CEGUI::MultiColumnList * mc, int row, int col);
	CEGUI::uint bmx_cegui_multicolumnlist_getnominatedselectioncolumnid(CEGUI::MultiColumnList * mc);
	CEGUI::uint bmx_cegui_multicolumnlist_getnominatedselectioncolum(CEGUI::MultiColumnList * mc);
	CEGUI::uint bmx_cegui_multicolumnlist_getnominatedselectionrow(CEGUI::MultiColumnList * mc);
	CEGUI::MultiColumnList::SelectionMode bmx_cegui_multicolumnlist_getselectionmode(CEGUI::MultiColumnList * mc);
	int bmx_cegui_multicolumnlist_isvertscrollbaralwaysshown(CEGUI::MultiColumnList * mc);
	int bmx_cegui_multicolumnlist_ishorzscrollbaralwaysshown(CEGUI::MultiColumnList * mc);
	CEGUI::uint bmx_cegui_multicolumnlist_getcolumnid(CEGUI::MultiColumnList * mc, CEGUI::uint colIdx);
	CEGUI::uint bmx_cegui_multicolumnlist_getrowid(CEGUI::MultiColumnList * mc, CEGUI::uint rowIdx);
	CEGUI::uint bmx_cegui_multicolumnlist_getrowwithid(CEGUI::MultiColumnList * mc, CEGUI::uint rowId);
	void bmx_cegui_multicolumnlist_getlistrenderarea(CEGUI::MultiColumnList * mc, float * x, float * y, float * w, float * h);
	BBObject * bmx_cegui_multicolumnlist_getvertscrollbar(CEGUI::MultiColumnList * mc);
	BBObject * bmx_cegui_multicolumnlist_gethorzscrollbar(CEGUI::MultiColumnList * mc);
	BBObject * bmx_cegui_multicolumnlist_getlistheader(CEGUI::MultiColumnList * mc);
	float bmx_cegui_multicolumnlist_gettotalrowsheight(CEGUI::MultiColumnList * mc);
	float bmx_cegui_multicolumnlist_getwidestcolumnitemwidth(CEGUI::MultiColumnList * mc, CEGUI::uint colIdx);
	float bmx_cegui_multicolumnlist_gethighestrowitemheight(CEGUI::MultiColumnList * mc, CEGUI::uint rowIdx);
	void bmx_cegui_multicolumnlist_resetlist(CEGUI::MultiColumnList * mc);
	void bmx_cegui_multicolumnlist_addcolumn(CEGUI::MultiColumnList * mc, const CEGUI::utf8 * text, CEGUI::uint colId, float width);
	void bmx_cegui_multicolumnlist_addcolumnu(CEGUI::MultiColumnList * mc, const CEGUI::utf8 * text, CEGUI::uint colId, const CEGUI::UDim * width);
	void bmx_cegui_multicolumnlist_insertcolumn(CEGUI::MultiColumnList * mc, const CEGUI::utf8 * text, CEGUI::uint colId, float width, CEGUI::uint position);
	void bmx_cegui_multicolumnlist_insertcolumnu(CEGUI::MultiColumnList * mc, const CEGUI::utf8 * text, CEGUI::uint colId, const CEGUI::UDim * width, CEGUI::uint position);
	void bmx_cegui_multicolumnlist_removecolumn(CEGUI::MultiColumnList * mc, CEGUI::uint colIdx);
	void bmx_cegui_multicolumnlist_removecolumnwithid(CEGUI::MultiColumnList * mc, CEGUI::uint colId);
	void bmx_cegui_multicolumnlist_movecolumn(CEGUI::MultiColumnList * mc, CEGUI::uint colIdx, CEGUI::uint position);
	void bmx_cegui_multicolumnlist_movecolumnwithid(CEGUI::MultiColumnList * mc, CEGUI::uint colId, CEGUI::uint position);
	CEGUI::uint bmx_cegui_multicolumnlist_addrow(CEGUI::MultiColumnList * mc, CEGUI::uint rowId);
	CEGUI::uint bmx_cegui_multicolumnlist_addrowitem(CEGUI::MultiColumnList * mc, CEGUI::ListboxItem * item, CEGUI::uint colId, CEGUI::uint rowId);
	CEGUI::uint bmx_cegui_multicolumnlist_insertrow(CEGUI::MultiColumnList * mc, CEGUI::uint rowIdx, CEGUI::uint rowId);
	CEGUI::uint bmx_cegui_multicolumnlist_insertrowitem(CEGUI::MultiColumnList * mc, CEGUI::ListboxItem * item, CEGUI::uint colId, CEGUI::uint rowIdx, CEGUI::uint rowId);
	void bmx_cegui_multicolumnlist_removerow(CEGUI::MultiColumnList * mc, CEGUI::uint rowIdx);
	void bmx_cegui_multicolumnlist_setitemforgridref(CEGUI::MultiColumnList * mc, CEGUI::ListboxItem * item, int row, int col);
	void bmx_cegui_multicolumnlist_setitem(CEGUI::MultiColumnList * mc, CEGUI::ListboxItem * item, CEGUI::uint colId, CEGUI::uint rowIdx);
	void bmx_cegui_multicolumnlist_setselectionmode(CEGUI::MultiColumnList * mc, CEGUI::MultiColumnList::SelectionMode selMode);
	void bmx_cegui_multicolumnlist_setnominatedselectioncolumnid(CEGUI::MultiColumnList * mc, CEGUI::uint colId);
	void bmx_cegui_multicolumnlist_setnominatedselectioncolumn(CEGUI::MultiColumnList * mc, CEGUI::uint colIdx);
	void bmx_cegui_multicolumnlist_setnominatedselectionrow(CEGUI::MultiColumnList * mc, CEGUI::uint rowIdx);
	void bmx_cegui_multicolumnlist_setsortdirection(CEGUI::MultiColumnList * mc, CEGUI::ListHeaderSegment::SortDirection direction);
	void bmx_cegui_multicolumnlist_setsortcolumn(CEGUI::MultiColumnList * mc, CEGUI::uint colIdx);
	void bmx_cegui_multicolumnlist_setsortcolumnbyid(CEGUI::MultiColumnList * mc, CEGUI::uint colId);
	void bmx_cegui_multicolumnlist_setshowvertscrollbar(CEGUI::MultiColumnList * mc, int setting);
	void bmx_cegui_multicolumnlist_setshowhorzscrollbar(CEGUI::MultiColumnList * mc, int setting);
	void bmx_cegui_multicolumnlist_clearallselections(CEGUI::MultiColumnList * mc);
	void bmx_cegui_multicolumnlist_setitemselectstate(CEGUI::MultiColumnList * mc, CEGUI::ListboxItem * item, int state);
	void bmx_cegui_multicolumnlist_setitemselectstateforgridref(CEGUI::MultiColumnList * mc, int row, int col, int state);
	void bmx_cegui_multicolumnlist_handleupdateditemdata(CEGUI::MultiColumnList * mc);
	void bmx_cegui_multicolumnlist_setcolumnheaderwidth(CEGUI::MultiColumnList * mc, CEGUI::uint colIdx, float width);
	void bmx_cegui_multicolumnlist_setusersortcontrolenabled(CEGUI::MultiColumnList * mc, int setting);
	void bmx_cegui_multicolumnlist_setusercolumnsizingenabled(CEGUI::MultiColumnList * mc, int setting);
	void bmx_cegui_multicolumnlist_setusercolumndraggingenabled(CEGUI::MultiColumnList * mc, int setting);
	void bmx_cegui_multicolumnlist_autosizecolumnheader(CEGUI::MultiColumnList * mc, CEGUI::uint colIdx);
	void bmx_cegui_multicolumnlist_setrowid(CEGUI::MultiColumnList * mc, CEGUI::uint rowIdx, CEGUI::uint rowId);

	int bmx_cegui_dragcontainer_isdraggingenabled(CEGUI::DragContainer * dc);
	void bmx_cegui_dragcontainer_setdraggingenabled(CEGUI::DragContainer * dc, int setting);
	int bmx_cegui_dragcontainer_isbeingdragged(CEGUI::DragContainer * dc);
	float bmx_cegui_dragcontainer_getpixeldragthreshold(CEGUI::DragContainer * dc);
	void bmx_cegui_dragcontainer_setpixeldragthreshold(CEGUI::DragContainer * dc, float pixels);
	float bmx_cegui_dragcontainer_getdragalpha(CEGUI::DragContainer * dc);
	void bmx_cegui_dragcontainer_setdragalpha(CEGUI::DragContainer * dc, float alpha);
	BBObject * bmx_cegui_dragcontainer_getdragcursorimage(CEGUI::DragContainer * dc);
	void bmx_cegui_dragcontainer_setdragcursorimage(CEGUI::DragContainer * dc, CEGUI::Image * image);
	void bmx_cegui_dragcontainer_setdragcursorimagemode(CEGUI::DragContainer * dc, CEGUI::MouseCursorImage image);
	void bmx_cegui_dragcontainer_setdragcursorimagebyname(CEGUI::DragContainer * dc, const CEGUI::utf8 * imageset, const CEGUI::utf8 * image);
	BBObject * bmx_cegui_dragcontainer_getcurrentdroptarget(CEGUI::DragContainer * dc);

	MaxCEColour * bmx_cegui_colour_create(float red, float green, float blue, float alpha);
	void bmx_cegui_colour_delete(MaxCEColour * col);
	CEGUI::argb_t bmx_cegui_colour_getargb(MaxCEColour * col);
	float bmx_cegui_colour_getalpha(MaxCEColour * col);
	float bmx_cegui_colour_getred(MaxCEColour * col);
	float bmx_cegui_colour_getgreen(MaxCEColour * col);
	float bmx_cegui_colour_getblue(MaxCEColour * col);
	float bmx_cegui_colour_gethue(MaxCEColour * col);
	float bmx_cegui_colour_getsaturation(MaxCEColour * col);
	float bmx_cegui_colour_getlumination(MaxCEColour * col);
	void bmx_cegui_colour_setargb(MaxCEColour * col, CEGUI::argb_t argb);
	void bmx_cegui_colour_setalpha(MaxCEColour * col, float alpha);
	void bmx_cegui_colour_setred(MaxCEColour * col, float red);
	void bmx_cegui_colour_setgreen(MaxCEColour * col, float green);
	void bmx_cegui_colour_setblue(MaxCEColour * col, float blue);
	void bmx_cegui_colour_set(MaxCEColour * col, float red, float green, float blue, float alpha);
	void bmx_cegui_colour_setrgb(MaxCEColour * col, float red, float green, float blue);
	void bmx_cegui_colour_sethsl(MaxCEColour * col, float hue, float saturation, float luminance, float alpha);
	void bmx_cegui_colour_inverColour(MaxCEColour * col);
	void bmx_cegui_colour_invertColourWithAlpha(MaxCEColour * col);

	MaxCEColourRect * bmx_cegui_colourrect_create(MaxCEColour * col);
	void bmx_cegui_colourrect_setalpha(MaxCEColourRect * rect, float alpha);
	void bmx_cegui_colourrect_settopalpha(MaxCEColourRect * rect, float alpha);
	void bmx_cegui_colourrect_setbottomalpha(MaxCEColourRect * rect, float alpha);
	void bmx_cegui_colourrect_setleftalpha(MaxCEColourRect * rect, float alpha);
	void bmx_cegui_colourrect_setrightalpha(MaxCEColourRect * rect, float alpha);
	int bmx_cegui_colourrect_ismonochromatic(MaxCEColourRect * rect);
	MaxCEColourRect * bmx_cegui_colourrect_getsubrectangle(MaxCEColourRect * rect, float left, float right, float top, float bottom);
	MaxCEColour * bmx_cegui_colourrect_getcolouratpoint(MaxCEColourRect * rect, float x, float y);
	void bmx_cegui_colourrect_setcolours(MaxCEColourRect * rect, MaxCEColour * col);
	void bmx_cegui_colourrect_modulatealpha(MaxCEColourRect * rect, float alpha);
	void bmx_cegui_colourrect_settopleft(MaxCEColourRect * rect, MaxCEColour * col);
	void bmx_cegui_colourrect_settopright(MaxCEColourRect * rect, MaxCEColour * col);
	void bmx_cegui_colourrect_setbottomleft(MaxCEColourRect * rect, MaxCEColour * col);
	void bmx_cegui_colourrect_setbottomright(MaxCEColourRect * rect, MaxCEColour * col);
	MaxCEColour * bmx_cegui_colourrect_gettopleft(MaxCEColourRect * rect);
	MaxCEColour * bmx_cegui_colourrect_gettopright(MaxCEColourRect * rect);
	MaxCEColour * bmx_cegui_colourrect_getbottomleft(MaxCEColourRect * rect);
	MaxCEColour * bmx_cegui_colourrect_getbottomright(MaxCEColourRect * rect);
	void bmx_cegui_colourrect_delete(MaxCEColourRect * rect);

	float bmx_cegui_menubase_getitemspacing(CEGUI::MenuBase * base);
	int bmx_cegui_menubase_ismultiplepopupsallowed(CEGUI::MenuBase * base);
	BBObject * bmx_cegui_menubase_getpopupmenuitem(CEGUI::MenuBase * base);
	void bmx_cegui_menubase_setitemspacing(CEGUI::MenuBase * base, float spacing);
	void bmx_cegui_menubase_changepopupmenuitem(CEGUI::MenuBase * base, CEGUI::MenuItem * item);
	void bmx_cegui_menubase_setallowmultiplepopups(CEGUI::MenuBase * base, int setting);
	
	float bmx_cegui_popupmenu_getfadeintime(CEGUI::PopupMenu * menu);
	float bmx_cegui_popupmenu_getfadeouttime(CEGUI::PopupMenu * menu);
	int bmx_cegui_popupmenu_ispopupmenuopen(CEGUI::PopupMenu * menu);
	void bmx_cegui_popupmenu_setfadeintime(CEGUI::PopupMenu * menu, float fadetime);
	void bmx_cegui_popupmenu_setfadeouttime(CEGUI::PopupMenu * menu, float fadetime);
	void bmx_cegui_popupmenu_openpopupmenu(CEGUI::PopupMenu * menu, int _notify);
	void bmx_cegui_popupmenu_closepopupmenu(CEGUI::PopupMenu * menu, int _notify);

	void bmx_cegui_itementry_getitempixelsize(CEGUI::ItemEntry * entry, float * w, float * h);
	BBObject * bmx_cegui_itementry_getownerlist(CEGUI::ItemEntry * entry);
	int bmx_cegui_itementry_isselected(CEGUI::ItemEntry * entry);
	int bmx_cegui_itementry_isselectable(CEGUI::ItemEntry * entry);
	void bmx_cegui_itementry_setselected(CEGUI::ItemEntry * entry, int setting);
	void bmx_cegui_itementry_selectentry(CEGUI::ItemEntry * entry);
	void bmx_cegui_itementry_deselect(CEGUI::ItemEntry * entry);
	void bmx_cegui_itementry_setselectable(CEGUI::ItemEntry * entry, int setting);

	int bmx_cegui_menuitem_ishovering(CEGUI::MenuItem * item);
	int bmx_cegui_menuitem_ispushed(CEGUI::MenuItem * item);
	int bmx_cegui_menuitem_isopened(CEGUI::MenuItem * item);
	BBObject * bmx_cegui_menuitem_getpopupmenu(CEGUI::MenuItem * item);
	void bmx_cegui_menuitem_setpopupmenu(CEGUI::MenuItem * item, CEGUI::PopupMenu * popup);
	void bmx_cegui_menuitem_openpopupmenu(CEGUI::MenuItem * item, int _notify);
	void bmx_cegui_menuitem_closepopupmenu(CEGUI::MenuItem * item, int _notify);
	int bmx_cegui_menuitem_togglepopupmenu(CEGUI::MenuItem * item);

	void bmx_cegui_treeitem_settextcolours(CEGUI::TreeItem * ti, MaxCEColourRect * cols);
	void bmx_cegui_treeitem_settextcoloursforcorners(CEGUI::TreeItem * ti, MaxCEColour * topLeftColour, MaxCEColour * topRightColour, MaxCEColour * bottomLeftColour, MaxCEColour * bottomRightColour);
	void bmx_cegui_treeitem_settextcoloursforcolour(CEGUI::TreeItem * ti, MaxCEColour * col);
	BBString * bmx_cegui_treeitem_gettext(CEGUI::TreeItem * ti);
	BBString * bmx_cegui_treeitem_gettooltiptext(CEGUI::TreeItem * ti);
	CEGUI::uint bmx_cegui_treeitem_getid(CEGUI::TreeItem * ti);
	BBObject * bmx_cegui_treeitem_getuserdata(CEGUI::TreeItem * ti);
	int bmx_cegui_treeitem_isselected(CEGUI::TreeItem * ti);
	int bmx_cegui_treeitem_isdisabled(CEGUI::TreeItem * ti);
	int bmx_cegui_treeitem_isautodeleted(CEGUI::TreeItem * ti);
	BBObject * bmx_cegui_treeitem_getownerwindow(CEGUI::TreeItem * ti);
	MaxCEColourRect * bmx_cegui_treeitem_getselectioncolours(CEGUI::TreeItem * ti);
	BBObject * bmx_cegui_treeitem_getselectionbrushimage(CEGUI::TreeItem * ti);
	void bmx_cegui_treeitem_settext(CEGUI::TreeItem * ti, const CEGUI::utf8 * text);
	void bmx_cegui_treeitem_settooltiptext(CEGUI::TreeItem * ti, const CEGUI::utf8 * text);
	void bmx_cegui_treeitem_setid(CEGUI::TreeItem * ti, CEGUI::uint itemId);
	void bmx_cegui_treeitem_setuserdata(CEGUI::TreeItem * ti, BBObject * itemData);
	void bmx_cegui_treeitem_setselected(CEGUI::TreeItem * ti, int setting);
	void bmx_cegui_treeitem_setdisabled(CEGUI::TreeItem * ti, int setting);
	void bmx_cegui_treeitem_setautodeleted(CEGUI::TreeItem * ti, int setting);
	void bmx_cegui_treeitem_setownerwindow(CEGUI::TreeItem * ti, CEGUI::Window * owner);
	void bmx_cegui_treeitem_setselectioncolours(CEGUI::TreeItem * ti, MaxCEColourRect * cols);
	void bmx_cegui_treeitem_setselectioncoloursforcorners(CEGUI::TreeItem * ti, MaxCEColour * topLeftColour, MaxCEColour * topRightColour, MaxCEColour * bottomLeftColour, MaxCEColour * bottomRightColour);
	void bmx_cegui_treeitem_setselectioncoloursforcolour(CEGUI::TreeItem * ti, MaxCEColour * col);
	void bmx_cegui_treeitem_setselectionbrushimage(CEGUI::TreeItem * ti, CEGUI::Image * image);
	void bmx_cegui_treeitem_setselectionbrushimagebyname(CEGUI::TreeItem * ti, const CEGUI::utf8 * imageset, const CEGUI::utf8 * image);
	void bmx_cegui_treeitem_setbuttonlocation(CEGUI::TreeItem * ti, float x, float y, float w, float h);
	void bmx_cegui_treeitem_getbuttonlocation(CEGUI::TreeItem * ti, float * x, float * y, float * w, float * h);
	int bmx_cegui_treeitem_getisopen(CEGUI::TreeItem * ti);
	void bmx_cegui_treeitem_toggleisopen(CEGUI::TreeItem * ti);
	BBObject * bmx_cegui_treeitem_gettreeitemfromindex(CEGUI::TreeItem * ti, int itemIndex);
	int bmx_cegui_treeitem_getitemcount(CEGUI::TreeItem * ti);
	void bmx_cegui_treeitem_additem(CEGUI::TreeItem * ti, CEGUI::TreeItem * item);
	void bmx_cegui_treeitem_seticon(CEGUI::TreeItem * ti, CEGUI::Image * theIcon);
	void bmx_cegui_treeitem_getpixelsize(CEGUI::TreeItem * ti, float * width, float * height);

	CEGUI::Font * bmx_cegui_listboxtextitem_getfont(CEGUI::ListboxTextItem * item);
	MaxCEColourRect * bmx_cegui_listboxtextitem_gettextcolours(CEGUI::ListboxTextItem * item);
	void bmx_cegui_listboxtextitem_setfont(CEGUI::ListboxTextItem * item, CEGUI::Font * font);
	void bmx_cegui_listboxtextitem_setfontbyname(CEGUI::ListboxTextItem * item, const CEGUI::utf8 * fontName);
	void bmx_cegui_listboxtextitem_settextcolours(CEGUI::ListboxTextItem * item, MaxCEColourRect * cols);
	void bmx_cegui_listboxtextitem_settextcoloursforcorner(CEGUI::ListboxTextItem * item, MaxCEColour * topLeftColour, MaxCEColour * topRightColour, MaxCEColour * bottomLeftColour, MaxCEColour * bottomRightColour);
	void bmx_cegui_listboxtextitem_settextcoloursforcolour(CEGUI::ListboxTextItem * item, MaxCEColour * col);
	void bmx_cegui_listboxtextitem_getpixelsize(CEGUI::ListboxTextItem * item, float * width, float * height);
	CEGUI::ListboxTextItem * bmx_cegui_listboxtextitem_new(const CEGUI::utf8 * text, CEGUI::uint itemId, int disabled, int autoDelete);
	void bmx_cegui_listboxtextitem_delete(CEGUI::ListboxTextItem * item);

	BBObject * bmx_cegui_scrollablepane_getcontentpane(CEGUI::ScrollablePane * sp);
	int bmx_cegui_scrollablepane_isvertscrollbaralwaysshown(CEGUI::ScrollablePane * sp);
	void bmx_cegui_scrollablepane_setshowvertscrollbar(CEGUI::ScrollablePane * sp, int setting);
	int bmx_cegui_scrollablepane_ishorzscrollbaralwaysshown(CEGUI::ScrollablePane * sp);
	void bmx_cegui_scrollablepane_setshowhorzscrollbar(CEGUI::ScrollablePane * sp, int setting);
	int bmx_cegui_scrollablepane_iscontentpaneautosized(CEGUI::ScrollablePane * sp);
	void bmx_cegui_scrollablepane_setcontentpaneautosized(CEGUI::ScrollablePane * sp, int setting);
	void bmx_cegui_scrollablepane_getcontentpanearea(CEGUI::ScrollablePane * sp, float * x, float * y, float * w, float * h);
	void bmx_cegui_scrollablepane_setcontentpanearea(CEGUI::ScrollablePane * sp, float x, float y, float w, float h);
	float bmx_cegui_scrollablepane_gethorizontalstepsize(CEGUI::ScrollablePane * sp);
	void bmx_cegui_scrollablepane_sethorizontalstepsize(CEGUI::ScrollablePane * sp, float stepSize);
	float bmx_cegui_scrollablepane_gethorizontaloverlapsize(CEGUI::ScrollablePane * sp);
	void bmx_cegui_scrollablepane_sethorizontaloverlapsize(CEGUI::ScrollablePane * sp, float overlap);
	float bmx_cegui_scrollablepane_gethorizontalscrollposition(CEGUI::ScrollablePane * sp);
	void bmx_cegui_scrollablepane_sethorizontalscrollposition(CEGUI::ScrollablePane * sp, float position);
	float bmx_cegui_scrollablepane_getverticalstepsize(CEGUI::ScrollablePane * sp);
	void bmx_cegui_scrollablepane_setverticalstepsize(CEGUI::ScrollablePane * sp, float stepSize);
	float bmx_cegui_scrollablepane_getverticaloverlapsize(CEGUI::ScrollablePane * sp);
	void bmx_cegui_scrollablepane_setverticaloverlapsize(CEGUI::ScrollablePane * sp, float overlap);
	float bmx_cegui_scrollablepane_getverticalscrollposition(CEGUI::ScrollablePane * sp);
	void bmx_cegui_scrollablepane_setverticalscrollposition(CEGUI::ScrollablePane * sp, float position);
	void bmx_cegui_scrollablepane_getviewablearea(CEGUI::ScrollablePane * sp, float * x, float * y, float * w, float * h);
	BBObject * bmx_cegui_scrollablepane_getvertscrollbar(CEGUI::ScrollablePane * sp);
	BBObject * bmx_cegui_scrollablepane_gethorzscrollbar(CEGUI::ScrollablePane * sp);

	int bmx_cegui_tree_getitemcount(CEGUI::Tree * tree);
	int bmx_cegui_tree_getselectedcount(CEGUI::Tree * tree);
	BBObject * bmx_cegui_tree_getfirstselecteditem(CEGUI::Tree * tree);
	BBObject * bmx_cegui_tree_getlastselecteditem(CEGUI::Tree * tree);
	BBObject * bmx_cegui_tree_getnextselected(CEGUI::Tree * tree, CEGUI::TreeItem * startItem);
	int bmx_cegui_tree_issortenabled(CEGUI::Tree * tree);
	void bmx_cegui_tree_setitemrenderarea(CEGUI::Tree * tree, float x, float y, float w, float h);
	BBObject * bmx_cegui_tree_getvertscrollbar(CEGUI::Tree * tree);
	BBObject * bmx_cegui_tree_gethorzscrollbar(CEGUI::Tree * tree);
	int bmx_cegui_tree_ismultiselectenabled(CEGUI::Tree * tree);
	int bmx_cegui_tree_isitemtooltipsenabled(CEGUI::Tree * tree);
	BBObject * bmx_cegui_tree_findfirstitemwithtext(CEGUI::Tree * tree, const CEGUI::utf8 * text);
	BBObject * bmx_cegui_tree_findnextitemwithtext(CEGUI::Tree * tree, const CEGUI::utf8 * text, CEGUI::TreeItem * startItem);
	BBObject * bmx_cegui_tree_findfirstitemwithid(CEGUI::Tree * tree, CEGUI::uint searchID);
	BBObject * bmx_cegui_tree_findnextitemwithid(CEGUI::Tree * tree, CEGUI::uint searchID, CEGUI::TreeItem * startItem);
	int bmx_cegui_tree_istreeiteminlist(CEGUI::Tree * tree, CEGUI::TreeItem * item);
	int bmx_cegui_tree_isvertscrollbaralwaysshown(CEGUI::Tree * tree);
	int bmx_cegui_tree_ishorzscrollbaralwaysshown(CEGUI::Tree * tree);
	void bmx_cegui_tree_resetlist(CEGUI::Tree * tree);
	void bmx_cegui_tree_additem(CEGUI::Tree * tree, CEGUI::TreeItem * item);
	void bmx_cegui_tree_insertitem(CEGUI::Tree * tree, CEGUI::TreeItem * item, CEGUI::TreeItem * position);
	void bmx_cegui_tree_removeitem(CEGUI::Tree * tree, CEGUI::TreeItem * item);
	void bmx_cegui_tree_clearallselections(CEGUI::Tree * tree);
	void bmx_cegui_tree_setsortingenabled(CEGUI::Tree * tree, int setting);
	void bmx_cegui_tree_setmultiselectenabled(CEGUI::Tree * tree, int setting);
	void bmx_cegui_tree_setshowvertscrollbar(CEGUI::Tree * tree, int setting);
	void bmx_cegui_tree_setshowhorzscrollbar(CEGUI::Tree * tree, int setting);
	void bmx_cegui_tree_setitemtooltipsenabled(CEGUI::Tree * tree, int setting);
	void bmx_cegui_tree_setitemselectstate(CEGUI::Tree * tree, CEGUI::TreeItem * item, int state);
	void bmx_cegui_tree_setitemselectstateindex(CEGUI::Tree * tree, int itemIndex, int state);
	void bmx_cegui_tree_setlooknfeel(CEGUI::Tree * tree, const CEGUI::utf8 * look);
	void bmx_cegui_tree_handleupdateditemdata(CEGUI::Tree * tree);
	void bmx_cegui_tree_ensureitemisvisible(CEGUI::Tree * tree, CEGUI::TreeItem * item);

	void bmx_cegui_tabbutton_setselected(CEGUI::TabButton * button, int selected);
	int bmx_cegui_tabbutton_isselected(CEGUI::TabButton * button);
	void bmx_cegui_tabbutton_settargetwindow(CEGUI::TabButton * button, CEGUI::Window * wnd);
	BBObject * bmx_cegui_tabbutton_gettargetwindow(CEGUI::TabButton * button);

	MaxResourceProvider * bmx_cegui_resourceprovider_create(BBObject * handle);
	void bmx_cegui_resourceprovider_delete(CEGUI::ResourceProvider * provider);
	void bmx_cegui_rawdatacontainer_delete(MaxRawDataContainer * container);
	BBString * bmx_cegui_resourceprovider_getdefaultresourcegroup(CEGUI::ResourceProvider * provider);
	void bmx_cegui_resourceprovider_setdefaultresourcegroup(CEGUI::ResourceProvider * provider, CEGUI::utf8 * group);

	void bmx_cegui_defaultresourceprovider_setresourcegroupdirectory(CEGUI::DefaultResourceProvider * provider, CEGUI::utf8 * resourceGroup, CEGUI::utf8 * directory);
	BBString * bmx_cegui_defaultresourceprovider_getresourcegroupdirectory(CEGUI::DefaultResourceProvider * provider, CEGUI::utf8 * resourceGroup);
	void bmx_cegui_defaultresourceprovider_clearresourcegroupdirectory(CEGUI::DefaultResourceProvider * provider, CEGUI::utf8 * resourceGroup);

	void bmx_cegui_rawdatacontainer_setdata(MaxRawDataContainer * container, CEGUI::uint8 * data);
	CEGUI::uint8 * bmx_cegui_rawdatacontainer_getdataptr(MaxRawDataContainer * container);
	void bmx_cegui_rawdatacontainer_setsize(MaxRawDataContainer * container, int size);
	int bmx_cegui_rawdatacontainer_getsize(MaxRawDataContainer * container);

	int bmx_cegui_listbox_getitemcount(CEGUI::Listbox * lb);
	int bmx_cegui_listbox_getselectedcount(CEGUI::Listbox * lb);
	BBObject * bmx_cegui_listbox_getfirstselecteditem(CEGUI::Listbox * lb);
	BBObject * bmx_cegui_listbox_getnextselected(CEGUI::Listbox * lb, CEGUI::ListboxItem * startItem);
	BBObject * bmx_cegui_listbox_getlistboxitemfromindex(CEGUI::Listbox * lb, int index);
	int bmx_cegui_listbox_getitemindex(CEGUI::Listbox * lb, CEGUI::ListboxItem * item);
	int bmx_cegui_listbox_issortenabled(CEGUI::Listbox * lb);
	int bmx_cegui_listbox_ismultiselectenabled(CEGUI::Listbox * lb);
	int bmx_cegui_listbox_isitemtooltipsenabled(CEGUI::Listbox * lb);
	int bmx_cegui_listbox_isitemselected(CEGUI::Listbox * lb, int index);
	BBObject * bmx_cegui_listbox_finditemwithtext(CEGUI::Listbox * lb, const CEGUI::utf8 * text, CEGUI::ListboxItem * startItem);
	int bmx_cegui_listbox_islistboxiteminlist(CEGUI::Listbox * lb, CEGUI::ListboxItem * item);
	int bmx_cegui_listbox_isvertscrollbaralwaysshown(CEGUI::Listbox * lb);
	int bmx_cegui_listbox_ishorzscrollbaralwaysshown(CEGUI::Listbox * lb);
	void bmx_cegui_listbox_resetlist(CEGUI::Listbox * lb);
	void bmx_cegui_listbox_additem(CEGUI::Listbox * lb, CEGUI::ListboxItem * item);
	void bmx_cegui_listbox_insertitem(CEGUI::Listbox * lb, CEGUI::ListboxItem * item, CEGUI::ListboxItem * position);
	void bmx_cegui_listbox_removeitem(CEGUI::Listbox * lb, CEGUI::ListboxItem * item);
	void bmx_cegui_listbox_clearallselections(CEGUI::Listbox * lb);
	void bmx_cegui_listbox_setsortingenabled(CEGUI::Listbox * lb, int setting);
	void bmx_cegui_listbox_setmultiselectenabled(CEGUI::Listbox * lb, int setting);
	void bmx_cegui_listbox_setshowvertscrollbar(CEGUI::Listbox * lb, int setting);
	void bmx_cegui_listbox_setshowhorzscrollbar(CEGUI::Listbox * lb, int setting);
	void bmx_cegui_listbox_setitemtooltipsenabled(CEGUI::Listbox * lb, int setting);
	void bmx_cegui_listbox_setitemselectstate(CEGUI::Listbox * lb, CEGUI::ListboxItem * item, int state);
	void bmx_cegui_listbox_setitemselectstateforindex(CEGUI::Listbox * lb, int itemIndex, int state);
	void bmx_cegui_listbox_handleupdateditemdata(CEGUI::Listbox * lb);
	void bmx_cegui_listbox_ensureitemisvisibleforindex(CEGUI::Listbox * lb, int itemIndex);
	void bmx_cegui_listbox_ensureitemisvisible(CEGUI::Listbox * lb, CEGUI::ListboxItem * item);
	void bmx_cegui_listbox_getlistrenderarea(CEGUI::Listbox * lb, float * x, float * y, float * w, float * h);
	BBObject * bmx_cegui_listbox_getvertscrollbar(CEGUI::Listbox * lb);
	BBObject * bmx_cegui_listbox_gethorzscrollbar(CEGUI::Listbox * lb);
	float bmx_cegui_listbox_gettotalitemsheight(CEGUI::Listbox * lb);
	float bmx_cegui_listbox_getwidestitemwidth(CEGUI::Listbox * lb);

	int bmx_cegui_itemlistbase_getitemcount(CEGUI::ItemListBase * lb);
	BBObject * bmx_cegui_itemlistbase_getitemfromindex(CEGUI::ItemListBase * lb, int index);
	int bmx_cegui_itemlistbase_getitemindex(CEGUI::ItemListBase * lb, CEGUI::ItemEntry * item);
	BBObject * bmx_cegui_itemlistbase_finditemwithtext(CEGUI::ItemListBase * lb, CEGUI::uint8 * text, CEGUI::ItemEntry * startItem);
	int bmx_cegui_itemlistbase_isiteminlist(CEGUI::ItemListBase * lb, CEGUI::ItemEntry * item);
	int bmx_cegui_itemlistbase_isautoresizeenabled(CEGUI::ItemListBase * lb);
	int bmx_cegui_itemlistbase_issortenabled(CEGUI::ItemListBase * lb);
	CEGUI::ItemListBase::SortMode bmx_cegui_itemlistbase_getsortmode(CEGUI::ItemListBase * lb);
	void bmx_cegui_itemlistbase_resetlist(CEGUI::ItemListBase * lb);
	void bmx_cegui_itemlistbase_additem(CEGUI::ItemListBase * lb, CEGUI::ItemEntry * item);
	void bmx_cegui_itemlistbase_insertitem(CEGUI::ItemListBase * lb, CEGUI::ItemEntry * item, CEGUI::ItemEntry * position);
	void bmx_cegui_itemlistbase_removeitem(CEGUI::ItemListBase * lb, CEGUI::ItemEntry * item);
	void bmx_cegui_itemlistbase_handleupdateditemdata(CEGUI::ItemListBase * lb, int resort);
	void bmx_cegui_itemlistbase_setautoresizeenabled(CEGUI::ItemListBase * lb, int setting);
	void bmx_cegui_itemlistbase_sizetocontent(CEGUI::ItemListBase * lb);
	void bmx_cegui_itemlistbase_endinitialisation(CEGUI::ItemListBase * lb);
	void bmx_cegui_itemlistbase_performchildwindowlayout(CEGUI::ItemListBase * lb);
	void bmx_cegui_itemlistbase_getitemrenderarea(CEGUI::ItemListBase * lb, float * x, float * y, float * w, float * h);
	BBObject * bmx_cegui_itemlistbase_getcontentpane(CEGUI::ItemListBase * lb);
	void bmx_cegui_itemlistbase_setsortenabled(CEGUI::ItemListBase * lb, int setting);
	void bmx_cegui_itemlistbase_setsortmode(CEGUI::ItemListBase * lb, CEGUI::ItemListBase::SortMode mode);
	void bmx_cegui_itemlistbase_sortlist(CEGUI::ItemListBase * lb, int relayout);

	CEGUI::GlobalEventSet * bmx_cegui_globaleventset_getinstance();
	MaxConnection * bmx_cegui_globaleventset_subscribeevent(CEGUI::GlobalEventSet * handle, const CEGUI::utf8 * name, MaxCEEventCallback * cb);

	MaxWindowFactory * bmx_cegui_windowfactory_create(BBObject * handle, const CEGUI::utf8 * type);

	int bmx_cegui_scrolledcontainer_iscontentpaneautosized(CEGUI::ScrolledContainer * sc);
	void bmx_cegui_scrolledcontainer_setcontentpaneautosized(CEGUI::ScrolledContainer * sc, int setting);
	void bmx_cegui_scrolledcontainer_getcontentarea(CEGUI::ScrolledContainer * sc, float * x, float * y, float * w, float * h);
	void bmx_cegui_scrolledcontainer_setcontentarea(CEGUI::ScrolledContainer * sc, float x, float y, float w, float h);
	void bmx_cegui_scrolledcontainer_getchildextentsarea(CEGUI::ScrolledContainer * sc, float * x, float * y, float * w, float * h);
	
	void bmx_cegui_clippedcontainer_getcliparea(CEGUI::ClippedContainer * cc, float * x, float * y, float * w, float * h);
	BBObject * bmx_cegui_clippedcontainer_getclipperwindow(CEGUI::ClippedContainer * cc);
	void bmx_cegui_clippedcontainer_setcliparea(CEGUI::ClippedContainer * cc, float x, float y, float w, float h);
	void bmx_cegui_clippedcontainer_setclipperwindow(CEGUI::ClippedContainer * cc, CEGUI::Window * w);
	
	CEGUI::MouseCursor * bmx_cegui_mousecursor_getsingleton();
	void bmx_cegui_mousecursor_setimagewithname(CEGUI::MouseCursor * mc, const CEGUI::utf8 * imageset, const CEGUI::utf8 * imageName);
	void bmx_cegui_mousecursor_setimage(CEGUI::MouseCursor * mc, CEGUI::Image * image);
	BBObject * bmx_cegui_mousecursor_getimage(CEGUI::MouseCursor * mc);
	void bmx_cegui_mousecursor_draw(CEGUI::MouseCursor * mc);
	void bmx_cegui_mousecursor_setposition(CEGUI::MouseCursor * mc, float x, float y);
	void bmx_cegui_mousecursor_offsetPosition(CEGUI::MouseCursor * mc, float x, float y);
	void bmx_cegui_mousecursor_setconstraintarea(CEGUI::MouseCursor * mc, float x, float y, float w, float h);
	void bmx_cegui_mousecursor_hide(CEGUI::MouseCursor * mc);
	void bmx_cegui_mousecursor_show(CEGUI::MouseCursor * mc);
	void bmx_cegui_mousecursor_setvisible(CEGUI::MouseCursor * mc, int visible);
	int bmx_cegui_mousecursor_isvisible(CEGUI::MouseCursor * mc);
	void bmx_cegui_mousecursor_getposition(CEGUI::MouseCursor * mc, float * x, float * y);
	void bmx_cegui_mousecursor_getconstraintarea(CEGUI::MouseCursor * mc, float * x, float * y, float * w, float * h);
	void bmx_cegui_mousecursor_getdisplayindependentposition(CEGUI::MouseCursor * mc, float * x, float * y);
	
	float bmx_cegui_coordconverter_windowtoscreenx(CEGUI::Window * window, float x);
	float bmx_cegui_coordconverter_windowtoscreeny(CEGUI::Window * window, float y);
	void bmx_cegui_coordconverter_windowtoscreen(CEGUI::Window * window, float x, float y, float * toX, float * toY);
	void bmx_cegui_coordconverter_windowtoscreenrect(CEGUI::Window * window, float x, float y, float w, float h, float * toX, float * toY, float * toW, float * toH);
	float bmx_cegui_coordconverter_screentowindowx(CEGUI::Window * window, float x);
	float bmx_cegui_coordconverter_screentowindowy(CEGUI::Window * window, float y);
	void bmx_cegui_coordconverter_screentowindow(CEGUI::Window * window, float x, float y, float * toX, float * toY);
	void bmx_cegui_coordconverter_screentowindowrect(CEGUI::Window * window, float x, float y, float w, float h, float * toX, float * toY, float * toW, float * toH);

	int bmx_cegui_combobox_ishit(CEGUI::Combobox * cb, float x, float y, int allowDisabled);
	int bmx_cegui_combobox_getsingleclickenabled(CEGUI::Combobox * cb);
	int bmx_cegui_combobox_isdropdownlistvisible(CEGUI::Combobox * cb);
	BBObject * bmx_cegui_combobox_geteditbox(CEGUI::Combobox * cb);
	BBObject * bmx_cegui_combobox_getpushbutton(CEGUI::Combobox * cb);
	BBObject * bmx_cegui_combobox_getdroplist(CEGUI::Combobox * cb);
	int bmx_cegui_combobox_hasinputfocus(CEGUI::Combobox * cb);
	int bmx_cegui_combobox_isreadonly(CEGUI::Combobox * cb);
	int bmx_cegui_combobox_istextvalid(CEGUI::Combobox * cb);
	BBString * bmx_cegui_combobox_getvalidationstring(CEGUI::Combobox * cb);
	int bmx_cegui_combobox_getcaretindex(CEGUI::Combobox * cb);
	int bmx_cegui_combobox_getselectionstartindex(CEGUI::Combobox * cb);
	int bmx_cegui_combobox_getselectionendindex(CEGUI::Combobox * cb);
	int bmx_cegui_combobox_getselectionlength(CEGUI::Combobox * cb);
	int bmx_cegui_combobox_getmaxtextlength(CEGUI::Combobox * cb);
	int bmx_cegui_combobox_getitemcount(CEGUI::Combobox * cb);
	BBObject * bmx_cegui_combobox_getselecteditem(CEGUI::Combobox * cb);
	BBObject * bmx_cegui_combobox_getlistboxitemfromindex(CEGUI::Combobox * cb, int index);
	int bmx_cegui_combobox_getitemindex(CEGUI::Combobox * cb, CEGUI::ListboxItem * item);
	int bmx_cegui_combobox_issortenabled(CEGUI::Combobox * cb);
	int bmx_cegui_combobox_isitemselected(CEGUI::Combobox * cb, int index);
	BBObject * bmx_cegui_combobox_finditemwithtext(CEGUI::Combobox * cb, const CEGUI::utf8 * text, CEGUI::ListboxItem * startItem);
	int bmx_cegui_combobox_islistboxiteminlist(CEGUI::Combobox * cb, CEGUI::ListboxItem * item);
	int bmx_cegui_combobox_isvertscrollbaralwaysshown(CEGUI::Combobox * cb);
	int bmx_cegui_combobox_ishorzscrollbaralwaysshown(CEGUI::Combobox * cb);
	void bmx_cegui_combobox_showdroplist(CEGUI::Combobox * cb);
	void bmx_cegui_combobox_hidedroplist(CEGUI::Combobox * cb);
	void bmx_cegui_combobox_setsingleclickenabled(CEGUI::Combobox * cb, int setting);
	void bmx_cegui_combobox_setreadonly(CEGUI::Combobox * cb, int setting);
	void bmx_cegui_combobox_setvalidationstring(CEGUI::Combobox * cb, const CEGUI::utf8 * validationString);
	void bmx_cegui_combobox_setcaretindex(CEGUI::Combobox * cb, int caretPos);
	void bmx_cegui_combobox_setselection(CEGUI::Combobox * cb, int startPos, int endPos);
	void bmx_cegui_combobox_setmaxtextlength(CEGUI::Combobox * cb, int maxLen);
	void bmx_cegui_combobox_activateeditbox(CEGUI::Combobox * cb);
	void bmx_cegui_combobox_resetlist(CEGUI::Combobox * cb);
	void bmx_cegui_combobox_additem(CEGUI::Combobox * cb, CEGUI::ListboxItem * item);
	void bmx_cegui_combobox_insertitem(CEGUI::Combobox * cb, CEGUI::ListboxItem * item, CEGUI::ListboxItem * position);
	void bmx_cegui_combobox_removeitem(CEGUI::Combobox * cb, CEGUI::ListboxItem * item);
	void bmx_cegui_combobox_clearallselections(CEGUI::Combobox * cb);
	void bmx_cegui_combobox_setsortingenabled(CEGUI::Combobox * cb, int setting);
	void bmx_cegui_combobox_setshowvertscrollbar(CEGUI::Combobox * cb, int setting);
	void bmx_cegui_combobox_setshowhorzscrollbar(CEGUI::Combobox * cb, int setting);
	void bmx_cegui_combobox_setitemselectstate(CEGUI::Combobox * cb, CEGUI::ListboxItem * item, int state);
	void bmx_cegui_combobox_setitemselectstateindex(CEGUI::Combobox * cb, int itemIndex, int state);
	void bmx_cegui_combobox_handleupdatedlistitemdata(CEGUI::Combobox * cb);

	CEGUI::UDim * bmx_cegui_udim_create(float scale, float offset);
	void bmx_cegui_udim_delete(CEGUI::UDim * udim);

	void bmx_cegui_image_getsize(CEGUI::Image * image, float * width, float * height);
	float bmx_cegui_image_getwidth(CEGUI::Image * image);
	float bmx_cegui_image_getheight(CEGUI::Image * image);
	void bmx_cegui_image_getoffsets(CEGUI::Image * image, float * x, float * y);
	float bmx_cegui_image_getoffsetx(CEGUI::Image * image);
	float bmx_cegui_image_getoffsety(CEGUI::Image * image);
	BBString * bmx_cegui_image_getname(CEGUI::Image * image);
	BBString * bmx_cegui_image_getimagesetname(CEGUI::Image * image);
	const CEGUI::Imageset * bmx_cegui_image_getimageset(CEGUI::Image * image);
	void bmx_cegui_image_getsourcetexturearea(CEGUI::Image * image, float * x, float * y, float * w, float * h);

	int bmx_cegui_font_iscodepointavailable(CEGUI::Font * font, CEGUI::utf32 cp);
	float bmx_cegui_font_getlinespacing(CEGUI::Font * font, float yScale);
	float bmx_cegui_font_getfontheight(CEGUI::Font * font, float yScale);
	float bmx_cegui_font_getbaseline(CEGUI::Font * font, float yScale);
	void bmx_cegui_font_removeproperty(CEGUI::Font * font, const CEGUI::utf8 * name);
	void bmx_cegui_font_clearproperties(CEGUI::Font * font);
	int bmx_cegui_font_ispropertypresent(CEGUI::Font * font, const CEGUI::utf8 * name);
	BBString * bmx_cegui_font_getpropertyhelp(CEGUI::Font * font, const CEGUI::utf8 * name);
	BBString * bmx_cegui_font_getproperty(CEGUI::Font * font, const CEGUI::utf8 * name);
	void bmx_cegui_font_setproperty(CEGUI::Font * font, const CEGUI::utf8 * name, const CEGUI::utf8 * value);
	int bmx_cegui_font_ispropertydefault(CEGUI::Font * font, const CEGUI::utf8 * name);
	BBString * bmx_cegui_font_getpropertydefault(CEGUI::Font * font, const CEGUI::utf8 * name);
	int bmx_cegui_font_getpropertyasbool(CEGUI::Font * font, const CEGUI::utf8 * name);
	int bmx_cegui_font_getpropertyasint(CEGUI::Font * font, const CEGUI::utf8 * name);
	void bmx_cegui_font_setpropertyasbool(CEGUI::Font * font, const CEGUI::utf8 * name, int value);

	void bmx_cegui_font_setdefaultresourcegroup(const CEGUI::utf8 * resourceGroup);
	BBString * bmx_cegui_font_getdefaultresourcegroup();

	BBObject * bmx_cegui_dragdropeventargs_getdragdropitem(CEGUI::DragDropEventArgs * args);
	BBObject * bmx_cegui_mousecursoreventargs_getimage(CEGUI::MouseCursorEventArgs * args);
	CEGUI::utf32 bmx_cegui_keyeventargs_getcodepoint(CEGUI::KeyEventArgs * args);
	CEGUI::Key::Scan bmx_cegui_keyeventargs_getscancode(CEGUI::KeyEventArgs * args);
	CEGUI::uint bmx_cegui_keyeventargs_getsyskeys(CEGUI::KeyEventArgs * args);
	void bmx_cegui_mouseeventargs_getposition(CEGUI::MouseEventArgs * args, float * x, float * y);
	void bmx_cegui_mouseeventargs_getmovedelta(CEGUI::MouseEventArgs * args, float * x, float * y);
	CEGUI::MouseButton bmx_cegui_mouseeventargs_getbutton(CEGUI::MouseEventArgs * args);
	CEGUI::uint bmx_cegui_mouseeventargs_getsyskeys(CEGUI::MouseEventArgs * args);
	float bmx_cegui_mouseeventargs_getwheelchange(CEGUI::MouseEventArgs * args);
	CEGUI::uint bmx_cegui_mouseeventargs_getclickcount(CEGUI::MouseEventArgs * args);
	CEGUI::uint bmx_cegui_headersequenceeventargs_getoldindex(CEGUI::HeaderSequenceEventArgs * args);
	CEGUI::uint bmx_cegui_headersequenceeventargs_getnewindex(CEGUI::HeaderSequenceEventArgs * args);
	BBObject * bmx_cegui_treeeventargs_gettreeitem(CEGUI::TreeEventArgs * args);

	BBString * bmx_cegui_imageset_getname(CEGUI::Imageset * is);
	CEGUI::uint bmx_cegui_imageset_getimagecount(CEGUI::Imageset * is);
	int bmx_cegui_imageset_isimagedefined(CEGUI::Imageset * is, const CEGUI::utf8 * name);
	void bmx_cegui_imageset_undefineimage(CEGUI::Imageset * is, const CEGUI::utf8 * name);
	void bmx_cegui_imageset_undefineallimages(CEGUI::Imageset * is);
	void bmx_cegui_imageset_getimagesize(CEGUI::Imageset * is, const CEGUI::utf8 * name, float * width, float * height);
	float bmx_cegui_imageset_getimagewidth(CEGUI::Imageset * is, const CEGUI::utf8 * name);
	float bmx_cegui_imageset_getimageheight(CEGUI::Imageset * is, const CEGUI::utf8 * name);
	void bmx_cegui_imageset_getimageoffset(CEGUI::Imageset * is, const CEGUI::utf8 * name, float * x, float * y);
	float bmx_cegui_imageset_getimageoffsetx(CEGUI::Imageset * is, const CEGUI::utf8 * name);
	float bmx_cegui_imageset_getimageoffsety(CEGUI::Imageset * is, const CEGUI::utf8 * name);
	void bmx_cegui_imageset_defineimage(CEGUI::Imageset * is, const CEGUI::utf8 * name, float x, float y, float width, float height, float renderOffsetX, float renderOffsetY);
	int bmx_cegui_imageset_isautoscaled(CEGUI::Imageset * is);
	void bmx_cegui_imageset_getnativeresoultion(CEGUI::Imageset * is, float * width, float * height);
	void bmx_cegui_imageset_setautoscalingenabled(CEGUI::Imageset * is, int setting);
	void bmx_cegui_imageset_setnativeresolution(CEGUI::Imageset * is, float width, float height);
	void bmx_cegui_imageset_notifydisplaysizechanged(CEGUI::Imageset * is, float width, float height);
	void bmx_cegui_imageset_setdefaultresourcegroup(const CEGUI::utf8 * resourceGroup);
	BBString * bmx_cegui_imageset_getdefaultresourcegroup();

	BBObject * bmx_cegui_groupbox_getcontentpane(CEGUI::GroupBox * box);

	int bmx_cegui_listheadersegment_issizingenabled(CEGUI::ListHeaderSegment * seg);
	CEGUI::ListHeaderSegment::SortDirection bmx_cegui_listheadersegment_getsortdirection(CEGUI::ListHeaderSegment * seg);
	int bmx_cegui_listheadersegment_isdragmovingenabled(CEGUI::ListHeaderSegment * seg);
	void bmx_cegui_listheadersegment_getdragmoveoffset(CEGUI::ListHeaderSegment * seg, float * x, float * y);
	int bmx_cegui_listheadersegment_isclickable(CEGUI::ListHeaderSegment * seg);
	int bmx_cegui_listheadersegment_issegmenthovering(CEGUI::ListHeaderSegment * seg);
	int bmx_cegui_listheadersegment_issegmentpushed(CEGUI::ListHeaderSegment * seg);
	int bmx_cegui_listheadersegment_issplitterhovering(CEGUI::ListHeaderSegment * seg);
	int bmx_cegui_listheadersegment_isbeingdragmoved(CEGUI::ListHeaderSegment * seg);
	int bmx_cegui_listheadersegment_isbeingdragsized(CEGUI::ListHeaderSegment * seg);
	BBObject * bmx_cegui_listheadersegment_getsizingcursorimage(CEGUI::ListHeaderSegment * seg);
	BBObject * bmx_cegui_listheadersegment_getmovingcursorimage(CEGUI::ListHeaderSegment * seg);
	void bmx_cegui_listheadersegment_setsizingenabled(CEGUI::ListHeaderSegment * seg, int setting);
	void bmx_cegui_listheadersegment_setsortdirection(CEGUI::ListHeaderSegment * seg, CEGUI::ListHeaderSegment::SortDirection sortDir);
	void bmx_cegui_listheadersegment_setdragmovingenabled(CEGUI::ListHeaderSegment * seg, int setting);
	void bmx_cegui_listheadersegment_setclickable(CEGUI::ListHeaderSegment * seg, int setting);


	CEGUI::uint bmx_cegui_listheader_getcolumncount(CEGUI::ListHeader * head);
	BBObject * bmx_cegui_listheader_getsegmentfromcolumn(CEGUI::ListHeader * head, CEGUI::uint column);
	BBObject * bmx_cegui_listheader_getsegmentfromid(CEGUI::ListHeader * head, CEGUI::uint id);
	BBObject * bmx_cegui_listheader_getsortsegment(CEGUI::ListHeader * head);
	CEGUI::uint bmx_cegui_listheader_getcolumnfromsegment(CEGUI::ListHeader * head, CEGUI::ListHeaderSegment * segment);
	CEGUI::uint bmx_cegui_listheader_getcolumnfromid(CEGUI::ListHeader * head, CEGUI::uint id);
	CEGUI::uint bmx_cegui_listheader_getsortcolumn(CEGUI::ListHeader * head);
	CEGUI::uint bmx_cegui_listheader_getcolumnwithtext(CEGUI::ListHeader * head, const CEGUI::utf8 * text);
	float bmx_cegui_listheader_getpixeloffsettosegment(CEGUI::ListHeader * head, CEGUI::ListHeaderSegment * segment);
	float bmx_cegui_listheader_getpixeloffsettocolumn(CEGUI::ListHeader * head, CEGUI::uint column);
	float bmx_cegui_listheader_gettotalsegmentspixelextent(CEGUI::ListHeader * head);
	float bmx_cegui_listheader_getcolumnwidth(CEGUI::ListHeader * head, CEGUI::uint column);
	CEGUI::ListHeaderSegment::SortDirection bmx_cegui_listheader_getsortdirection(CEGUI::ListHeader * head);
	int bmx_cegui_listheader_issortingenabled(CEGUI::ListHeader * head);
	int bmx_cegui_listheader_iscolumnsizingenabled(CEGUI::ListHeader * head);
	int bmx_cegui_listheader_iscolumndraggingenabled(CEGUI::ListHeader * head);
	float bmx_cegui_listheader_getsegmentoffset(CEGUI::ListHeader * head);
	void bmx_cegui_listheader_setsortingenabled(CEGUI::ListHeader * head, int setting);
	void bmx_cegui_listheader_setsortdirection(CEGUI::ListHeader * head, CEGUI::ListHeaderSegment::SortDirection direction);
	void bmx_cegui_listheader_setsortsegment(CEGUI::ListHeader * head, CEGUI::ListHeaderSegment * segment);
	void bmx_cegui_listheader_setsortcolumn(CEGUI::ListHeader * head, CEGUI::uint column);
	void bmx_cegui_listheader_setsortcolumnfromid(CEGUI::ListHeader * head, CEGUI::uint id);
	void bmx_cegui_listheader_setcolumnsizingenabled(CEGUI::ListHeader * head, int setting);
	void bmx_cegui_listheader_setcolumndraggingenabled(CEGUI::ListHeader * head, int setting);
	void bmx_cegui_listheader_addcolumn(CEGUI::ListHeader * head, const CEGUI::utf8 * text, CEGUI::uint id, float width);
	void bmx_cegui_listheader_insertcolumn(CEGUI::ListHeader * head, const CEGUI::utf8 * text, CEGUI::uint id, float width, CEGUI::uint position);
	void bmx_cegui_listheader_insertcolumnatsegment(CEGUI::ListHeader * head, const CEGUI::utf8 * text, CEGUI::uint id, float width, CEGUI::ListHeaderSegment * position);
	void bmx_cegui_listheader_removecolumn(CEGUI::ListHeader * head, CEGUI::uint column);
	void bmx_cegui_listheader_removesegment(CEGUI::ListHeader * head, CEGUI::ListHeaderSegment * segment);
	void bmx_cegui_listheader_movecolumn(CEGUI::ListHeader * head, CEGUI::uint column, CEGUI::uint position);
	void bmx_cegui_listheader_movecolumnatsegment(CEGUI::ListHeader * head, CEGUI::uint column, CEGUI::ListHeaderSegment * position);
	void bmx_cegui_listheader_movesegment(CEGUI::ListHeader * head, CEGUI::ListHeaderSegment * segment, CEGUI::uint position);
	void bmx_cegui_listheader_movesegmentatsegment(CEGUI::ListHeader * head, CEGUI::ListHeaderSegment * segment, CEGUI::ListHeaderSegment * position);
	void bmx_cegui_listheader_setsegmentoffset(CEGUI::ListHeader * head, float offset);
	void bmx_cegui_listheader_setcolumnwidth(CEGUI::ListHeader * head, CEGUI::uint column, float width);

	BBObject * bmx_cegui_schememanager_createscheme(const CEGUI::utf8 * filename, const CEGUI::utf8 * resourceGroup, CEGUI::XMLResourceExistsAction action);
	void bmx_cegui_schememanager_destroyname(const CEGUI::utf8 * scheme);
	void bmx_cegui_schememanager_destroyobj(CEGUI::Scheme * scheme);
	void bmx_cegui_schememanager_destroyall();
	BBObject * bmx_cegui_schememanager_get(const CEGUI::utf8 * name);
	int bmx_cegui_schememanager_isdefined(const CEGUI::utf8 * name);
	void bmx_cegui_schememanager_createall(const CEGUI::utf8 * pattern, const CEGUI::utf8 * resourceGroup);

	BBString * bmx_cegui_widgetlookmanager_getdefaultresourcegroup();
	void bmx_cegui_widgetlookmanager_setdefaultresourcegroup(const CEGUI::utf8 * resourceGroup);

	void bmx_cegui_renderedstring_free(MaxRenderedString * s);

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
	MaxConnection * subscribeEvent(CEGUI::GlobalEventSet * handle, const CEGUI::utf8 * name);

private:
	BBObject *callbackHandle;
	CEGUI::Event::Connection conn;
};

class MaxLogger : public CEGUI::Logger
{
public:
	MaxLogger(BBObject * handle);
	void logEvent(const CEGUI::String& message, CEGUI::LoggingLevel level = CEGUI::Standard);
	void setLogFilename(const CEGUI::String&, bool);

private:
	BBObject * maxHandle;
};

class MaxCEColour
{
public:
	MaxCEColour(CEGUI::colour col);
	~MaxCEColour();

	CEGUI::colour Colour();
	
private:
	CEGUI::colour colour;
};

class MaxCEColourRect
{
public:
	MaxCEColourRect(CEGUI::ColourRect col);
	~MaxCEColourRect();

	CEGUI::ColourRect Rect();
	
private:
	CEGUI::ColourRect rect;
};

class MaxResourceProvider : public CEGUI::ResourceProvider
{
public:
	MaxResourceProvider(BBObject * handle);
	virtual ~MaxResourceProvider();
	virtual void loadRawDataContainer(const CEGUI::String &filename, CEGUI::RawDataContainer &output, const CEGUI::String &resourceGroup);
	virtual void unloadRawDataContainer(CEGUI::RawDataContainer &data);
	virtual size_t getResourceGroupFileNames(std::vector<CEGUI::String>& out_vec, const CEGUI::String& file_pattern, const CEGUI::String& resource_group);
	
private:
	BBObject * maxHandle;
};

class MaxRawDataContainer
{
public:
	MaxRawDataContainer(CEGUI::RawDataContainer & container);
	
	CEGUI::RawDataContainer & Container();

private:
	CEGUI::RawDataContainer & container;
};

class MaxWindowFactory : public CEGUI::WindowFactory
{
public:
	MaxWindowFactory(BBObject * handle, const CEGUI::utf8 * typeName);
	~MaxWindowFactory();

	virtual CEGUI::Window * createWindow(const CEGUI::String &name);
	virtual void destroyWindow(CEGUI::Window *window);
	const CEGUI::String & getTypeName();

private:
	BBObject * maxHandle;
};

class MaxRenderer
{
public:
	MaxRenderer();
	virtual CEGUI::Renderer & Renderer() {} ;
};

class MaxRenderedString
{
public:
	MaxRenderedString(const CEGUI::RenderedString & s);
	~MaxRenderedString();
	
	CEGUI::RenderedString & String();

private:
	CEGUI::RenderedString str;
};

