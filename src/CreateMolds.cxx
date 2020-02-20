#include "CreateMolds.h"

// As left and right hemisphere needs two different molds (different orientation of extrusion), we create 4 molds, 
// the user has to chose the 2 corresponding to the hemisphere

void CreateMolds(std::vector<ImagePointer> cuts, int nb_cuts_mold1) {
	
	// Set the same origin to each image for multiplications and thresholding
	ImagePointer iref = ReadImage("OrientedHemisphereSeg.nii.gz");
	iref = SetOriginVoxel(iref, "50%");
	for (int i = 0; i < cuts.size(); i++){
		cuts[i] = SetOriginVoxel(cuts[i], "50%");
	}

	//________________________________________First Mold_________________________________________
	//___________________________________________________________________________________________
	//Dilation of the image: mold will have the shape of the hemisphere
	ImageType::SizeType radius; radius.Fill(35);
	ImagePointer ref_mold = MorphologyTransformation(iref, false, 1, radius);
	Write("ref_mold.nii.gz", ref_mold);
	// ImagePointer ref_mold = ReadImage("ref_mold.nii.gz");

	// Solution 1
	// Reorient the segmentation before the extrusion
	ImagePointer hemSeg1 = Thresh(ResliceImage(iref, 0.0, -9.0, 0.0), -1, 0, 1, 0);
	hemSeg1 = SwapDimensions_OrientWorker(hemSeg1, "PRI");
	ImagePointer iExtrude1 = ExtrudeSegmentation(hemSeg1); 
	ref_mold = SwapDimensions_OrientWorker(ref_mold, "LPI");
	iExtrude1 = SwapDimensions_OrientWorker(iExtrude1, "LPI");
	ImagePointer mold1 = BinaryMathOperation(MINIMUM, iExtrude1, ref_mold);
	mold1 = SwapDimensions_OrientWorker(mold1, "LPI");
	// Write("mold1.nii.gz", mold1);
	// ImagePointer mold1 = ReadImage("mold1.nii.gz");

	// Creation of higher contours, to make the cutting easier
	ImagePointer contour1 = SwapDimensions_OrientWorker(ref_mold, "PIR");
	ImagePointer contour_ext1 = ExtrudeSegmentation(Thresh(contour1, -1, 0, 1, 0));
	radius.Fill(0); radius[2] = 15;
	contour1 = SwapDimensions_OrientWorker(contour1, "LPI");
	contour1 = MorphologyTransformation(contour_ext1, false, 1, radius);
	Write("contour_dilate1.nii.gz", contour1);
	// ImagePointer contour1 = ReadImage("contour_dilate1.nii.gz");
	contour1 = SwapDimensions_OrientWorker(contour1, "LPI");
	contour_ext1 = SwapDimensions_OrientWorker(contour_ext1, "LPI");
	contour1 = BinaryMathOperation(ADD, contour1, contour_ext1);
	contour1 = ReplaceIntensities(contour1, 2, 0);
	mold1 = BinaryMathOperation(MAXIMUM, mold1, contour1);
	// Write ("mold_hem1.nii.gz", mold1);
	// ImagePointer mold1 = ReadImage("mold_hem1.nii.gz");



	// Solution 2
	ImagePointer hemSeg2 = Thresh(ResliceImage(iref, 0.0, 9.0, 0.0), -1, 0, 1, 0);
	hemSeg2 = SwapDimensions_OrientWorker(hemSeg2, "ARI");
	ImagePointer iExtrude2 = ExtrudeSegmentation(hemSeg2);
	iExtrude2 = SwapDimensions_OrientWorker(iExtrude2, "LPI");
	ref_mold = SwapDimensions_OrientWorker(ref_mold, "LPI");
	ImagePointer mold2 = BinaryMathOperation(MINIMUM, iExtrude2, ref_mold);
	mold2 = SwapDimensions_OrientWorker(mold2, "LPI");
	// Write("mold2.nii.gz", mold2);
	// ImagePointer mold2 = ReadImage("mold2.nii.gz");

	// Creation of higher contours, to make the cutting easier
	ImagePointer contour2 = SwapDimensions_OrientWorker(ref_mold, "AIR");
	ImagePointer contour_ext2 = ExtrudeSegmentation(Thresh(contour2, -1, 0, 1, 0));
	radius.Fill(0); radius[2] = 15;
	contour2 = SwapDimensions_OrientWorker(contour2, "LPI");
	contour2 = MorphologyTransformation(contour_ext2, false, 1, radius);
	contour2 = SwapDimensions_OrientWorker(contour2, "LPI");
	contour_ext2 = SwapDimensions_OrientWorker(contour_ext2, "LPI");
	contour2 = BinaryMathOperation(ADD, contour2, contour_ext2);
	contour2 = ReplaceIntensities(contour2, 2, 0);
	contour2 = SwapDimensions_OrientWorker(contour2, "LPI");
	mold2 = SwapDimensions_OrientWorker(mold2, "LPI");
	mold2 = BinaryMathOperation(MAXIMUM, mold2, contour2);
	Write ("mold_hem2.nii.gz", mold2);
	// ImagePointer mold2 = ReadImage("mold_hem2.nii.gz");


	mold1 = SwapDimensions_OrientWorker(mold1, "LPI");
	mold2 = SwapDimensions_OrientWorker(mold2, "LPI");
	std::array<ImagePointer, 3> CMP = PhysicalCoordinates(mold1);
	// Write("CMP1.nii.gz", CMP[1]);

	ImagePointer ThreshMold1 = Thresh(CMP[1], -40, 30, 1, 0); // Flatten the mold and resize the height of the contours for solution 1
	ImagePointer ThreshMold2 = Thresh(CMP[1], -30, 30, 1, 0); // Flatten the mold and resize the height of the contours for solution 2
	ImagePointer ThreshCut1 = Thresh(CMP[1], 25, 1000, 0, 1); // Thresh the cuts, must cut all tissue but not the mold
	ImagePointer ThreshCut2 = Thresh(CMP[1], -25, 1000, 1, 0);// Thresh the cuts, must cut all tissue but not the mold
	ThreshMold1 = SwapDimensions_OrientWorker(ThreshMold1, "LPI");
	ThreshMold2 = SwapDimensions_OrientWorker(ThreshMold2, "LPI");
	ThreshCut1 = SwapDimensions_OrientWorker(ThreshCut1, "LPI");
	ThreshCut2 = SwapDimensions_OrientWorker(ThreshCut2, "LPI");

	mold1 = SwapDimensions_OrientWorker(mold1, "LPI");
	mold2 = SwapDimensions_OrientWorker(mold2, "LPI");
	
	mold1 = BinaryMathOperation(MINIMUM, mold1, ThreshMold1);
	mold2 = BinaryMathOperation(MINIMUM, mold2, ThreshMold2);


	ImagePointer slitmold1 = mold1;
	ImagePointer slitmold2 = mold2;
	ImagePointer cut1, cut2;

	// Create the two molds with slits
	for (int i = 0; i < nb_cuts_mold1; i++){		
		cut1 = BinaryMathOperation(MINIMUM, SwapDimensions_OrientWorker(cuts[i], "LPI"), ThreshCut1);
		cut1 = Thresh(cut1, -1, 0, 1, 0);
		slitmold1 = BinaryMathOperation(MINIMUM, cut1, slitmold1);
		cut2 = BinaryMathOperation(MINIMUM, SwapDimensions_OrientWorker(cuts[i], "LPI"), ThreshCut2);
		cut2 = Thresh(cut2, -1, 0, 1, 0);
		slitmold2 = BinaryMathOperation(MINIMUM, cut2, slitmold2);
	}
	Write("slitmold_hem1.nii.gz", slitmold1);
	Write("slitmold_hem2.nii.gz", slitmold2);
	

	//_______________________________________Second Mold_________________________________________
	//___________________________________________________________________________________________
	ImagePointer mtlSeg = ReadImage("MTLSeg.nii.gz");
	mtlSeg = SetOriginVoxel(mtlSeg, "50%");

	// Solution 1
	mtlSeg = SwapDimensions_OrientWorker(mtlSeg, "ILP");
	ImagePointer iExtMTL = ExtrudeSegmentation(Thresh(mtlSeg, -1, 0, 1, 0));
	ImagePointer mold1_mtl = SwapDimensions_OrientWorker(iExtMTL, "LPI");
	Write("mold1_mtl.nii.gz", mold1_mtl);
	// ImagePointer mold1_mtl = ReadImage("mold1_mtl.nii.gz");

	// Solution 2
	mtlSeg = SwapDimensions_OrientWorker(mtlSeg, "SLP");
	iExtMTL = ExtrudeSegmentation(Thresh(mtlSeg, -1, 0, 1, 0));
	ImagePointer mold2_mtl = SwapDimensions_OrientWorker(iExtMTL, "LPI");
	Write("mold2_mtl.nii.gz", mold2_mtl);
	// ImagePointer mold2_mtl = ReadImage("mold2_mtl.nii.gz");

	std::array<ImagePointer, 3> CMP2 = PhysicalCoordinates(cuts[0]);
	Write("CMP2.nii.gz", CMP2[2]);
	

	ImagePointer slitmold_mtl1 = mold1_mtl;
	ImagePointer slitmold_mtl2 = mold2_mtl;
	ThreshCut1 = Thresh(CMP2[2], -1000, -35, 0, 1);
	ThreshCut2 = Thresh(CMP2[2], 55, 1000, 0, 1);

	
	for (int i = nb_cuts_mold1; i < cuts.size(); i++){
		cut1 = BinaryMathOperation(MINIMUM, SwapDimensions_OrientWorker(cuts[i], "LPI"), SwapDimensions_OrientWorker(ThreshCut1, "LPI"));
		cut1 = Thresh(cut1, -1, 0, 1, 0);
		slitmold_mtl1 = BinaryMathOperation(MINIMUM, SwapDimensions_OrientWorker(cut1, "LPI"), SwapDimensions_OrientWorker(slitmold_mtl1, "LPI"));
		cut2 = BinaryMathOperation(MINIMUM, SwapDimensions_OrientWorker(cuts[i], "LPI"), SwapDimensions_OrientWorker(ThreshCut2, "LPI"));
		cut2 = Thresh(cut2, -1, 0, 1, 0);
		slitmold_mtl2 = BinaryMathOperation(MINIMUM, SwapDimensions_OrientWorker(cut2, "LPI"), SwapDimensions_OrientWorker(slitmold_mtl2, "LPI"));
	}

	Write("slitmold_mtl1.nii.gz", slitmold_mtl1);
	Write("slitmold_mtl2.nii.gz", slitmold_mtl2);
}

void CreateHemisphereMold(std::vector<ImagePointer> cuts, int nb_cuts_mold1) {
	
	ImagePointer iref = ReadImage("OrientedHemisphereSeg.nii.gz");
	iref = SetOriginVoxel(iref, "50%");
	for (int i = 0; i < cuts.size(); i++) {
		cuts[i] = SetOriginVoxel(cuts[i], "50%");
	}

	//Dilation of the image: mold will have the shape of the hemisphere
	ImageType::SizeType radius; radius.Fill(35);
	ImagePointer ref_mold = MorphologyTransformation(iref, false, 1, radius);
	Write("ref_mold.nii.gz", ref_mold);
	// ImagePointer ref_mold = ReadImage("ref_mold.nii.gz");

	// Solution 1
	std::cout << "		Solution 1" << std::endl;
	// Reorient the segmentation before the extrusion
	ImagePointer hemSeg1 = Thresh(ResliceImage(iref, 0.0, -9.0, 0.0), -1, 0, 1, 0);

	hemSeg1 = SwapDimensions_OrientWorker(hemSeg1, "PRI");
	ImagePointer iExtrude1 = ExtrudeSegmentation(hemSeg1);
	ref_mold = SwapDimensions_OrientWorker(ref_mold, "LPI");
	iExtrude1 = SwapDimensions_OrientWorker(iExtrude1, "LPI");
	ImagePointer mold1 = BinaryMathOperation(MINIMUM, iExtrude1, ref_mold);
	mold1 = SwapDimensions_OrientWorker(mold1, "LPI");
	// Write("mold1.nii.gz", mold1);
	// ImagePointer mold1 = ReadImage("mold1.nii.gz");

	// Creation of higher contours, to make the cutting easier
	ImagePointer contour1 = SwapDimensions_OrientWorker(ref_mold, "PIR");
	ImagePointer contour_ext1 = ExtrudeSegmentation(Thresh(contour1, -1, 0, 1, 0));
	radius.Fill(0); radius[2] = 15;
	contour1 = SwapDimensions_OrientWorker(contour1, "LPI");
	contour1 = MorphologyTransformation(contour_ext1, false, 1, radius);
	Write("contour_dilate1.nii.gz", contour1);
	// ImagePointer contour1 = ReadImage("contour_dilate1.nii.gz");
	contour1 = SwapDimensions_OrientWorker(contour1, "LPI");
	contour_ext1 = SwapDimensions_OrientWorker(contour_ext1, "LPI");
	contour1 = BinaryMathOperation(ADD, contour1, contour_ext1);
	contour1 = ReplaceIntensities(contour1, 2, 0);
	mold1 = BinaryMathOperation(MAXIMUM, mold1, contour1);
	// Write ("mold_hem1.nii.gz", mold1);
	// ImagePointer mold1 = ReadImage("mold_hem1.nii.gz");



	// Solution 2
	std::cout << "		Solution 2" << std::endl;
	ImagePointer hemSeg2 = Thresh(ResliceImage(iref, 0.0, 9.0, 0.0), -1, 0, 1, 0);
	hemSeg2 = SwapDimensions_OrientWorker(hemSeg2, "ARI");
	ImagePointer iExtrude2 = ExtrudeSegmentation(hemSeg2);
	iExtrude2 = SwapDimensions_OrientWorker(iExtrude2, "LPI");
	ref_mold = SwapDimensions_OrientWorker(ref_mold, "LPI");
	ImagePointer mold2 = BinaryMathOperation(MINIMUM, iExtrude2, ref_mold);
	mold2 = SwapDimensions_OrientWorker(mold2, "LPI");
	// Write("mold2.nii.gz", mold2);
	// ImagePointer mold2 = ReadImage("mold2.nii.gz");

	// Creation of higher contours, to make the cutting easier
	ImagePointer contour2 = SwapDimensions_OrientWorker(ref_mold, "AIR");
	ImagePointer contour_ext2 = ExtrudeSegmentation(Thresh(contour2, -1, 0, 1, 0));
	radius.Fill(0); radius[2] = 15;
	contour2 = SwapDimensions_OrientWorker(contour2, "LPI");
	contour2 = MorphologyTransformation(contour_ext2, false, 1, radius);
	// Write("contour_dilate2.nii.gz", contour2);
	// ImagePointer contour2 = ReadImage("contour_dilate2.nii.gz");
	contour2 = SwapDimensions_OrientWorker(contour2, "LPI");
	contour_ext2 = SwapDimensions_OrientWorker(contour_ext2, "LPI");
	contour2 = BinaryMathOperation(ADD, contour2, contour_ext2);
	contour2 = ReplaceIntensities(contour2, 2, 0);
	contour2 = SwapDimensions_OrientWorker(contour2, "LPI");
	mold2 = SwapDimensions_OrientWorker(mold2, "LPI");
	mold2 = BinaryMathOperation(MAXIMUM, mold2, contour2);
	Write("mold_hem2.nii.gz", mold2);
	// ImagePointer mold2 = ReadImage("mold_hem2.nii.gz");


	mold1 = SwapDimensions_OrientWorker(mold1, "LPI");
	mold2 = SwapDimensions_OrientWorker(mold2, "LPI");
	std::array<ImagePointer, 3> CMP = PhysicalCoordinates(mold1);
	// Write("CMP1.nii.gz", CMP[1]);

	ImagePointer ThreshMold1 = Thresh(CMP[1], -40, 30, 1, 0);
	ImagePointer ThreshMold2 = Thresh(CMP[1], -30, 30, 1, 0);
	ImagePointer ThreshCut1 = Thresh(CMP[1], 25, 1000, 0, 1);
	ImagePointer ThreshCut2 = Thresh(CMP[1], -25, 1000, 1, 0);
	ThreshMold1 = SwapDimensions_OrientWorker(ThreshMold1, "LPI");
	ThreshMold2 = SwapDimensions_OrientWorker(ThreshMold2, "LPI");
	ThreshCut1 = SwapDimensions_OrientWorker(ThreshCut1, "LPI");
	ThreshCut2 = SwapDimensions_OrientWorker(ThreshCut2, "LPI");

	mold1 = SwapDimensions_OrientWorker(mold1, "LPI");
	mold2 = SwapDimensions_OrientWorker(mold2, "LPI");

	mold1 = BinaryMathOperation(MINIMUM, mold1, ThreshMold1);
	mold2 = BinaryMathOperation(MINIMUM, mold2, ThreshMold2);


	ImagePointer slitmold1 = mold1;
	ImagePointer slitmold2 = mold2;
	ImagePointer cut1, cut2;

	for (int i = 0; i < nb_cuts_mold1; i++) {
		cut1 = BinaryMathOperation(MINIMUM, SwapDimensions_OrientWorker(cuts[i], "LPI"), ThreshCut1);
		cut1 = Thresh(cut1, -1, 0, 1, 0);
		slitmold1 = BinaryMathOperation(MINIMUM, cut1, slitmold1);
		cut2 = BinaryMathOperation(MINIMUM, SwapDimensions_OrientWorker(cuts[i], "LPI"), ThreshCut2);
		cut2 = Thresh(cut2, -1, 0, 1, 0);
		slitmold2 = BinaryMathOperation(MINIMUM, cut2, slitmold2);
	}
	Write("slitmold_hem1.nii.gz", slitmold1);
	Write("slitmold_hem2.nii.gz", slitmold2);
}

void CreateMTLMold(std::vector<ImagePointer> cuts, int nb_cuts_mold1) {

	ImagePointer mtlSeg = ReadImage("MTLSeg.nii.gz");
	mtlSeg = SetOriginVoxel(mtlSeg, "50%");

	// Solution 1
	std::cout << "		Solution 1" << std::endl;
	mtlSeg = SwapDimensions_OrientWorker(mtlSeg, "ILP");
	ImagePointer iExtMTL = ExtrudeSegmentation(Thresh(mtlSeg, -1, 0, 1, 0));
	ImagePointer mold1_mtl = SwapDimensions_OrientWorker(iExtMTL, "LPI");
	Write("mold1_mtl.nii.gz", mold1_mtl);
	// ImagePointer mold1_mtl = ReadImage("mold1_mtl.nii.gz");

	// Solution 2
	std::cout << "		Solution 2" << std::endl;
	mtlSeg = SwapDimensions_OrientWorker(mtlSeg, "SLP");
	iExtMTL = ExtrudeSegmentation(Thresh(mtlSeg, -1, 0, 1, 0));
	ImagePointer mold2_mtl = SwapDimensions_OrientWorker(iExtMTL, "LPI");
	Write("mold2_mtl.nii.gz", mold2_mtl);
	// ImagePointer mold2_mtl = ReadImage("mold2_mtl.nii.gz");

	std::array<ImagePointer, 3> CMP2 = PhysicalCoordinates(cuts[0]);
	Write("CMP2.nii.gz", CMP2[2]);

	ImagePointer slitmold_mtl1 = mold1_mtl;
	ImagePointer slitmold_mtl2 = mold2_mtl;
	ImagePointer ThreshCut1 = Thresh(CMP2[2], -1000, -35, 0, 1);
	ImagePointer ThreshCut2 = Thresh(CMP2[2], 55, 1000, 0, 1);
	ImagePointer cut1, cut2;

	for (int i = nb_cuts_mold1; i < cuts.size(); i++) {
		cut1 = BinaryMathOperation(MINIMUM, SwapDimensions_OrientWorker(cuts[i], "LPI"), SwapDimensions_OrientWorker(ThreshCut1, "LPI"));
		cut1 = Thresh(cut1, -1, 0, 1, 0);
		slitmold_mtl1 = BinaryMathOperation(MINIMUM, SwapDimensions_OrientWorker(cut1, "LPI"), SwapDimensions_OrientWorker(slitmold_mtl1, "LPI"));
		cut2 = BinaryMathOperation(MINIMUM, SwapDimensions_OrientWorker(cuts[i], "LPI"), SwapDimensions_OrientWorker(ThreshCut2, "LPI"));
		cut2 = Thresh(cut2, -1, 0, 1, 0);
		slitmold_mtl2 = BinaryMathOperation(MINIMUM, SwapDimensions_OrientWorker(cut2, "LPI"), SwapDimensions_OrientWorker(slitmold_mtl2, "LPI"));
	}

	Write("slitmold_mtl1.nii.gz", slitmold_mtl1);
	Write("slitmold_mtl2.nii.gz", slitmold_mtl2);
}
