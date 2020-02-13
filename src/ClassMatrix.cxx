#include "ClassMatrix.h"

// This class is derived from itk::Matrix, and is used for single axis rotations. 

std::ostream& operator<<(std::ostream& out, const RotationAxis value) {
	static std::map<RotationAxis, std::string> strings;
	strings[RotationAxis::XROTATION] = "X";
	strings[RotationAxis::YROTATION] = "Y";
	strings[RotationAxis::ZROTATION] = "Z";
	strings[RotationAxis::ERRORTYPE] = "ERRORTYPE";

	return out << strings[value];
}

//Get the angle from the matrix, knowing the axis of rotation
const double MatrixType::GetAngle() {

	MatrixType mat = *this;

	if (this->rotationAxis == RotationAxis::ERRORTYPE) {
		std::cerr << "Axis of rotation needs to be specified." << std::endl;
	}
	switch (this->rotationAxis)
	{
	case RotationAxis::XROTATION:
		this->angle = asin(mat[2][1]); break;
	case RotationAxis::YROTATION:
		this->angle = asin(mat[0][2]); break;
	case RotationAxis::ZROTATION:
		this->angle = asin(mat[1][0]); break;
	default:
		this->angle = 0.0; break;
	}
	return this->angle;
};

//Return the last column of the transformation matrix
double* MatrixType::GetTranslation() {

	MatrixType mat = *this;

	double t[3];
	for (int i = 0;i < 3;i++) {
		t[i] = mat[i][3];
	}
	return t;
};

const RotationAxis MatrixType::GetRotationAxis() {
	return this->rotationAxis;
};

//Find the (only) axis of rotation from the matrix
void MatrixType::SetRotationAxis()
{
	itk::Matrix<double, 4, 4> mat = *this;

	if ((abs(mat[0][0] - 0.999) < 0.01) && (abs(mat[1][0]) < pow(10,-3)) && (abs(mat[2][0]) < pow(10,-3)) && (abs(mat[0][1]) < pow(10,-3)) && (abs(mat[0][2]) < pow(10,-3))) {
		this->rotationAxis = RotationAxis::XROTATION;
	}
	else if ((abs(mat[1][1] - 0.999) < 0.01) && (abs(mat[1][0]) < pow(10,-3)) && (abs(mat[0][1]) < pow(10,-3)) && (abs(mat[2][1]) < pow(10,-3)) && (abs(mat[1][2]) < pow(10,-3))) {
		this->rotationAxis = RotationAxis::YROTATION;
	}
	else if ((abs(mat[2][2] - 0.999) < 0.01) && (abs(mat[2][0]) < pow(10,-3)) && (abs(mat[2][1]) < pow(10,-3)) && (abs(mat[0][2]) < pow(10,-3)) && (abs(mat[1][2]) < pow(10,-3))) {
		this->rotationAxis = RotationAxis::ZROTATION;
	}
}

void MatrixType::SetRotationAxis(RotationAxis axis)
{
	this->rotationAxis = axis;
}

//Modify the transformation matrix with the new values
void MatrixType::SetParameters(const RotationAxis axis, const double angle_value) {
	itk::Matrix<double, 4, 4> mat = *this;

	switch (axis) {
	case RotationAxis::XROTATION:
		*this[0][0] = 1;	*this[0][1] = 0; 					*this[0][2] = 0; 					*this[0][3] = 0;
		*this[1][0] = 0; 	*this[1][1] = cos(angle_value); 	*this[1][2] = -sin(angle_value);	*this[1][3] = 0;
		*this[2][0] = 0; 	*this[2][1] = sin(angle_value);		*this[2][2] = cos(angle_value); 	*this[2][3] = 0;
		*this[3][0] = 0; 	*this[3][1] = 0; 					*this[3][2] = 0; 					*this[3][3] = 1;
		break;
	case RotationAxis::YROTATION:
		*this[0][0] = cos(angle_value);		*this[0][1] = 0; *this[0][2] = sin(angle_value); 	*this[0][3] = 0;
		*this[1][0] = 0; 			 		*this[1][1] = 1; *this[1][2] = 0;					*this[1][3] = 0;
		*this[2][0] = -sin(angle_value); 	*this[2][1] = 0; *this[2][2] = cos(angle_value); 	*this[2][3] = 0;
		*this[3][0] = 0; 			 		*this[3][1] = 0; *this[3][2] = 0; 					*this[3][3] = 1;
		break;
	case RotationAxis::ZROTATION:
		*this[0][0] = cos(angle_value);		*this[0][1] = -sin(angle_value); 	*this[0][2] = 0; *this[0][3] = 0;
		*this[1][0] = sin(angle_value);  	*this[1][1] = cos(angle_value);  	*this[1][2] = 0; *this[1][3] = 0;
		*this[2][0] = 0; 			 		*this[2][1] = 0; 		  			*this[2][2] = 1; *this[2][3] = 0;
		*this[3][0] = 0; 			 		*this[3][1] = 0;			  		*this[3][2] = 0; *this[3][3] = 1;
		break;
	default:
		std::cerr << "Invalid rotation axis." << std::endl;
		break;
	}

	this->angle = angle_value;
	this->rotationAxis = axis;
}

void MatrixType::SetParameters(const RotationAxis axis, const double angle_value, double t_x, double t_y, double t_z) {
	MatrixType mat;
	mat.angle = angle_value;
	mat.rotationAxis = axis;
	switch (axis) {
	case RotationAxis::XROTATION:
		mat[0][0] = 1; mat[0][1] = 0; 				 mat[0][2] = 0; 					mat[0][3] = t_x;
		mat[1][0] = 0; mat[1][1] = cos(angle_value); mat[1][2] = -sin(angle_value);	mat[1][3] = -t_y;
		mat[2][0] = 0; mat[2][1] = sin(angle_value); mat[2][2] = cos(angle_value); 	mat[2][3] = t_z;
		mat[3][0] = 0; mat[3][1] = 0; 				 mat[3][2] = 0; 					mat[3][3] = 1;
		break;
	case RotationAxis::YROTATION:
		mat[0][0] = cos(angle_value);	mat[0][1] = 0; mat[0][2] = sin(angle_value); 	mat[0][3] = t_x;
		mat[1][0] = 0; 			 		mat[1][1] = 1; mat[1][2] = 0;					mat[1][3] = t_y;
		mat[2][0] = -sin(angle_value);	mat[2][1] = 0; mat[2][2] = cos(angle_value); 	mat[2][3] = t_z;
		mat[3][0] = 0; 			 		mat[3][1] = 0; mat[3][2] = 0; 					mat[3][3] = 1;
		break;
	case RotationAxis::ZROTATION:
		mat[0][0] = cos(angle_value);	mat[0][1] = -sin(angle_value); 	mat[0][2] = 0; mat[0][3] = t_x;
		mat[1][0] = sin(angle_value); 	mat[1][1] = cos(angle_value);  	mat[1][2] = 0; mat[1][3] = t_y;
		mat[2][0] = 0; 			 		mat[2][1] = 0; 		  			mat[2][2] = 1; mat[2][3] = t_z;
		mat[3][0] = 0; 			 		mat[3][1] = 0;			  		mat[3][2] = 0; mat[3][3] = 1;
		break;
	default:
		std::cerr << "Invalid rotation axis." << std::endl;
		break;
	}
	*this = mat;
	this->angle = angle_value;
	this->rotationAxis = axis;
}