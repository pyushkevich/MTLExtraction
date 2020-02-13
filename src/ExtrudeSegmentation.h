#ifndef _ExtrudeSegmentation_h
#define _ExtrudeSegmentation_h

#include "UsingNames.h"

TPixel MinimumIntensityProjectionFunctor(const TPixel& value, int line_pos, int line_len);
ImagePointer ExtrudeSegmentation(ImagePointer iref, ImagePointer isrc);
ImagePointer ExtrudeSegmentation(ImagePointer isrc);

#endif