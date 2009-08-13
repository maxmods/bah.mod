#include <string.h>
#include "libtcod.hpp"
#include "gui.hpp"

class Separator : public Widget {
public :
	Separator(const char *txt, const char *tip=NULL) : Widget(0,0,0,1),txt(NULL) {
		if ( txt ) {
			this->txt=strdup(txt);
		}
		if ( tip ) setTip(tip);
	}
	virtual ~Separator() {
		if ( txt ) {
			free(txt);
		}
	}
	void computeSize() {
		w = (txt ? (int)strlen(txt)+2 : 0 );
	}
	void expand(int width, int height) {
		if ( w < width ) w=width;
	}
	void render() {
		con->pushBrush();
		con->setBackgroundBrush(back,TCOD_COLOROP_SET);
		con->setForegroundColor(fore);
		con->rect(x,y,w,1,TCOD_CHAR_HLINE);
		con->setChar(x-1,y,TCOD_CHAR_TEEE);
		con->setChar(x+w,y,TCOD_CHAR_TEEW);
		con->setBackgroundColor(fore);
		con->setForegroundColor(back);
		con->printCenter(x+w/2,y," %s ",txt);
		con->popBrush();
	}
	char *txt;
};

ToolBar::ToolBar(int x, int y, const char *name, const char *tip) 
	: Container(x,y,0,2),name(NULL),fixedWidth(0) {
	if ( name ) {
		this->name = strdup(name);
		w = strlen(name)+4;
	}
	if ( tip ) setTip(tip);
}

ToolBar::ToolBar(int x, int y, int w, const char *name, const char *tip) 
	: Container(x,y,w,2),name(NULL),fixedWidth(w) {
	if ( name ) {
		this->name = strdup(name);
		fixedWidth = w = MAX((int)strlen(name)+4,w);
	}
	if ( tip ) setTip(tip);
}

ToolBar::~ToolBar() {
	if ( name ) free(name);
}

void ToolBar::setName(const char *name) {
	if ( this->name ) free(this->name);
	if ( name ) {
		this->name = strdup(name);
		fixedWidth = MAX((int)strlen(name)+4,fixedWidth);
	} else {
		name=NULL;
	}
}

void ToolBar::render() {
	con->pushBrush();
	con->setBackgroundBrush(back,TCOD_COLOROP_SET);
	con->setForegroundColor(fore);
	con->brushChar(x,y,TCOD_CHAR_NW);
	con->brushChar(x+w-1,y,TCOD_CHAR_NE);
	con->brushChar(x,y+h-1,TCOD_CHAR_SW);
	con->brushChar(x+w-1,y+h-1,TCOD_CHAR_SE);
	con->rect(x+1,y,w-2,1,TCOD_CHAR_HLINE);
	con->rect(x+1,y+h-1,w-2,1,TCOD_CHAR_HLINE);
	if ( h > 2 ) {
		con->rect(x,y+1,1,h-2,TCOD_CHAR_VLINE);
		con->rect(x+w-1,y+1,1,h-2,TCOD_CHAR_VLINE);
		con->rect(x+1,y+1,w-2,h-2); 
	}
	if ( name ) {
		char oldchar=0;
		if ((int)strlen(name) > w-3 ) {
			// truncate title
			oldchar=name[w-3];
			name[w-3]=0;
		}
		int xs = x + (w-strlen(name)-2)/2;
		// swap colors
		TCODColor oldback=con->getBackgroundColor();
		con->setBackgroundColor(con->getForegroundColor());
		con->setForegroundColor(oldback);
		// print title
		con->printLeft(xs,y," %s ",name);
		if ( oldchar ) name[w-3]=oldchar;
	}
	con->popBrush();
	Container::render();
}

void ToolBar::computeSize() {
	int cury=y+1;
	w=name ? strlen(name)+4 : 2;
	for (Widget **wid=content.begin(); wid != content.end(); wid ++ ) {
		if ( (*wid)->isVisible() ) {
			(*wid)->x=x+1;
			(*wid)->y=cury;
			(*wid)->computeSize();
			if ((*wid)->w +2 > w) w=(*wid)->w + 2;
			cury+=(*wid)->h;
		}
	}
	if ( w < fixedWidth ) w=fixedWidth;
	h=cury-y+1;
	for (Widget **wid=content.begin(); wid != content.end(); wid ++ ) {
		if ( (*wid)->isVisible() ) {
			(*wid)->expand(w-2,(*wid)->h);
		}
	}	
}
void ToolBar::addSeparator(const char *txt, const char *tip) {
	addWidget(new Separator(txt,tip));
}

