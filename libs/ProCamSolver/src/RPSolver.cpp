#include "ProCamSolver/RPSolver.h"

using namespace nrlib;

namespace ProCamSolver {
	///////////////////////////////////////////////////////////////////////////////
	//
	// Default place to put the residual of the fit after solving
	//
	///////////////////////////////////////////////////////////////////////////////
	double RPSolver::residual = 0.0;

	/////////////////////////////////////////////////////////////////////////////
	// set this to false to fix radial distortion of all views to 0.0
	/////////////////////////////////////////////////////////////////////////////
	bool RPSolver::radial = true;

	///////////////////////////////////////////////////////////////////////////////
	//
	// cor 			- pixel correlations
	// r			- contains residual of fit on return
	//
	///////////////////////////////////////////////////////////////////////////////
	RPSolver::RPSolver(std::vector<Correspondence> &cor, double &r) {
	  C = cor;
	  init_views();
	  r = solve();
	}


	///////////////////////////////////////////////////////////////////////////////
	//
	// cor 			- pixel correlations
	// fixedIntrinsics	- prior known intrinsic matrices for certain views
	// r			- contains residual of fit on return
	//
	///////////////////////////////////////////////////////////////////////////////
	RPSolver::RPSolver(std::vector<Correspondence> &cor,
			 std::map<int, transformMatrix> &fixedIntrinsics,
			 double &r) {
	  init_intrinsics(fixedIntrinsics);
	  C = cor;
	  init_views();
	  r = solve();
	}


	///////////////////////////////////////////////////////////////////////////////
	//
	// cor 			- pixel correlations
	// initials 		- should contain initial guesses at translation and
	//                        rotation of some (not necessarily all) views
	// 			  and any prior known intrinsic matrices.
	//			  If fx is non-zero in a view then the intrinsics
	//			  for that view are held fixed.
	// r			- contains residual of fit on return
	//
	///////////////////////////////////////////////////////////////////////////////
	RPSolver::RPSolver(std::vector<Correspondence> &cor,
			 std::map<int, RadialViewProjectionMatrix> &initials,
			 double &r) {
	  init_VPM(initials);
	  C = cor;
	  init_views();
	  r = solve();
	}



	///////////////////////////////////////////////////////////////////////////////
	//
	// initialises the intrinsic matrices of *this and marks them as fixed
	//
	///////////////////////////////////////////////////////////////////////////////
	void RPSolver::init_intrinsics(std::map<int, transformMatrix> &initials) {
	  std::map<int, transformMatrix>::iterator it;
	  coord	t_guess(3);
	  t_guess[0] = 0.011; t_guess[1] = 0.012; t_guess[2] = 0.013; 
  
	  for(it = initials.begin(); it != initials.end(); ++it) {
		(*this)[it->first].fx = it->second[0][0];
		(*this)[it->first].fy = it->second[1][1];
		(*this)[it->first].cx = it->second[0][2];
		(*this)[it->first].cy = it->second[1][2];
		(*this)[it->first].t  = t_guess * (double)it->first;
		paramMask.insert(it->first);
	  }
	}


	///////////////////////////////////////////////////////////////////////////////
	//
	// initialises the view-projection matrices of this to allow initial guesses
	// at the correct values. intrinsic matrices are taken to be fixed if fx
	// is not equal to zero, otherwise they are variable.
	//
	///////////////////////////////////////////////////////////////////////////////
	void RPSolver::init_VPM(std::map<int, RadialViewProjectionMatrix> &initials) {
	  std::map<int, RadialViewProjectionMatrix>::iterator it;

	  for(it = initials.begin(); it != initials.end(); ++it) {
		insert(*it);
		if(vpmat(it).fx != 0.0) paramMask.insert(it->first);
	  }
	}


	///////////////////////////////////////////////////////////////////////////////
	//
	// Initialise the map from view IDs to positions in the parameter vector
	//
	///////////////////////////////////////////////////////////////////////////////
	void RPSolver::init_views() {
	  iterator 	view;
	  int		i;
	  coord		t_guess(3);

	  // --- create views for all correspondences
	  t_guess[0] = 0.011; t_guess[1] = 0.012; t_guess[2] = 0.013; 
	  for(i = 0; i< C.size(); ++i) {
		if(find(C[i].i) == end()) {
		  // --- initial guess at translation
		  (*this)[C[i].i].t = t_guess*(double)C[i].i;
		}
		if(find(C[i].j) == end()) {
		  (*this)[C[i].j].t = t_guess*(double)C[i].j;
		}
	  }

	  i = 0;
	  for(view = begin(); view != end(); ++view) {
		paramBase[id(view)] = i;
		if(id(view) != 0) {
		  // view 0 has fixed r and t by definition
		  i += 6;
		}
		if(has_variable_M(id(view))) {
		  // intrinsic matrix is not fixed
		  vpmat(view).fx = 1.0;
		  i += 4;
		}
		++i;
	  }
	  pn = i;
	}


	///////////////////////////////////////////////////////////////////////////////
	//
	// Find the camera matrices which minimise the sum of squared errors
	//
	///////////////////////////////////////////////////////////////////////////////
	double RPSolver::solve() {
	  Frprmn<RPSolver>	mySolver(*this, 1e-5);
	  VecDoub		p(pn);
	  int 			n,b;
	  iterator		view;
	  double		r;

	  vpmatrices_to_params(p);
	  p = mySolver.minimize(p);
	  params_to_vpmatrices(p);
	  r = (*this)(p);

	  // --- re-scale the translations
	  // --- distance view1->view 2 = 1.0
	  // --------------------------------
	  double scale = 1.0/sqrt(((*this)[1].t * (*this)[1].t).sum());
	  for(view = begin(); view != end(); ++view) {
		vpmat(view).t = vpmat(view).t * scale;
	  }

	  return(r);
	}


	///////////////////////////////////////////////////////////////////////////////
	//
	// returns the error for a given set of parameters
	//
	///////////////////////////////////////////////////////////////////////////////
	double RPSolver::operator()(VecDoub_I &p) {
	  int	n;
	  int	viewpair;
	  double e2;
	  std::map<int, Matrix<double> > R;
	  coord  dt(3);
	  
	  params_to_vpmatrices(p);
	  e2 = 0.0;
	  for(n=0; n<C.size(); ++n) {
		viewpair = C[n].i*65536 + C[n].j;
		if(R.find(viewpair) == R.end()) {
		  RadialViewProjectionMatrix &	pi((*this)[C[n].i]);
		  RadialViewProjectionMatrix &	pj((*this)[C[n].j]);
		  dt = pj.t - pi.t; // normalise the translation scale
		  dt /= sqrt((dt*dt).sum());
		  R[viewpair] = 
	            pj.DT()*pj.M1T()*pj.R()*
                    transformMatrix(cross_prod,dt[0],dt[1],dt[2])
                    *pi.R1()*pi.M1()*pi.D();
                }
		e2 += correspondence_error(C[n], R[viewpair]);
	  }

	  // --- force the scale: sum of 
	  // --- translation distances = 1.0
	  // -------------------------------
	  double k;
	  iterator view;
  
	  k = 0.0;
	  for(view = begin(); view != end(); ++view) {
		k += (vpmat(view).t * vpmat(view).t).sum();
	  }
	  k -= 1.0;
	  e2 += k*k;

	  return(e2);
	}


	///////////////////////////////////////////////////////////////////////////////
	//
	// calculate derivative of error with each parameter using finite difference
	// approximation
	//
	///////////////////////////////////////////////////////////////////////////////
	void RPSolver::df(VecDoub_I &p, VecDoub_O &deriv) {
	  double e_old;
	  double e;
	  double x_old;
	  VecDoub ph = p;
	  double dx;

	  // ---- derivative by finite difference
	  // ------------------------------------
	  e_old = (*this)(p);
	  for(int i=0; i<deriv.size(); ++i) {
		x_old = p[i];
		dx = 1e-8*fabs(x_old);
		if(dx == 0.0) dx = 1e-8;
		ph[i] = x_old + dx;
		dx = ph[i] - x_old;
		e = (*this)(ph);
		ph[i] = x_old;
		deriv[i] = (e - e_old)/dx;
	  }
	}


	///////////////////////////////////////////////////////////////////////////////
	//
	// copies the parameters in p to this map of view projection matrices
	//
	///////////////////////////////////////////////////////////////////////////////
	void RPSolver::params_to_vpmatrices(VecDoub_I &p) {
	  iterator	view;
	  int 		i;

	  i = 0;
	  for(view = begin(); view != end(); ++view) {
		if(id(view) != 0) {
		  // view 0 has fixed r and t by definition
		  vpmat(view).rot[0] 	= p[i];
		  vpmat(view).rot[1] 	= p[i+1];
		  vpmat(view).rot[2] 	= p[i+2];
		  vpmat(view).t[0] 		= p[i+3];
		  vpmat(view).t[1] 		= p[i+4];
		  vpmat(view).t[2] 		= p[i+5];
		  i += 6;
		}
		if(has_variable_M(id(view))) {
		  // intrinsic matrix is not fixed
		  vpmat(view).fx 	= p[i];
		  vpmat(view).fy 	= p[i+1];
		  vpmat(view).cx 	= p[i+2];
		  vpmat(view).cy 	= p[i+3];
		  i += 4;
		}
		if(radial) vpmat(view).d = p[i];
		++i;
	  }
	}


	///////////////////////////////////////////////////////////////////////////////
	//
	// copies the parameters in this map of view projection matrices to p
	//
	///////////////////////////////////////////////////////////////////////////////
	void RPSolver::vpmatrices_to_params(VecDoub &p) {
	  iterator	view;
	  int 		i;

	  i = 0;
	  for(view = begin(); view != end(); ++view) {
		if(id(view) != 0) {
		  // view 0 has fixed r and t by definition
		  p[i]   = vpmat(view).rot[0];
		  p[i+1] = vpmat(view).rot[1];
		  p[i+2] = vpmat(view).rot[2];
		  p[i+3] = vpmat(view).t[0];
		  p[i+4] = vpmat(view).t[1];
		  p[i+5] = vpmat(view).t[2];
		  i += 6;
		}
		if(has_variable_M(id(view))) {
		  // intrinsic matrix is not fixed
		  p[i]   = vpmat(view).fx;
		  p[i+1] = vpmat(view).fy;
		  p[i+2] = vpmat(view).cx;
		  p[i+3] = vpmat(view).cy;
		  i += 4;
		}
		if(radial) p[i] = vpmat(view).d;	// radial distortion
		++i;
	  }
	}


	///////////////////////////////////////////////////////////////////////////////
	// 
	// returns the Sampson reprojection error due to correspondence, c.
	// R is the (pre-computed) radial fundamental matrix for the two
	// views of the correspondence.
	//
	///////////////////////////////////////////////////////////////////////////////
	double RPSolver::correspondence_error(Correspondence &c, Matrix<double> &R) {
	  Matrix<double>		qi(4,1);
	  Matrix<double>		qj(1,4);
	  Matrix<double>		ei(4,1);
	  Matrix<double>		ej(1,4);
	  double			E;
  
	  qi[0][0] = c.xi*c.xi + c.yi*c.yi;
	  qi[1][0] = c.xi;
	  qi[2][0] = c.yi;
	  qi[3][0] = 1.0;

	  qj[0][0] = c.xj*c.xj + c.yj*c.yj;
	  qj[0][1] = c.xj;
	  qj[0][2] = c.yj;
	  qj[0][3] = 1.0;

	  E = (qj * R * qi)[0][0];
	  ei = R*qi;
	  ej = qj*R;
	  E /= ei[0][0]*ei[0][0] + ei[1][0]*ei[1][0] + ei[2][0]*ei[2][0] 
		+ ej[0][0]*ej[0][0] + ej[0][1]*ej[0][1] + ej[0][2]*ej[0][2];
	  return(E*E);
	}


	///////////////////////////////////////////////////////////////////////////////
	//
	// returns true iff the n'th view has a variable intrinsic matrix (i.e.
	// the intrinsic matrix is not already known)
	//
	///////////////////////////////////////////////////////////////////////////////
	bool RPSolver::has_variable_M(int n) {
	  return(paramMask.find(n) == paramMask.end());
	}
}