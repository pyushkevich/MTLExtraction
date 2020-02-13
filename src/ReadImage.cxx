#include "ReadImage.h"

ImagePointer ReadImage(char* FileName) {
	
	using ReaderType = itk::ImageFileReader<ImageType>;
	ReaderType::Pointer reader = ReaderType::New();
	reader->SetFileName(FileName);
	try
	{
		reader->Update();
	}
	catch (itk::ExceptionObject & err)
	{
		std::cerr << "ExceptionObject caught !" << std::endl;
		std::cerr << err << std::endl;
	}

	ImagePointer image = reader->GetOutput();
	return image;
}

ImagePointer ReadImage(std::string name) {
	
	using ReaderType = itk::ImageFileReader<ImageType>;
	ReaderType::Pointer reader = ReaderType::New();
	reader->SetFileName(name);
	try
	{
		reader->Update();
	}
	catch (itk::ExceptionObject & err)
	{
		std::cerr << "ExceptionObject caught !" << std::endl;
		std::cerr << err << std::endl;
	}

	ImagePointer image = reader->GetOutput();
	return image;
}
