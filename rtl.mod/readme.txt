Building The Shared Library
===========================

Windows
-------

In the src folder, simply run the command :

make -f makefile.mingw

You may have to first delete the .o, .a and .dll files.
(Yes, I was too lazy to make a "clean" script!)


MacOS
-----

Download a distribution of libfribidi, and :

./configure
make


It will create a .dylib named something like, libfribidi.0.3.0.dylib.
Rename to libfribidi.dylib.

Then run the following command to fix the expected install path :

install_name_tool -id @executable_path/libfribidi.dylib libfribidi.dylib

Now you can drop the .dylib into your application bundle.



Linux
-----

Install the libfribidi package for your system. You will also need the libfribidi-dev package
for compiling.
