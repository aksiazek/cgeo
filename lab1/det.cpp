#include <chrono>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <random>

#include "predicates.h"

using namespace std;

unsigned seed = chrono::system_clock::now().time_since_epoch().count();
default_random_engine generator(seed);
FILE* file;
double point_a[2];
double point_b[2];

int classify(double val, int stat[], int start) {
	int ret;
	
	if(val < 0.0) {
		ret = 0;
		stat[start]++;
	} else if (val == 0.0) {
		ret = 1;
		stat[start+1]++;
	} else {
		ret = 2;
		stat[start+2]++;
	}
	
	return ret;
}

double manual_det1(double* a, double* b, double* c) {
	return a[0]*b[1] + b[0]*c[1] + c[0]*a[1] - b[1]*c[0] - c[1]*a[0] - a[1]*b[0];
}

double manual_det2(double* a, double* b, double* c) {
	return (a[0] - c[0])*(b[1]-c[1]) - (a[1]-c[1])*(b[0]-c[0]);
}



void make_unstructured_dataset(const char* filename, long long a, long long b, int num_points) {
	uniform_real_distribution<double> distribution(a, b);

	int stat[15]; // 3 classifications times 5 methods
	for (int i = 0; i < 15; ++i) {
		stat[i] = 0;
	}
			
	int diff[5][5];
	for (int i = 0; i < 5; ++i)
		for (int j = 0; j < 5; ++j)
			diff[i][j] = 0;
	
	int classes[5];
	
	file = fopen(filename, "w");
	if(file != NULL) {
		for (int i = 0; i < num_points; ++i) {
			double point[2];
			point[0] = distribution(generator);
			point[1] = distribution(generator);
			
			classes[0] = classify(manual_det1(point_a, point_b, point), stat, 0);
			classes[1] = classify(manual_det2(point_a, point_b, point), stat, 3);
			classes[2] = classify(orient2dfast(point_a, point_b, point), stat, 6);
			classes[3] = classify(orient2dexact(point_a, point_b, point), stat, 9);
			classes[4] = classify(orient2dslow(point_a, point_b, point), stat, 12);
			for (int i = 0; i < 5; ++i)
				for (int j = 0; j < 5; ++j)
					if(classes[i] != classes[j])
						diff[i][j]++;
			
			fprintf(file, "%lf %lf %d %d %d %d %d\n", point[0], point[1],
			classes[0], classes[1], classes[2], classes[3], classes[4]);
		}
		fclose(file);
	}
	puts(filename);
	puts("\n");
	printf("manual_det1: %d %d %d\n", stat[0], stat[1], stat[2]);
	printf("manual_det2: %d %d %d\n", stat[3], stat[4], stat[5]);
	printf("orient2dfast: %d %d %d\n", stat[6], stat[7], stat[8]);
	printf("orient2dexact: %d %d %d\n", stat[9], stat[10], stat[11]);
	printf("orient2dslow: %d %d %d\n", stat[12], stat[13], stat[14]);
	
	puts("manual_det1 manual_det2 orient2dfast orient2dexact orient2dslow\n");
	for (int i = 0; i < 5; ++i) {
		for (int j = 0; j < 5; ++j)
			printf("%d ", diff[i][j]);
		puts("\n");
	}	
}

void make_linear_dataset(const char* filename, int a, int b, int num_points) {
	uniform_real_distribution<double> distribution(a, b);
	
	int stat[15]; // 3 classifications times 5 methods
	for (int i = 0; i < 15; ++i) {
		stat[i] = 0;
	}
	
	int diff[5][5];
	for (int i = 0; i < 5; ++i)
		for (int j = 0; j < 5; ++j)
			diff[i][j] = 0;
	
	int classes[5];
	
	file = fopen(filename, "w");
	if(file != NULL) {
		for (int i = 0; i < num_points; ++i) {
			double point[2];
			point[0] = distribution(generator);
			point[1] = 0.05*point[0] + 0.05;
			
			classes[0] = classify(manual_det1(point_a, point_b, point), stat, 0);
			classes[1] = classify(manual_det2(point_a, point_b, point), stat, 3);
			classes[2] = classify(orient2dfast(point_a, point_b, point), stat, 6);
			classes[3] = classify(orient2dexact(point_a, point_b, point), stat, 9);
			classes[4] = classify(orient2dslow(point_a, point_b, point), stat, 12);
			for (int i = 0; i < 5; ++i)
				for (int j = 0; j < 5; ++j)
					if(classes[i] != classes[j])
						diff[i][j]++;
			
			fprintf(file, "%lf %lf %d %d %d %d %d\n", point[0], point[1],
			classes[0], classes[1], classes[2], classes[3], classes[4]);
		}
		fclose(file);
	}
	puts(filename);
	puts("\n");
	printf("manual_det1: %d %d %d\n", stat[0], stat[1], stat[2]);
	printf("manual_det2: %d %d %d\n", stat[3], stat[4], stat[5]);
	printf("orient2dfast: %d %d %d\n", stat[6], stat[7], stat[8]);
	printf("orient2dexact: %d %d %d\n", stat[9], stat[10], stat[11]);
	printf("orient2dslow: %d %d %d\n", stat[12], stat[13], stat[14]);
	
	puts("manual_det1 manual_det2 orient2dfast orient2dexact orient2dslow\n");
	for (int i = 0; i < 5; ++i) {
		for (int j = 0; j < 5; ++j)
			printf("%d ", diff[i][j]);
		puts("\n");
	}	
}

void make_circular_dataset(const char* filename, int a, int b, int num_points)  {
	uniform_real_distribution<double> distribution(a, b);
	int R = 100;
	// (0,0) middle
	
	int stat[15]; // 3 classifications times 5 methods
	for (int i = 0; i < 15; ++i) {
		stat[i] = 0;
	}
	int diff[5][5];
	for (int i = 0; i < 5; ++i)
		for (int j = 0; j < 5; ++j)
			diff[i][j] = 0;
	
	int classes[5];
	
	file = fopen(filename, "w");
	if(file != NULL) {
		for (int i = 0; i < num_points; ++i) {
			double t = distribution(generator);
			double point[2];
			point[0] = R * cos(t);
			point[1] = R * sin(t);
			
			classes[0] = classify(manual_det1(point_a, point_b, point), stat, 0);
			classes[1] = classify(manual_det2(point_a, point_b, point), stat, 3);
			classes[2] = classify(orient2dfast(point_a, point_b, point), stat, 6);
			classes[3] = classify(orient2dexact(point_a, point_b, point), stat, 9);
			classes[4] = classify(orient2dslow(point_a, point_b, point), stat, 12);
			for (int i = 0; i < 5; ++i)
				for (int j = 0; j < 5; ++j)
					if(classes[i] != classes[j])
						diff[i][j]++;
			
			fprintf(file, "%lf %lf %d %d %d %d %d\n", point[0], point[1],
			classes[0], classes[1], classes[2], classes[3], classes[4]);
		}
		fclose(file);
	}
	puts(filename);
	puts("\n");
	printf("manual_det1: %d %d %d\n", stat[0], stat[1], stat[2]);
	printf("manual_det2: %d %d %d\n", stat[3], stat[4], stat[5]);
	printf("orient2dfast: %d %d %d\n", stat[6], stat[7], stat[8]);
	printf("orient2dexact: %d %d %d\n", stat[9], stat[10], stat[11]);
	printf("orient2dslow: %d %d %d\n", stat[12], stat[13], stat[14]);
	
	puts("manual_det1 manual_det2 orient2dfast orient2dexact orient2dslow\n");
	for (int i = 0; i < 5; ++i) {
		for (int j = 0; j < 5; ++j)
			printf("%d ", diff[i][j]);
		puts("\n");
	}	
}
 
int main() {
	point_a[0] = -1.0; point_a[1] = 0.0;
	point_b[0] = 1.0; point_b[1] = 0.1;
	
	make_unstructured_dataset("a.dat", -100, 100, 1e5);
	make_unstructured_dataset("b.dat", -1e14, 1e14, 1e5);
	make_circular_dataset("c.dat", 0, 2*M_PI, 1000);
	make_linear_dataset("d.dat", -1000, 1000, 1000);

	return 0;
}
