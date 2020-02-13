#ifndef __UsingNames_h_
#define __UsingNames_h_

#include "ClassMatrix.h"

#include "itkImageRegionIterator.h"
#include "itkImageFileWriter.h"

using TPixel = int;
const int VDim = 3;
using ImageType = itk::Image<TPixel, VDim>;
using ImagePointer = ImageType::Pointer;
using ImageType2d = itk::Image<TPixel, 2>;
using IndexType = ImageType::IndexType;
using SizeType = ImageType::SizeType;
using RegionType = itk::ImageRegion<VDim>;
using IteratorType = itk::ImageRegionIterator<ImageType>;
using WriterType = itk::ImageFileWriter<ImageType>;

#endif