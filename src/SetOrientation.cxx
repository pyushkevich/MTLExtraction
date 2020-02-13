#include "SetOrientation.h"

// This is a Convert3D function

ImagePointer SetOrientation(ImagePointer img, std::string rai)
{
    // Create a direction matrix
    vnl_matrix_fixed<double, VDim, VDim> eye, dm;
    eye.set_identity(); dm.set_identity();

    // RAI codes
    char codes[3][2] = { {'R', 'L'}, {'A', 'P'}, {'I', 'S'}};

    for(size_t i = 0; i < VDim; i++)
        {
        bool matched = false;
        for(size_t j = 0; j < VDim; j++)
        {
            for(size_t k = 0; k < 2; k++)
            {   
                if(toupper(rai[i]) == codes[j][k])
                {
                    // Set the row of the direction matrix
                    dm.set_column(i, (k==0 ? 1.0 : -1.0) * eye.get_row(j));

                    // Clear that code (so that we catch orientation codes like RRS)
                    codes[j][0] = codes[j][1] = 'X';

                    // We found a code for i
                    matched = true;
                }
            }
        }

        if(!matched){
            std::cerr << "Orientation code " << rai.c_str() << " is invalid" << std::endl;
            return img;
        }
            
    }

    // Set the direction in the image
    img->SetDirection(itk::Matrix<double,VDim,VDim>(dm));
    img->Update();
    return img;
}