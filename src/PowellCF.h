#ifndef _PowellCF_h_
#define _PowellCF_h_

#include "itkPowellOptimizer.h"
#include "ReadImage.h"
#include "EnergyFunctions.h"
#include "ReferencePlan.h"
#include "CreateCylinder.h"


class CylinderPowellCF : public itk::SingleValuedCostFunction
{
public:
	ImagePointer m_hem     = ReadImage("hemisphere_seg.nii.gz");
	ImagePointer m_cyl     = CreateSmallCylinder(m_hem);
	ImagePointer m_cyl_bar = Thresh(m_cyl, 1, vnl_huge_val(0), 0, 1);
	ImagePointer m_mtl     = ReadImage("mtl_seg.nii.gz");

	using Self = CylinderPowellCF;
	using Superclass = itk::SingleValuedCostFunction;
	using Pointer = itk::SmartPointer<Self>;
	using ConstPointer = itk::SmartPointer<const Self>;
	itkNewMacro(Self);
	itkTypeMacro(CylinderPowellCF, SingleValuedCostFunction);

	enum { SpaceDimension = 4 };

	using ParametersType = Superclass::ParametersType;
	using DerivativeType = Superclass::DerivativeType;
	using MeasureType = Superclass::MeasureType;

	CylinderPowellCF() = default;


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


	MeasureType GetValue(const ParametersType& parameters) const override
	{		
		double angle_value = parameters[0] * itk::Math::pi_over_180;
		double tx = parameters[1];
		double ty = parameters[2];
		double tz = parameters[3];
		double pen, opt;
		// The cylinder is translated while the hemisphere is rotated, so the cuts are a single axis plan
		ImagePointer new_cyl = ResliceImage(m_hem, m_cyl, tx, ty, tz);
		ImagePointer new_hem = ResliceImage(m_hem, m_hem, RotationAxis::YROTATION, angle_value);
		ImagePointer new_mtl = ResliceImage(m_hem, m_mtl, RotationAxis::YROTATION, angle_value);
		ImagePointer new_cyl_bar = Thresh(new_cyl, 1, vnl_huge_val(0), 0, 1);

		std::vector<ImagePointer> opt_table;
		opt_table.push_back(new_cyl);
		opt_table.push_back(new_hem);
		ImagePointer img_opt = IntersectionImages(opt_table);

		std::vector<ImagePointer> penalty_table;
		penalty_table.push_back(new_cyl_bar);
		penalty_table.push_back(new_mtl);
		ImagePointer img_pen = IntersectionImages(penalty_table);

		opt = GetVolume(img_opt) * pow(10, -4);
		pen = GetVolume(img_pen);

		MeasureType measure = opt - pen;
		return measure;
	}

	unsigned int GetNumberOfParameters() const override
	{
		return SpaceDimension;
	}
};

class FirstCutPowellCF : public itk::SingleValuedCostFunction
{
public:
	ImagePointer m_hem   = ReadImage("OrientedHemisphere.nii.gz");
	ImagePointer m_mtl   = ReadImage("OrientedMTL.nii.gz");
	ImagePointer m_fl    = ReadImage("OrientedROI.nii.gz");
	ImagePointer m_lineN = ReadImage("planN.nii.gz");
	ImagePointer m_lineP = ReadImage("planP.nii.gz");

	using Self = FirstCutPowellCF;
	using Superclass = itk::SingleValuedCostFunction;
	using Pointer = itk::SmartPointer<Self>;
	using ConstPointer = itk::SmartPointer<const Self>;
	itkNewMacro(Self);
	itkTypeMacro(FirstCutPowellCF, SingleValuedCostFunction);

	enum { SpaceDimension = 2 };

	using ParametersType = Superclass::ParametersType;
	using DerivativeType = Superclass::DerivativeType;
	using MeasureType = Superclass::MeasureType;

	FirstCutPowellCF() = default;

	MeasureType GetValue(const ParametersType& parameters) const override
	{
		double tx = parameters[0];
		double angle_value = parameters[1] * itk::Math::pi_over_180;
		double opt, penFL, penMTL;
		ImagePointer cutN = ResliceImage(m_hem, m_lineN, RotationAxis::YROTATION, angle_value, tx, 0.0, 0.0);
		ImagePointer cutP = ResliceImage(m_hem, m_lineP, RotationAxis::YROTATION, angle_value, tx, 0.0, 0.0);


		std::vector<ImagePointer> opt_table;
		opt_table.push_back(cutN);
		opt_table.push_back(m_hem);
		ImagePointer img_opt = IntersectionImages(opt_table);

		std::vector<ImagePointer> pen_tableFL;
		pen_tableFL.push_back(m_fl);
		pen_tableFL.push_back(cutP);
		ImagePointer img_penFL = IntersectionImages(pen_tableFL);

		std::vector<ImagePointer> pen_tableMTL;
		pen_tableMTL.push_back(m_mtl);
		pen_tableMTL.push_back(cutN);
		ImagePointer img_penMTL = IntersectionImages(pen_tableMTL);

		opt = GetVolume(img_opt) * pow(10, -6);
		penFL = 0.1 * GetVolume(img_penFL);
		penMTL = 10 * GetVolume(img_penMTL);

		MeasureType measure = opt - penFL - penMTL;
		// std::cout << "\nParameters = " << parameters << "\nVopt = " << opt << "\nVpenFL = " << penFL << "\nVpenMTL = " << penMTL << "\nMeasure = " << measure << std::endl;
		return measure;
	}

	void GetDerivative(const ParametersType& parameters,
		DerivativeType& derivative) const override
	{
		double rightPart[SpaceDimension];
		for (unsigned int i = 0; i < SpaceDimension; i++)
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

		derivative = DerivativeType(SpaceDimension);

		derivative[0] = (turnAngleValue - baseValue) / deltaAngle;
		derivative[1] = (turnXValue - baseValue) / deltaTranslation;
	}

	unsigned int GetNumberOfParameters() const override
	{
		return SpaceDimension;
	}
};

class CutsPowellCF : public itk::SingleValuedCostFunction
{
public:
	ImagePointer m_hem = ReadImage("OrientedHemisphereSeg.nii.gz");
	ImagePointer m_cyl = ReadImage("OrientedCylinder.nii.gz");
	ImagePointer m_cyl_bar = Thresh(m_cyl, 1, vnl_huge_val(0), 0, 1);
	ImagePointer m_mtl = ReadImage("OrientedMTL.nii.gz");
	ImagePointer m_lineN = ReadImage("planN.nii.gz");
	ImagePointer m_lineP = ReadImage("planP.nii.gz");
	ImagePointer m_plan0 = ReadImage("plan0.nii.gz");

	using Self = CutsPowellCF;
	using Superclass = itk::SingleValuedCostFunction;
	using Pointer = itk::SmartPointer<Self>;
	using ConstPointer = itk::SmartPointer<const Self>;
	itkNewMacro(Self);
	itkTypeMacro(CutsPowellCF, SingleValuedCostFunction);

	enum { SpaceDimension = 12 };

	using ParametersType = Superclass::ParametersType;
	using DerivativeType = Superclass::DerivativeType;
	using MeasureType = Superclass::MeasureType;

	CutsPowellCF() = default;


	MeasureType GetValue(const ParametersType& parameters) const override
	{
		ImagePointer planP;
		double opt, penMTL, penCyl;
		std::vector<ImagePointer> planP_table;
		std::vector<ImagePointer> opt_table;
		std::vector<ImagePointer> pen_tableMTL;
		std::vector<ImagePointer> pen_tableCyl;
		float angle, tx;
		
		planP_table.push_back(m_plan0);
		std::string file_name, file_nb;

		for (int i = 0;i < 3;i++) {
			angle = parameters[2 * i + 1] * itk::Math::pi_over_180;
			tx = parameters[2 * i];
			planP = ResliceImage(m_hem, m_lineP, RotationAxis::YROTATION, angle, tx, 0.0, 0.0);
			planP_table.push_back(planP);
		}
		for (int i = 3;i < 6;i++) {
			angle = parameters[2 * i + 1] * itk::Math::pi_over_180;
			tx = parameters[2 * i];
			planP = ResliceImage(m_hem, m_lineP, RotationAxis::ZROTATION, angle, tx, 0.0, 0.0);
			planP_table.push_back(planP);
		}
		
		ImagePointer img_planP = IntersectionImages(planP_table);
		ImagePointer img_planN = Thresh(img_planP, -1, 0, 1, 0);

		opt_table.push_back(img_planP);
		opt_table.push_back(m_hem);
		ImagePointer img_opt = IntersectionImages(opt_table);

		pen_tableMTL.push_back(img_planN);
		pen_tableMTL.push_back(m_mtl);
		ImagePointer img_penMTL = IntersectionImages(pen_tableMTL);

		pen_tableCyl.push_back(img_planP);
		pen_tableCyl.push_back(m_hem);
		pen_tableCyl.push_back(m_cyl_bar);
		ImagePointer img_penCyl = IntersectionImages(pen_tableCyl);

		opt = GetVolume(img_opt) * pow(10, -3);
		penMTL = 50 * GetVolume(img_penMTL);
		penCyl = 0.1 * GetVolume(img_penCyl);

		MeasureType measure = opt + penMTL + penCyl;
		//std::cout << "\nParameters = " << parameters << "\nVopt = " << opt << "\nVpenMTL = " << GetVolume(img_penMTL)
		//<< "\nVpenCyl = " << GetVolume(img_penCyl) << "\nMeasure = " << measure << std::endl;

		return measure;
	}

	void GetDerivative(const ParametersType& parameters,
		DerivativeType& derivative) const override
		{
		std::vector<double> rightPart;
		for (unsigned int i = 0; i < SpaceDimension; i++)
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

		derivative = DerivativeType(SpaceDimension);

		derivative[0] = (turnAngleValue - baseValue) / deltaAngle;
		derivative[1] = (turnXValue - baseValue) / deltaTranslation;
	}

	unsigned int GetNumberOfParameters() const override
	{
		return SpaceDimension;
	}
};

int OrientationCylinderPowell();
int OrientationCutsPowell();
int OrientationFirstCutPowell();

#endif