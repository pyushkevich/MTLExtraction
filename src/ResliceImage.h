#ifndef __ResliceImage_h_
#define __ResliceImage_h_

#include "UsingNames.h"

#include "ReadMatrix.h"
#include "itkAffineTransform.h"
#include "itkResampleImageFilter.h"
#include "itkImageMomentsCalculator.h"


ImagePointer ResliceImage(ImagePointer FixeImage, ImagePointer MovingImage, char* FileMatrix);
ImagePointer ResliceImage(ImagePointer MovingImage, char* FileMatrix);
ImagePointer ResliceImage(ImagePointer FixeImage, ImagePointer MovingImage, MatrixType matrix);
ImagePointer ResliceImage(ImagePointer MovingImage, MatrixType matrix);
ImagePointer ResliceImage(ImagePointer FixeImage, ImagePointer MovingImage, RotationAxis axis, double angle);
ImagePointer ResliceImage(ImagePointer MovingImage, RotationAxis axis, double angle);
ImagePointer ResliceImage(ImagePointer FixeImage, ImagePointer MovingImage, double t_x, double t_y, double t_z);
ImagePointer ResliceImage(ImagePointer MovingImage, double t_x, double t_y, double t_z);
ImagePointer ResliceImage(ImagePointer FixeImage, ImagePointer MovingImage, RotationAxis axis, double angle, double t_x, double t_y, double t_z);
ImagePointer ResliceImage(ImagePointer MovingImage, RotationAxis axis, double angle, double t_x, double t_y, double t_z);
itk::Vector<double,3> MatchGravityCenters(ImagePointer iref, ImagePointer isrc);

#endif