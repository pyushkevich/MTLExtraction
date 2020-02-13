#ifndef __ReadImage_h_
#define __ReadImage_h_


#include "itkImageFileReader.h"
#include "UsingNames.h"

ImagePointer ReadImage(char*); //Read a 3D image
ImagePointer ReadImage(std::string);

#endif