#include "Thresholding.h"

ImagePointer Thresh(ImagePointer input, double u1, double u2, double vIn, double vOut) {
	typename FilterType::Pointer filter = FilterType::New();
	filter->SetInput(input);
	filter->SetLowerThreshold(u1);
	filter->SetUpperThreshold(u2);
	filter->SetInsideValue(vIn);
	filter->SetOutsideValue(vOut);
	try
	{
		filter->UpdateLargestPossibleRegion();
	}
	catch (itk::ExceptionObject & error)
	{
		std::cerr << "Error: " << error << std::endl;
	}
	return filter->GetOutput();
}
