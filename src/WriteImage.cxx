#include "WriteImage.h"

void Write(char* filename, ImagePointer image) {
	
	using WriterType = itk::ImageFileWriter<ImageType>;
	WriterType::Pointer writer = WriterType::New();
	writer->SetFileName(filename);
	writer->SetInput(image);

	try
	{
		writer->Update();
	}
	catch (itk::ExceptionObject & error)
	{
		std::cerr << "Error: " << error << std::endl;
	}
	writer->ReleaseDataFlagOn();
}

void Write(std::string filename, ImagePointer image) {
	
	using WriterType = itk::ImageFileWriter<ImageType>;
	WriterType::Pointer writer = WriterType::New();
	writer->SetFileName(filename);
	writer->SetInput(image);

	try
	{
		writer->Update();
	}
	catch (itk::ExceptionObject & error)
	{
		std::cerr << "Error: " << error << std::endl;
	}
	writer->ReleaseDataFlagOn();
}