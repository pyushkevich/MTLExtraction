
#ifndef __Thresholding_h_
#define __Thresholding_h_

#include "UsingNames.h"
#include "itkBinaryThresholdImageFilter.h"
typedef itk::BinaryThresholdImageFilter<ImageType, ImageType> FilterType;

ImagePointer Thresh(ImagePointer, double, double, double, double);

#endif