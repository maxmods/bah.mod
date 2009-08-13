#include <stdarg.h>
#include <stdio.h>
#include <math.h>
#include "libtcod.hpp"
#include "gui/gui.hpp"

#define CON_WIDTH 128
#define CON_HEIGHT 96
TCODConsole *guicon=NULL;
uint32 seed=0xdeadbeef;

// ui
ToolBar *params=NULL;
ToolBar *history=NULL;
ToolBar *colorMapGui=NULL;

char wallRatioTxt[128]="";
char seedTxt[128]="";

void render() {
	TCODConsole::root->setBackgroundColor(TCODColor::black);
	TCODConsole::root->setForegroundColor(TCODColor::white);
	TCODConsole::root->clear();
	// render the dungeon
	for (int x=0; x < CON_WIDTH; x ++ ) {
		for (int y=0;y < CON_HEIGHT; y++ ) {
			TCODConsole::root->setBack(x,y,TCODColor::black);
		}
	}
}

void buildGui() {
	// status bar
	new StatusBar(0,0,CON_WIDTH,1);

	VBox *vbox=new VBox(0,2,1);
	// stats
	ToolBar *stats = new ToolBar(0,0,21,"Stats","Statistics about the current dungeon");
	stats->addWidget(new Label(0,0,wallRatioTxt,"Ratio of wall cells / total number of cells"));
	stats->addWidget(new Label(0,0,seedTxt,"Current random seed used to build the map"));
	vbox->addWidget(stats);
}

int main(int argc, char *argv[]) {
	TCODConsole::initRoot(CON_WIDTH,CON_HEIGHT,"libtcod 1.5 dungeon tool",false);
	guicon = new TCODConsole(CON_WIDTH,CON_HEIGHT);
	guicon->setKeyColor(TCODColor(255,0,255));
	Widget::setConsole(guicon);
	TCODSystem::setFps(25);
	buildGui();
	uint8 fade=50;
	bool creditsEnd=false;

	while ( ! TCODConsole::isWindowClosed() ) {
		TCOD_key_t key=TCODConsole::checkForKeypress();
		Widget::updateWidgets(key);
		switch(key.vk) {
			case TCODK_PRINTSCREEN : TCODSystem::saveScreenshot(NULL); break;
			default:break;
		}
		render();
		guicon->setBackgroundColor(TCODColor(255,0,255));
		guicon->clear();
		Widget::renderWidgets();
		if (! creditsEnd ) {
			creditsEnd=TCODConsole::renderCredits(CON_WIDTH-20,CON_HEIGHT-7,true);
		}
		if ( Widget::focus ) {
			if ( fade < 200 ) fade += 20;
		} else {
			if ( fade > 80 ) fade -= 20;
		}
		TCODConsole::blit(guicon,0,0,CON_WIDTH,CON_HEIGHT,TCODConsole::root,0,0,fade);
		TCODConsole::flush();
	}

	return 0;
}

