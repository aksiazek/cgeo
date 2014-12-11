#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "../gogui/gogui_core/include/gogui.hpp"

using namespace gogui;

void load_dataset(vector<Line>& lines, vector<Point>& points, bool is_result) {
	double x, y, a, b;
	
	if(!is_result) {
	        while(scanf("%lf %lf %lf %lf", &x, &y, &a, &b) == 4) {
		        // do NOT use feof
		        Point first(a, b), second(x, y);
		        points.push_back(first);
		        points.push_back(second);
		        lines.push_back(Line (first, second));
		        //first.setStatus(GeoObject::Status::Processed);
		        
	        }
	} else {
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
		        lines.push_back(one);
		        one.setStatus(GeoObject::Status::Processed);
		        Line two(second, fourth);
		        lines.push_back(two);
		        two.setStatus(GeoObject::Status::Processed);
		        Line three(first, third);
		        lines.push_back(three);
		        three.setStatus(GeoObject::Status::Processed);
		        Line four(third, fourth);
		        lines.push_back(four);
		        four.setStatus(GeoObject::Status::Processed);
	        }
	
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
