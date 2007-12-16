
#import <AppKit/AppKit.h>
#import <Carbon/Carbon.h>

#include <brl.mod/blitz.mod/blitz.h>

void __bb_appstub_appstub();

static int app_argc;
static char **app_argv;
static char *app_dir,*app_file;
static int app_gui;

static NSMutableArray *_appArgs;
static NSAutoreleasePool *_globalPool;

static void sighandler( int n ){
	char	buffer[256];
	sprintf(buffer,"SIGNAL: %i",n );
	brl_blitz_RuntimeError( bbStringFromCString(buffer) );
}

void bbFlushAutoreleasePool(){
	[_globalPool release];
	_globalPool=[[NSAutoreleasePool alloc] init];
}

static void run(){

	bbStartup( app_argc,app_argv,app_dir,app_file );

	signal( SIGPIPE,SIG_IGN );
	
	//alas, can't get these working in Cocoa!
	if( !app_gui ){
		signal( SIGBUS,sighandler );
		signal( SIGSEGV,sighandler );
		signal( SIGTERM,sighandler );
		signal( SIGTRAP,sighandler );
	}
	
	__bb_appstub_appstub();
	
	exit( 0 );
}

int main( int argc,char *argv[] ){
	int i;
	CFURLRef url;
	char *p;
	
	app_argc=argc;
	app_argv=argv;
	app_dir=malloc(1024);
	app_file=malloc(1024);
	
	url=CFBundleCopyExecutableURL( CFBundleGetMainBundle() );
	CFURLGetFileSystemRepresentation( url,true,(UInt8*)app_file,1024 );

	strcpy( app_dir,app_file );
	if( p=strstr( app_dir,".app/Contents/MacOS/" ) ){
		app_gui=1;
		*p=0;
	}
	p=strrchr( app_dir,'/' );
	assert(p);
	*p=0;

	// Bring the app to front :-)
	ProcessSerialNumber psn = { 0, kCurrentProcess };
	SetFrontProcess( &psn );
	
	run();	
}
