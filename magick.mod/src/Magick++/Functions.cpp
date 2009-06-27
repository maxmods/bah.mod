// This may look like C code, but it is really -*- C++ -*-
//
// Copyright Bob Friesenhahn, 1999, 2002, 2003
//
// Simple C++ function wrappers for ImageMagick equivalents
//

#define MAGICK_IMPLEMENTATION
#define MAGICK_PLUSPLUS_IMPLEMENTATION

#include "Magick++/Include.h"
#include <string>

using namespace std;

#include "Magick++/Functions.h"

// Clone C++ string as allocated C string, de-allocating any existing string
void Magick::CloneString( char **destination_, const std::string &source_ )
{
  MagickLib::CloneString( destination_, source_.c_str() );
}
