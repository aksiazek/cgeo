#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "../gogui/gogui_core/include/gogui.hpp"

using namespace gogui;

void load_dataset(vector<Line>& lines, vector<Point>& points, bool is_result) {
	double x, y, a, b;
	FILE* original_data = fopen("lines.dat", "rb");
	if(original_data != NULL) {
	        if(!is_result) {
	                while(fscanf(original_data, "%lf %lf %lf %lf", &x, &y, &a, &b) == 4) {
		                // do NOT use feof
		                Point first(a, b), second(x, y);
		                points.push_back(first);
		                points.push_back(second);
		                lines.push_back(Line (first, second));   
	                }
	        } else {
	                while(fscanf(original_data, "%lf %lf %lf %lf", &x, &y, &a, &b) == 4) {
		                // do NOT use feof
		                Point first(a, b), second(x, y);
		                points.push_back(first);
		                points.push_back(second);
		                lines.push_back(Line (first, second));   
	                }
	                if(scanf("%lf %lf %lf %lf", &x, &y, &a, &b) == 4) {
	                        Point first(x, a), second(x, b), third(y, a), fourth(y, b);
	                        points.push_back(first);
		                points.push_back(second);
		                points.push_back(third);
		                points.push_back(fourth);
		                first.setStatus(GeoObject::Status::Processed);
		                second.setStatus(GeoObject::Status::Processed);
		                third.setStatus(GeoObject::Status::Processed);
		                fourth.setStatus(GeoObject::Status::Processed);
		                Line one(first, second);
		                one.setStatus(GeoObject::Status::Processed);
		                lines.push_back(one);
		                Line two(second, fourth);
		                two.setStatus(GeoObject::Status::Processed);
		                lines.push_back(two);
		                Line three(first, third);
		                three.setStatus(GeoObject::Status::Processed);
		                lines.push_back(three);
		                Line four(third, fourth);
		                four.setStatus(GeoObject::Status::Processed);
		                lines.push_back(four);  
	                }
	                while(scanf("%lf %lf %lf %lf", &x, &y, &a, &b) == 4) {
		                // do NOT use feof
		                Point first(a, b), second(x, y);
		                points.push_back(first);
		                points.push_back(second);
		                Line crossing(first, second);
		                crossing.setStatus(GeoObject::Status::Processed);
		                lines.push_back(crossing);
	                }
	        }
	        fclose(original_data);
	}
}

int main(int argc, char* argv[]) {

    vector<Line> lines;
    vector<Point> points;

    if((argc > 1) && (strcmp(argv[1], "result") == 0))
        load_dataset(lines, points, true);
    else
        load_dataset(lines, points, false);
    snapshot();
    printJSON();

    return 0;
}
