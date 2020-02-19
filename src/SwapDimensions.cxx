#include "SwapDimensions.h"
#include "itkSpatialOrientation.h"
#include "itkOrientImageFilter.h"

// This is a Convert3D function

using namespace itk::SpatialOrientation;

ValidCoordinateOrientationFlags GetOrientationFlagFromString(const std::string &code)
{
  // Convert the code to upper case
  std::string code_upper = code;
  for(int i = 0; i < code_upper.size(); i++)
    code_upper[i] = std::toupper(code_upper[i]);

  // Lazy :)
  std::map<std::string, ValidCoordinateOrientationFlags> emap;
  emap["RIP"] = ITK_COORDINATE_ORIENTATION_RIP;
  emap["LIP"] = ITK_COORDINATE_ORIENTATION_LIP;
  emap["RSP"] = ITK_COORDINATE_ORIENTATION_RSP;
  emap["LSP"] = ITK_COORDINATE_ORIENTATION_LSP;
  emap["RIA"] = ITK_COORDINATE_ORIENTATION_RIA;
  emap["LIA"] = ITK_COORDINATE_ORIENTATION_LIA;
  emap["RSA"] = ITK_COORDINATE_ORIENTATION_RSA;
  emap["LSA"] = ITK_COORDINATE_ORIENTATION_LSA;
  emap["IRP"] = ITK_COORDINATE_ORIENTATION_IRP;
  emap["ILP"] = ITK_COORDINATE_ORIENTATION_ILP;
  emap["SRP"] = ITK_COORDINATE_ORIENTATION_SRP;
  emap["SLP"] = ITK_COORDINATE_ORIENTATION_SLP;
  emap["IRA"] = ITK_COORDINATE_ORIENTATION_IRA;
  emap["ILA"] = ITK_COORDINATE_ORIENTATION_ILA;
  emap["SRA"] = ITK_COORDINATE_ORIENTATION_SRA;
  emap["SLA"] = ITK_COORDINATE_ORIENTATION_SLA;
  emap["RPI"] = ITK_COORDINATE_ORIENTATION_RPI;
  emap["LPI"] = ITK_COORDINATE_ORIENTATION_LPI;
  emap["RAI"] = ITK_COORDINATE_ORIENTATION_RAI;
  emap["LAI"] = ITK_COORDINATE_ORIENTATION_LAI;
  emap["RPS"] = ITK_COORDINATE_ORIENTATION_RPS;
  emap["LPS"] = ITK_COORDINATE_ORIENTATION_LPS;
  emap["RAS"] = ITK_COORDINATE_ORIENTATION_RAS;
  emap["LAS"] = ITK_COORDINATE_ORIENTATION_LAS;
  emap["PRI"] = ITK_COORDINATE_ORIENTATION_PRI;
  emap["PLI"] = ITK_COORDINATE_ORIENTATION_PLI;
  emap["ARI"] = ITK_COORDINATE_ORIENTATION_ARI;
  emap["ALI"] = ITK_COORDINATE_ORIENTATION_ALI;
  emap["PRS"] = ITK_COORDINATE_ORIENTATION_PRS;
  emap["PLS"] = ITK_COORDINATE_ORIENTATION_PLS;
  emap["ARS"] = ITK_COORDINATE_ORIENTATION_ARS;
  emap["ALS"] = ITK_COORDINATE_ORIENTATION_ALS;
  emap["IPR"] = ITK_COORDINATE_ORIENTATION_IPR;
  emap["SPR"] = ITK_COORDINATE_ORIENTATION_SPR;
  emap["IAR"] = ITK_COORDINATE_ORIENTATION_IAR;
  emap["SAR"] = ITK_COORDINATE_ORIENTATION_SAR;
  emap["IPL"] = ITK_COORDINATE_ORIENTATION_IPL;
  emap["SPL"] = ITK_COORDINATE_ORIENTATION_SPL;
  emap["IAL"] = ITK_COORDINATE_ORIENTATION_IAL;
  emap["SAL"] = ITK_COORDINATE_ORIENTATION_SAL;
  emap["PIR"] = ITK_COORDINATE_ORIENTATION_PIR;
  emap["PSR"] = ITK_COORDINATE_ORIENTATION_PSR;
  emap["AIR"] = ITK_COORDINATE_ORIENTATION_AIR;
  emap["ASR"] = ITK_COORDINATE_ORIENTATION_ASR;
  emap["PIL"] = ITK_COORDINATE_ORIENTATION_PIL;
  emap["PSL"] = ITK_COORDINATE_ORIENTATION_PSL;
  emap["AIL"] = ITK_COORDINATE_ORIENTATION_AIL;
  emap["ASL"] = ITK_COORDINATE_ORIENTATION_ASL;

  if(emap.find(code) != emap.end())
    return emap[code];

  else
    return ITK_COORDINATE_ORIENTATION_INVALID;
}

ImagePointer SwapDimensions_OrientWorker(ImagePointer img, const std::string &code){

    ValidCoordinateOrientationFlags oflag = GetOrientationFlagFromString(code);

    typedef itk::OrientImageFilter<ImageType, ImageType> FilterType;
    typename FilterType::Pointer fltOrient = FilterType::New();
    fltOrient->SetInput(img);
    fltOrient->UseImageDirectionOn();
    fltOrient->SetDesiredCoordinateOrientation(oflag);
    fltOrient->UpdateLargestPossibleRegion();
    return fltOrient->GetOutput();
};
