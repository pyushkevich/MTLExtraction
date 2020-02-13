#include "ExtrudeSegmentation.h"

#include "ReadImage.h"

#include "BinaryMathOperation.h"
#include "itkImageLinearIteratorWithIndex.h"
#include "itkImageDuplicator.h"

#include "WriteImage.h"

TPixel LineMinimum = 0;

TPixel MinimumIntensityProjectionFunctor(const TPixel& value, int line_pos, int line_len)
{
    extern TPixel LineMinimum;
    
    if (line_pos == 0)
    {
        LineMinimum = value;
        return value;
    }

    if (value < LineMinimum)
        LineMinimum = value;

    return LineMinimum;
}


ImagePointer ExtrudeSegmentation(ImagePointer iref, ImagePointer isrc) {
	//ImagePointer mult = BinaryMathOperation(MULTIPLY, isrc, iref);

    // Go trhough all the pixels of the multiplication
    // If the value of the pixel is 0, then all the following line is zero.

    typedef itk::ImageLinearIteratorWithIndex<ImageType> IteratorType;

    // This is the line iterator, although for even greater speed we operate
    // directly on pointers, so we only use it's NextLine functionality()
    IteratorType itOutput(iref, iref->GetLargestPossibleRegion());

    // Create the input iterator that similarly iterates over line
    IteratorType itInput(isrc, isrc->GetLargestPossibleRegion());

    // Get the line length
    int line_length = iref->GetLargestPossibleRegion().GetSize(0);

    // Progress
    const size_t numberOfLinesToProcess = iref->GetLargestPossibleRegion().GetNumberOfPixels() / line_length;

    // Start iterating over lines
    while (!itOutput.IsAtEnd())
    {
        // Iterate over this line
        for (int p = 0; p < line_length; p++, ++itInput, ++itOutput)
        {
            itOutput.Set(MinimumIntensityProjectionFunctor(itInput.Get(), p, line_length));
        }

        itInput.NextLine();
        itOutput.NextLine();
    }
    iref->Update();
    return iref;

}

ImagePointer ExtrudeSegmentation(ImagePointer isrc) {
	
    using DuplicatorType = itk::ImageDuplicator<ImageType>;
    DuplicatorType::Pointer duplicator = DuplicatorType::New();
    duplicator->SetInputImage(isrc);
    duplicator->Update();

    ImageType::Pointer iref = duplicator->GetOutput();

    // Go trhough all the pixels of the multiplication
    // If the value of the pixel is 0, then all the following line is zero.

    typedef itk::ImageLinearIteratorWithIndex<ImageType> IteratorType;

    // This is the line iterator, although for even greater speed we operate
    // directly on pointers, so we only use it's NextLine functionality()
    IteratorType itOutput(iref, iref->GetLargestPossibleRegion());

    // Create the input iterator that similarly iterates over line (accross the X axis)
    IteratorType itInput(isrc, isrc->GetLargestPossibleRegion());

    // Get the line length
    int line_length = iref->GetLargestPossibleRegion().GetSize(0);

    // Progress
    const size_t numberOfLinesToProcess = iref->GetLargestPossibleRegion().GetNumberOfPixels() / line_length;

    // Start iterating over lines
    while (!itOutput.IsAtEnd())
    {
        // Iterate over this line
        for (int p = 0; p < line_length; p++, ++itInput, ++itOutput)
        {
            itOutput.Set(MinimumIntensityProjectionFunctor(itInput.Get(), p, line_length));
        }
        itInput.NextLine();
        itOutput.NextLine();
    }
    iref->Update();
    return iref;

}