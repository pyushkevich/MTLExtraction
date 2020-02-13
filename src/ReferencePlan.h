#ifndef __ReferencePlan_h_
#define __ReferencePlan_h_

#include "UsingNames.h"

ImagePointer CreatePlanN(ImagePointer iref, float width);

ImagePointer CreateRealCut(ImagePointer planN);

#endif