#include <cmath>
#include <cstdio>
#include <cstring>

#include "predicates.h"
#include "gogui/libraries/cpp/gogui.hpp"

using namespace std;

FILE* file;


int classify(double val) {
	
	if(val < 0.0) {
        return  0;
	} else if (val == 0.0) {
        return  1;
    } else
        return 2;
}

double manual_det1(double* a, double* b, double* c) {
	return a[0]*b[1] + b[0]*c[1] + c[0]*a[1] - b[1]*c[0] - c[1]*a[0] - a[1]*b[0];
}

double manual_det2(double* a, double* b, double* c) {
	return (a[0] - c[0])*(b[1]-c[1]) - (a[1]-c[1])*(b[0]-c[0]);
}

void load_dataset(gogui::vector<gogui::Point>* cloud, const char* filename) {
	file = fopen(filename, "r");
	if(file != NULL) {
		while(!eof) {
			
			double point[2];
			fscanf(file, "%lf %lf", &point[0], &point[1]);
			
			gogui::Point p(point[0],point[1]);
			cloud->push_back(p);
            
		}
		fclose(file);
	}
}

int main() {

	gogui::vector<gogui::Point> cloud;
	load_dataset(&cloud, "a.dat");
	
	
	return 0;
}
