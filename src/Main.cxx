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

/*
void Test(std::string Createdfolder) {
	std::string file;
	std::size_t find_hem, find_finalmtl, find_mtl1, find_mtl2, find_roi, find_molds, find_cuts1, find_cuts2, find_oriented;

	for (const auto& entry : std::filesystem::directory_iterator(Createdfolder)) {
		file = entry.path().string();
		for (auto& c : file) { c = tolower(c); }

		//find_hem = file.find("hemisphere");
		find_finalmtl = file.find("finalmtl");
		find_mtl1 = file.find("/mtl.");
		find_mtl2 = file.find("\\mtl.");
		find_roi = file.find("roi");
		find_molds = file.find("slitmold");
		find_cuts1 = file.find("/cut");
		find_cuts2 = file.find("\\cut");
		find_oriented = file.find("oriented");


		if ((find_mtl1 & find_mtl2 & find_roi & find_molds & find_cuts1 & find_cuts2 & find_oriented) == std::string::npos) {
			
			try { 
				std::cout << "File : " << file << " removed." << std::endl;
				remove(entry.path()); 
			}
			catch (std::filesystem::filesystem_error & error){
				//std::cerr << "Error: " << error << std::endl;
				std::cerr << "Error deleting " << file << std::endl;
			}
		}
	}
	std::cout << "\nFiles kept:" << std::endl;
	for (const auto& entry : std::filesystem::directory_iterator(Createdfolder)) {
		std::cout << entry.path().string() << std::endl;
	}
	exit(0);
}
*/

int main(int argc, char* argv[]) {
	argv[1] = "U:/Files_for_Automatisation/INDD118374L";
	// Test(std::string(argv[1]));

	std::chrono::duration<double> t_total;
	std::chrono::time_point<std::chrono::system_clock> start_prog, end_prog;
	start_prog = std::chrono::system_clock::now();

	/* The user needs to give the path of the folder countaining following files:
		- file of the hemisphere
		- segmentation of the hemisphere
		- segmentation of the MTL
		- optional segmentation of an additional ROI to preserve
	*/
	

	//___________________________________________________________________________________________
	//________________________________________New Folder_________________________________________
	//___________________________________________________________________________________________
	std::string Createdfolder = std::string(argv[1]) + "/Files";

	#if defined(WIN)
	if (mkdir(Createdfolder.c_str()) != 0) { std::cout << strerror(errno) << std::endl; }
	#else
	if (mkdir(Createdfolder.c_str(), S_IRWXU) != 0) { std::cout << strerror(errno) << std::endl; }
	#endif

	if (chdir(Createdfolder.c_str()) != 0) {std::cout << strerror(errno) << std::endl;}


	//___________________________________________________________________________________________
	//_______________________________________Reading Files_______________________________________
	//___________________________________________________________________________________________
	// User sets the width for the cut
	float widthCut;
	std::cout << "Enter the width of the cut in mm: ";
	std::cin >> widthCut;
	std::cout << "Starting optimization..." << std::endl;

	std::string folder = std::string(argv[1]);
	ImagePointer hem = ReadImage(folder + "/hemisphere.nii.gz");
	ImagePointer hemSeg = ReadImage(folder + "/hemisphere_seg.nii.gz");
	ImagePointer mtlSeg = ReadImage(folder + "/mtl_seg.nii.gz");
	ImagePointer roiSeg; 
	bool add_roi = false;

	#if defined(WIN)
	// Read the segmentation of another ROI to preserve, if it exists
	for (const auto& entry : std::filesystem::directory_iterator(folder)) {
		std::size_t find_hem = entry.path().string().find("roi_seg");
		if (find_hem != std::string::npos) {
			roiSeg = ReadImage(folder + "/roi_seg.nii.gz");
			add_roi = true;
			break;
		}
	}
	#elif defined(UNIX)
	DIR* dir;
	struct dirent* ent;
	if ((dir = opendir(folder)) != NULL) {
		while ((ent = readdir(dir)) != NULL) {
			std::size_t find_hem = ent->d_name.find("roi_seg");
			if (find_hem != std::string::npos) {
				roiSeg = ReadImage(folder + "/roi_seg.nii.gz");
				add_roi = true;
				break;
			}
		}
		closedir(dir);
	}
	else {
		/* could not open directory */
		perror("");
		return EXIT_FAILURE;
	}
	#endif
	
	
	
		
	//___________________________________________________________________________________________
	//________________________________________Treatments_________________________________________
	//___________________________________________________________________________________________

	std::cout << "Padding..." << std::endl;
	ImageType::SizeType padExtent; padExtent.Fill(50);

	ImagePointer m_hem = PadImage(hem, padExtent, padExtent, 0);
	Write("hemisphere.nii.gz", m_hem);

	ImagePointer m_hemSeg = PadImage(hemSeg, padExtent, padExtent, 0);
	Write("hemisphere_seg.nii.gz", m_hemSeg);

	ImagePointer m_mtl = PadImage(mtlSeg, padExtent, padExtent, 0);
	Write("mtl_seg.nii.gz", m_mtl);

	if (add_roi) {
		ImagePointer m_roi = PadImage(roiSeg, padExtent, padExtent, 0);
		Write("roi_seg.nii.gz", m_roi);
	}

	ImagePointer planN = CreatePlanN(m_hem, widthCut);
	Write("planN.nii.gz", planN);
	Write("planP.nii.gz", Thresh(planN, -1, 0, 1, 0));
	

	//___________________________________________________________________________________________
	//_______________________________________Optimisation________________________________________
	//___________________________________________________________________________________________

	std::chrono::duration<double> duration1, duration2, duration3;

	std::chrono::time_point<std::chrono::system_clock> start, end;

  
    start = std::chrono::system_clock::now();
	std::cout << "Orienting cylinder..." << std::endl;
	OrientationCylinderPowell();
    end = std::chrono::system_clock::now(); 
  
    duration1 = (end - start) / 60; //in minutes
	std::cout << "\n		Orienting cylinder took " << duration1.count() << "min." << std::endl;

	if (add_roi) {
		start = std::chrono::system_clock::now();
		std::cout << "Orienting first cut..." << std::endl;
		OrientationFirstCutPowell();
		end = std::chrono::system_clock::now(); 
  
		duration2 = (end - start) / 60; //in minutes
		std::cout << "\n		Finding the first cut took " << duration2.count() << "min." << std::endl;
	}
	// If there is no other ROI to preserve, the first plan is replaced by a 3D image with the sigle value 1, 
	// so it won't impact other cuts
	else {
		ImagePointer plan0 = m_hem;
		plan0->FillBuffer(1);
		Write("plan0.nii.gz", plan0);
		ImagePointer cut0 = m_hem;
		cut0->FillBuffer(0);
		Write("cut0.nii.gz", cut0);
	}
   		
    start = std::chrono::system_clock::now();
	std::cout << "Orienting cuts..." << std::endl;
	OrientationCutsPowell();
    end = std::chrono::system_clock::now(); 
  
    duration3 = (end - start) / 60; //in minutes

	std::cout << "\n		Finding the other cuts took " << duration3.count() << "min." << std::endl;

	//___________________________________________________________________________________________
	//________________________________________Cleaning___________________________________________
	//___________________________________________________________________________________________

	std::string file;
	std::size_t find_hem, find_finalmtl, find_mtl1, find_mtl2, find_roi, find_molds, find_cuts1, find_cuts2, find_oriented;

	#if defined(WIN)
	for (const auto& entry : std::filesystem::directory_iterator(Createdfolder)) {
		file = entry.path().string();
		for (auto& c : file) { c = tolower(c); }

		find_finalmtl = file.find("finalmtl");
		find_mtl1 = file.find("/mtl.");
		find_mtl2 = file.find("\\mtl.");
		find_roi = file.find("roi");
		find_molds = file.find("slitmold");
		find_cuts1 = file.find("/cut");
		find_cuts2 = file.find("\\cut");
		find_oriented = file.find("oriented");


		if ((find_mtl1 & find_mtl2 & find_roi & find_molds & find_cuts1 & find_cuts2 & find_oriented) == std::string::npos) {
			try {
				std::cout << "File : " << file << " removed." << std::endl;
				remove(entry.path());
			}
			catch (std::filesystem::filesystem_error & error) {
				//std::cerr << "Error: " << error << std::endl;
				std::cerr << "Error deleting " << file << std::endl;
			}
		}
	}
	#elif defined(UNIX)

	DIR* dir;
	struct dirent* ent;
	if ((dir = opendir(Createdfolder)) != NULL) {
		while ((ent = readdir(dir)) != NULL) {
			file = std::string(ent->d_name);
			for (auto& c : file) { c = tolower(c); }
			find_finalmtl = file.find("finalmtl");
			find_mtl1 = file.find("/mtl.");
			find_mtl2 = file.find("\\mtl.");
			find_roi = file.find("roi");
			find_molds = file.find("slitmold");
			find_cuts1 = file.find("/cut");
			find_cuts2 = file.find("\\cut");
			find_oriented = file.find("oriented");
			if ((find_mtl1 & find_mtl2 & find_roi & find_molds & find_cuts1 & find_cuts2 & find_oriented) == std::string::npos) {
				if (remove(ent->d_name) != 0)
					perror("Error deleting file");
				else
					puts("File successfully deleted");
			}
		}
		closedir(dir);
	}
	else {
		/* could not open directory */
		perror("");
		return EXIT_FAILURE;
	}

	#endif

	std::cout << "\n***All files have been created for " << argv[1] << "***" << std::endl << std::endl;

	end_prog = std::chrono::system_clock::now();
	t_total = (end_prog - start_prog) / 60; //in minutes
	std::cout << "\n================================= " << t_total.count() << "min =================================\n" << std::endl;

	return EXIT_SUCCESS;
}
