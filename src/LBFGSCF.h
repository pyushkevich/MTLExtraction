#ifndef _LBFGSCF_h
#define _LBFGSCF_h

#include "itkLBFGSOptimizer.h"
#include "itkSingleValuedCostFunction.h"

class CylinderLBFGSCF : public itk::SingleValuedCostFunction
{
public:
	ImagePointer m_hem = ReadImage("/Users/Jade/Desktop/Files_for_Automatisation/hemisphere_seg.nii.gz");
	ImagePointer m_cyl = ReadImage("/Users/Jade/Desktop/Files_for_Automatisation/cylinder.nii.gz");
	ImagePointer m_cyl_bar = Thresh(m_cyl, 1, vnl_huge_val(0), 0, 1);
	ImagePointer m_mtl = ReadImage("/Users/Jade/Desktop/Files_for_Automatisation/MTL_seg.nii.gz");

	using Self = CylinderLBFGSCF;
	using Superclass = itk::SingleValuedCostFunction;
	using Pointer = itk::SmartPointer<Self>;
	using ConstPointer = itk::SmartPointer<const Self>;
	itkNewMacro(Self);
	itkTypeMacro(CylinderLBFGSCF, SingleValuedCostFunction);

	enum { SpaceDimension = 4 };

	using ParametersType = Superclass::ParametersType;
	using DerivativeType = Superclass::DerivativeType;

	using VectorType = vnl_vector<double>;
	using MatrixType = vnl_matrix<double>;

	using MeasureType = double;

	CylinderLBFGSCF() = default;

	MeasureType GetValue(const ParametersType& parameters) const override
	{

		std::string path = "/Users/Jade/Desktop/MTLExtraction-Mac/mesure";
		std::string nb;
		std::string ext = ".nii.gz";
		std::string file;

		double angle_value = parameters[0];
		double tx = parameters[1];
		double ty = parameters[2];
		double tz = parameters[3];
		double pen, opt;
		ImagePointer new_cyl = ResliceImage(m_hem, m_cyl, RotationAxis::YROTATION, angle_value, tx, ty, tz);
		ImagePointer new_cyl_bar = Thresh(new_cyl, 1, vnl_huge_val(0), 0, 1);

		std::vector<ImagePointer> opt_table;
		opt_table.push_back(new_cyl);
		opt_table.push_back(m_hem);
		ImagePointer img_opt = IntersectionImages(opt_table);

		std::vector<ImagePointer> penalty_table;
		penalty_table.push_back(new_cyl_bar);
		penalty_table.push_back(m_mtl);
		ImagePointer img_pen = IntersectionImages(penalty_table);

		opt = GetVolume(img_opt) * pow(10, -3);
		pen = GetVolume(img_pen) * pow(10, -3);

		MeasureType measure = 0.1 * opt - 30 * pen;
		std::cout << "\nParameters = " << parameters << "\nVopt = " << opt << "\nVpen = " << pen << std::endl;

		if (pen < 0.6) {
			nb = std::to_string(int(opt * 100));
			
			file = path + nb + ext;
			Write(file, new_cyl);
			std::cout << "Written!" << std::endl;
		}

		return measure;
	}

	void GetDerivative(const ParametersType& parameters,
		DerivativeType& derivative) const override
	{
		double rightPart[4];
		for (unsigned int i = 0; i < 4; i++)
		{
			rightPart[i] = parameters[i];
		}

		const MeasureType baseValue = this->GetValue(parameters);

		constexpr double deltaAngle = 0.00175; // in radians = about 0.1 degree
		const double deltaTranslation = deltaAngle; // just to keep the scaling

		ParametersType parametersPlustDeltaAngle; parametersPlustDeltaAngle.Fill(0.0);
		parametersPlustDeltaAngle[0] = parameters[0] + deltaAngle;
		const MeasureType turnAngleValue = this->GetValue(parametersPlustDeltaAngle);

		ParametersType parametersPlustDeltaX; parametersPlustDeltaX.Fill(0.0);
		parametersPlustDeltaX[1] = parameters[1] + deltaTranslation;
		const MeasureType turnXValue = this->GetValue(parametersPlustDeltaX);

		ParametersType parametersPlustDeltaY; parametersPlustDeltaY.Fill(0.0);
		parametersPlustDeltaY[2] = parameters[2] + deltaTranslation;
		const MeasureType turnYValue = this->GetValue(parametersPlustDeltaY);

		ParametersType parametersPlustDeltaZ; parametersPlustDeltaZ.Fill(0.0);
		parametersPlustDeltaZ[3] = parameters[3] + deltaTranslation;
		const MeasureType turnZValue = this->GetValue(parametersPlustDeltaZ);

		derivative = DerivativeType(SpaceDimension);

		derivative[0] = (turnAngleValue - baseValue) / deltaAngle;
		derivative[1] = (turnXValue - baseValue) / deltaTranslation;
		derivative[2] = (turnYValue - baseValue) / deltaTranslation;
		derivative[3] = (turnZValue - baseValue) / deltaTranslation;

	}

	unsigned int GetNumberOfParameters() const override
	{
		return SpaceDimension;
	}
};

int OrientationCylinderLBFGS();


#endif