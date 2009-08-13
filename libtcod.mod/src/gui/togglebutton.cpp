#include <string.h>
#include "libtcod.hpp"
#include "gui.hpp"

void ToggleButton::render() {
	con->setForegroundColor(mouseIn ? foreFocus : fore);
	con->getBackgroundImage()->rect(x,y,w,h,mouseIn ? backFocus : back);
	if ( label ) {
		con->printLeft(x,y,"%c %s",pressed ? TCOD_CHAR_CHECKBOX_SET : TCOD_CHAR_CHECKBOX_UNSET, label);
	} else {
		con->printLeft(x,y,"%c",pressed ? TCOD_CHAR_CHECKBOX_SET : TCOD_CHAR_CHECKBOX_UNSET);
	}
}

void ToggleButton::onButtonPress() {
}

void ToggleButton::onButtonRelease() {
}

void ToggleButton::onButtonClick() {
	pressed=!pressed;
	if ( cbk ) cbk(this,userData);
}



