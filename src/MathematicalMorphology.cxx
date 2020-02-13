#include "MathematicalMorphology.h"

ImagePointer MorphologyTransformation(ImagePointer img, bool erode, TPixel value, SizeType radius)
{
	// Define the structuring element
	typedef itk::BinaryBallStructuringElement<TPixel, VDim> Element;
	Element elt;
	elt.SetRadius(radius);
	elt.CreateStructuringElement();

	// Chose the right filter
	typedef itk::BinaryMorphologyImageFilter<ImageType, ImageType, Element> FilterType;
	ImagePointer output;
	if (erode)
	{
		typedef itk::BinaryErodeImageFilter<ImageType, ImageType, Element> FilterType;
		typename FilterType::Pointer filter = FilterType::New();
		filter->SetInput(img);
		filter->SetErodeValue(value);
		filter->SetKernel(elt);
		filter->Update();
		output = filter->GetOutput();
	}
	else
	{
		typedef itk::BinaryDilateImageFilter<ImageType, ImageType, Element> FilterType;
		typename FilterType::Pointer filter = FilterType::New();
		filter->SetInput(img);
		filter->SetDilateValue(value);
		filter->SetKernel(elt);
		filter->Update();
		output = filter->GetOutput();
	}

	return output;
}