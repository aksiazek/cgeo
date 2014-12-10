#include <chrono>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <random>

using namespace std;

const char* filename = "lines.dat";
double x_min = -10, x_max = 6.4, y_min = -4, y_max = 4.3;

uniform_real_distribution<double> x_distribution(x_min, x_max);
uniform_real_distribution<double> y_distribution(y_min, y_max);
unsigned seed = chrono::system_clock::now().time_since_epoch().count();
default_random_engine generator(seed);
FILE* file;

inline void make_vertical_lines(int num_lines)
{
	for (int i = 0; i < num_lines; ++i) 
	{			
	    double x = x_distribution(generator);
	    double one = y_distribution(generator);
	    double two = y_distribution(generator);
        fprintf(file, "%lf %lf %lf %lf\n", x, one, x, two);
        printf("%lf %lf %lf %lf\n", x, one, x, two);
	}
}

inline void make_horizontal_lines(int num_lines) 
{	
	for (int i = 0; i < num_lines; ++i) 
	{			
	    double y = y_distribution(generator);
	    double one = x_distribution(generator);
	    double two = x_distribution(generator);
        fprintf(file, "%lf %lf %lf %lf\n", one, y, two, y);
        printf("%lf %lf %lf %lf\n", one, y, two, y);
	}
}
 
int main() 
{
    file = fopen(filename, "w");
    if(file != NULL) {
        make_vertical_lines(10);
        make_horizontal_lines(10);
        fclose(file);
    }

    return 0;
}
