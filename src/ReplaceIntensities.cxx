#include "ReplaceIntensities.h"

// This is a Convert3D function

ImagePointer ReplaceIntensities(ImagePointer img, double I1, double I2)
{
    // This is a slightly sensitive procedure. We need to set an epsilon so that
    // intensities that are within machine precision are treated as equal. We define
    // this as 2 (a - b) / (a + b) < eps.
    double epsilon = 0.000001;

    // Create an iterator to process the image
    typedef itk::ImageRegionIteratorWithIndex<ImageType> Iterator;
    for(Iterator it(img, img->GetBufferedRegion()); !it.IsAtEnd(); ++it)
    {
        // Get the pixel value
        TPixel val = it.Get();

        if( (val == I1) || fabs(2*(val-I1)/(val+I1)) < epsilon)
        {
            it.Set(I2);
        }
    }
    img->Update();
    return img;
}