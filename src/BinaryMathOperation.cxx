#include "BinaryMathOperation.h"

// This is function of Convert3D, simplified for this programm (using only 3D images)

ImagePointer BinaryMathOperation(Operation op, ImagePointer i1, ImagePointer i2)
{
	// Select the operation
	typedef typename itk::ImageToImageFilter<ImageType, ImageType> BaseFilterType;
	typename BaseFilterType::Pointer filter;

	switch (op)
	{
	case ADD:
		filter = itk::AddImageFilter<ImageType, ImageType, ImageType>::New();
		break;
	case ATAN2:
		filter = itk::Atan2ImageFilter<ImageType, ImageType, ImageType>::New();
		break;
	case SUBTRACT:
		filter = itk::SubtractImageFilter<ImageType, ImageType, ImageType>::New();
		break;
	case MULTIPLY:
		filter = itk::MultiplyImageFilter<ImageType, ImageType, ImageType>::New();
		break;
	case DIVIDE:
		filter = itk::DivideImageFilter<ImageType, ImageType, ImageType>::New();
		break;
	case MINIMUM:
		filter = itk::MinimumImageFilter<ImageType, ImageType, ImageType>::New();
		break;
	case MAXIMUM:
		filter = itk::MaximumImageFilter<ImageType, ImageType, ImageType>::New();
		break;
	}

	// Run the filter
	filter->SetInput(0, i1);
	filter->SetInput(1, i2);
	try
	{
		filter->Update();
	}
	catch (itk::ExceptionObject & err)
	{
		std::cerr << "ExceptionObject caught !" << std::endl;
		std::cerr << err << std::endl;
	}
	return filter->GetOutput();
}