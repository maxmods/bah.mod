#include "libtcod.hpp"
#include "gui.hpp"

void StatusBar::render() {
	con->getBackgroundImage()->rect(x,y,w,h,back);
	if ( focus && focus->tip ) {
		con->setForegroundColor(fore);
		con->printLeftRect(x+1,y,w,h,focus->tip);
	}
}


