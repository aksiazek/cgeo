#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <algorithm>
#include <iterator>
#include <utility>
#include <stack>

#include "../gogui/gogui_core/include/gogui.hpp"

using namespace std;

struct point {
		double x, y;
};

gogui::Point first_element(0,0);

double manual_det1(gogui::Point a, gogui::Point b, gogui::Point c) {
	return a.x*b.y + b.x*c.y + c.x*a.y - b.y*c.x - c.y*a.x - a.y*b.x;
}

double ccw(gogui::Point p1, gogui::Point p2, gogui::Point p3) {
    return (p1.x - p3.x)*(p2.y - p3.y) - (p1.y - p3.y)*(p2.x - p3.x);
}

bool order_by_angle(gogui::Point b, gogui::Point c) {
	return ccw(first_element, b, c) > 0;
}

void load_dataset(gogui::vector<gogui::Point>& cloud, const char* filename) {
	FILE* file = fopen(filename, "r");
	if(file != NULL) {
		point p;
		while(!feof(file)) {
			fscanf(file, "%lf %lf", &p.x, &p.y);
			cloud.push_back(gogui::Point (p.x, p.y));
		}
		fclose(file);
	}
	gogui::snapshot();
}

bool order_by_y_x(gogui::Point a, gogui::Point b) { 
	if (a.y == b.y)
		return a.x < b.x;
	else
		return a.y < b.y;
}

void graham_hull(gogui::vector<gogui::Point>& cloud) {
	gogui::vector<gogui::Point>::size_type N = cloud.size();
	gogui::vector<gogui::Point>::iterator first_element_y = min_element(cloud.begin(), cloud.end(), order_by_y_x);
	swap(*cloud.begin(), *first_element_y);
	first_element = *cloud.begin();
	
	//printf("%lf %lf\n\n", cloud[0].x, cloud[0].y);	
	sort(cloud.begin()+1, cloud.end(), order_by_angle);
	
	/*for (unsigned i = 0; i < N; i++) 
		printf("%lf %lf\n", cloud[i].x, cloud[i].y);*/
	
	gogui::vector<gogui::Line> lines;
	gogui::Line line1(cloud[0], cloud[1]);
	lines.push_back(line1);
	gogui::snapshot();
	
	unsigned i = 2, l_ind = 0;
	while (i < N) {
		double side = ccw(lines[l_ind].point1, lines[l_ind].point2, cloud[i]); 
		if (side > 0) {
			gogui::Line line(lines[l_ind].point2, cloud[i]);
			lines.push_back(line);
			l_ind++;
			gogui::snapshot();
			i++;
		} else if (side == 0) {
			double dist_now = lines[l_ind].point2.distance(lines[l_ind].point1);
			double dist_new = cloud[i].distance(lines[l_ind].point1);
			
			if(dist_now < dist_new) {
				gogui::Point prev = lines[l_ind].point1;
				lines.pop_back();
				gogui::snapshot();
				gogui::Line line(prev, cloud[i]);
				lines.push_back(line);
				gogui::snapshot();
			}
			i++;
		} else {
			lines.pop_back();
			l_ind--;
			gogui::snapshot();
		}
	}
	
	gogui::Line closing_line(lines[l_ind].point2, first_element);
    lines.push_back(closing_line);
	gogui::snapshot();
	gogui::printJSON();
	//printf("hull size: %d\n", lines.size());
}

void jarvis_hull(gogui::vector<gogui::Point>& cloud) {
	
	stack<gogui::Point> hull;
	
	gogui::vector<gogui::Point>::size_type N = cloud.size();
	gogui::vector<gogui::Point>::iterator first_element_y = min_element(cloud.begin(), cloud.end(), order_by_y_x);
	
	gogui::vector<gogui::Point>::iterator i = first_element_y;
	
	gogui::vector<gogui::Line> lines;	
	hull.push(*i);
	//printf("push y_min elem %lf %lf\n", i->x, i->y);
	
	do {
		bool any = false;
		gogui::vector<gogui::Point>::iterator k;
		
		for(gogui::vector<gogui::Point>::iterator j = cloud.begin(); j < cloud.end(); j++) {
			if(j == i) continue;
			if(!any) {
				k = j;
				//printf("first kandidate %lf %lf\n", k->x, k->y);
				gogui::Line line(*i, *k);
				lines.push_back(line);
				gogui::snapshot();
				any = true;
				continue;
			}
			
			double direction = ccw(hull.top(), *k, *j);
			if (direction > 0) {
				k = j;
				lines.pop_back();
				gogui::Line line(*i, *k);
				lines.push_back(line);
				gogui::snapshot();
				//printf("new kandidate %lf %lf\n", k->x, k->y);
			} else if (direction == 0) {
				double dist_now = k->distance(hull.top());
				double dist_new = j->distance(hull.top());
				if(dist_now < dist_new) {	
					k = j;
					lines.pop_back();
					gogui::Line line(*i, *k);
					lines.push_back(line);
					gogui::snapshot();
				}
			}
		}
		hull.push(*k);
		//printf("push kandidate %lf %lf\n", k->x, k->y);
		i = k;
		
	} while (i != first_element_y);
	
	gogui::printJSON();
	//printf("hull size: %d\n", lines.size());
}

int main(int argc, char* argv[]) {

	if(argc != 3) {
			printf("usage: %s [dataset] [jarvis|graham]\n", argv[0]);
			exit(-1);
	}
	gogui::vector<gogui::Point> cloud;
	load_dataset(cloud, argv[1]);
	
	if(strcmp(argv[2], "jarvis") == 0)
		jarvis_hull(cloud);
	else if(strcmp(argv[2], "graham") == 0)
		graham_hull(cloud);
	
	return 0;
}
