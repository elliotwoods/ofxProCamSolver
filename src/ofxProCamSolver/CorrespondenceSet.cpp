#include "CorrespondenceSet.h"

namespace ofxProCamSolver {

	//---------
	template<typename T>
	void CorrespondenceSet_<T>::save(string filename) {
		if (filename=="")
			filename = ofSystemSaveDialog("correspondences", "Save ofxProCamSolver::CorrespondenceSet").getPath();
		if (filename=="") {
			ofLogError("ofxProCamSolver") << "No file selected for save, aborting";
			return;
		}

		//count
		uint32_t count = this->size();
		ofstream file;
		file.open(filename, ios::binary);
		file.write((char*)&count, sizeof(uint32_t));

		//items
		CorrespondenceSet_<T>::iterator it;
		for (it = this->begin(); it != this->end(); it++)
			file.write((char*)&*it, sizeof(Correspondence_<T>));
		file.close();
	}

	
	//---------
	template<typename T>
	void CorrespondenceSet_<T>::load(string filename) {
		if (filename=="")
			filename = ofSystemLoadDialog("Load ofxProCamSolver::CorrespondenceSet").getPath();
		if (filename=="") {
			ofLogError("ofxProCamSolver") << "No file selected for load, aborting";
			return;
		}
		
		//count
		uint32_t count;
		ifstream file;
		file.open(filename, ios::binary);
		file.read((char*)&count, sizeof(uint32_t));
		this->resize(count);

		//items
		CorrespondenceSet_<T>::iterator it;
		for (it = this->begin(); it != this->end(); it++)
			file.read((char*)&(*it), sizeof(Correspondence_<T>));
	}

	//---------
	template<typename T>
	set<int> CorrespondenceSet_<T>::getViewIndices() const {
		set<int> indices;
		CorrespondenceSet_<T>::const_iterator it;
		for (it = this->begin(); it != this->end(); it++) {
			if (indices.count(it->cameraID1) == 0)
				indices.insert(it->cameraID1);
			if (indices.count(it->cameraID2) == 0)
				indices.insert(it->cameraID2);
		}

		return indices;
	}

	template class CorrespondenceSet_<double>;
	template class CorrespondenceSet_<float>;
}