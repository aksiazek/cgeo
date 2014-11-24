#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <algorithm>
#include <iterator>
#include <utility>
#include <set>
#include <stack>

#include "../gogui/gogui_core/include/gogui.hpp"

using namespace gogui;
using std::set;
using std::swap;
using std::pair;
using std::sort;
using std::lower_bound;

/* polygon */
void load_dataset(vector<Line>& lines, vector<Point>& points, const char* filename) {
	FILE* file = fopen(filename, "rb");
	if(file != NULL) {
	    bool first = true;
		double x, y, prev_x, prev_y, first_x, first_y;
		while(fscanf(file, "%lf %lf", &x, &y) == 2) {
			if(!first) {
			    Point curr(x, y);
			    Point prev(prev_x, prev_y);
			    lines.push_back(Line(prev, curr));
			    points.push_back(curr);
			} else {
			    first = false;
			    first_x = x;
			    first_y = y;
			    points.push_back(Point(first_x, first_y));
			}
			prev_x = x;
			prev_y = y;
		}
		if(!first) {
		    Point curr(x, y);
		    Point fp(first_x, first_y);
		    lines.push_back(Line(curr, fp));
		}
		fclose(file);
	}
}

int n; // nr of points
unsigned int min = 0;
unsigned int max = 0;

typedef struct {
	double x, y;
	bool in_first_branch = false;
	
} point_t;

std::vector<point_t> list; // min && max belong to both

bool is_y_monotone(vector<Line>& lines) 
{
	n = lines.size();
    for (int i = 1; i < n; i++) {
		if(lines[i].point1.y < lines[min].point1.y)
			min = i;
		if(lines[i].point1.y > lines[max].point1.y)
			max = i;
    }
    
    for (int i = 0; i < n; i++) {
		point_t point;
		point.x = lines[i].point1.x;
		point.y = lines[i].point1.y;
		list.push_back(point);
	}
    
    assert(n > 0);
    //printf("%lf %lf\n", lines[min].point1.y, lines[max].point1.y);
    unsigned int prev = min;
    bool correct = true;
    bool up = true;
    for(unsigned int i = min+1; i != min; i = (i+1) % n) {
		if((up && (lines[i].point1.y < lines[prev].point1.y))
			|| (!up && (lines[i].point1.y > lines[prev].point1.y)))
			correct = false;
		if(up)
			list[i].in_first_branch = true;
		if(i == max)
			up = false;
		//printf("%lf %lf\n", lines[prev].point1.y, lines[i].point1.y);
		prev = i;
	}
    
    return correct;
}

bool comparator(point_t a, point_t b)
{
	return (a.y > b.y);
}

double angle (point_t p, point_t q, point_t r)
{
	// angle in < pqr (q)
	double dotprod = (r.x - q.x)*(p.x - q.x) + (r.y - q.y)*(p.y - q.y);
	double len1squared = (p.x - q.x) * (p.x - q.x) + (p.y - q.y) * (p.y - q.y);
	double len2squared = (r.x - q.x) * (r.x - q.x) + (r.y - q.y) * (r.y - q.y);
	return acos(dotprod/sqrt(len1squared*len2squared));
}

void setPoint(vector<Point>& points, point_t& p, GeoObject::Status status) {
	for(unsigned int i = 0; i < points.size(); i++) {
		if((points[i].x == p.x) && (points[i].y == p.y)) {
			points[i].setStatus(status);
			return;
		}
	}
}

bool neq(point_t a, point_t b)
{
	return !((a.x == b.x) && (a.y == b.y));
}

double ccw(point_t p1, point_t p2, point_t p3) {
    return (p1.x - p3.x)*(p2.y - p3.y) - (p1.y - p3.y)*(p2.x - p3.x);
}

void triangulate(vector<Line>& lines, vector<Point>& points) 
{
	point_t min_p = list[min];
	point_t max_p = list[max];
    sort(list.begin(), list.end(), comparator);
    /*for (int i = 0; i < n; i++) {
		printf("%lf %lf %d\n", list[i].x, list[i].y, list[i].in_first_branch);
		
	}*/
	std::stack<point_t> stack;
	point_t a = list.back();
	list.pop_back();
	stack.push(a);
	point_t b = list.back();
	stack.push(b);
	list.pop_back();
	
	setPoint(points, a, GeoObject::Status::Active);
	setPoint(points, b, GeoObject::Status::Active);
	snapshot();
	
	while(!list.empty())
	{
		point_t p = list.back();
		list.pop_back();
		point_t q = stack.top();
		
		if((p.in_first_branch != q.in_first_branch))
		{
			while(!stack.empty())
			{
				point_t r = stack.top();
				stack.pop();
				setPoint(points, r, GeoObject::Status::Processed);
				snapshot();
				lines.push_back(Line(Point(p.x, p.y), Point(r.x, r.y)));
				snapshot();
			}
			
		} else {
			point_t mark = stack.top();
			setPoint(points, mark, GeoObject::Status::Processed);
			snapshot();
			
			stack.pop();
			
			point_t r = stack.top();
			
			while ((!p.in_first_branch && (ccw(p,q,r) > 0))
					|| (p.in_first_branch && (ccw(p,q,r) < 0)))
			{
				lines.push_back(Line(Point(p.x, p.y), Point(r.x, r.y)));
				snapshot();
				q = r;
				
				point_t mark = stack.top();
				setPoint(points, mark, GeoObject::Status::Processed);
				snapshot();
				
				stack.pop();
				
				if(stack.empty())
					break;
				r = stack.top();
			}
		}
		stack.push(q);
		stack.push(p);
		setPoint(points, p, GeoObject::Status::Active);
		setPoint(points, q, GeoObject::Status::Active);
		snapshot();
	}
	
}

int main(int argc, char* argv[]) {

	if(argc != 2) {
			printf("usage: %s [dataset] \n", argv[0]);
			exit(-1);
	}
	vector<Line> lines;
	vector<Point> points;
	load_dataset(lines, points, argv[1]);
	snapshot();
	if(is_y_monotone(lines))
		triangulate(lines, points);
	
	printJSON();
	return 0;
}
