#include "PowellCF.h"

#include "WriteImage.h"

// Orient the cylinder until the MTL fits the best into it. This position will deterinate the volume of tissue needed to be cut.

int OrientationCylinderPowell() {

	using OptimizerType = itk::PowellOptimizer;

	// Declaration of a itkOptimizer
	OptimizerType::Pointer  itkOptimizer = OptimizerType::New();


	// Declaration of the CostFunction
	CylinderPowellCF::Pointer costFunction = CylinderPowellCF::New();


	itkOptimizer->SetCostFunction(costFunction);


	using ParametersType = CylinderPowellCF::ParametersType;

	const unsigned int spaceDimension =
		costFunction->GetNumberOfParameters();

	// Initially, the center of the cylinder is superposed to the gravity center of the MTL
	ParametersType  initialPosition(spaceDimension);
	itk::Vector<double, 3> t = MatchGravityCenters(costFunction->m_mtl, costFunction->m_cyl);
	initialPosition[0] = 0.0;
	initialPosition[1] = t[0];
	initialPosition[2] = -1 * t[1];
	initialPosition[3] = t[2];
	
	itkOptimizer->SetMaximize(true);
	itkOptimizer->SetStepLength(10);
	itkOptimizer->SetStepTolerance(1);
	itkOptimizer->SetValueTolerance(0.1);
	itkOptimizer->SetMaximumIteration(1);

	itkOptimizer->SetInitialPosition(initialPosition);

	try
	{
		itkOptimizer->StartOptimization();
	}
	catch (itk::ExceptionObject & e)
	{
		std::cout << "Exception thrown ! " << std::endl;
		std::cout << "An error occurred during Optimization" << std::endl;
		std::cout << "Location    = " << e.GetLocation() << std::endl;
		std::cout << "Description = " << e.GetDescription() << std::endl;
		return EXIT_FAILURE;
	}

	ParametersType finalPosition = itkOptimizer->GetCurrentPosition();
	const double   finalAngle = finalPosition[0];
	const double   finalTranslationX = finalPosition[1];
	const double   finalTranslationY = finalPosition[2];
	const double   finalTranslationZ = finalPosition[3];

	const unsigned int numberOfIterations = itkOptimizer->GetCurrentIteration();
	const double       bestValue = itkOptimizer->GetValue();

	// Print out results
	const double finalAngleInRadians = finalAngle * itk::Math::pi_over_180;
	//std::cout << " Result = " << std::endl;
	//std::cout << " Angle (radians) " << finalAngleInRadians << std::endl;
	//std::cout << " Angle (degrees) " << finalAngle << std::endl;
	//std::cout << " Translation X  = " << finalTranslationX << std::endl;
	//std::cout << " Translation Y  = " << finalTranslationY << std::endl;
	//std::cout << " Translation Z  = " << finalTranslationZ << std::endl;
	//std::cout << " Iterations     = " << numberOfIterations << std::endl;
	//std::cout << " Metric value   = " << bestValue << std::endl;

	//// Exercise various member functions.
	//std::cout << "Maximize: " << itkOptimizer->GetMaximize() << std::endl;
	//std::cout << "StepLength: " << itkOptimizer->GetStepLength();
	//std::cout << std::endl;
	//std::cout << "CurrentIteration: " << itkOptimizer->GetCurrentIteration();
	//std::cout << std::endl << std::endl;


	ImagePointer cyl_sol = ResliceImage(costFunction->m_hem, costFunction->m_cyl, finalTranslationX, finalTranslationY, finalTranslationZ);
	ImagePointer hem_solSeg = ResliceImage(costFunction->m_hem, costFunction->m_hem, RotationAxis::YROTATION, finalAngleInRadians);
	ImagePointer hem = ReadImage("hemisphere.nii.gz");
	ImagePointer hem_sol = ResliceImage(costFunction->m_hem, hem, RotationAxis::YROTATION, finalAngleInRadians);
	ImagePointer mtl_sol = ResliceImage(costFunction->m_hem, costFunction->m_mtl, RotationAxis::YROTATION, finalAngleInRadians);


	Write("OrientedCylinder.nii.gz", cyl_sol);
	Write("OrientedHemisphere.nii.gz", hem_sol);
	Write("OrientedHemisphereSeg.nii.gz", hem_solSeg);
	Write("OrientedMTL.nii.gz", mtl_sol);

	ImagePointer img_roi = ReadImage("roi_seg.nii.gz");
	ImagePointer roi_sol = ResliceImage(costFunction->m_hem, img_roi, RotationAxis::YROTATION, finalAngleInRadians);
	Write("OrientedROI.nii.gz", roi_sol);

	return EXIT_SUCCESS;
}

