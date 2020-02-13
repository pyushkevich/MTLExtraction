#include "CoordinateMap.h"

// Divide an image into 3 images corresponding to each axis X, Y, Z
// Using physical coordinates
std::array<ImagePointer, 3> PhysicalCoordinates(ImagePointer img){

	std::array<ImagePointer,3> CoordImages;
	// Create three new images
	ImagePointer coord[VDim];
	IteratorType it[VDim];

	for (size_t i = 0; i < VDim; i++)
	{
		coord[i] = ImageType::New();
		coord[i]->SetRegions(img->GetBufferedRegion());
		coord[i]->CopyInformation(img);
		coord[i]->Allocate();
		it[i] = IteratorType(coord[i], img->GetBufferedRegion());
	}
	while (!it[0].IsAtEnd())
	{
		IndexType idx = it[0].GetIndex();
		typename ImageType::PointType p;
		img->TransformIndexToPhysicalPoint(idx, p);
		for (size_t i = 0; i < VDim; i++){
			it[i].Set(p[i]);
		}
		for (size_t i = 0; i < VDim; i++) {
			++it[i];
		}		
	}

	for (int i = 0;i < VDim;i++) {
		CoordImages[i] = coord[i];
	}
	
	return CoordImages;
}

// Using index
std::array<ImagePointer, 3> IndexCoordinates(ImagePointer img) {

	std::array<ImagePointer, 3> CoordImages;
	// Create three new images
	ImagePointer coord[VDim];
	IteratorType it[VDim];

	for (size_t i = 0; i < VDim; i++)
	{
		coord[i] = ImageType::New();
		coord[i]->SetRegions(img->GetBufferedRegion());
		coord[i]->CopyInformation(img);
		try {
			coord[i]->Allocate();
		}
		catch (std::bad_alloc) {
			std::cerr << "Bad alloc." << std::endl;
		}
		it[i] = IteratorType(coord[i], img->GetBufferedRegion());
	}
	for (int i = 0;i < VDim;i++) {
		CoordImages[i] = coord[i];
	}
	return CoordImages;
}