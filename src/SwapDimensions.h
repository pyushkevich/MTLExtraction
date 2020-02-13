#ifndef _SwapDimensions_h_
#define _SwapDimensions_h_

#include "UsingNames.h"

using namespace itk::SpatialOrientation;

ValidCoordinateOrientationFlags GetOrientationFlagFromString(const std::string &code);
ImagePointer SwapDimensions_OrientWorker(ImagePointer img, const std::string &code);

#endif