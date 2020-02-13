#ifndef _OriginVoxels_h_
#define _OriginVoxels_h_

#include "itkMatrixOffsetTransformBase.h"
#include "UsingNames.h"

// Support for regular expressions via KWSYS in ITK
#include <itksys/RegularExpression.hxx>

typedef vnl_vector_fixed<double, VDim> RealVector;
typedef itk::Matrix<double, 4, 4> TransformMatrixType;
// How the specification is made
enum VecSpec { PHYSICAL, VOXELS, PERCENT, NONE };


std::string str_to_lower(const char* input);
TransformMatrixType GetVoxelSpaceToRASPhysicalSpaceMatrix(ImageType::Pointer image);
RealVector ReadRealVector(ImagePointer img, const char* vec_in, bool is_point);
void ReadVecSpec(const char* vec_in, vnl_vector_fixed<double, VDim>& vout, VecSpec& type);
ImagePointer SetOriginVoxel(ImagePointer img, const char* vec_in);

#endif