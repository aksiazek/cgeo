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

point first_element;

double manual_det1(point a, point b, point c) {
	return a.x*b.y + b.x*c.y + c.x*a.y - b.y*c.x - c.y*a.x - a.y*b.x;
}

double ccw(point p1, point p2, point p3) {
    return (p1.x - p3.x)*(p2.y - p3.y) - (p1.y - p3.y)*(p2.x - p3.x);
}

bool order_by_angle(point b, point c) {
	return ((first_element.x - c.x)*(b.y-c.y) - (first_element.y-c.y)*(b.x-c.x)) < 0;
}

void load_dataset(vector<point>& cloud, const char* filename) {
	FILE* file = fopen(filename, "r");
	if(file != NULL) {
		point p;
		while(!feof(file)) {
			fscanf(file, "%lf %lf", &p.x, &p.y);
			cloud.push_back(p);
		}
		fclose(file);
	}
}

bool order_by_y_x(point a, point b) { 
	if (a.y != b.y)
		return a.y < b.y; 
	else
		return a.x < b.x; 
}

void graham_hull(vector<point>& cloud) {
	vector<point>::size_type N = cloud.size();
	vector<point>::iterator first_element_y = min_element(cloud.begin(), cloud.end(), order_by_y_x);
	swap(*cloud.begin(), *first_element_y);
	first_element = *cloud.begin();
	
	//printf("%lf\n", first_element.y);
	
	sort(cloud.begin()+1, cloud.end(), order_by_angle);
	
	/*for (unsigned i = 0; i < N; i++) 
		printf("%lf %lf\n", cloud[i].x, cloud[i].y);*/
	
	stack<point> hull;
	hull.push(cloud[0]); hull.push(cloud[1]); hull.push(cloud[2]);
	
	unsigned i = 3, hull_size = 3;
	while (i < N) {
		point stack_top = hull.top();
		hull.pop();
		if (ccw(hull.top(), stack_top, cloud[i]) < 0) {
			hull_size++;
			hull.push(stack_top);
			hull.push(cloud[i]);
			i++;
		} else {
			// hull.pop();
			hull_size--;	
		}
	}
	
	point first = hull.top();
	for (unsigned i = 0; i < hull_size; i++) {
		point p = hull.top();
		hull.pop();
		printf("%lf %lf\n", p.x, p.y);
	}
	printf("%lf %lf\n", first.x, first.y);
}

void jarvis_hull(vector<point>& cloud) {
	
	stack<point> hull;
	
	vector<point>::size_type N = cloud.size();
	vector<point>::iterator first_element_y = min_element(cloud.begin(), cloud.end(), order_by_y_x);
	
	vector<point>::iterator i = first_element_y;
	
	hull.push(*i);
	//printf("push y_min elem %lf %lf\n", i->x, i->y);
	
	do {
		bool any = false;
		vector<point>::iterator k;
		
		for(vector<point>::iterator j = cloud.begin(); j < cloud.end(); j++) {
			if(j == i) continue;
			if(!any) {
				k = j;
				//printf("first kandidate %lf %lf\n", k->x, k->y);
				any = true;
				continue;
			}
			
			if (ccw(hull.top(), *k, *j) > 0) {
				k = j;
				//printf("new kandidate %lf %lf\n", k->x, k->y);
			}
			
		}
		hull.push(*k);
		//printf("push kandidate %lf %lf\n", k->x, k->y);
		i = k;
		
	} while (i != first_element_y);
	
	//point first = hull.top();
	while(!hull.empty()) {
		point p = hull.top();
		hull.pop();
		printf("%lf %lf\n", p.x, p.y);
	}
	//printf("%lf %lf\n", first.x, first.y);
}

int main(int argc, char* argv[]) {

	if(argc != 3) {
			printf("usage: %s [dataset] [jarvis|graham]\n", argv[0]);
			exit(-1);
	}
	vector<point> cloud;
	load_dataset(cloud, argv[1]);
	
	if(strcmp(argv[2], "jarvis") == 0)
		jarvis_hull(cloud);
	else if(strcmp(argv[2], "graham") == 0)
		graham_hull(cloud);
	
	return 0;
}
