#ifndef _EnergyFunction_h_
#define _EnergyFunction_h_

#include "UsingNames.h"
#include "ResliceImage.h"


ImagePointer GenerateImage(ImagePointer img, RotationAxis axis, double angle, double translations[3]);
ImagePointer GenerateImage(ImagePointer img, RotationAxis axis, double angle, double t_x, double t_y, double t_z);
ImagePointer IntersectionImages(std::vector<ImagePointer> table_images);
double GetVolume(ImagePointer img);
#endif