#include "ReadMatrix.h"

#include <iostream>
#include <fstream>
#include "itkMatrixOffsetTransformBase.h"
#include "itkTransformFactory.h"
#include "itkTransformFileReader.h"
#include "itkAffineTransform.h"
#include "itkResampleImageFilter.h"

typedef vnl_matrix_fixed<double, 3, 3> Mat3;
typedef vnl_matrix_fixed<double, 4, 4> Mat4;
typedef vnl_vector_fixed<double, 3> Vec3;


MatrixType ReadMatrix(const std::string fname)
{
	MatrixType mat;

	std::string name = fname;
	std::size_t matFile = name.find(".mat");
	std::size_t txtFile = name.find(".txt");

	if (matFile != std::string::npos){
		std::ifstream fin(fname);
		for (size_t i = 0; i < 4; i++)
			for (size_t j = 0; j < 4; j++)
				if (fin.good())
				{
					fin >> mat[i][j];
				}
				else
				{
					std::cerr << "Unable to read matrix" << std::endl;
					exit(0);
				}
		fin.close();

		// Get the transform matrix and the offset vector
		vnl_matrix<double> A_ras = mat.GetVnlMatrix().extract(VDim, VDim);
		vnl_vector<double> b_ras = mat.GetVnlMatrix().extract(VDim, 1, 0, VDim).get_column(0);

		// Extrernal matrices are assumed to be RAS to RAS, so we must convert to LPS to LPS
		vnl_vector<double> v_lps_to_ras(VDim, 1.0);
		v_lps_to_ras[0] = v_lps_to_ras[1] = -1.0;
		vnl_diag_matrix<double> m_lps_to_ras(v_lps_to_ras);
		vnl_matrix<double> A_lps = m_lps_to_ras * A_ras * m_lps_to_ras;
		vnl_vector<double> b_lps = m_lps_to_ras * b_ras;

		// Stick these into the itk matrix/vector
		itk::Matrix<double, VDim, VDim> amat(A_lps);
		itk::Vector<double, VDim> aoff;
		aoff.SetVnlVector(b_lps);

		for (unsigned int i = 0; i < 3; i++) {
			for (unsigned int j = 0; j < 3; j++) {
				mat[i][j] = amat[i][j];
			}
			mat[i][3] = aoff[i];
		}
	}

	else  if (txtFile != std::string::npos)
	{
		typedef itk::MatrixOffsetTransformBase<double, VDim, VDim> MOTBType;
		itk::TransformFactory<MOTBType>::RegisterTransform();
		itk::TransformFileReader::Pointer fltReader = itk::TransformFileReader::New();
		fltReader->SetFileName(fname);
		fltReader->Update();

		itk::TransformBase *base = fltReader->GetTransformList()->front();
		MOTBType *motb = dynamic_cast<MOTBType *>(base);

		if(motb)
		{
			for (size_t i = 0; i < VDim; i++){
				for (size_t j = 0; j < VDim; j++){
					mat[i][j] = motb->GetMatrix()[i][j];
				}
				mat[i][3] = motb->GetOffset()[i];
			}
		}
	}

	else
	{
		std::cerr << "Wrong format." << std::endl;
	}

	mat.SetRotationAxis();
	mat.angle = mat.GetAngle();
	return mat;
}
