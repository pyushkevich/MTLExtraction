#include "ReferencePlan.h"

//#include "ReadImage.h"
#include "Thresholding.h"
#include "BinaryMathOperation.h"

ImagePointer CreatePlanN(ImagePointer iref, float width)
{
	vnl_matrix_fixed<double, 3U, 3U> dir = iref->GetDirection().GetVnlMatrix();

	RegionType region;
	const RegionType largestRegion_ref = iref->GetLargestPossibleRegion();

	region.SetSize(largestRegion_ref.GetSize());
	region.SetIndex(largestRegion_ref.GetIndex());

	// Create the images
	ImagePointer imgN = ImageType::New();
	imgN->SetRegions(region);
	imgN->Allocate();
	imgN->FillBuffer(0);
	imgN->CopyInformation(iref);
	imgN->Update();


	IteratorType itP(imgN, imgN->GetBufferedRegion());
	itP.GoToBegin();

	ImageType::SizeType size = imgN->GetLargestPossibleRegion().GetSize();
	double middleX = size[0] / 2 - 60;
	double middleY = size[1] / 2;
	double middleZ = size[2] / 2;

	double x_min = middleX - (width / (2 * imgN->GetSpacing()[0]));
	double x_max = middleX + (width / (2 * imgN->GetSpacing()[0]));

	while (!itP.IsAtEnd())
	{
		if ((itP.GetIndex()[0] > x_min) && (itP.GetIndex()[0] < x_max))
		{
			itP.Set(2);
		}
		if (dir[0][0] == -1){
			if (itP.GetIndex()[0] < x_min) {
				itP.Set(1);
			}
		}
		else{
			if (itP.GetIndex()[0] > x_max) {
				itP.Set(1);
			}
		}
		
		++itP;
	}

	imgN->Update();
	return imgN;
}

/*
ImagePointer CreateRealCut(ImagePointer planN, char orientation)
{
	ImagePointer imgP = Thresh(planN, -0.1, 1.9, 0, 1);
	ImagePointer Image;
	ImageType::PointType max;
	double thresh;

	std::array<ImagePointer, 3> CMP = PhysicalCoordinates(planN);

	ImageType::IndexType size;
	
	switch (orientation) {
	case 'v':
		size[0] = CMP[1]->GetLargestPossibleRegion().GetSize()[0];
		size[1] = CMP[1]->GetLargestPossibleRegion().GetSize()[1];
		size[2] = CMP[1]->GetLargestPossibleRegion().GetSize()[2];
		CMP[1]->TransformIndexToPhysicalPoint(size, max);

		thresh = -1 * max[1] + 90 * CMP[1]->GetSpacing()[1];
		Image = Thresh(CMP[1], -vnl_huge_val(0), thresh, 1, 0);
		
		Image = BinaryMathOperation(MINIMUM, Image, imgP);
		break;
	case 'h':
		size[0] = CMP[2]->GetLargestPossibleRegion().GetSize()[0];
		size[1] = CMP[2]->GetLargestPossibleRegion().GetSize()[1];
		size[2] = CMP[2]->GetLargestPossibleRegion().GetSize()[2];
		CMP[2]->TransformIndexToPhysicalPoint(size, max);
		thresh = -1 * max[2] + 50 * CMP[2]->GetSpacing()[2];

		Image = Thresh(CMP[2], -vnl_huge_val(0), thresh, 0, 1);

		Image = BinaryMathOperation(MINIMUM, Image, imgP);
		break;
	}

	return Image;
}
*/

ImagePointer CreateRealCut(ImagePointer planN)
{
	ImagePointer cut = Thresh(planN, -1, 1.9, 0, 1);
	cut = BinaryMathOperation(MINIMUM, planN, cut);	

	return cut;
}