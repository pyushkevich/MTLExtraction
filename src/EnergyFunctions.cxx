#include "EnergyFunctions.h"
#include "itkImageDuplicator.h"
#include "BinaryMathOperation.h"
#include "itkStatisticsImageFilter.h"

ImagePointer GenerateImage(ImagePointer img, RotationAxis axis, double angle, double translations[3]) {

	
	using DuplicatorType = itk::ImageDuplicator<ImageType>;
	DuplicatorType::Pointer duplicator = DuplicatorType::New();
	duplicator->SetInputImage(img);
	duplicator->Update();
	ImagePointer new_img = duplicator->GetOutput();
	
//	ImagePointer new_img;
	MatrixType mat;
	mat.SetRotationAxis(axis);
	double angle_rad = angle * vnl_math::pi_over_180;
	mat.SetParameters(axis, angle_rad, translations[0], translations[1], translations[2]);

	new_img = ResliceImage(img, img, mat);

	return new_img;
}
ImagePointer GenerateImage(ImagePointer img, RotationAxis axis, double angle, double t_x, double t_y, double t_z) {
	ImagePointer new_img;
	MatrixType mat;
	double angle_rad = angle * vnl_math::pi_over_180;
	mat.SetParameters(axis, angle_rad, t_x, t_y, t_z);
	new_img = ResliceImage(img, img, mat);

	return new_img;
}

ImagePointer IntersectionImages(std::vector<ImagePointer> table_images){
	
	std::string file_path = "/Users/Jade/Desktop/MTLExtraction-Mac/bin/img_mult";
	std::string file_name;
	std::string file_nb;
	
	ImagePointer img = *table_images.begin();
	
	int nb_images = table_images.size();

	for (int i=1;i<nb_images;i++){
		img = BinaryMathOperation(MULTIPLY, img, table_images.at(i));
		file_nb = std::to_string(i);
		file_name = file_path + file_nb + ".nii.gz";
		//Write(file_name,img);
	}
	return img;
}

double GetVolume(ImagePointer img){

	using StatisticsFilterType = itk::StatisticsImageFilter< ImageType >;
  	StatisticsFilterType::Pointer stat = StatisticsFilterType::New();

	ImageType::SpacingType spacing = img->GetSpacing(); 
	double volumeOfOneVoxel = spacing[0] * spacing[1] * spacing[2];
  	stat->SetInput(img);
	stat->UpdateLargestPossibleRegion();
	double nrOfVoxels = stat->GetSum();

	double totalVolume = nrOfVoxels * volumeOfOneVoxel;
	return totalVolume;
}