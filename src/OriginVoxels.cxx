#include "OriginVoxels.h"

// This a Convert3D function

std::string str_to_lower(const char* input)
{
    std::string s(input);
    std::transform(s.begin(), s.end(), s.begin(), (int(*)(int)) tolower);
    return s;
}

TransformMatrixType GetVoxelSpaceToRASPhysicalSpaceMatrix(ImageType::Pointer image)
{
    // Generate intermediate terms
    vnl_matrix<double> m_dir, m_ras_matrix;
    vnl_diag_matrix<double> m_scale, m_lps_to_ras;
    vnl_vector<double> v_origin, v_ras_offset;

    // Compute the matrix
    m_dir = image->GetDirection().GetVnlMatrix();
    m_scale.set(image->GetSpacing().GetVnlVector());
    m_lps_to_ras.set(vnl_vector<double>(ImageType::ImageDimension, 1.0));
    m_lps_to_ras[0] = -1;
    m_lps_to_ras[1] = -1;
    m_ras_matrix = m_lps_to_ras * m_dir * m_scale;

    // Compute the vector
    v_origin = image->GetOrigin().GetVnlVector();
    v_ras_offset = m_lps_to_ras * v_origin;

    // Create the larger matrix
    TransformMatrixType mat;
    vnl_vector<double> vcol(ImageType::ImageDimension + 1, 1.0);
    vcol.update(v_ras_offset);
    mat.SetIdentity();
    mat.GetVnlMatrix().update(m_ras_matrix);
    mat.GetVnlMatrix().set_column(ImageType::ImageDimension, vcol);

    return mat;
}

RealVector ReadRealVector(ImagePointer img, const char* vec_in, bool is_point)
{
    // Output vector
    RealVector x, scale, offset;
    VecSpec type;

    // Read the vector
    ReadVecSpec(vec_in, x, type);

    // Check the type of the vector
    if (type != VOXELS && type != PHYSICAL && type != PERCENT)
        std::cerr << "Invalid vector spec " << vec_in << " (must end with 'mm' or 'vox' or '%' )" << std::endl;

    // If in percent, scale by voxel size
    if (type == PERCENT)
    {
        for (size_t i = 0; i < VDim; i++)
            x[i] *= img->GetBufferedRegion().GetSize()[i] / 100.0;
        type = VOXELS;
    }

    // If the vector is in vox units, map it to physical units
    if (type == VOXELS)
    {
        // Get the matrix
        TransformMatrixType MP = GetVoxelSpaceToRASPhysicalSpaceMatrix(img);

        // Create the vector to multiply by
        vnl_vector_fixed<double, VDim + 1> X, XP;
        for (size_t d = 0; d < VDim; d++)
            X[d] = x[d];
        X[VDim] = is_point ? 1.0 : 0.0;

        // Apply matrix
        XP = MP * X;
        for (size_t d = 0; d < VDim; d++)
            x[d] = XP[d];
    }

    return x;
}

void ReadVecSpec(const char* vec_in, vnl_vector_fixed<double, VDim>& vout, VecSpec& type)
{
    // Set up the regular expressions for numerical string parsing
    itksys::RegularExpression re1(
        "([-+]?[0-9]*\\.?[0-9]+([eE][-+]?[0-9]+)?)(mm|vox|%)?");
    itksys::RegularExpression re2(
        "([-+]?[0-9]*\\.?[0-9]+([eE][-+]?[0-9]+)?)x"
        "([-+]?[0-9]*\\.?[0-9]+([eE][-+]?[0-9]+)?)(mm|vox|%)?");
    itksys::RegularExpression re3(
        "([-+]?[0-9]*\\.?[0-9]+([eE][-+]?[0-9]+)?)x"
        "([-+]?[0-9]*\\.?[0-9]+([eE][-+]?[0-9]+)?)x"
        "([-+]?[0-9]*\\.?[0-9]+([eE][-+]?[0-9]+)?)(mm|vox|%)?");

    // Lowercase string
    std::string vec = str_to_lower(vec_in);
    std::string spec;

    // Check if it's a single-component specification
    if (VDim == 2 && re2.find(vec))
    {
        vout[0] = atof(re2.match(1).c_str());
        vout[1] = atof(re2.match(3).c_str());
        spec = re2.match(5);
    }
    else if (VDim == 3 && re3.find(vec))
    {
        vout[0] = atof(re3.match(1).c_str());
        vout[1] = atof(re3.match(3).c_str());
        vout[2] = atof(re3.match(5).c_str());
        spec = re3.match(7);
    }
    else if (re1.find(vec))
    {
        vout.fill(atof(re1.match(1).c_str()));
        spec = re1.match(3);
    }
    else std::cerr << "Invalid vector specification " << vec_in << std::endl;

    // Get the type of spec. Luckily, all suffixes have the same length
    switch (spec.length()) {
    case 0: type = NONE; break;
    case 1: type = PERCENT; break;
    case 2: type = PHYSICAL; break;
    case 3: type = VOXELS; break;
    default: std::cerr << "Internal error in VecSpec code" << std::endl;
    }
}

ImagePointer SetOriginVoxel(ImagePointer img, const char* vec_in)
{
    // Read the physical RAS coordinate of the voxel that should be made the origin
    RealVector vec = ReadRealVector(img, vec_in, false);

    // This voxel should have the coordinate zero
    for (int i = 0; i < VDim; i++)
    {
        // Here we are making the RAS/LPS switch and inverting the coordinate
        vec[i] = (i >= 2) ? -vec[i] : vec[i];
    }

    // Get physical coordinate of this voxel
    img->SetOrigin(vec.data_block());
    try {
        img->Update();
    }
    catch (itk::ExceptionObject & error) {
        std::cerr << "Error: " << error << std::endl;
    }
    return img;
}
