#ifndef _MathematicalMorphology_h
#define _MathematicalMorphology_h

#include "UsingNames.h"

#include "itkBinaryDilateImageFilter.h"
#include "itkBinaryErodeImageFilter.h"
#include "itkBinaryBallStructuringElement.h"

ImagePointer MorphologyTransformation(ImagePointer img, bool erode, TPixel value, SizeType radius);

#endif