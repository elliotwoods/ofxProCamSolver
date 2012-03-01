///////////////////////////////////////////////////////////////////////////////
#pragma once

#include <ostream>
#include "ProCamSolver/transformMatrix.h"

namespace ProCamSolver {
	class ViewProjectionMatrix {
	public:
	  ViewProjectionMatrix();

	  Matrix<double>	R();	// rotation matrix
	  Matrix<double>	R1();	// inverse rotation matrix
	  Matrix<double>	M();	// projection matrix
	  Matrix<double>	M1();	// inverse projection matrix
	  Matrix<double>	M1T();	// transpose inverse projection matrix
	  Matrix<double>	T();	// translation cross-product

	  Matrix<double>	dR_drx(); // derivatives
	  Matrix<double>	dR_dry();
	  Matrix<double>	dR_drz();
	  Matrix<double>	dR1_drx();
	  Matrix<double>	dR1_dry();
	  Matrix<double>	dR1_drz();
	  Matrix<double>	dM1_dfx();
	  Matrix<double>	dM1_dfy();
	  Matrix<double>	dM1_dcx();
	  Matrix<double>	dM1_dcy();
	  Matrix<double>	dT_dtx();
	  Matrix<double>	dT_dty();
	  Matrix<double>	dT_dtz();

	  operator Matrix<double>();	// cast to the view-projection matrix
	  //  ViewProjectionMatrix & operator =(const Matrix<double> &);

	  coord		rot;	// rotation vector
	  coord		t;	// translation vector
	  double	cx;	// origin of sensor
	  double	cy;
	  double	fx;	// x-focal length
	  double	fy;	// y-focal length

	};




	///////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////
	inline ViewProjectionMatrix::ViewProjectionMatrix() : 
	  rot(3), 
	  t(3), 
	  fx(0.0),
	  fy(1.0),
	  cx(0.0),
	  cy(0.0)
	{
	  rot[0] = 0.0;
	  rot[1] = 0.0;
	  rot[2] = 0.0;
	  t[0] = 0.0;
	  t[1] = 0.0;
	  t[2] = 0.0;
	}


	///////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////
	//inline ViewProjectionMatrix &
	//ViewProjectionMatrix::operator =(const Matrix<double> &P) {
	//  Matrix<double>::operator=(P);
	//  return(*this);
	//}


	///////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////
	inline Matrix<double> ViewProjectionMatrix::R() {
	  return(transformMatrix(xrotate,rot[0]) *
		 transformMatrix(yrotate,rot[1]) *
		 transformMatrix(zrotate,rot[2]));

	}


	///////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////
	inline Matrix<double> ViewProjectionMatrix::R1() {
	  return(transformMatrix(zrotate,-rot[2]) *
		 transformMatrix(yrotate,-rot[1]) *
		 transformMatrix(xrotate,-rot[0]));
	}


	///////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////
	inline Matrix<double> ViewProjectionMatrix::M() {
	  return(transformMatrix(intrinsic,fx,fy,cx,cy));
	}


	///////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////
	inline Matrix<double> ViewProjectionMatrix::M1() {
	  return(transformMatrix(inverse_intrinsic,fx,fy,cx,cy));
	}


	///////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////
	inline Matrix<double> ViewProjectionMatrix::M1T() {
	  return(transformMatrix(transpose_inverse_intrinsic,fx,fy,cx,cy));
	}


	///////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////
	inline Matrix<double> ViewProjectionMatrix::T() {
	  return(transformMatrix(cross_prod,t[0],t[1],t[2]));
	}


	///////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////
	inline Matrix<double> ViewProjectionMatrix::dR_drx() {
	  const double Pi = 3.1415926535897932;
	  return(transformMatrix(xrotate,rot[0] + Pi/2.0) *
		 transformMatrix(yrotate,rot[1]) *
		 transformMatrix(zrotate,rot[2]));
	}


	///////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////
	inline Matrix<double> ViewProjectionMatrix::dR_dry() {
	  const double Pi = 3.1415926535897932;
	  return(transformMatrix(xrotate,rot[0]) *
		 transformMatrix(yrotate,rot[1] + Pi/2.0) *
		 transformMatrix(zrotate,rot[2]));
	}


	///////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////
	inline Matrix<double> ViewProjectionMatrix::dR_drz() {
	  const double Pi = 3.1415926535897932;
	  return(transformMatrix(xrotate,rot[0]) *
		 transformMatrix(yrotate,rot[1]) *
		 transformMatrix(zrotate,rot[2] + Pi/2.0));
	}


	///////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////
	inline Matrix<double> ViewProjectionMatrix::dR1_drx() {
	  const double Pi = 3.1415926535897932;
	  return(transformMatrix(zrotate,-rot[2]) *
		 transformMatrix(yrotate,-rot[1]) *
		 transformMatrix(xrotate,-rot[0] - Pi/2.0));
	}


	///////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////
	inline Matrix<double> ViewProjectionMatrix::dR1_dry() {
	  const double Pi = 3.1415926535897932;
	  return(transformMatrix(zrotate,-rot[2]) *
		 transformMatrix(yrotate,-rot[1] - Pi/2.0) *
		 transformMatrix(xrotate,-rot[0]));
	}


	///////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////
	inline Matrix<double> ViewProjectionMatrix::dR1_drz() {
	  const double Pi = 3.1415926535897932;
	  return(transformMatrix(zrotate,-rot[2] - Pi/2.0) *
		 transformMatrix(yrotate,-rot[1]) *
		 transformMatrix(xrotate,-rot[0]));
	}


	///////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////
	inline Matrix<double> ViewProjectionMatrix::dM1_dfx() {
	  Matrix<double> result(3,3);
	  result = 0.0;
	  result[0][0] = -1.0/(fx*fx);
	  return(result);
	}


	///////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////
	inline Matrix<double> ViewProjectionMatrix::dM1_dfy() {
	  Matrix<double> result(3,3);
	  result = 0.0;
	  result[1][1] = -1.0/(fy*fy);
	  return(result);
	}


	///////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////
	inline Matrix<double> ViewProjectionMatrix::dM1_dcx() {
	  Matrix<double> result(3,3);
	  result = 0.0;
	  result[0][2] = -1.0/fx;
	  return(result);
	}


	///////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////
	inline Matrix<double> ViewProjectionMatrix::dM1_dcy() {
	  Matrix<double> result(3,3);
	  result = 0.0;
	  result[1][2] = -1.0/fy;
	  return(result);
	}


	///////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////
	inline Matrix<double> ViewProjectionMatrix::dT_dtx() {
	  Matrix<double> result(3,3);
	  result = 0.0;
	  result[1][2] = -1.0;
	  result[2][1] = 1.0;
	  return(result);
	}


	///////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////
	inline Matrix<double> ViewProjectionMatrix::dT_dty() {
	  Matrix<double> result(3,3);
	  result = 0.0;
	  result[0][2] = 1.0;
	  result[2][0] = -1.0;
	  return(result);
	}


	///////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////
	inline Matrix<double> ViewProjectionMatrix::dT_dtz() {
	  Matrix<double> result(3,3);
	  result = 0.0;
	  result[0][1] = -1.0;
	  result[1][0] = 1.0;
	  return(result);
	}


	///////////////////////////////////////////////////////////////////////////////
	//
	// casts to a matrix containing the view-projection matrix
	//
	///////////////////////////////////////////////////////////////////////////////
	inline ViewProjectionMatrix::operator Matrix<double>() {
	  transformMatrix	I(identity_matrix);
	  return(M() * R() * (I | t));
	}


	///////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////
	inline std::ostream &operator <<(std::ostream &out, ViewProjectionMatrix &v) {
	  out << "rotation = " 
		  << v.rot[0] << ", " << v.rot[1] << ", " << v.rot[2] << std::endl;
	  out << "translation = "
		  << v.t[0] << ", " << v.t[1] << ", " << v.t[2] << std::endl;
	  out << "intrinsics = " 
		  << v.fx << ", " << v.fy << " | " << v.cx << ", " << v.cy << std::endl;
	  return(out);
	}
}