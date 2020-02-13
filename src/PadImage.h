#ifndef __PadImage_h_
#define __PadImage_h_

#include "UsingNames.h"
#include "itkConstantPadImageFilter.h"

ImagePointer PadImage(ImagePointer, ImageType::SizeType, ImageType::SizeType, double);

#endif