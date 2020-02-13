#ifndef INIT_H
#define INIT_H

#include "UsingNames.h"
#include "BinaryMathOperation.h"
#include "Thresholding.h"

ImagePointer ExtractSegmentatedImage(ImagePointer img, ImagePointer img_seg);
ImagePointer ExtractBackgroung(ImagePointer img, ImagePointer img_seg);


#endif