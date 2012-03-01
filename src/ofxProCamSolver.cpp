#include "ofxProCamSolver.h"

namespace ofxProCamSolver {
	//-----------
	float Solver::solve(const CorrespondenceSetf &correspondences) {
		//use the constructor to copy to doubles
		CorrespondenceSetd correspondencesd(correspondences);
		return this->solve(correspondencesd);
	}

	//-----------
	float Solver::solve(const CorrespondenceSetd &correspondences) {

		assert(sizeof(ofxProCamSolver::Correspondenced)==sizeof(ProCamSolver::Correspondence));
		vector<ProCamSolver::Correspondence> &imageData(*(vector<ProCamSolver::Correspondence>*)&correspondences);

		double residual;

		ofLogNotice("ofxProCamSolver") << "solving with " << imageData.size() << " correspondences";
		clock_t start = clock();
		ProCamSolver::RPSolver solver(imageData, residual);
		clock_t end = clock();
		ofLogNotice("ofxProCamSolver") << "Complete. Calculation took " << (float(end - start) / float(CLOCKS_PER_SEC));

		// --- Print out the results of the fit
		// ------------------------------------
		std::cout << "residual = " << residual << std::endl;
		ProCamSolver::RPSolver::iterator it;
		for(it = solver.begin(); it != solver.end(); it++) {
			std::cout << "Camera " << it->first << std::endl;
			std::cout << "--------" << std::endl;
			std::cout << it->second;
			std::cout << std::endl;
		}
		return (float)residual;
	}
}
