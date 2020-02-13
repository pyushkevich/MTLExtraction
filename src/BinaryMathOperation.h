#ifndef __BinaryMathOperation_h_
#define __BinaryMathOperation_h_

#include "UsingNames.h"

#include "itkAddImageFilter.h"
#include "itkSubtractImageFilter.h"
#include "itkMultiplyImageFilter.h"
#include "itkDivideImageFilter.h"
#include "itkMinimumImageFilter.h"
#include "itkMaximumImageFilter.h"
#include "itkAtan2ImageFilter.h"

// Operations supported by this class
enum Operation
{
	ADD = 0,
	ATAN2,
	SUBTRACT,
	MULTIPLY,
	DIVIDE,
	MAXIMUM,
	MINIMUM
};

ImagePointer BinaryMathOperation(Operation, ImagePointer, ImagePointer);

#endif