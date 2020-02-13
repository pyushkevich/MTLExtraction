#include "PadImage.h"

// This is a Convert3D function

ImagePointer PadImage(ImagePointer img, ImageType::SizeType padExtentLower, ImageType::SizeType padExtentUpper, double padValue)
{	
	typedef itk::ConstantPadImageFilter< ImageType, ImageType > ConstantPad;
	typename ConstantPad::Pointer padFilter = ConstantPad::New();
	padFilter->SetInput(img);
	padFilter->SetPadLowerBound(padExtentLower);
	padFilter->SetPadUpperBound(padExtentUpper);

	padFilter->SetConstant(padValue);
	try
	{
		padFilter->Update();
	}
	catch (itk::ExceptionObject & error)
	{
		std::cerr << "Error: " << error << std::endl;
	}
	return padFilter->GetOutput();
}
