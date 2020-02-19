#include "ResliceImage.h"

ImagePointer ResliceImage(ImagePointer FixeImage, ImagePointer MovingImage, char* FileMatrix)
{
	return ResliceImage(MovingImage, std::string(FileMatrix));
}

ImagePointer ResliceImage(ImagePointer MovingImage, char* FileMatrix)
{
	return ResliceImage(MovingImage, std::string(FileMatrix));
}

ImagePointer ResliceImage(ImagePointer MovingImage, std::string FileMatrix)
{
	ImagePointer FixeImage = MovingImage;
	// Create an initial identity transform
	typedef itk::AffineTransform<double, VDim> TranType;
	typename TranType::Pointer atran = TranType::New();
	atran->SetIdentity();

	// Read transform based on type
	size_t mat, txt;
	mat = FileMatrix.find(".mat");
	txt = FileMatrix.find(".txt");

  	if(txt != std::string::npos)
    {
		typedef itk::MatrixOffsetTransformBase<double, VDim, VDim> MOTBType;
		typedef itk::AffineTransform<double, VDim> AffTran;
		itk::TransformFactory<MOTBType>::RegisterTransform();
		itk::TransformFactory<AffTran>::RegisterTransform();

		itk::TransformFileReader::Pointer fltReader = itk::TransformFileReader::New();
		fltReader->SetFileName(FileMatrix);
		fltReader->Update();

		itk::TransformBase *base = fltReader->GetTransformList()->front();
		MOTBType *motb = dynamic_cast<MOTBType *>(base);

		if(motb)
		{
			atran->SetMatrix(motb->GetMatrix());
			atran->SetOffset(motb->GetOffset());
		}
    }
  	else if(mat != std::string::npos)
		{
		// Read the matrix
		itk::Matrix<double,VDim+1,VDim+1> matrix;
		matrix = ReadMatrix(FileMatrix);

		// Get the transform matrix and the offset vector
		vnl_matrix<double> A_ras = matrix.GetVnlMatrix().extract(VDim, VDim); 
		vnl_vector<double> b_ras = matrix.GetVnlMatrix().extract(VDim, 1, 0, VDim).get_column(0);

		// Extrernal matrices are assumed to be RAS to RAS, so we must convert to LPS to LPS
		vnl_vector<double> v_lps_to_ras(VDim, 1.0);
		v_lps_to_ras[0] = v_lps_to_ras[1] = -1.0;
		vnl_diag_matrix<double> m_lps_to_ras(v_lps_to_ras);
		vnl_matrix<double> A_lps = m_lps_to_ras * A_ras * m_lps_to_ras;
		vnl_vector<double> b_lps = m_lps_to_ras * b_ras;

		// Stick these into the itk matrix/vector
		itk::Matrix<double,VDim,VDim> amat(A_lps);
		itk::Vector<double, VDim> aoff;
		aoff.SetVnlVector(b_lps);

		// Put the values in the transform
		atran->SetMatrix(amat);
		atran->SetOffset(aoff);
    }

	// Build the resampling filter
	typedef itk::ResampleImageFilter<ImageType,ImageType> ResampleFilterType;
	typename ResampleFilterType::Pointer fltSample = ResampleFilterType::New();
	using LinearInterpolatorType = itk::LinearInterpolateImageFunction<ImageType, double>;
  	LinearInterpolatorType::Pointer interpolator = LinearInterpolatorType::New();

	// Initialize the resampling filter with an identity transform
	fltSample->SetInput(MovingImage);
	fltSample->SetTransform(atran);
  
	// Set the unknown intensity to positive value
	fltSample->SetDefaultPixelValue(0);

	// Set the interpolator
	fltSample->SetInterpolator(interpolator);

	// Calculate where the transform is taking things
	itk::ContinuousIndex<double, VDim> idx[3];
	for(size_t i = 0; i < VDim; i++){
		idx[0][i] = 0.0;
		idx[1][i] = FixeImage->GetBufferedRegion().GetSize(i) / 2.0;
		idx[2][i] = FixeImage->GetBufferedRegion().GetSize(i) - 1.0;
	}
  	for(size_t j = 0; j < VDim; j++)
    {
		itk::ContinuousIndex<double, VDim> idxmov;
		itk::Point<double, VDim> pref, pmov;
		FixeImage->TransformContinuousIndexToPhysicalPoint(idx[j], pref);
		pmov = atran->TransformPoint(pref);
		MovingImage->TransformPhysicalPointToContinuousIndex(pmov, idxmov);
    }

  	vnl_matrix<double> amat(VDim+1, VDim+1, 0); 
 	vnl_vector<double> atmp(VDim+1, 0); 
  	amat.update(atran->GetMatrix().GetVnlMatrix(), 0, 0);
  	atmp.update(atran->GetOffset().GetVnlVector(), 0);
  	amat.set_column(VDim, atmp);

	// Set the spacing, origin, direction of the output
	fltSample->UseReferenceImageOn();
	fltSample->SetReferenceImage(MovingImage);
	fltSample->Update();
    

	return fltSample->GetOutput();
}

ImagePointer ResliceImage(ImagePointer FixeImage, ImagePointer MovingImage, MatrixType matrix)
{
	// Create an initial identity transform
	typedef itk::AffineTransform<double, VDim> TranType;
	typename TranType::Pointer atran = TranType::New();
	atran->SetIdentity();

	// get transform parameters from MatrixType
	TranType::ParametersType parameters(3 * 3 + 3);
	for (unsigned int i = 0; i < 3; i++) {
		for (unsigned int j = 0; j < 3; j++) {
			parameters[i * 3 + j] = matrix[i][j];
		}
	}
	for (unsigned int i = 0; i < 3; i++) {
		parameters[i + 3 * 3] = matrix[i][3];
	}

	// Put the values in the transform
	atran->SetParameters(parameters);

	// Build the resampling filter
	typedef itk::ResampleImageFilter<ImageType, ImageType> ResampleFilterType;
	typename ResampleFilterType::Pointer fltSample = ResampleFilterType::New();

	// Initialize the resampling filter with an identity transform
	fltSample->SetInput(MovingImage);
	fltSample->SetTransform(atran);

	// Set the unknown intensity to positive value
	fltSample->SetDefaultPixelValue(0);

	// Set the interpolator
	using FixedLinearInterpolatorType = itk::LinearInterpolateImageFunction<ImageType, double>;
	fltSample->SetInterpolator(FixedLinearInterpolatorType::New());

	// Calculate where the transform is taking things
	itk::ContinuousIndex<double, VDim> idx[3];
	idx->Fill(0.0);
	for (size_t i = 0; i < VDim; i++)
	{
		idx[0][i] = 0.0;
		idx[1][i] = FixeImage->GetBufferedRegion().GetSize(i) / 2.0;
		idx[2][i] = FixeImage->GetBufferedRegion().GetSize(i) - 1.0;
	}
	for (size_t j = 0; j < VDim; j++)
	{
		itk::ContinuousIndex<double, VDim> idxmov;
		itk::Point<double, VDim> pref, pmov;
		FixeImage->TransformContinuousIndexToPhysicalPoint(idx[j], pref);
		pmov = atran->TransformPoint(pref);
		MovingImage->TransformPhysicalPointToContinuousIndex(pmov, idxmov);
	}


	// Set the spacing, origin, direction of the output
	fltSample->UseReferenceImageOn();
	fltSample->SetReferenceImage(FixeImage);
	fltSample->Update();
	ImagePointer output = fltSample->GetOutput();
	return output;
}

ImagePointer ResliceImage(ImagePointer MovingImage, MatrixType matrix)
{
	ImagePointer FixeImage = MovingImage;
	// Create an initial identity transform
	typedef itk::AffineTransform<double, VDim> TranType;
	typename TranType::Pointer atran = TranType::New();
	atran->SetIdentity();

	// get transform parameters from MatrixType
	TranType::ParametersType parameters(3 * 3 + 3);
	for (unsigned int i = 0; i < 3; i++) {
		for (unsigned int j = 0; j < 3; j++) {
			parameters[i * 3 + j] = matrix[i][j];
		}
	}
	for (unsigned int i = 0; i < 3; i++) {
		parameters[i + 3 * 3] = matrix[i][3];
	}

	// Put the values in the transform
	atran->SetParameters(parameters);

	// Build the resampling filter
	typedef itk::ResampleImageFilter<ImageType, ImageType> ResampleFilterType;
	typename ResampleFilterType::Pointer fltSample = ResampleFilterType::New();

	// Initialize the resampling filter with an identity transform
	fltSample->SetInput(MovingImage);
	fltSample->SetTransform(atran);

	// Set the unknown intensity to positive value
	fltSample->SetDefaultPixelValue(0);

	// Set the interpolator
	using FixedLinearInterpolatorType = itk::LinearInterpolateImageFunction<ImageType, double>;
	fltSample->SetInterpolator(FixedLinearInterpolatorType::New());

	// Calculate where the transform is taking things
	itk::ContinuousIndex<double, VDim> idx[3];
	idx->Fill(0.0);
	for (size_t i = 0; i < VDim; i++)
	{
		idx[0][i] = 0.0;
		idx[1][i] = FixeImage->GetBufferedRegion().GetSize(i) / 2.0;
		idx[2][i] = FixeImage->GetBufferedRegion().GetSize(i) - 1.0;
	}
	for (size_t j = 0; j < VDim; j++)
	{
		itk::ContinuousIndex<double, VDim> idxmov;
		itk::Point<double, VDim> pref, pmov;
		FixeImage->TransformContinuousIndexToPhysicalPoint(idx[j], pref);
		pmov = atran->TransformPoint(pref);
		MovingImage->TransformPhysicalPointToContinuousIndex(pmov, idxmov);
	}


	// Set the spacing, origin, direction of the output
	fltSample->UseReferenceImageOn();
	fltSample->SetReferenceImage(FixeImage);
	fltSample->Update();
	ImagePointer output = fltSample->GetOutput();
	return output;
}

ImagePointer ResliceImage(ImagePointer FixeImage, ImagePointer MovingImage, RotationAxis axis, double angle)
{
	ImagePointer output = ResliceImage(FixeImage, MovingImage, axis, angle, 0.0, 0.0, 0.0);
	return output;
}

ImagePointer ResliceImage(ImagePointer MovingImage, RotationAxis axis, double angle)
{
	ImagePointer output = ResliceImage(MovingImage, MovingImage, axis, angle, 0.0, 0.0, 0.0);
	return output;
}

ImagePointer ResliceImage(ImagePointer FixeImage, ImagePointer MovingImage, double t_x, double t_y, double t_z)
{
	ImagePointer output = ResliceImage(FixeImage, MovingImage, RotationAxis::XROTATION, 0.0, t_x, t_y, t_z);
	return output;
}

ImagePointer ResliceImage(ImagePointer MovingImage, double t_x, double t_y, double t_z)
{
	ImagePointer output = ResliceImage(MovingImage, MovingImage, RotationAxis::XROTATION, 0.0, t_x, t_y, t_z);
	return output;
}

ImagePointer ResliceImage(ImagePointer FixeImage, ImagePointer MovingImage, RotationAxis axis, double angle, double t_x, double t_y, double t_z)
{
	// Create an initial identity transform
	typedef itk::AffineTransform<double, VDim> TranType;
	typename TranType::Pointer atran = TranType::New();
	atran->SetIdentity();

	// Create the matrix
	MatrixType matrix;

	matrix.SetParameters(axis, angle, t_x, t_y, t_z);

	// get transform parameters from MatrixType
	TranType::ParametersType parameters(3 * 3 + 3);
	for (unsigned int i = 0; i < 3; i++) {
		for (unsigned int j = 0; j < 3; j++) {
			parameters[i * 3 + j] = matrix[i][j];
		}
	}
	for (unsigned int i = 0; i < 3; i++) {
		parameters[i + 3 * 3] = matrix[i][3];
	}
	// Put the values in the transform
	atran->SetParameters(parameters);

	// Get the moving image center in LPS coords
	itk::ContinuousIndex<double, 3> cimg_idx;
	itk::Point<double, 3> cimg_pt;
	for (unsigned int i = 0; i < 3; i++)
		cimg_idx[i] =
		FixeImage->GetLargestPossibleRegion().GetIndex()[i] +
		0.5 * FixeImage->GetLargestPossibleRegion().GetSize()[i];
	FixeImage->TransformContinuousIndexToPhysicalPoint(cimg_idx, cimg_pt);
	vnl_vector_ref<double> cimg = cimg_pt.GetVnlVector();

	TranType::ParametersType Fixeparameters(3);
	for (unsigned int i = 0; i < 3; i++) {
		Fixeparameters[i] = cimg[i];
	}

	atran->SetFixedParameters(Fixeparameters);

	// Build the resampling filter
	typedef itk::ResampleImageFilter<ImageType, ImageType> ResampleFilterType;
	typename ResampleFilterType::Pointer fltSample = ResampleFilterType::New();

	// Initialize the resampling filter with an identity transform
	fltSample->SetInput(MovingImage);
	fltSample->SetTransform(atran);

	// Set the unknown intensity to positive value
	fltSample->SetDefaultPixelValue(0);

	// Set the interpolator
	using FixedLinearInterpolatorType = itk::LinearInterpolateImageFunction<ImageType, double>;
	fltSample->SetInterpolator(FixedLinearInterpolatorType::New());


	// Calculate where the transform is taking things
	itk::ContinuousIndex<double, VDim> idx[3];

	idx->Fill(0.0);
	for (size_t i = 0; i < VDim; i++)
	{
		idx[0][i] = 0.0;
		idx[1][i] = FixeImage->GetBufferedRegion().GetSize(i);
		idx[2][i] = FixeImage->GetBufferedRegion().GetSize(i);
	}

	for (size_t j = 0; j < VDim; j++)
	{
		itk::ContinuousIndex<double, VDim> idxmov;
		itk::Point<double, VDim> pref, pmov;
		FixeImage->TransformContinuousIndexToPhysicalPoint(idx[j], pref);
		pmov = atran->TransformPoint(pref);
		MovingImage->TransformPhysicalPointToContinuousIndex(pmov, idxmov);
	}


	// Set the spacing, origin, direction of the output
	fltSample->UseReferenceImageOn();
	fltSample->SetReferenceImage(FixeImage);
	fltSample->Update();

	return fltSample->GetOutput();
}

ImagePointer ResliceImage(ImagePointer MovingImage, RotationAxis axis, double angle, double t_x, double t_y, double t_z)
{
	ImagePointer FixeImage = MovingImage;
	// Create an initial identity transform
	typedef itk::AffineTransform<double, VDim> TranType;
	typename TranType::Pointer atran = TranType::New();
	atran->SetIdentity();

	// Create the matrix
	MatrixType matrix;

	matrix.SetParameters(axis, angle, t_x, t_y, t_z);

	// get transform parameters from MatrixType
	TranType::ParametersType parameters(3 * 3 + 3);
	for (unsigned int i = 0; i < 3; i++) {
		for (unsigned int j = 0; j < 3; j++) {
			parameters[i * 3 + j] = matrix[i][j];
		}
	}
	for (unsigned int i = 0; i < 3; i++) {
		parameters[i + 3 * 3] = matrix[i][3];
	}
	// Put the values in the transform
	atran->SetParameters(parameters);

	// Get the moving image center in LPS coords
	itk::ContinuousIndex<double, 3> cimg_idx;
	itk::Point<double, 3> cimg_pt;
	for (unsigned int i = 0; i < 3; i++)
		cimg_idx[i] =
		FixeImage->GetLargestPossibleRegion().GetIndex()[i] +
		0.5 * FixeImage->GetLargestPossibleRegion().GetSize()[i];
	FixeImage->TransformContinuousIndexToPhysicalPoint(cimg_idx, cimg_pt);
	vnl_vector_ref<double> cimg = cimg_pt.GetVnlVector();

	TranType::ParametersType Fixeparameters(3);
	for (unsigned int i = 0; i < 3; i++) {
		Fixeparameters[i] = cimg[i];
	}

	atran->SetFixedParameters(Fixeparameters);

	// Build the resampling filter
	typedef itk::ResampleImageFilter<ImageType, ImageType> ResampleFilterType;
	typename ResampleFilterType::Pointer fltSample = ResampleFilterType::New();

	// Initialize the resampling filter with an identity transform
	fltSample->SetInput(MovingImage);
	fltSample->SetTransform(atran);

	// Set the unknown intensity to positive value
	fltSample->SetDefaultPixelValue(0);

	// Set the interpolator
	using FixedLinearInterpolatorType = itk::LinearInterpolateImageFunction<ImageType, double>;
	fltSample->SetInterpolator(FixedLinearInterpolatorType::New());


	// Calculate where the transform is taking things
	itk::ContinuousIndex<double, VDim> idx[3];

	idx->Fill(0.0);
	for (size_t i = 0; i < VDim; i++)
	{
		idx[0][i] = 0.0;
		idx[1][i] = FixeImage->GetBufferedRegion().GetSize(i);
		idx[2][i] = FixeImage->GetBufferedRegion().GetSize(i);
	}

	for (size_t j = 0; j < VDim; j++)
	{
		itk::ContinuousIndex<double, VDim> idxmov;
		itk::Point<double, VDim> pref, pmov;
		FixeImage->TransformContinuousIndexToPhysicalPoint(idx[j], pref);
		pmov = atran->TransformPoint(pref);
		MovingImage->TransformPhysicalPointToContinuousIndex(pmov, idxmov);
	}


	// Set the spacing, origin, direction of the output
	fltSample->UseReferenceImageOn();
	fltSample->SetReferenceImage(FixeImage);
	fltSample->Update();

	return fltSample->GetOutput();
}

itk::Vector<double, 3> MatchGravityCenters(ImagePointer iref, ImagePointer isrc)
{
	itk::Matrix<double, 3, 3> dir = iref->GetDirection();
	using ImageCalculatorType = itk::ImageMomentsCalculator<ImageType>;

	ImageCalculatorType::Pointer fixedCalculator = ImageCalculatorType::New();
	fixedCalculator->SetImage(iref);
	fixedCalculator->Compute();
	itk::Vector<double, 3> center_ref = fixedCalculator->GetCenterOfGravity();
	ImageCalculatorType::Pointer movingCalculator = ImageCalculatorType::New();
	movingCalculator->SetImage(isrc);
	movingCalculator->Compute();
	itk::Vector<double, 3> center_src = movingCalculator->GetCenterOfGravity();
	itk::Vector<double, 3> t;
	ImageType::PointType origin = iref->GetOrigin();

	t[0] = center_src[0] - center_ref[0];
	t[1] = center_src[1] - center_ref[1];
	t[2] = center_src[2] - center_ref[2];
	return t;
}