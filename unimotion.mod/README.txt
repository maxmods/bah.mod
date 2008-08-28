UniMotion - Unified Motion detection for Apple portables.

This software allows you to read the output from the Apple Motion Sensor (aka. Sudden Motion Sensor, AMS, SMS) that has now become standard hardware in all portable computers from Apple.

Compiling

A Makefile is included so all you have to do is cd to the 'unimotion' directory and type 'make'.

Embedding

There are 2 ways to embed UniMotion:
1) You can use the binary and library in the motion directory as a commandline application (motion -h for usage).
2) You can link to Frameworks/libUniMotion.dylib (check unimotion.h for usage).

libUniMotion.dylib in the bin directory is designed to reside next to the executable that links to it (eg. calibrate and motion do this).
libUniMotion.dylib in the Frameworks directory is designed to reside in a Frameworks directory of an app bundle. For example, Calibrate.app/Contents/Frameworks.

License info

The library is covered under the LGPL. Permission was obtained from the author and contributors of Motion to make this possible. You can freely use it as long as you release your changes to it and dynamically link to it. This is to protect the freedom of end users to upgrade the library (eg. to support new hardware).

Note that the library is covered under the LGPL version 2.1 ONLY. It cannot be combined with [L]GPL v3 code because the new licenses are not compatible with previous versions of the [L]GPL.

The applications are covered under the GPL. If you are writing a commercial application you should stick to the library and avoid the apps (though calling the motion program and parsing it's output is ok).

Note that the applications are covered under the GPL version 2 ONLY. Their code cannot be combined with [L]GPL v3 code because the new licenses are not compatible with previous versions of the [L]GPL.

Why do I use the [L]GPL v2? I think Linus said it best in a recent interview I read:
> The GPLv3 tries to control the 'use' of that source code.
> Now it's, "I give you my source code, so if you use it,
> you'd better make your devices hackable by me." See?
> Petty and small-minded, in my opinion.

Contributors

Christian Klein - Motion author
Pall Thayer - Support for iBook and high res Powerbook
Steven Riggins - Support for high res Powerbook
Randy Green - Support for MacBook Pro
Lincoln Ramsay - Unified hardware support

Machines

There are a number of machines with SMS hardware. As of now, there are 3 different sets of data to handle.

iBook/Powerbook 1G = ~50, positive force = left, towards you, down
High-res Powerbook 1G = ~50, positive force = right, away from you, up
MacBook [Pro] 1G = ~250, positive force = left, towards you, down

Based on reports by users, the offset can vary by as much as 10 on iBook/PowerBook and High-res Powerbook and 30 on MacBook [Pro]. If your machine does not return values in the range indicated please email me (ramsayl@bigfoot.com) with the information.

Force vs Rotation

The SMS hardware reports force, it does not report rotation. Using the X and Y values for simple games will probably work but accuracy will be lost after about 45 degrees of rotation. To get 360 degree rotation based on the force values you need to do some funky maths. Look online for "AN005 Tilt" and you should find a document that explains the maths.
