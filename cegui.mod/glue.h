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
#include "CEGUIDefaultLogger.h"

class MaxCEEventCallback;
class MaxEventArgs;
class MaxConnection;
class MaxLogger;
class MaxListHeaderSegment;
class MaxCEColour;
class MaxCEColourRect;

extern "C" {

#include "blitz.h"
#include "keycodes.h"

	bool _bah_cegui_TCEEventCallback__callback(BBObject * cb, BBObject * args);
	void _bah_cegui_TCECustomLogger__logEvent(BBObject * handle, const CEGUI::utf8 * message, CEGUI::LoggingLevel level);

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
	
	BBObject * _bah_cegui_TCETexture__create(CEGUI::Texture * texture);
	BBObject * _bah_cegui_TCEListboxItem__create(CEGUI::ListboxItem * item);
	BBObject * _bah_cegui_TCEImage__create(CEGUI::Image * image);
	BBObject * _bah_cegui_TCETreeItem__create(CEGUI::TreeItem * item);
	
	BBObject * newObjectForEventArgs(CEGUI::EventArgs * args, MaxEventArgs * maxArgs);
	
	CEGUI::Renderer * bmx_cegui_new_oglrenderer();
	CEGUI::System * bmx_cegui_new_system(CEGUI::Renderer * r);
	void bmx_cegui_delete_system(CEGUI::System * s);
	void bmx_cegui_delete_renderer(CEGUI::Renderer * r);

	CEGUI::Scheme * bmx_cegui_schememanager_loadScheme(const CEGUI::utf8 * scheme, const CEGUI::utf8 * resourceGroup);
	bool bmx_cegui_schememanager_isschemepresent(const CEGUI::utf8 * scheme);
	CEGUI::Scheme * bmx_cegui_schememanager_getscheme(const CEGUI::utf8 * name);
	void bmx_cegui_schememanager_unloadallschemes();
	
	void bmx_cegui_system_setDefaultFont(CEGUI::System * sys, const CEGUI::utf8 * font);
	void bmx_cegui_system_setDefaultMouseCursor(CEGUI::System * sys, const CEGUI::utf8 * l, const CEGUI::utf8 * kind);
	void bmx_cegui_system_renderGUI(CEGUI::System * sys);
	void bmx_cegui_system_setGUISheet(CEGUI::System * sys, CEGUI::Window * window);
	bool bmx_cegui_system_injectTimePulse(CEGUI::System * sys, float t);
	bool bmx_cegui_system_injectMousePosition(CEGUI::System * sys, int x, int y);
	bool bmx_cegui_system_injectMouseButtonDown(CEGUI::System * sys, int button);
	bool bmx_cegui_system_injectMouseButtonUp(CEGUI::System * sys, int button);
	bool bmx_cegui_system_injectMouseWheelChange(CEGUI::System * sys, int delta);
	CEGUI::uint bmx_cegui_mapmaxtocekey(int key);
	bool bmx_cegui_system_injectkeydown(CEGUI::System * sys, int key);
	bool bmx_cegui_system_injectkeyup(CEGUI::System * sys, int key);
	bool bmx_cegui_system_injectchar(CEGUI::System * sys, CEGUI::utf32 key);
	CEGUI::Font * bmx_cegui_system_getDefaultFont(CEGUI::System * sys);
	void bmx_cegui_system_signalredraw(CEGUI::System * sys);
	bool bmx_cegui_system_isredrawrequested(CEGUI::System * sys);
	double bmx_cegui_system_getsingleclicktimeout(CEGUI::System * sys);
	double bmx_cegui_system_getmulticlicktimeout(CEGUI::System * sys);
	void bmx_cegui_system_getmulticlicktoleranceareasize(CEGUI::System * sys, float * width, float * height);
	void bmx_cegui_system_setsingleclicktimeout(CEGUI::System * sys, double timeout);
	void bmx_cegui_system_setmulticlicktimeout(CEGUI::System * sys, double timeout);
	void bmx_cegui_system_setmulticlicktoleranceareasize(CEGUI::System * sys, float width, float height);


	CEGUI::WindowManager * bmx_cegui_windowmanager_getsingleton();
	BBObject * bmx_cegui_windowmanager_loadWindowLayout(CEGUI::WindowManager * mgr, const CEGUI::utf8 * filename, const CEGUI::utf8 * namePrefix, const CEGUI::utf8 * resourceGroup);
	BBObject * bmx_cegui_windowmanager_getwindow(CEGUI::WindowManager * mgr, const CEGUI::utf8 * name);
	BBObject * bmx_cegui_windowmanager_createwindow(CEGUI::WindowManager * mgr, const CEGUI::utf8 * windowType, const CEGUI::utf8 * name, const CEGUI::utf8 * prefix);
	bool bmx_cegui_windowmanager_iswindowpresent(CEGUI::WindowManager * mgr, const CEGUI::utf8 * name);
	void bmx_cegui_windowmanager_destroyallwindows(CEGUI::WindowManager * mgr);
	void bmx_cegui_windowmanager_destroywindowwindow(CEGUI::WindowManager * mgr, CEGUI::Window * window);
	void bmx_cegui_windowmanager_destroywindowname(CEGUI::WindowManager * mgr, const CEGUI::utf8 * window);
	void bmx_cegui_windowmanager_renamewindowwindow(CEGUI::WindowManager * mgr, CEGUI::Window * window, const CEGUI::utf8 * newName);
	void bmx_cegui_windowmanager_renamewindowname(CEGUI::WindowManager * mgr, const CEGUI::utf8 * window, const CEGUI::utf8 * newName);

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

	void bmx_cegui_logger_setlogginglevel(CEGUI::LoggingLevel level);
	void bmx_cegui_logger_setlogfilename(const CEGUI::utf8 * filename, bool append);
	void bmx_cegui_logger_logevent(const CEGUI::utf8 * message, CEGUI::LoggingLevel level);
	MaxLogger * bmx_cegui_customlogger_create(BBObject * handle);
	void bmx_cegui_customlogger_delete(MaxLogger * logger);
	CEGUI::LoggingLevel bmx_cegui_logger_getlogginglevel();

	bool bmx_cegui_eventargs_gethandled(MaxEventArgs * args);
	BBObject * bmx_cegui_windoweventargs_getwindow(MaxEventArgs * args);
	BBObject * bmx_cegui_activationeventargs_getotherwindow(MaxEventArgs * args);
	float bmx_cegui_updateeventargs_gettimesincelastframe(MaxEventArgs * args);

	bool bmx_cegui_framewindow_issizingenabled(CEGUI::FrameWindow * win);
	bool bmx_cegui_framewindow_isframeenabled(CEGUI::FrameWindow * win);
	bool bmx_cegui_framewindow_istitlebarenabled(CEGUI::FrameWindow * win);
	bool bmx_cegui_framewindow_isclosebuttonenabled(CEGUI::FrameWindow * win);
	bool bmx_cegui_framewindow_isrollupenabled(CEGUI::FrameWindow * win);
	bool bmx_cegui_framewindow_isrolledup(CEGUI::FrameWindow * win);
	float bmx_cegui_framewindow_getsizingborderthickness(CEGUI::FrameWindow * win);
	void bmx_cegui_framewindow_setsizingenabled(CEGUI::FrameWindow * win, bool setting);
	void bmx_cegui_framewindow_setframeenabled(CEGUI::FrameWindow * win, bool setting);
	void bmx_cegui_framewindow_settitlebarenabled(CEGUI::FrameWindow * win, bool setting);
	void bmx_cegui_framewindow_setclosebuttonenabled(CEGUI::FrameWindow * win, bool setting);
	void bmx_cegui_framewindow_rollupenabled(CEGUI::FrameWindow * win, bool setting);
	void bmx_cegui_framewindow_togglerollup(CEGUI::FrameWindow * win);
	void bmx_cegui_framewindow_setsizingborderthickness(CEGUI::FrameWindow * win, float pixels);
	bool bmx_cegui_framewindow_isdragmovingenabled(CEGUI::FrameWindow * win);
	void bmx_cegui_framewindow_setdragmovingenabled(CEGUI::FrameWindow * win, bool setting);
	bool bmx_cegui_framewindow_ishit(CEGUI::FrameWindow * win, float x, float y);
	BBObject * bmx_cegui_framewindow_gettitlebar(CEGUI::FrameWindow * win);
	BBObject * bmx_cegui_framewindow_getclosebutton(CEGUI::FrameWindow * win);

	bool bmx_cegui_titlebar_isdraggingenabled(CEGUI::Titlebar * tb);
	void bmx_cegui_titlebar_setdraggingenabled(CEGUI::Titlebar * tb, bool setting);

	bool bmx_cegui_buttonbase_ishovering(CEGUI::ButtonBase * base);
	bool bmx_cegui_buttonbase_ispushed(CEGUI::ButtonBase * base);

	bool bmx_cegui_radiobutton_isselected(CEGUI::RadioButton * rb);
	CEGUI::ulong bmx_cegui_radiobutton_getgroupid(CEGUI::RadioButton * rb);
	BBObject * bmx_cegui_radiobutton_getselectedbuttoningroup(CEGUI::RadioButton * rb);
	void bmx_cegui_radiobutton_setselected(CEGUI::RadioButton * rb, bool selected);
	void bmx_cegui_radiobutton_setgroupid(CEGUI::RadioButton * rb, CEGUI::ulong group);

	void bmx_cegui_combodroplist_setarmed(CEGUI::ComboDropList * list, bool setting);
	bool bmx_cegui_combodroplist_isarmed(CEGUI::ComboDropList * list);
	void bmx_cegui_combodroplist_setautoarmenabled(CEGUI::ComboDropList * list, bool setting);
	bool bmx_cegui_combodroplist_isautoarmenabled(CEGUI::ComboDropList * list);

	bool bmx_cegui_multilineeditbox_hasinputfocus(CEGUI::MultiLineEditbox * eb);
	bool bmx_cegui_multilineeditbox_isreadonly(CEGUI::MultiLineEditbox * eb);
	int bmx_cegui_multilineeditbox_getcaratindex(CEGUI::MultiLineEditbox * eb);
	int bmx_cegui_multilineeditbox_getselectionstartindex(CEGUI::MultiLineEditbox * eb);
	int bmx_cegui_multilineeditbox_getselectionendindex(CEGUI::MultiLineEditbox * eb);
	int bmx_cegui_multilineeditbox_getselectionlength(CEGUI::MultiLineEditbox * eb);
	int bmx_cegui_multilineeditbox_getmaxtextlength(CEGUI::MultiLineEditbox * eb);
	bool bmx_cegui_multilineeditbox_iswordwrapped(CEGUI::MultiLineEditbox * eb);
	BBObject * bmx_cegui_multilineeditbox_getvertscrollbar(CEGUI::MultiLineEditbox * eb);
	bool bmx_cegui_multilineeditbox_isvertscrollbaralwaysshown(CEGUI::MultiLineEditbox * eb);
	BBObject * bmx_cegui_multilineeditbox_gethorzscrollbar(CEGUI::MultiLineEditbox * eb);
	void bmx_cegui_multilineeditbox_gettextrenderarea(CEGUI::MultiLineEditbox * eb, float * x, float * y, float * w, float * h);
	int bmx_cegui_multilineeditbox_getlinenumberfromindex(CEGUI::MultiLineEditbox * eb, int index);
	void bmx_cegui_multilineeditbox_setreadonly(CEGUI::MultiLineEditbox * eb, bool setting);
	void bmx_cegui_multilineeditbox_setcaratindex(CEGUI::MultiLineEditbox * eb, int caratPos);
	void bmx_cegui_multilineeditbox_setselection(CEGUI::MultiLineEditbox * eb, int startPos, int endPos);
	void bmx_cegui_multilineeditbox_setmaxtextlength(CEGUI::MultiLineEditbox * eb, int maxLen);
	void bmx_cegui_multilineeditbox_ensurecaratisvisible(CEGUI::MultiLineEditbox * eb);
	void bmx_cegui_multilineeditbox_setwordwrapping(CEGUI::MultiLineEditbox * eb, bool setting);
	void bmx_cegui_multilineeditbox_setshowvertscrollbar(CEGUI::MultiLineEditbox * eb, bool setting);

	
	bool bmx_cegui_scrolleditemlistbase_isvertscrollbaralwaysshown(CEGUI::ScrolledItemListBase * lb);
	bool bmx_cegui_scrolleditemlistbase_ishorzscrollbaralwaysshown(CEGUI::ScrolledItemListBase * lb);
	BBObject * bmx_cegui_scrolleditemlistbase_getvertscrollbar(CEGUI::ScrolledItemListBase * lb);
	BBObject * bmx_cegui_scrolleditemlistbase_gethorzscrollbar(CEGUI::ScrolledItemListBase * lb);
	void bmx_cegui_scrolleditemlistbase_setshowvertscrollbar(CEGUI::ScrolledItemListBase * lb, bool mode);
	void bmx_cegui_scrolleditemlistbase_setshowhorzscrollbar(CEGUI::ScrolledItemListBase * lb, bool mode);

	int bmx_cegui_itemlistbox_getselectedcount(CEGUI::ItemListbox * lb);
	BBObject * bmx_cegui_itemlistbox_getlastselecteditem(CEGUI::ItemListbox * lb);
	BBObject * bmx_cegui_itemlistbox_getfirstselecteditem(CEGUI::ItemListbox * lb, int startIndex);
	BBObject * bmx_cegui_itemlistbox_getnextselecteditem(CEGUI::ItemListbox * lb);
	BBObject * bmx_cegui_itemlistbox_getnextselecteditemafter(CEGUI::ItemListbox * lb, const CEGUI::ItemEntry * startItem);
	bool bmx_cegui_itemlistbox_ismultiselectenabled(CEGUI::ItemListbox * lb);
	bool bmx_cegui_itemlistbox_isitemselected(CEGUI::ItemListbox * lb, int index);
	void bmx_cegui_itemlistbox_setmultiselectenabled(CEGUI::ItemListbox * lb, bool state);
	void bmx_cegui_itemlistbox_clearallselections(CEGUI::ItemListbox * lb);
	void bmx_cegui_itemlistbox_selectrange(CEGUI::ItemListbox * lb, int a, int z);
	void bmx_cegui_itemlistbox_selectallitems(CEGUI::ItemListbox * lb);


	CEGUI::Imageset * bmx_cegui_imagesetmanager_createimagesetfromimagefile(const CEGUI::utf8 * name, const CEGUI::utf8 * filename, const CEGUI::utf8 * resourceGroup);
	CEGUI::Imageset * bmx_cegui_imagesetmanager_createimageset(const CEGUI::utf8 * filename, const CEGUI::utf8 * resourceGroup);
	CEGUI::Imageset * bmx_cegui_imagesetmanager_createimagesetfromtexture(const CEGUI::utf8 * name, CEGUI::Texture * texture);
	void bmx_cegui_imagesetmanager_destroyimageset(CEGUI::Imageset * imageset);
	void bmx_cegui_imagesetmanager_destroyimagesetname(const CEGUI::utf8 * name);
	void bmx_cegui_imagesetmanager_destroyallimagesets();
	CEGUI::Imageset * bmx_cegui_imagesetmanager_getimageset(const CEGUI::utf8 * name);
	bool bmx_cegui_imagesetmanager_isimagesetpresent(const CEGUI::utf8 * name);
	void bmx_cegui_imagesetmanager_notifyscreenresolution(float width, float height);

	
	CEGUI::Font * bmx_cegui_fontmanager_createfont(const CEGUI::utf8 * filename, const CEGUI::utf8 * resourceGroup);
	bool bmx_cegui_fontmanager_isfontpresent(const CEGUI::utf8 * name);

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
	bool bmx_cegui_tabcontrol_istabcontentsselected(CEGUI::TabControl * tc, CEGUI::Window * wnd);
	int bmx_cegui_tabcontrol_getselectedtabindex(CEGUI::TabControl * tc);
	float bmx_cegui_tabcontrol_gettabheight(CEGUI::TabControl * tc);
	float bmx_cegui_tabcontrol_gettabtextpadding(CEGUI::TabControl * tc);
	void bmx_cegui_tabcontrol_settabheight(CEGUI::TabControl * tc, float height);
	void bmx_cegui_tabcontrol_settabtextpadding(CEGUI::TabControl * tc, float padding);
	void bmx_cegui_tabcontrol_addtab(CEGUI::TabControl * tc, CEGUI::Window * wnd);
	void bmx_cegui_tabcontrol_removetab(CEGUI::TabControl * tc, const CEGUI::utf8 * name);
	void bmx_cegui_tabcontrol_removetabforid(CEGUI::TabControl * tc, CEGUI::uint ID);

	BBObject * bmx_cegui_renderer_createtexture(CEGUI::Renderer * rend, const CEGUI::utf8 * filename, const CEGUI::utf8 * resourceGroup);
	BBObject * bmx_cegui_renderer_createtexturewithsize(CEGUI::Renderer * rend, float size);
	void bmx_cegui_renderer_destroytexture(CEGUI::Renderer * rend, CEGUI::Texture * texture);
	void bmx_cegui_renderer_destroyalltextures(CEGUI::Renderer * rend);
	bool bmx_cegui_renderer_isqueueingenabled(CEGUI::Renderer * rend);
	float bmx_cegui_renderer_getwidth(CEGUI::Renderer * rend);
	float bmx_cegui_renderer_getheight(CEGUI::Renderer * rend);
	void bmx_cegui_renderer_getsize(CEGUI::Renderer * rend, float * width, float * height);
	CEGUI::uint bmx_cegui_renderer_getmaxtexturesize(CEGUI::Renderer * rend);
	CEGUI::uint bmx_cegui_renderer_gethorzscreendpi(CEGUI::Renderer * rend);
	CEGUI::uint bmx_cegui_renderer_getvertscreendpi(CEGUI::Renderer * rend);
	void bmx_cegui_renderer_resetzvalue(CEGUI::Renderer * rend);
	void bmx_cegui_renderer_advancezvalue(CEGUI::Renderer * rend);
	float bmx_cegui_renderer_getcurrentz(CEGUI::Renderer * rend);
	float bmx_cegui_renderer_getzlayer(CEGUI::Renderer * rend, CEGUI::uint layer);
	const CEGUI::utf8 * bmx_cegui_renderer_getidentifierstring(CEGUI::Renderer * rend);


	int bmx_cegui_texture_getwidth(CEGUI::Texture * texture);
	int bmx_cegui_texture_getoriginalwidth(CEGUI::Texture * texture);
	float bmx_cegui_texture_getxscale(CEGUI::Texture * texture);
	int bmx_cegui_texture_getheight(CEGUI::Texture * texture);
	int bmx_cegui_texture_getoriginalheight(CEGUI::Texture * texture);
	float bmx_cegui_texture_getyscale(CEGUI::Texture * texture);
	void bmx_cegui_texture_loadfromfile(CEGUI::Texture * texture, const CEGUI::utf8 * filename, const CEGUI::utf8 * resourceGroup);
	void bmx_cegui_texture_loadfrommemory(CEGUI::Texture * texture, const void * buffer, CEGUI::uint width, CEGUI::uint height, CEGUI::Texture::PixelFormat pixelFormat);

	void bmx_cegui_scheme_loadresources(CEGUI::Scheme * scheme);
	void bmx_cegui_scheme_unloadresources(CEGUI::Scheme * scheme);
	bool bmx_cegui_scheme_resourcesloaded(CEGUI::Scheme * scheme);
	const CEGUI::utf8 * bmx_cegui_scheme_getname(CEGUI::Scheme * scheme);

	const CEGUI::utf8 * bmx_cegui_listboxitem_gettext(CEGUI::ListboxItem * item);
	const CEGUI::utf8 * bmx_cegui_listboxitem_gettooltiptext(CEGUI::ListboxItem * item);
	CEGUI::uint bmx_cegui_listboxitem_getid(CEGUI::ListboxItem * item);
	bool bmx_cegui_listboxitem_isselected(CEGUI::ListboxItem * item);
	bool bmx_cegui_listboxitem_isdisabled(CEGUI::ListboxItem * item);
	bool bmx_cegui_listboxitem_isautodeleted(CEGUI::ListboxItem * item);
	BBObject * bmx_cegui_listboxitem_getownerwindow(CEGUI::ListboxItem * item);
	void bmx_cegui_listboxitem_settext(CEGUI::ListboxItem * item, const CEGUI::utf8 * text);
	void bmx_cegui_listboxitem_settooltiptext(CEGUI::ListboxItem * item, const CEGUI::utf8 * text);
	void bmx_cegui_listboxitem_setid(CEGUI::ListboxItem * item, CEGUI::uint itemId);
	void bmx_cegui_listboxitem_setselected(CEGUI::ListboxItem * item, bool setting);
	void bmx_cegui_listboxitem_setdisabled(CEGUI::ListboxItem * item, bool setting);
	void bmx_cegui_listboxitem_setautodeleted(CEGUI::ListboxItem * item, bool setting);
	void bmx_cegui_listboxitem_setownerwindow(CEGUI::ListboxItem * item, CEGUI::Window * owner);

	bool bmx_cegui_multicolumnlist_isusersortcontrolenabled(CEGUI::MultiColumnList * mc);
	bool bmx_cegui_multicolumnlist_isusercolumnsizingenabled(CEGUI::MultiColumnList * mc);
	bool bmx_cegui_multicolumnlist_isusercolumndraggingenabled(CEGUI::MultiColumnList * mc);
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
	bool bmx_cegui_multicolumnlist_islistboxitemincolumn(CEGUI::MultiColumnList * mc, const CEGUI::ListboxItem * item, CEGUI::uint colIdx);
	bool bmx_cegui_multicolumnlist_islistboxiteminrow(CEGUI::MultiColumnList * mc, const CEGUI::ListboxItem * item, CEGUI::uint rowIdx);
	bool bmx_cegui_multicolumnlist_islistboxiteminlist(CEGUI::MultiColumnList * mc, const CEGUI::ListboxItem * item);
	BBObject * bmx_cegui_multicolumnlist_findcolumnitemwithtext(CEGUI::MultiColumnList * mc, const CEGUI::utf8 * text, CEGUI::uint colIdx, const CEGUI::ListboxItem * startItem);
	BBObject * bmx_cegui_multicolumnlist_findrowitemwithtext(CEGUI::MultiColumnList * mc, const CEGUI::utf8 * text, CEGUI::uint rowIdx, const CEGUI::ListboxItem * startItem);
	BBObject * bmx_cegui_multicolumnlist_findlistitemwithtext(CEGUI::MultiColumnList * mc, const CEGUI::utf8 * text, const CEGUI::ListboxItem * startItem);
	BBObject * bmx_cegui_multicolumnlist_getfirstselecteditem(CEGUI::MultiColumnList * mc);
	BBObject * bmx_cegui_multicolumnlist_getnextselected(CEGUI::MultiColumnList * mc, const CEGUI::ListboxItem * startItem);
	CEGUI::uint bmx_cegui_multicolumnlist_getselectedcount(CEGUI::MultiColumnList * mc);
	bool bmx_cegui_multicolumnlist_isitemselected(CEGUI::MultiColumnList * mc, int row, int col);
	CEGUI::uint bmx_cegui_multicolumnlist_getnominatedselectioncolumnid(CEGUI::MultiColumnList * mc);
	CEGUI::uint bmx_cegui_multicolumnlist_getnominatedselectioncolum(CEGUI::MultiColumnList * mc);
	CEGUI::uint bmx_cegui_multicolumnlist_getnominatedselectionrow(CEGUI::MultiColumnList * mc);
	CEGUI::MultiColumnList::SelectionMode bmx_cegui_multicolumnlist_getselectionmode(CEGUI::MultiColumnList * mc);
	bool bmx_cegui_multicolumnlist_isvertscrollbaralwaysshown(CEGUI::MultiColumnList * mc);
	bool bmx_cegui_multicolumnlist_ishorzscrollbaralwaysshown(CEGUI::MultiColumnList * mc);
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
	void bmx_cegui_multicolumnlist_insertcolumn(CEGUI::MultiColumnList * mc, const CEGUI::utf8 * text, CEGUI::uint colId, float width, CEGUI::uint position);
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
	void bmx_cegui_multicolumnlist_setshowvertscrollbar(CEGUI::MultiColumnList * mc, bool setting);
	void bmx_cegui_multicolumnlist_setshowhorzscrollbar(CEGUI::MultiColumnList * mc, bool setting);
	void bmx_cegui_multicolumnlist_clearallselections(CEGUI::MultiColumnList * mc);
	void bmx_cegui_multicolumnlist_setitemselectstate(CEGUI::MultiColumnList * mc, CEGUI::ListboxItem * item, bool state);
	void bmx_cegui_multicolumnlist_setitemselectstateforgridref(CEGUI::MultiColumnList * mc, int row, int col, bool state);
	void bmx_cegui_multicolumnlist_handleupdateditemdata(CEGUI::MultiColumnList * mc);
	void bmx_cegui_multicolumnlist_setcolumnheaderwidth(CEGUI::MultiColumnList * mc, CEGUI::uint colIdx, float width);
	void bmx_cegui_multicolumnlist_setusersortcontrolenabled(CEGUI::MultiColumnList * mc, bool setting);
	void bmx_cegui_multicolumnlist_setusercolumnsizingenabled(CEGUI::MultiColumnList * mc, bool setting);
	void bmx_cegui_multicolumnlist_setusercolumndraggingenabled(CEGUI::MultiColumnList * mc, bool setting);
	void bmx_cegui_multicolumnlist_autosizecolumnheader(CEGUI::MultiColumnList * mc, CEGUI::uint colIdx);
	void bmx_cegui_multicolumnlist_setrowid(CEGUI::MultiColumnList * mc, CEGUI::uint rowIdx, CEGUI::uint rowId);

	bool bmx_cegui_dragcontainer_isdraggingenabled(CEGUI::DragContainer * dc);
	void bmx_cegui_dragcontainer_setdraggingenabled(CEGUI::DragContainer * dc, bool setting);
	bool bmx_cegui_dragcontainer_isbeingdragged(CEGUI::DragContainer * dc);
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
	bool bmx_cegui_colourrect_ismonochromatic(MaxCEColourRect * rect);
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
	bool bmx_cegui_menubase_ismultiplepopupsallowed(CEGUI::MenuBase * base);
	BBObject * bmx_cegui_menubase_getpopupmenuitem(CEGUI::MenuBase * base);
	void bmx_cegui_menubase_setitemspacing(CEGUI::MenuBase * base, float spacing);
	void bmx_cegui_menubase_changepopupmenuitem(CEGUI::MenuBase * base, CEGUI::MenuItem * item);
	void bmx_cegui_menubase_setallowmultiplepopups(CEGUI::MenuBase * base, bool setting);
	
	float bmx_cegui_popupmenu_getfadeintime(CEGUI::PopupMenu * menu);
	float bmx_cegui_popupmenu_getfadeouttime(CEGUI::PopupMenu * menu);
	bool bmx_cegui_popupmenu_ispopupmenuopen(CEGUI::PopupMenu * menu);
	void bmx_cegui_popupmenu_setfadeintime(CEGUI::PopupMenu * menu, float fadetime);
	void bmx_cegui_popupmenu_setfadeouttime(CEGUI::PopupMenu * menu, float fadetime);
	void bmx_cegui_popupmenu_openpopupmenu(CEGUI::PopupMenu * menu, bool _notify);
	void bmx_cegui_popupmenu_closepopupmenu(CEGUI::PopupMenu * menu, bool _notify);

	void bmx_cegui_itementry_getitempixelsize(CEGUI::ItemEntry * entry, float * w, float * h);
	BBObject * bmx_cegui_itementry_getownerlist(CEGUI::ItemEntry * entry);
	bool bmx_cegui_itementry_isselected(CEGUI::ItemEntry * entry);
	bool bmx_cegui_itementry_isselectable(CEGUI::ItemEntry * entry);
	void bmx_cegui_itementry_setselected(CEGUI::ItemEntry * entry, bool setting);
	void bmx_cegui_itementry_selectentry(CEGUI::ItemEntry * entry);
	void bmx_cegui_itementry_deselect(CEGUI::ItemEntry * entry);
	void bmx_cegui_itementry_setselectable(CEGUI::ItemEntry * entry, bool setting);

	bool bmx_cegui_menuitem_ishovering(CEGUI::MenuItem * item);
	bool bmx_cegui_menuitem_ispushed(CEGUI::MenuItem * item);
	bool bmx_cegui_menuitem_isopened(CEGUI::MenuItem * item);
	BBObject * bmx_cegui_menuitem_getpopupmenu(CEGUI::MenuItem * item);
	void bmx_cegui_menuitem_setpopupmenu(CEGUI::MenuItem * item, CEGUI::PopupMenu * popup);
	void bmx_cegui_menuitem_openpopupmenu(CEGUI::MenuItem * item, bool _notify);
	void bmx_cegui_menuitem_closepopupmenu(CEGUI::MenuItem * item, bool _notify);
	bool bmx_cegui_menuitem_togglepopupmenu(CEGUI::MenuItem * item);

	void bmx_cegui_treeitem_settextcolours(CEGUI::TreeItem * ti, MaxCEColourRect * cols);
	void bmx_cegui_treeitem_settextcoloursforcorners(CEGUI::TreeItem * ti, MaxCEColour * topLeftColour, MaxCEColour * topRightColour, MaxCEColour * bottomLeftColour, MaxCEColour * bottomRightColour);
	void bmx_cegui_treeitem_settextcoloursforcolour(CEGUI::TreeItem * ti, MaxCEColour * col);
	const CEGUI::utf8 * bmx_cegui_treeitem_gettext(CEGUI::TreeItem * ti);
	const CEGUI::utf8 * bmx_cegui_treeitem_gettooltiptext(CEGUI::TreeItem * ti);
	CEGUI::uint bmx_cegui_treeitem_getid(CEGUI::TreeItem * ti);
	BBObject * bmx_cegui_treeitem_getuserdata(CEGUI::TreeItem * ti);
	bool bmx_cegui_treeitem_isselected(CEGUI::TreeItem * ti);
	bool bmx_cegui_treeitem_isdisabled(CEGUI::TreeItem * ti);
	bool bmx_cegui_treeitem_isautodeleted(CEGUI::TreeItem * ti);
	BBObject * bmx_cegui_treeitem_getownerwindow(CEGUI::TreeItem * ti);
	MaxCEColourRect * bmx_cegui_treeitem_getselectioncolours(CEGUI::TreeItem * ti);
	BBObject * bmx_cegui_treeitem_getselectionbrushimage(CEGUI::TreeItem * ti);
	void bmx_cegui_treeitem_settext(CEGUI::TreeItem * ti, const CEGUI::utf8 * text);
	void bmx_cegui_treeitem_settooltiptext(CEGUI::TreeItem * ti, const CEGUI::utf8 * text);
	void bmx_cegui_treeitem_setid(CEGUI::TreeItem * ti, CEGUI::uint itemId);
	void bmx_cegui_treeitem_setuserdata(CEGUI::TreeItem * ti, BBObject * itemData);
	void bmx_cegui_treeitem_setselected(CEGUI::TreeItem * ti, bool setting);
	void bmx_cegui_treeitem_setdisabled(CEGUI::TreeItem * ti, bool setting);
	void bmx_cegui_treeitem_setautodeleted(CEGUI::TreeItem * ti, bool setting);
	void bmx_cegui_treeitem_setownerwindow(CEGUI::TreeItem * ti, CEGUI::Window * owner);
	void bmx_cegui_treeitem_setselectioncolours(CEGUI::TreeItem * ti, MaxCEColourRect * cols);
	void bmx_cegui_treeitem_setselectioncoloursforcorners(CEGUI::TreeItem * ti, MaxCEColour * topLeftColour, MaxCEColour * topRightColour, MaxCEColour * bottomLeftColour, MaxCEColour * bottomRightColour);
	void bmx_cegui_treeitem_setselectioncoloursforcolour(CEGUI::TreeItem * ti, MaxCEColour * col);
	void bmx_cegui_treeitem_setselectionbrushimage(CEGUI::TreeItem * ti, CEGUI::Image * image);
	void bmx_cegui_treeitem_setselectionbrushimagebyname(CEGUI::TreeItem * ti, const CEGUI::utf8 * imageset, const CEGUI::utf8 * image);
	void bmx_cegui_treeitem_setbuttonlocation(CEGUI::TreeItem * ti, float x, float y, float w, float h);
	void bmx_cegui_treeitem_getbuttonlocation(CEGUI::TreeItem * ti, float * x, float * y, float * w, float * h);
	bool bmx_cegui_treeitem_getisopen(CEGUI::TreeItem * ti);
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

	BBObject * bmx_cegui_scrollablepane_getcontentpane(CEGUI::ScrollablePane * sp);
	bool bmx_cegui_scrollablepane_isvertscrollbaralwaysshown(CEGUI::ScrollablePane * sp);
	void bmx_cegui_scrollablepane_setshowvertscrollbar(CEGUI::ScrollablePane * sp, bool setting);
	bool bmx_cegui_scrollablepane_ishorzscrollbaralwaysshown(CEGUI::ScrollablePane * sp);
	void bmx_cegui_scrollablepane_setshowhorzscrollbar(CEGUI::ScrollablePane * sp, bool setting);
	bool bmx_cegui_scrollablepane_iscontentpaneautosized(CEGUI::ScrollablePane * sp);
	void bmx_cegui_scrollablepane_setcontentpaneautosized(CEGUI::ScrollablePane * sp, bool setting);
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

class MaxLogger : public CEGUI::DefaultLogger
{
public:
	MaxLogger(BBObject * handle);
	virtual void logEvent(const CEGUI::String& message, CEGUI::LoggingLevel level = CEGUI::Standard);


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

