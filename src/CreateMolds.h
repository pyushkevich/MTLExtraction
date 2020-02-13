#ifndef _CreateMolds_h
#define _CreateMolds_h

#include "UsingNames.h"

#include "ReadImage.h"
#include "WriteImage.h"
#include "MathematicalMorphology.h"
#include "ResliceImage.h"
#include "CoordinateMap.h"
#include "Thresholding.h"
#include "BinaryMathOperation.h"
#include "ExtrudeSegmentation.h"
#include "ReplaceIntensities.h"
#include "SwapDimensions.h"
#include "OriginVoxels.h"

void CreateMolds(std::vector<ImagePointer> cuts, int nb_cuts_mold1);

void CreateHemisphereMold(std::vector<ImagePointer> cuts, int nb_cuts_mold1);
void CreateMTLMold(std::vector<ImagePointer> cuts, int nb_cuts_mold1);


#endif