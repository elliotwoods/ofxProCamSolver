///////////////////////////////////////////////////////////////////////////////
//
// Class for representing corresponding pixels from 2 views
//
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include <ostream>

namespace ProCamSolver {
	class Correspondence {
	public:
	  int	 i;	// first camera's ID
	  double xi;	// (x,y) of pixel in first view
	  double yi;
	  int	 j;	// second camera's ID
	  double xj;	// (x,y) of corresponding pixel in second view
	  double yj;
	  double w;	// weighting: unused at the moment
	};


	///////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////

	inline std::ostream &operator <<(std::ostream &out, Correspondence &c) {
	  out << c.i << " " 
		  << c.xi << " "
		  << c.yi << " "
		  << c.j << " "
		  << c.xj << " "
		  << c.yj;
	  return(out);
	}
}