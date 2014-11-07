#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <algorithm>
#include <iterator>
#include <utility>
#include <set>

#include "../gogui/gogui_core/include/gogui.hpp"

using namespace gogui;
using std::set;
using std::swap;
using std::pair;
using std::sort;
using std::lower_bound;

double X; // broom global state needed
double reverse;

namespace gogui {
	class SortedLine : public Line {
		public:
			double x_range = abs(point2.x - point1.x);
		
			bool operator<(const SortedLine &that) const {
				
				double r = abs(point2.x - X) / x_range;
				double y = (1-r)*point1.y + r*point2.y;
				
				double r_that = abs(that.point2.x - X) / that.x_range;
				double y_that = (1-r_that)*that.point1.y + r_that*that.point2.y;
				
				bool ret = y < y_that;
				if(!reverse) return ret; else return !ret;
			}
			using Line::Line;	
			
			/*SortedLine& operator=(const SortedLine& that) {
				point1 = that.point1;
				point2 = that.point2;
				return *this;
			}*/
	};
	
	template<>
	void vector<SortedLine>::forEachLine(LineAcceptor f) const {
		for (const Line &line : *this) {
			f(line);
		}
	}

	template<>
	void vector<SortedLine>::forEachPoint(PointAcceptor f) const {
	}
}

enum EventType { 
	Left, Right, Inner // two ends of a line or intersection
};

static const char * EnumStrings[] = { "LEFT", "RIGHT", "INNER" };

const char * getTextForEnum( int enumVal )
{
  return EnumStrings[enumVal];
}


class Event : public Point {
	public:
		EventType type;
		int line_index; //index in lines
		
		vector<SortedLine>::iterator* intersected = nullptr; // undefined for non-inner
		Event(const Point& p, EventType t, int l) 
			: Point(p.x, p.y), type(t), line_index(l) {}
};

void load_dataset(vector<SortedLine>& lines, const char* filename) {
	FILE* file = fopen(filename, "rb");
	if(file != NULL) {
		double x, y, a, b;
		while(fscanf(file, "%lf %lf %lf %lf", &x, &y, &a, &b) == 4) {
			// do NOT use feof
			if(a < x) {
				Point first(a, b), second(x, y);
				lines.push_back(SortedLine (first, second));
			} else {
				Point first(x, y), second(a, b);
				lines.push_back(SortedLine (first, second));
			}
		}
		fclose(file);
	}
	//snapshot();
}

bool cmp(double a, double b) {
	return fabs(a - b) < EPSILON;
}

bool intersection(const Line& a, const Line& b, Point& savepoint) {
	printf("Intersecting test -> line one: p1: %lf %lf , p2: %lf %lf\n", 
		a.point1.x, a.point1.y,
		a.point2.x, a.point2.y);
	
	printf("Intersecting test -> line two: p1: %lf %lf , p2: %lf %lf\n", 
		b.point1.x, b.point1.y,
		b.point2.x, b.point2.y);
	
	double denominator = (a.point1.x - a.point2.x)*(b.point1.y-b.point2.y) -
		(b.point1.x - b.point2.x)*(a.point1.y-a.point2.y);
	if (denominator == 0) {
		puts("denom fail");
		return false;
	}
	double part1 = (a.point1.x*a.point2.y) - (a.point1.y*a.point2.x);
	double part2 = (b.point1.x*b.point2.y) - (b.point1.y*b.point2.x);
	savepoint.x = part1*(b.point1.x-b.point2.x) - (a.point1.x-a.point2.x)*part2;
	savepoint.x /= denominator;
	savepoint.y = part1*(b.point1.y-b.point2.y) - (a.point1.y-a.point2.y)*part2;
	savepoint.y /= denominator;
	printf("Possible savepoint: %lf %lf\n", savepoint.x, savepoint.y);
	
	std::vector<double> ys;
	ys.push_back(a.point1.y);
	ys.push_back(a.point2.y);
	ys.push_back(b.point1.y);
	ys.push_back(b.point2.y);
	sort(ys.begin(), ys.end());
	
	if((savepoint.x < (1-EPSILON)*std::min(a.point1.x, b.point1.x)) || 
		savepoint.x > (1+EPSILON)*std::max(a.point2.x, b.point2.x) 
		|| (savepoint.y < (1-EPSILON)*ys[0]) || (savepoint.y > (1+EPSILON)*ys[3])) {
		puts("bounds fail");
		return false;
	}
	
	return true;
}

bool lines_order_by_x(const SortedLine& lhs, const SortedLine& rhs)
{ return lhs.point1.x < rhs.point1.x; }

void handle_left_end
	(set<Event>::iterator& current_event, set<Event>& events, set<SortedLine>& broom_state, 
	vector<SortedLine>& lines, vector<Point>& events_cloud, vector<Point>::iterator& cloud_pointer) 
{
	SortedLine& line = lines[current_event->line_index];
	
	printf("Left end at: %lf %lf\n", current_event->x, current_event->y);
	set<SortedLine>::iterator neighbor 
		= broom_state.insert(line).first;
	line.setStatus(GeoObject::Status::Active);
	snapshot();
	printf("Broom includes line: p1: %lf %lf , p2: %lf %lf\n", 
		line.point1.x, line.point1.y,
		line.point2.x, line.point2.y);
		
	bool check = true;
	if(neighbor != broom_state.begin())
		neighbor--;
	else
		check = false;
		
	//printf("check bool: %d\n", check);
	Point possible(0,0);
	if(check && intersection(line, *neighbor, possible)) {
		Event inter(possible, Inner, current_event->line_index);
		vector<SortedLine>::iterator sect = lower_bound(lines.begin(), lines.end(), *neighbor, lines_order_by_x); 
		inter.intersected = &sect;
		events.insert(inter);
		printf("Intersecting line one: p1: %lf %lf , p2: %lf %lf\n", 
		line.point1.x, line.point1.y,
		line.point2.x, line.point2.y);
		printf("Intersecting line two: p1: %lf %lf , p2: %lf %lf\n", 
		neighbor->point1.x, neighbor->point1.y,
		neighbor->point2.x, neighbor->point2.y);
		printf("new event intersection: %lf %lf\n", 
			possible.x, possible.y);
		events_cloud.insert(cloud_pointer, possible);
		snapshot();
	}
		
	set<SortedLine>::iterator neighbor2 = broom_state.find(line);
	
	check = true;
	if(++neighbor2 == broom_state.end())
		check = false;
	
	/*printf("neighbor two after: p1: %lf %lf , p2: %lf %lf\n", 
		neighbor2->point1.x, neighbor2->point1.y,
		neighbor2->point2.x, neighbor2->point2.y);*/
	
	//printf("check bool: %d\n", check);
	Point maybe(0,0);
	if(check && intersection(line, *neighbor2, maybe)) {
		Event inter(maybe, Inner, current_event->line_index);
		vector<SortedLine>::iterator sect = lower_bound(lines.begin(), lines.end(), *neighbor, lines_order_by_x); 
		inter.intersected = &sect;
		events.insert(inter);
		
		printf("Intersecting line one: p1: %lf %lf , p2: %lf %lf\n", 
		line.point1.x, line.point1.y,
		line.point2.x, line.point2.y);
		printf("Intersecting line two: p1: %lf %lf , p2: %lf %lf\n", 
		neighbor2->point1.x, neighbor2->point1.y,
		neighbor2->point2.x, neighbor2->point2.y);
		
		printf("new event intersection: %lf %lf\n", 
			maybe.x, maybe.y);
		events_cloud.insert(cloud_pointer, maybe);
		snapshot();
	}
}

void handle_right_end
	(set<Event>::iterator& current_event, set<Event>& events, set<SortedLine>& broom_state, 
	vector<SortedLine>& lines, vector<Point>& events_cloud, vector<Point>::iterator& cloud_pointer) 
{
	SortedLine& line = lines[current_event->line_index];
	printf("Right end at: %lf %lf\n", current_event->x, current_event->y);
	set<SortedLine>::iterator neighbor = 
		broom_state.find(line);
	
	bool check = true;
	if(neighbor != broom_state.begin())
		neighbor--;
	else
		check = false;
	
	broom_state.erase(line);
	line.setStatus(GeoObject::Status::Processed);
	snapshot();
	printf("Broom loses line: p1: %lf %lf , p2: %lf %lf\n", 
		line.point1.x, line.point1.y,
		line.point2.x, line.point2.y);
	set<SortedLine>::iterator neighbor2 = 
		broom_state.find(line);
	if(neighbor2 == broom_state.end())
		check = false;
	else neighbor2++;
	
	Point maybe(0,0);
	if(check && intersection(*neighbor, *neighbor2, maybe)) {
		vector<SortedLine>::iterator line_1 = lower_bound(lines.begin(), lines.end(), *neighbor, lines_order_by_x); 
		vector<SortedLine>::iterator line_2 = lower_bound(lines.begin(), lines.end(), *neighbor2, lines_order_by_x); 
		Event inter(maybe, Inner, line_1 - lines.begin());
		inter.intersected = &line_2;
		events.insert(inter);
		printf("new event intersection: %lf %lf\n", 
			maybe.x, maybe.y);
			
		events_cloud.insert(cloud_pointer, maybe);
		snapshot();
	}				
}

void sweep(vector<SortedLine>& lines) {
	set<SortedLine> broom_state;
	set<Event> events;
	vector<Point> events_cloud;
	
	sort(lines.begin(), lines.end(), lines_order_by_x);
	/*for(vector<SortedLine>::iterator it = lines.begin(); it != lines.end(); ++it) {	
		printf("Left end sort at: %lf %lf\n", it->point1.x, it->point1.y);
		printf("Right end sort at: %lf %lf\n", it->point2.x, it->point2.y);
	}*/
	
	for(vector<SortedLine>::iterator it = lines.begin(); it != lines.end(); ++it) {	
		Event event_left(it->point1, Left, it - lines.begin());
		Event event_right(it->point2, Right, it - lines.begin());
		events.insert(event_left);
		events.insert(event_right);
		//printf("Left end init at: %lf %lf\n", event_left.x, event_left.y);
		//printf("Right end init at: %lf %lf\n", event_right.x, event_right.y);
	}
	
	for(set<Event>::iterator it = events.begin(); it != events.end(); ++it) {
		Point p(it->x, it->y);
		events_cloud.push_back(p);
		printf("Event init at: %lf %lf type: %s\n", it->x, it->y, getTextForEnum(it->type));
	}
	snapshot();

	set<Event>::iterator current_event;
	vector<Point>::iterator cloud_pointer = events_cloud.begin();
	do {
		current_event = events.begin();
		X = current_event->x;
				
		cloud_pointer->setStatus(GeoObject::Status::Active);
		snapshot();
		// update broom_state
		switch (current_event->type) {
			case Left : 
				handle_left_end(current_event, events, broom_state, lines, events_cloud, cloud_pointer);
				cloud_pointer->setStatus(GeoObject::Status::Normal);
				break;
			case Right:
				handle_right_end(current_event, events, broom_state, lines, events_cloud, cloud_pointer);
				cloud_pointer->setStatus(GeoObject::Status::Normal);
				break;
			case Inner:
				printf("Intersection at: %lf %lf\n", current_event->x, current_event->y);
					
				SortedLine& line = lines[current_event->line_index];
				broom_state.erase(line);
				broom_state.erase(*(*(current_event->intersected)));
				
				reverse = true;
				broom_state.insert(line);
				broom_state.insert(*(*(current_event->intersected)));
				reverse = false;
				// may break really...
				
				// more
				cloud_pointer->setStatus(GeoObject::Status::Processed);
				break;
		}
		
		//printf("%lf %lf\n", current_event->x, current_event->y);
		
		snapshot();
		events.erase(current_event);
		if(cloud_pointer != events_cloud.end()) 
			cloud_pointer++;
	} while (!events.empty());
	
	
}

int main(int argc, char* argv[]) {

	if(argc != 2) {
			printf("usage: %s [dataset] \n", argv[0]);
			exit(-1);
	}
	vector<SortedLine> lines;
	load_dataset(lines, argv[1]);
	snapshot();
	sweep(lines);
	printJSON();
	
	return 0;
}
