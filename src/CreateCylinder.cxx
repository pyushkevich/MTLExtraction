#include "CreateCylinder.h"

// Create a 3D image of a cylinder, representing the MRI holder
ImagePointer CreateSmallCylinder(ImagePointer iref) {
	
	RegionType region;
	const RegionType largestRegion_ref = iref->GetLargestPossibleRegion();

	region.SetSize(largestRegion_ref.GetSize());
	region.SetIndex(largestRegion_ref.GetIndex());

	// Create the image
	ImagePointer img = ImageType::New();
	img->SetRegions(region);
	img->Allocate();
	img->FillBuffer(0);
	img->CopyInformation(iref);
	img->Update();

	IteratorType it(img, img->GetBufferedRegion());
	it.GoToBegin();
	double x2, y2;

	ImageType::SizeType size = img->GetLargestPossibleRegion().GetSize();
	double middleX = size[0] / 2;
	double middleY = size[1] / 2;
	double middleZ = size[2] / 2;
	double radius = 31.0 / img->GetSpacing()[0];
	const double radius2 = radius * radius;

	double z_min = middleZ - (39 / img->GetSpacing()[2]);
	double z_max = middleZ + (39 / img->GetSpacing()[2]);

	while (!it.IsAtEnd())
	{
		//Condition on z
		if ((it.GetIndex()[2] > z_min) && (it.GetIndex()[2] < z_max))
		{
			x2 = (it.GetIndex()[0] - middleX) * (it.GetIndex()[0] - middleX);
			y2 = (it.GetIndex()[1] - middleY) * (it.GetIndex()[1] - middleY);
			if (x2 + y2 < radius2)
			{
				it.Set(1);
			}
		}
		++it;
	}


	img->Update();

	return img;
}