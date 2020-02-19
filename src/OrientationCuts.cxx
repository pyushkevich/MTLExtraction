#include "PowellCF.h"

#include "CreateMolds.h"


// Use the Powell Optimizer ot find a cut sparing the two ROIs (MTL is considered more important)
int OrientationFirstCutPowell() {
	using OptimizerType = itk::PowellOptimizer;

	// Declaration of a itkOptimizer
	OptimizerType::Pointer  itkOptimizer = OptimizerType::New();

	// Declaration of the CostFunction
	FirstCutPowellCF::Pointer costFunction = FirstCutPowellCF::New();

	itkOptimizer->SetCostFunction(costFunction);


	using ParametersType = FirstCutPowellCF::ParametersType;

	const unsigned int spaceDimension =
		costFunction->GetNumberOfParameters();

	ParametersType  initialPosition(spaceDimension);
	initialPosition.Fill(0.0);

	itkOptimizer->SetMaximize(true);
	itkOptimizer->SetMaximumIteration(2);
	itkOptimizer->SetStepLength(5);
	itkOptimizer->SetStepTolerance(0.1);
	itkOptimizer->SetValueTolerance(0.1);

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
	const double   finalAngleInDegrees = finalPosition[0];
	const double   finalTranslationX = finalPosition[1];
	

	const unsigned int numberOfIterations = itkOptimizer->GetCurrentIteration();
	const double       bestValue = itkOptimizer->GetValue();

	// Print out results
	const double finalAngle = finalAngleInDegrees * itk::Math::pi_over_180;
	// std::cout << "\n\n Result = " << std::endl;
	// std::cout << " Angle (radians) " << finalAngle << std::endl;
	// std::cout << " Angle (degrees) " << finalAngleInDegrees << std::endl;
	// std::cout << " Translation X  = " << finalTranslationX << std::endl;
	// std::cout << " Iterations     = " << numberOfIterations << std::endl;
	// std::cout << " Metric value   = " << bestValue << std::endl;

	ImagePointer lineN_sol = ResliceImage(costFunction->m_lineN, RotationAxis::YROTATION, finalAngle, finalTranslationX, 0.0, 0.0);
	ImagePointer cut = CreateRealCut(lineN_sol);
	Write("cut0.nii.gz", cut);
	ImagePointer line_sol = ResliceImage(costFunction->m_lineP, RotationAxis::YROTATION, finalAngle, finalTranslationX, 0.0, 0.0);
	Write("plan0.nii.gz", line_sol);

	return EXIT_SUCCESS;
}

int OrientationCutsPowell() {
	using OptimizerType = itk::PowellOptimizer;
	using ParametersType = CutsPowellCF::ParametersType;

	// Declaration of a itkOptimizer
	OptimizerType::Pointer  itkOptimizer = OptimizerType::New();

	// Declaration of the CostFunction
	CutsPowellCF::Pointer costFunction = CutsPowellCF::New();
	
	std::string pathW = "cut";
	std::string nb;
	std::string extensionW = ".nii.gz";
	std::string nameW;
	MatrixType mat;
	
	//First parameter is the x translation, second is the angle of rotation 
	ParametersType  initialPosition(12);
	//Vertical cuts
	initialPosition[0] = -30;
	initialPosition[1] = 90;
	initialPosition[2] = -30;
	initialPosition[3] = -90;
	initialPosition[4] = 0;
	initialPosition[5] = 0;
	//Horizontal cuts
	initialPosition[6] = -20;
	initialPosition[7] = -135;
	initialPosition[8] = 10;
	initialPosition[9] = 45;
	initialPosition[10] = 0;
	initialPosition[11] = -45;


	itkOptimizer->SetCostFunction(costFunction);
	itkOptimizer->SetMaximumIteration(2);
	itkOptimizer->SetMaximumLineIteration(5);
	itkOptimizer->SetMaximize(false);
	itkOptimizer->SetStepLength(5);
	itkOptimizer->SetStepTolerance(1);
	itkOptimizer->SetValueTolerance(1);
	

	itkOptimizer->SetInitialPosition(initialPosition);

	try
	{
		itkOptimizer->StartOptimization();
	}
	catch (itk::ExceptionObject & e)
	{
		std::cout << "An error occurred during Optimization" << std::endl;
		std::cout << "Location    = " << e.GetLocation() << std::endl;
		std::cout << "Description = " << e.GetDescription() << std::endl;
		return EXIT_FAILURE;
	}

	ParametersType finalPosition = itkOptimizer->GetCurrentPosition();
	float   finalAngle, finalTranslationX;
	const unsigned int numberOfIterations = itkOptimizer->GetCurrentIteration();
	const float       bestValue = itkOptimizer->GetValue();

	//// Print out results
	//std::cout << "\nResult = \n" << finalPosition << std::endl;
	//std::cout << " Metric value   = " << bestValue << std::endl;
	//std::cout << "Maximize: " << itkOptimizer->GetMaximize() << std::endl;
	//std::cout << "StepLength: " << itkOptimizer->GetStepLength() << std::endl;
	//std::cout << "CurrentIteration: " << itkOptimizer->GetCurrentIteration();
	//std::cout << std::endl << std::endl;


	std::chrono::duration<double> t_molds;
	std::chrono::time_point<std::chrono::system_clock> start_molds, end_molds;
	start_molds = std::chrono::system_clock::now();

	// table countains all the plans covering the MTL
	// new_table_cuts countains the useful cuts
	// new_table_plans countains the useful plans covering the MTL
	// cuts not cutting enough tissue to be considered useful are not kept

	std::vector<ImagePointer> table, new_table_cuts, new_table_plans;
	table.push_back(costFunction->m_plan0);
	new_table_plans.push_back(costFunction->m_plan0);
	new_table_plans.push_back(costFunction->m_hem);
	new_table_cuts.push_back(ReadImage("cut0.nii.gz"));

	ImagePointer inter_plans = IntersectionImages(new_table_plans);
	float V_before = GetVolume(inter_plans);
	float V_new = 0.0;


	ImagePointer imgP, imgN, realCut;
	int ind_cut = 0;
	// First three cuts around Y axis
	for (int i = 0; i < 3;i++) {
		++ind_cut;
		// Reslicing plans with  resulting parameters
		finalAngle = finalPosition[2 * i + 1] * itk::Math::pi_over_180;;
		finalTranslationX = finalPosition[2 * i];
		imgN = ResliceImage(costFunction->m_lineN, RotationAxis::YROTATION, finalAngle, finalTranslationX, 0.0, 0.0);
		imgP = ResliceImage(costFunction->m_lineP, RotationAxis::YROTATION, finalAngle, finalTranslationX, 0.0, 0.0);
		Write("plan" + std::to_string(ind_cut) + ".nii.gz", imgP);
		table.push_back(imgP);
		realCut = CreateRealCut(imgN);
		Write("cut" + std::to_string(ind_cut) + ".nii.gz", realCut);

		// Compare volume of the resulting piece of tissue before and after adding a cutting plan. 
		// If the plan doesn't cut enough tissue, it won't be added to the molds.
		new_table_plans.push_back(table[ind_cut]);
		inter_plans = IntersectionImages(new_table_plans);
		V_new = GetVolume(inter_plans);
		// std::cout << "V_before = " << V_before << "\nV_new = " << V_new << "\nDiff = " << (V_before - V_new) << std::endl;

		//Cut useful added to table countaining the cuts
		if ((V_before - V_new) > 1000){
			std::cout << "Cut " << ind_cut << " useful." << std::endl;
			V_before = V_new;
			new_table_cuts.push_back(realCut);
		}
		else{
			std::cout << "Cut " << ind_cut << " not useful." << std::endl;
			new_table_plans.pop_back();
		}
	}
	
	// Subsequent three cuts around Z axis
	int nb_cuts_mold2 = 0;
	for (int i = 3; i < 6;i++) {
		++ind_cut;
		finalAngle = finalPosition[2 * i + 1] * itk::Math::pi_over_180;;
		finalTranslationX = finalPosition[2 * i];
		imgN = ResliceImage(costFunction->m_lineN, RotationAxis::ZROTATION, finalAngle, finalTranslationX, 0.0, 0.0);
		imgP = ResliceImage(costFunction->m_lineP, RotationAxis::ZROTATION, finalAngle, finalTranslationX, 0.0, 0.0);
		Write("plan" + std::to_string(ind_cut) + ".nii.gz", imgP);
		table.push_back(imgP);
		realCut = CreateRealCut(imgN);
		Write("cut" + std::to_string(ind_cut) + ".nii.gz", realCut);

		new_table_plans.push_back(table[ind_cut]);
		inter_plans = IntersectionImages(new_table_plans);
		V_new = GetVolume(inter_plans);
		// std::cout << "V_before = " << V_before << "\nV_new = " << V_new << "\nDiff = " << (V_before - V_new) << std::endl;
		// Write("inter" + std::to_string(ind_cut) + ".nii.gz", inter_plans);
		
		
		if ((V_before - V_new) > 1000){
			// std::cout << "Cut " << ind_cut << " useful." << std::endl;
			V_before = V_new;
			new_table_cuts.push_back(realCut);
			++nb_cuts_mold2;
		}
		//Cut not useful removed
		else{
			// std::cout << "Cut " << ind_cut << " not useful." << std::endl;
			new_table_plans.pop_back();
		}
	}

	ImagePointer interCuts = IntersectionImages(new_table_plans);
	// Write("inter_cuts.nii.gz", interCuts);
	ImagePointer hem = ReadImage("OrientedHemisphere.nii.gz");
	ImagePointer hemSeg = ReadImage("OrientedHemisphereSeg.nii.gz");
	// Piece supposed to result after using the two molds
	ImagePointer finalMTL = BinaryMathOperation(MULTIPLY, hem, interCuts);
	Write("finalMTL.nii.gz", finalMTL);

	// Remove 2nd mold cuts to obtain the image of the piece of tissue resulting after using the 1st mold
	for (int i = 0; i < nb_cuts_mold2; i++) {
		new_table_plans.pop_back();
	}
	
	ImagePointer interCutsMold1 = IntersectionImages(new_table_plans);
	ImagePointer MTL = BinaryMathOperation(MULTIPLY, hem, interCutsMold1);
	ImagePointer MTLSeg = BinaryMathOperation(MULTIPLY, hemSeg, interCutsMold1);
	// Piece of tissue supposed to result after using the 1st mold
	Write("MTL.nii.gz", MTL);
	Write("MTLSeg.nii.gz", MTLSeg);

	int nb_cuts_1 = new_table_cuts.size() - nb_cuts_mold2;

	CreateMolds(new_table_cuts, nb_cuts_1);
	end_molds = std::chrono::system_clock::now();

	t_molds = (end_molds - start_molds) / 60; //in minutes

	std::cout << "\n		Creating the molds took " << t_molds.count() << "min." << std::endl;

	return EXIT_SUCCESS;
}