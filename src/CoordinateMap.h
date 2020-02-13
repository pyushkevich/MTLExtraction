#ifndef _CoordinateMap_h_
#define _CoordinateMap_h_

#include "UsingNames.h"

std::array<ImagePointer, 3> PhysicalCoordinates(ImagePointer img); //Create three images (one per axis) with physical coordinates

std::array<ImagePointer, 3> IndexCoordinates(ImagePointer img); //Create three images (one per axis) with index coordinates

#endif