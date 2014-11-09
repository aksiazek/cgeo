#include <chrono>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <random>

using namespace std;

unsigned seed = chrono::system_clock::now().time_since_epoch().count();
default_random_engine generator(seed);
FILE* file;

void make_unstructured_dataset(const char* filename, long long a, long long b, int num_points) {
	uniform_real_distribution<double> distribution(a, b);
	
	file = fopen(filename, "w");
	if(file != NULL) {
		for (int i = 0; i < num_points; ++i) {
			double point[2];
			point[0] = distribution(generator);
			point[1] = distribution(generator);
			
            fprintf(file, "%lf %lf \n", point[0], point[1]);
		}
		fclose(file);
	}
	puts(filename);
	puts("\n");
}

int main() {
    
    make_unstructured_dataset("dataset", -1000, 1000, 25);

	return 0;
}
