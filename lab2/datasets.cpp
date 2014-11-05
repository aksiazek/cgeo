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

void make_circular_dataset(const char* filename, int a, int b, int num_points, double R, double* center)  {
	uniform_real_distribution<double> distribution(a, b);
	
	file = fopen(filename, "w");
	if(file != NULL) {
		for (int i = 0; i < num_points; ++i) {
			double t = distribution(generator);
			double point[2];
            point[0] = center[0] + R * cos(t);
            point[1] = center[1] + R * sin(t);
			
            fprintf(file, "%lf %lf \n", point[0], point[1]);
		}
		fclose(file);
	}
	puts(filename);
	puts("\n");
}

void make_rectangle_dataset(const char* filename, double* top_left, double* bottom_right, int num_points)  {
	uniform_real_distribution<double> vertical(top_left[0], bottom_right[0]);
	uniform_real_distribution<double> horizontal(top_left[1], bottom_right[1]);
	
	double point[2];
	
	file = fopen(filename, "w");
	if(file != NULL) {
		for (int i = 0; i < num_points; i+=4) {
			
			// gora
            point[0] = horizontal(generator);
            point[1] = top_left[1];
            fprintf(file, "%lf %lf \n", point[0], point[1]);
            
            // lewo
            point[0] = top_left[0];
            point[1] = vertical(generator);
            fprintf(file, "%lf %lf \n", point[0], point[1]);
            
            // prawo
            point[0] = bottom_right[0];
            point[1] = vertical(generator);
            fprintf(file, "%lf %lf \n", point[0], point[1]);
            
            // dol
            point[0] = horizontal(generator);
            point[1] = bottom_right[1];
            fprintf(file, "%lf %lf \n", point[0], point[1]);
            
		}
		fclose(file);
	}
	puts(filename);
	puts("\n");
}

void make_square_dataset(const char* filename, double side_len, int each_side, int each_diagonal)  {
	uniform_real_distribution<double> side(0, side_len);
	uniform_real_distribution<double> diagonal(0, 1);
	
	// side_len > 0, always I quater of OXY
	double point[2]; double t;
	
	file = fopen(filename, "w");
	if(file != NULL) {
		for (int i = 0; i < each_side; i++) {
			
			// OX
            point[0] = 0;
            point[1] = side(generator);
            fprintf(file, "%lf %lf \n", point[0], point[1]);
            
            // OY
            point[0] = side(generator);
            point[1] = 0;
            fprintf(file, "%lf %lf \n", point[0], point[1]);
        }
        for (int i = 0; i < each_diagonal; i++) {   
            // przekatna 1
            t = diagonal(generator);
            point[0] = 10*t;
            point[1] = point[0];
            fprintf(file, "%lf %lf \n", point[0], point[1]);
            
            // przekatna 2
            t = diagonal(generator);
            point[0] = 10 - 10*t;
            point[1] = 10*t;
            fprintf(file, "%lf %lf \n", point[0], point[1]);
            
		}
		fclose(file);
	}
	puts(filename);
	puts("\n");
}
 
int main() {

    double srodek[2]; double lewy_gorny[2];  double prawy_dolny[2];
    
    make_unstructured_dataset("a.dat", -100, 100, 100);
    
    srodek[0] = 0; srodek[1] = 0;
    make_circular_dataset("b.dat", 0, 2*M_PI, 100, 10, srodek);
    
    lewy_gorny[0] = -10; lewy_gorny[1] = 10;
    prawy_dolny[0] = 10; prawy_dolny[1] = -10;
    make_rectangle_dataset("c.dat", lewy_gorny, prawy_dolny, 100);
    
	make_square_dataset("d.dat", 10, 25, 100);

	return 0;
}
