#include <iostream>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#define WIN
	#include <direct.h>
	#include <filesystem>
#elif defined(__APPLE__) || defined(__linux__) || defined(__unix__)
#define UNIX
	#include <sys/stat.h>
	#include <dirent.h>
#else
	#error "Unknown compiler"
#endif

#include "UsingNames.h"
#include "PowellCF.h"
#include "PadImage.h"
#include "WriteImage.h"
#include "CreateMolds.h"


int OptimizationMethod(std::string folder){

	ImagePointer mtlSeg = ReadImage(folder + "/mtl_seg.nii.gz");
	ImagePointer roiSeg; 
	bool add_roi = false;
	#if defined(WIN)
	// Read the segmentation of another ROI to preserve, if it exists
	for (const auto& entry : std::filesystem::directory_iterator(folder)) {
		std::size_t find_roi = entry.path().string().find("roi_seg");
		if (find_roi != std::string::npos) {
	 		roiSeg = ReadImage(folder + "/roi_seg.nii.gz");
	 		add_roi = true;
	 		break;
	 	}
	}
	#elif defined(UNIX)
	DIR* dir;
	struct dirent* ent;
	if ((dir = opendir(folder.c_str())) != NULL) {
		while ((ent = readdir(dir)) != NULL) {
	 		std::size_t find_roi = std::string(ent->d_name).find("roi_seg");
	 		if (find_roi != std::string::npos) {
	 			roiSeg = ReadImage(folder + "/roi_seg.nii.gz");
	 			add_roi = true;
	 			break;
	 		}
	 	}
	 	closedir(dir);
	}
	#endif
	
	
		
	//___________________________________________________________________________________________
	//________________________________________Treatments_________________________________________
	//___________________________________________________________________________________________

	ImageType::SizeType padExtent; padExtent.Fill(50);

	ImagePointer m_hem = ReadImage("hemisphere.nii.gz");
	ImagePointer m_mtl = PadImage(mtlSeg, padExtent, padExtent, 0);
	Write("mtl_seg.nii.gz", m_mtl);

	if (add_roi) {
	 	ImagePointer m_roi = PadImage(roiSeg, padExtent, padExtent, 0);
	 	Write("roi_seg.nii.gz", m_roi);
	}
	// If there is no other ROI to preserve, we create neutral images so it won't impact the method to find other cuts. 
	// plan0 is a 3D image with the single value 1
	// cut0 is a 3D image with the single value 0
	 
	else{
		ImagePointer plan0 = m_hem;
		plan0->FillBuffer(1);
		Write("plan0.nii.gz", plan0);
		ImagePointer cut0 = m_hem;
		cut0->FillBuffer(0);
		Write("cut0.nii.gz", cut0);
	}
	

	//___________________________________________________________________________________________
	//_______________________________________Optimisation________________________________________
	//___________________________________________________________________________________________

	std::chrono::duration<double> duration1, duration2, duration3;
	std::chrono::time_point<std::chrono::system_clock> start, end;

  
    start = std::chrono::system_clock::now();
	std::cout << "Orienting cylinder..." << std::endl;
	OrientationCylinderPowell(add_roi);
    end = std::chrono::system_clock::now(); 
  
    duration1 = (end - start) / 60; //in minutes
	std::cout << "\n		Orienting cylinder took " << duration1.count() << "min." << std::endl;

	if (add_roi == true) {
	 	start = std::chrono::system_clock::now();
	 	std::cout << "Orienting first cut..." << std::endl;
	 	OrientationFirstCutPowell();
	 	end = std::chrono::system_clock::now(); 
  
	 	duration2 = (end - start) / 60; //in minutes
	 	std::cout << "\n		Finding the first cut took " << duration2.count() << "min." << std::endl;
	}
	
    start = std::chrono::system_clock::now();
	std::cout << "Orienting cuts..." << std::endl;
	OrientationCutsPowell();
    end = std::chrono::system_clock::now(); 
  
    duration3 = (end - start) / 60; //in minutes

	std::cout << "\n		Finding other cuts took " << duration3.count() << "min." << std::endl;

	//___________________________________________________________________________________________
	//________________________________________Cleaning___________________________________________
	//___________________________________________________________________________________________

	std::string file;
	std::size_t find_hem, find_finalmtl, find_mtl, find_roi, find_molds, find_cuts, find_oriented;

	folder += "/Automatic";
	#if defined(WIN)
	for (const auto& entry : std::filesystem::directory_iterator(folder)) {
		file = entry.path().string();
		for (auto& c : file) { c = tolower(c); }

		find_finalmtl = file.find("finalmtl");
		find_mtl = file.find("\\mtl.");
		find_roi = file.find("roi");
		find_molds = file.find("slitmold");
		find_cuts = file.find("\\cut");
		find_oriented = file.find("oriented");

		//Remove unuseful files
		if ((find_finalmtl & find_mtl & find_roi & find_molds & find_cuts & find_oriented) == std::string::npos) {
			try {
				remove(entry.path());
			}
			catch (std::filesystem::filesystem_error & error) {
				std::cerr << "Error deleting " << file << std::endl;
			}
		}
		// If no other roi has been preserved, remove files of first cut
		else if(file.find("\\cut0") != std::string::npos){
			if(!add_roi) {
				try {
					remove(entry.path());
				}
				catch (std::filesystem::filesystem_error & error) {
					std::cerr << "Error deleting " << file << std::endl;
				}
			}
		else if(file.find("\\plan0") != std::string::npos){
			if(!add_roi) {
				try {
					remove(entry.path());
				}
				catch (std::filesystem::filesystem_error & error) {
					std::cerr << "Error deleting " << file << std::endl;
				}
			}
		}
	}
	#elif defined(UNIX)
	if ((dir = opendir(folder.c_str())) != NULL) {
		while ((ent = readdir(dir)) != NULL) {
			file = folder + "/" + std::string(ent->d_name);			
			for (auto& c : file) { c = tolower(c); }
			find_finalmtl = file.find("finalmtl");
			find_mtl = file.find("/mtl.");
			find_roi = file.find("roi");
			find_molds = file.find("slitmold");
			find_cuts = file.find("/cut");
			find_oriented = file.find("oriented");
			//Remove unuseful files
			if ((find_finalmtl & find_mtl & find_roi & find_molds & find_cuts & find_oriented) == std::string::npos) {		
				if (remove(file.c_str()) != 0)
					std::cerr << "Error deleting " << std::string(ent->d_name) << std::endl;
			}
			// If no other roi has been preserved, remove files of first cut
			else if(file.find("/cut0") != std::string::npos){
				if(!add_roi) {
					remove(file.c_str());
				}
			}
			else if(file.find("/plan0") != std::string::npos){
				if(!add_roi) {
					remove(file.c_str());
				}
			}
		}
		closedir(dir);
	}
	#endif

	return EXIT_SUCCESS;


}

int ManualMethod(std::string folder){
	// If files .mat already created: create the mold.
	// Otherwise, create images of the plan to orient.

	ImagePointer planP = ReadImage("planP.nii.gz");
	ImagePointer planN = ReadImage("planN.nii.gz");
	std::vector<ImagePointer> table1P, table2P, table1N, table2N;

	// Searching for transformation files
	std::string file;
	std::size_t find_txt, find_mat, find_mold1, find_mold2;
	bool mat_exist = false;



	#if defined(WIN)
	// Orient the cuts and store them into tables corresponding to the right mold
	for (const auto& entry : std::filesystem::directory_iterator(folder)) {
		file = entry.path().string();
		find_mat = file.find(".mat");
		find_txt = file.find(".txt");

		if ((find_mat != std::string::npos) || (find_txt != std::string::npos)) {
			std::vector<ImagePointer> table1, table2;
			find_mold1 = file.find("mold1");
			find_mold2 = file.find("mold2");
			if(find_mold1 != std::string::npos){
				table1P.push_back(ResliceImage(planP, file));
				table1N.push_back(ResliceImage(planN, file));
			}
			else if(find_mold2 != std::string::npos){
				table2P.push_back(ResliceImage(planP, file));
				table2N.push_back(ResliceImage(planN, file));
			}
		}
	}
	#elif defined(UNIX)
	DIR* dir;
	struct dirent* ent;
	chdir("..");
	char s[100];
	// std::string folder_cuts = std::string(getcwd(s, 100));
	if ((dir = opendir(getcwd(s, 100))) != NULL) {
		while ((ent = readdir(dir)) != NULL) {
			file = std::string(ent->d_name);
			find_mat = file.find("aa.mat");
			find_txt = file.find(".txt");

			if ((find_mat != std::string::npos) || (find_txt != std::string::npos)) {
				find_mold1 = file.find("mold1");
				find_mold2 = file.find("mold2");

				if(find_mold1 != std::string::npos){
					table1P.push_back(ResliceImage(planP, file));
					table1N.push_back(ResliceImage(planN, file));
				}
				else if(find_mold2 != std::string::npos){
					table2P.push_back(ResliceImage(planP, file));
					table2N.push_back(ResliceImage(planN, file));
				}
			}
		}
		closedir(dir);
		chdir("Manual");
	}
	#endif


	std::vector<ImagePointer> tableP = table1P;
	tableP.insert(tableP.end(), table2P.begin(), table2P.end() );
	ImagePointer interCuts = IntersectionImages(tableP);

	ImagePointer hem = ReadImage("OrientedHemisphere.nii.gz");
	ImagePointer hemSeg = ReadImage("OrientedHemisphereSeg.nii.gz");

	// Piece of tissue supposed to result after using the 1st mold
	ImagePointer interCutsMold1 = IntersectionImages(table1P);
	ImagePointer MTL = BinaryMathOperation(MULTIPLY, hem, interCutsMold1);
	ImagePointer MTLSeg = BinaryMathOperation(MULTIPLY, hemSeg, interCutsMold1);
	Write("MTL.nii.gz", MTL);
	Write("MTLSeg.nii.gz", MTLSeg);
	// Piece supposed to result after using the two molds
	ImagePointer finalMTL = BinaryMathOperation(MULTIPLY, hem, interCuts);
	Write("finalMTL.nii.gz", finalMTL);

	std::vector<ImagePointer> table_cuts;
	int ind = 0;
	for(int i = 0; i < table1N.size(); i++){
		++ind;
		ImagePointer img = CreateRealCut(table1N[i]);
		table_cuts.push_back(img);
		Write("cut" + std::to_string(ind) + ".nii.gz", img);
	}
	for(int i = 0; i < table2N.size(); i++){
		++ind;
		ImagePointer img = CreateRealCut(table2N[i]);
		table_cuts.push_back(img);
		Write("cut" + std::to_string(ind) + ".nii.gz", img);
	}
	int nb_cuts_1 = table1P.size();
	CreateMolds(table_cuts, nb_cuts_1);


	//___________________________________________________________________________________________
	//________________________________________Cleaning___________________________________________
	//___________________________________________________________________________________________

	folder += "/Manual";
	std::size_t find_hem, find_finalmtl, find_mtl, find_roi, find_molds, find_cuts;

	#if defined(WIN)
	for (const auto& entry : std::filesystem::directory_iterator(folder)) {
		file = entry.path().string();
		for (auto& c : file) { c = tolower(c); }

		find_finalmtl = file.find("finalmtl");
		find_mtl = file.find("\\mtl.");
		find_molds = file.find("slitmold");
		find_cuts = file.find("\\cut");


		if ((find_finalmtl & find_mtl & find_molds & find_cuts) == std::string::npos) {
			try {
				std::cout << "File : " << file << " removed." << std::endl;
				remove(entry.path());
			}
			catch (std::filesystem::filesystem_error & error) {
				std::cerr << "Error deleting " << file << std::endl;
			}
		}
	}
	#elif defined(UNIX)
	if ((dir = opendir(folder.c_str())) != NULL) {
		while ((ent = readdir(dir)) != NULL) {
			file = folder + "/" + std::string(ent->d_name);			
			for (auto& c : file) { c = tolower(c); }
			find_finalmtl = file.find("finalmtl");
			find_mtl = file.find("/mtl.");
			find_molds = file.find("slitmold");
			find_cuts = file.find("/cut");
			if ((find_finalmtl & find_mtl & find_molds & find_cuts) == std::string::npos) {		
				remove(file.c_str());
			}
		}
		closedir(dir);
	}
	#endif
	return EXIT_SUCCESS;
}







int main(int argc, char* argv[]) {
	/* 
	Using Powell optimizer:
		The user needs to give the path of the folder countaining following files:
		- file of the hemisphere
		- segmentation of the hemisphere
		- segmentation of the MTL
		- optional segmentation of an additional ROI to preserve
	Positionning the cuts manually:
		The user needs to give the path of the folder countaining the hemisphere.
		If the files of transformation matrix are already created, molds are created.
		Otherwise, image of plan to orient is created and user needs to run the program a 2nd time with .mat files.
	*/

	std::chrono::duration<double> t_total;
	std::chrono::time_point<std::chrono::system_clock> start_prog, end_prog;
	start_prog = std::chrono::system_clock::now();
	int exit;

	bool optimizer = true;

	// Manual mode or using optimization
	if(argc > 2){
		std::size_t manual = std::string(argv[2]).find("manual");
		if (manual != std::string::npos){
			optimizer = false;
		}
	}

	//___________________________________________________________________________________________
	//________________________________________Directory__________________________________________
	//___________________________________________________________________________________________
	
	//Open the directory countaining the files
	chdir(argv[1]);

	//Read and padd the files
	ImageType::SizeType padExtent; padExtent.Fill(50);
	ImagePointer hem = ReadImage("hemisphere.nii.gz");
	ImagePointer m_hem = PadImage(hem, padExtent, padExtent, 0);
	ImagePointer hemSeg = ReadImage("hemisphere_seg.nii.gz");
	ImagePointer m_hemSeg = PadImage(hemSeg, padExtent, padExtent, 0);



	//Create a folder for the files created
	std::string folder;
	#if defined(WIN)
		if (optimizer){ folder = "Automatic";}
		else{ folder = "Manual";}
		if (mkdir(folder.c_str()) != 0 & errno != 17) { std::cout << strerror(errno) << std::endl;}
	#else
		if (optimizer){ folder = "Automatic";}
		else{ folder = "Manual";}
		if (mkdir(folder.c_str(), S_IRWXU) != 0 & errno != 17) { std::cout << strerror(errno) << std::endl;}
	#endif

	//Open the new folder
	if (chdir(folder.c_str()) != 0) {std::cout << strerror(errno) << std::endl;}

	//___________________________________________________________________________________________
	//__________________________________________Method___________________________________________
	//___________________________________________________________________________________________
	
	// User sets the width for the cut
	float widthCut;
	std::cout << "Enter the width of the cut in mm: ";
	std::cin >> widthCut;

	ImagePointer planN = CreatePlanN(m_hemSeg, widthCut);
	ImagePointer planP = Thresh(planN, -1, 0, 1, 0);

	
	if(optimizer) {
		Write("planN.nii.gz", planN);
		Write("planP.nii.gz", planP);
		Write("hemisphere.nii.gz", m_hem);
		Write("hemisphere_seg.nii.gz", m_hemSeg);
		exit = OptimizationMethod(argv[1]);
	}

	else{
		// Search for tranformation matrices. 
		// If files don't exist yet, a 3D image of the plan to orient is created. Otherwise, molds are created.
		std::string file;
		std::size_t find_txt, find_mat, find_mold1, find_mold2;
		bool mat_exist = false;
		#if defined(WIN)
		for (const auto& entry : std::filesystem::directory_iterator(argv[1])) {
			file = entry.path().string();
			find_mat = file.find(".mat");
			find_txt = file.find(".txt");

			if ((find_mat != std::string::npos) || (find_txt != std::string::npos)) {
				find_mold1 = file.find("mold1");
				find_mold2 = file.find("mold2");
				if((find_mold1 != std::string::npos) || (find_mold2 != std::string::npos)){
					mat_exist = true;
				}
			}
		}
		#elif defined(UNIX)
		DIR* dir;
		struct dirent* ent;
		chdir("..");
		if ((dir = opendir(argv[1])) != NULL) {
			while ((ent = readdir(dir)) != NULL) {
				file = std::string(ent->d_name);
				find_mat = file.find(".mat");
				find_txt = file.find(".txt");

				if ((find_mat != std::string::npos) || (find_txt != std::string::npos)) {
					find_mold1 = file.find("mold1");
					find_mold2 = file.find("mold2");
					if((find_mold1 != std::string::npos) || (find_mold2 != std::string::npos)){
						mat_exist = true;
					}
				}
			}
			closedir(dir);
			chdir("Manual");
		}
		#endif

		if (mat_exist){
			Write("planN.nii.gz", planN);
			Write("planP.nii.gz", planP);
			Write("OrientedHemisphere.nii.gz", m_hem);
			Write("OrientedHemisphereSeg.nii.gz", m_hemSeg);
			exit = ManualMethod(argv[1]);
		}
		else{
			ImagePointer plan = ReplaceIntensities(planN, 1, 0);
			plan = ReplaceIntensities(plan, 2, 1);
			Write("plan.nii.gz", plan);
			std::cout << "Image of the plan created. Transformation files '.mat' or '.txt' needed." << std::endl;
		}
	}

	end_prog = std::chrono::system_clock::now();
	t_total = (end_prog - start_prog) / 60; //in minutes
	std::cout << "\n================================= " << t_total.count() << "min =================================\n" << std::endl;
	return exit;

}
