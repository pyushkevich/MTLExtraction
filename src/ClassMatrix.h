#ifndef MatrixType_h
#define MatrixType_h

#include "itkImage.h"

// This class is derived from itk::Matrix, and is used for single axis rotations. 
enum class RotationAxis
{
	XROTATION,
	YROTATION,
	ZROTATION,
	ERRORTYPE
};

std::ostream& operator<<(std::ostream&, const RotationAxis);

class MatrixType : public itk::Matrix<double, 4, 4> {
public:
	//attributes
	double angle;
	RotationAxis rotationAxis;

	//Constructor 
	MatrixType() {
		angle = 0.0;
		rotationAxis = RotationAxis::ERRORTYPE;
	};

	//Getters
	const double GetAngle();
	double* GetTranslation();
	const RotationAxis GetRotationAxis();
	
	//Setters
	void SetParameters(const RotationAxis axis, const double value);
	void SetParameters(const RotationAxis axis, const double angle_value, double t_x, double t_y, double t_z);
	void SetRotationAxis(); //Find the axis from the matrix
	void SetRotationAxis(RotationAxis axis);

};

#endif