#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "../gogui/gogui_core/include/gogui.hpp"

using namespace gogui;

void load_dataset(vector<Line>& lines, vector<Point>& points) {
	double x, y, a, b;
	while(scanf("%lf %lf %lf %lf", &x, &y, &a, &b) == 4) {
		// do NOT use feof
		Point first(a, b), second(x, y);
		points.push_back(first);
		points.push_back(second);
		lines.push_back(Line (first, second));
		
	}
}

int main() {

    vector<Line> lines;
    vector<Point> points;
    load_dataset(lines, points);
    snapshot();
    printJSON();

    return 0;
}
