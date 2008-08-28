/*
 *  UniMotion - Unified Motion detection for Apple portables.
 *
 *  Copyright (c) 2006 Lincoln Ramsay. All rights reserved.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "unimotion.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <CoreFoundation/CoreFoundation.h>

static CFStringRef app = 0;

void store_int(CFStringRef name, int _num)
{
    CFNumberRef num;

    num = CFNumberCreate(kCFAllocatorDefault, kCFNumberCFIndexType, &_num);

    CFPreferencesSetAppValue(name, num, app);
    CFPreferencesAppSynchronize(app);

    CFRelease(num);
}

void store_double(CFStringRef name, double _num)
{
    CFNumberRef num;

    num = CFNumberCreate(kCFAllocatorDefault, kCFNumberCFIndexType, &_num);

    CFPreferencesSetAppValue(name, num, app);
    CFPreferencesAppSynchronize(app);

    CFRelease(num);
}

Boolean getPrefDouble(CFStringRef key, CFStringRef app, double *val)
{
    CFPropertyListRef prefRef;
    Boolean ok;
    double ret;

    ok = false;
    prefRef = CFPreferencesCopyAppValue(key, app);
    if ( prefRef ) {
        if ( CFGetTypeID(prefRef) == CFNumberGetTypeID() && CFNumberIsFloatType(prefRef) ) {
            ok = CFNumberGetValue(prefRef, kCFNumberDoubleType, &ret);
            if ( ok && val )
                *val = ret;
        }
        CFRelease(prefRef);
    }
    return ok;
}

Boolean getPrefInt(CFStringRef key, CFStringRef app, int *val)
{
    CFPropertyListRef prefRef;
    Boolean ok;
    int ret;

    ok = false;
    prefRef = CFPreferencesCopyAppValue(key, app);
    if ( prefRef ) {
        if ( CFGetTypeID(prefRef) == CFNumberGetTypeID() && !CFNumberIsFloatType(prefRef) ) {
            ok = CFNumberGetValue(prefRef, kCFNumberIntType, &ret);
            if ( ok && val )
                *val = ret;
        }
        CFRelease(prefRef);
    }
    return ok;
}

struct default_real_values {
    double xreal;
    double yreal;
    double zreal;
};

static struct default_real_values drv[] = {
    {0.0,0.0,0.0},             // unknown
    {2.0e-2,2.0e-2,2.0e-2},    // powerbook
    {2.0e-2,2.0e-2,2.0e-2},    // ibook
    {-2.0e-2,-2.0e-2,-2.0e-2},  // highrespb
    {4.0e-3,4.0e-3,4.0e-3},    // macbookpro
};

int main( int argc, char **argv )
{
    int type;
    int x, y, z;
    double fx, fy, fz;
    int xoff, yoff, zoff;
    double xscale, yscale, zscale;
    int ok;
    char buf[255];

    type = detect_sms();
    if ( type == unknown )
        return 1;

    app = CFSTR("com.ramsayl.UniMotion");
    static CFStringRef xoffstr = CFSTR("x_offset");
    static CFStringRef yoffstr = CFSTR("y_offset");
    static CFStringRef zoffstr = CFSTR("z_offset");
    static CFStringRef xscalestr = CFSTR("x_real");
    static CFStringRef yscalestr = CFSTR("y_real");
    static CFStringRef zscalestr = CFSTR("z_real");

    ok = getPrefInt(xoffstr, app, &xoff);
    if ( !ok ) xoff = 0;
    ok = getPrefInt(yoffstr, app, &yoff);
    if ( !ok ) yoff = 0;
    ok = getPrefInt(zoffstr, app, &zoff);
    if ( !ok ) zoff = 0;
    ok = getPrefDouble(xscalestr, app, &xscale);
    if ( !ok ) xscale = drv[type].xreal;
    ok = getPrefDouble(yscalestr, app, &yscale);
    if ( !ok ) yscale = drv[type].yreal;
    ok = getPrefDouble(zscalestr, app, &zscale);
    if ( !ok ) zscale = drv[type].zreal;

    for (;;) {
        ok = read_sms_raw(type, &x, &y, &z);
        if ( ok ) printf("r: %d %d %d\n", x, y, z);
        ok = read_sms(type, &x, &y, &z);
        if ( ok ) {
            printf("c: %d %d %d", x, y, z);
            printf(" (offset: %d %d %d)\n", xoff, yoff, zoff);
        }
        ok = read_sms_real(type, &fx, &fy, &fz);
        if ( ok ) {
            printf("f: %.2f %.2f %.2f", fx, fy, fz);
            printf(" (scale: %e %e %e)\n", xscale, yscale, zscale);
        }

        printf("modify what? [xyz]off/[xyz]scale (Ctrl+D or Ctrl+C to exit)\n");
        if ( !gets(buf) )
            break;
        if ( strcmp(buf, "xoff") == 0 ) {
            ok = getPrefInt(xoffstr, app, &xoff);
            if ( !ok ) xoff = 0;
            printf("current xoff: %d\nnew xoff (default 0):\n", xoff);
            gets(buf);
            xoff = atoi(buf);
            store_int(xoffstr, xoff);
        } else if ( strcmp(buf, "yoff") == 0 ) {
            ok = getPrefInt(yoffstr, app, &yoff);
            if ( !ok ) yoff = 0;
            printf("current yoff: %d\nnew yoff (default 0):\n", yoff);
            gets(buf);
            yoff = atoi(buf);
            store_int(yoffstr, yoff);
        } else if ( strcmp(buf, "zoff") == 0 ) {
            ok = getPrefInt(zoffstr, app, &zoff);
            if ( !ok ) zoff = 0;
            printf("current zoff: %d\nnew zoff (default 0):\n", zoff);
            gets(buf);
            zoff = atoi(buf);
            store_int(zoffstr, zoff);
        } else if ( strcmp(buf, "xscale") == 0 ) {
            ok = getPrefDouble(xscalestr, app, &xscale);
            if ( !ok ) xscale = drv[type].xreal;
            printf("current xscale: %e\nnew xscale (or \"default\"):\n", xscale);
            gets(buf);
            if ( strcmp(buf,"default") == 0 ) {
                xscale = drv[type].xreal;
                ok = 1;
            } else {
                ok = sscanf(buf, "%lf", &xscale);
            }
            if ( ok == 1 )
                store_double(xscalestr, xscale);
        } else if ( strcmp(buf, "yscale") == 0 ) {
            ok = getPrefDouble(yscalestr, app, &yscale);
            if ( !ok ) yscale = drv[type].yreal;
            printf("current yscale: %e\nnew yscale (or \"default\"):\n", yscale);
            gets(buf);
            if ( strcmp(buf,"default") == 0 ) {
                yscale = drv[type].yreal;
                ok = 1;
            } else {
                ok = sscanf(buf, "%lf", &yscale);
            }
            if ( ok == 1 )
                store_double(yscalestr, yscale);
        } else if ( strcmp(buf, "zscale") == 0 ) {
            ok = getPrefDouble(zscalestr, app, &zscale);
            if ( !ok ) zscale = drv[type].zreal;
            printf("current zscale: %e\nnew zscale (or \"default\"):\n", zscale);
            gets(buf);
            if ( strcmp(buf,"default") == 0 ) {
                zscale = drv[type].zreal;
                ok = 1;
            } else {
                ok = sscanf(buf, "%lf", &zscale);
            }
            if ( ok == 1 )
                store_double(zscalestr, zscale);
        }
    }

    return 0;
}
