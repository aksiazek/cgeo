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

namespace gogui {
	class SortedLine : public Line {
		public:
			double x_range;
			double slope;
		
			bool operator<(const SortedLine &that) const {
				
				double r = fabs(point2.x - X) / x_range;
				double y = r*point1.y + (1-r)*point2.y;
				
				double r_that = fabs(that.point2.x - X) / that.x_range;
				double y_that = r_that*that.point1.y + (1-r_that)*that.point2.y;
				
				/*
				print_line("< this");
				printf("y: %lf\n", y);
				that.print_line("< that");
				printf("y+that: %lf\n", y_that);
				*/
				
				if(fabs(y - y_that) < EPSILON)
					return slope < that.slope;
				return y < y_that;
			}
			SortedLine(const Point &p1, const Point &p2) : Line(p1, p2)
			{
				x_range = fabs(p2.x - p1.x); // math error if equals
				slope = (p1.y - p2.y) / (p1.x - p2.x);
			}
			
			void print_line(const char* comment) const {
				printf("%s: %lf %lf %lf %lf\n", comment, point1.x, point1.y, point2.x, point2.y);
			}
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
		
		vector<SortedLine>::iterator intersected; // undefined for non-inner
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
}

bool intersection(const Line& a, const Line& b, Point& savepoint) {
	/*printf("Intersecting test -> line one: p1: %lf %lf , p2: %lf %lf\n", 
		a.point1.x, a.point1.y,
		a.point2.x, a.point2.y);
	
	printf("Intersecting test -> line two: p1: %lf %lf , p2: %lf %lf\n", 
		b.point1.x, b.point1.y,
		b.point2.x, b.point2.y);
	*/
	double denominator = (a.point1.x - a.point2.x)*(b.point1.y-b.point2.y) -
		(b.point1.x - b.point2.x)*(a.point1.y-a.point2.y);
	if (denominator == 0) {
		//puts("denom fail");
		return false;
	}
	double part1 = (a.point1.x*a.point2.y) - (a.point1.y*a.point2.x);
	double part2 = (b.point1.x*b.point2.y) - (b.point1.y*b.point2.x);
	savepoint.x = part1*(b.point1.x-b.point2.x) - (a.point1.x-a.point2.x)*part2;
	savepoint.x /= denominator;
	savepoint.y = part1*(b.point1.y-b.point2.y) - (a.point1.y-a.point2.y)*part2;
	savepoint.y /= denominator;
	//printf("Possible savepoint: %lf %lf\n", savepoint.x, savepoint.y);
	
	std::vector<double> ys;
	ys.push_back(a.point1.y);
	ys.push_back(a.point2.y);
	ys.push_back(b.point1.y);
	ys.push_back(b.point2.y);
	sort(ys.begin(), ys.end());
	
	if((savepoint.x < (1-EPSILON)*std::max(a.point1.x, b.point1.x)) || 
		savepoint.x > (1+EPSILON)*std::min(a.point2.x, b.point2.x) 
		|| (savepoint.y < (1-EPSILON)*ys[1]) || (savepoint.y > (1+EPSILON)*ys[2])) {
		//puts("bounds fail");
		return false;
	}
	
	return true;
}

bool lines_order_by_x(const SortedLine& lhs, const SortedLine& rhs) { 
	return lhs.point1.x < rhs.point1.x; 
}

inline void chart_new_intersection(bool exists, const SortedLine& line, const SortedLine& neighbor, 
			set<Event>& events, vector<SortedLine>& lines, vector<Point>& events_cloud)
{
	vector<Point>::iterator cloud_pointer;
	//printf("check bool: %d\n", exists);
	Point possible(0,0);
	if(exists && intersection(line, neighbor, possible)) {
		vector<SortedLine>::iterator line_1 = lower_bound(lines.begin(), lines.end(), line, lines_order_by_x); 
		vector<SortedLine>::iterator line_2 = lower_bound(lines.begin(), lines.end(), neighbor, lines_order_by_x); 
		
		Event inter(possible, Inner, line_1 - lines.begin());
		inter.intersected = line_2;
		events.insert(inter);
		
		assert(lines[inter.line_index] == line);
		assert(*inter.intersected == neighbor);
		
		//line.print_line("Intersecting line one");
		//neighbor->print_line("Intersecting line two");
		//printf("new event intersection: %lf %lf\n", 
			//possible.x, possible.y);
		
		cloud_pointer = lower_bound(events_cloud.begin(), events_cloud.end(), possible);
		//printf("lb intersection: %lf %lf\n", cloud_pointer->x, cloud_pointer->y);
		cloud_pointer = events_cloud.insert(cloud_pointer, possible);	
		//printf("cloud pointer point: %lf %lf\n", cloud_pointer->x, cloud_pointer->y);
		assert(*cloud_pointer == possible);
		/*for(vector<Point>::iterator it = events_cloud.begin(); it != events_cloud.end(); ++it)
				printf("cloud point: %lf %lf\n", it->x, it->y);*/
		
		snapshot();
	}
}

void handle_left_end
	(set<Event>::iterator& current_event, set<Event>& events, set<SortedLine>& broom_state, 
	vector<SortedLine>& lines, vector<Point>& events_cloud) 
{
	SortedLine& line = lines[current_event->line_index];
	
	/*printf("Left end at: %lf %lf\n", current_event->x, current_event->y);
	puts("Left end broom state");
	for(set<SortedLine>::iterator u = broom_state.begin(); u != broom_state.end(); ++u) {
		u->print_line("");
	}*/
	
	set<SortedLine>::iterator neighbor = broom_state.insert(line).first;
	line.setStatus(GeoObject::Status::Active);
	snapshot();
	//printJSON();
	
	//line.print_line("Broom includes line");
		
	bool check = true;
	if(neighbor != broom_state.begin())
		neighbor--;
	else
		check = false;
		
	chart_new_intersection(check, line, *neighbor, events, lines, events_cloud);
		
	set<SortedLine>::iterator neighbor2 = broom_state.find(line);
	
	check = true;
	if(++neighbor2 == broom_state.end())
		check = false;
	
	/*printf("neighbor two after: p1: %lf %lf , p2: %lf %lf\n", 
		neighbor2->point1.x, neighbor2->point1.y,
		neighbor2->point2.x, neighbor2->point2.y);*/
	
	chart_new_intersection(check, line, *neighbor2, events, lines, events_cloud);
}

void handle_right_end
	(set<Event>::iterator& current_event, set<Event>& events, set<SortedLine>& broom_state, 
	vector<SortedLine>& lines, vector<Point>& events_cloud) 
{
	SortedLine& line = lines[current_event->line_index];
	/*printf("Right end at: %lf %lf\n", current_event->x, current_event->y);
	puts("Right end broom state");
	for(set<SortedLine>::iterator u = broom_state.begin(); u != broom_state.end(); ++u) {
		u->print_line("");
		//assert(broom_state.find(*u) != broom_state.end());
	}*/
	set<SortedLine>::iterator neighbor = 
		broom_state.find(line);
	
	set<SortedLine>::iterator neighbor2 = 
		broom_state.find(line);
	
	bool check = true;
	if(neighbor != broom_state.begin())
		neighbor--;
	else
		check = false;
	
	if(++neighbor2 == broom_state.end())
		check = false;
	
	unsigned size = broom_state.size();
	
	//broom_state.find(line)->print_line("debug");
	broom_state.erase(line);
	line.setStatus(GeoObject::Status::Processed);
	snapshot(); 
	/*line.print_line("Broom loses line");
	puts("Right end broom state after erase");
	for(set<SortedLine>::iterator u = broom_state.begin(); u != broom_state.end(); ++u) {
		u->print_line("");
	}*/
	assert(size == (broom_state.size() + 1));
	
	chart_new_intersection(check, *neighbor, *neighbor2, events, lines, events_cloud);			
}

void handle_intersection
	(set<Event>::iterator& current_event, set<Event>& events, set<SortedLine>& broom_state, 
	vector<SortedLine>& lines, vector<Point>& events_cloud) 
{
	/*
	printf("Intersection at: %lf %lf\n", current_event->x, current_event->y);
		
	puts("Before broom");
	for(set<SortedLine>::iterator u = broom_state.begin(); u != broom_state.end(); ++u) {
		u->print_line("");
	}*/
	
	SortedLine& line = lines[current_event->line_index];
	SortedLine& interline = *(current_event->intersected);
	
	unsigned a = broom_state.size();
	//line.print_line("please rem");
	//interline.print_line("please rem");
	broom_state.erase(line);
	broom_state.erase(interline);
	assert(broom_state.size() == (a-2));
	
	/*puts("After erase");
	for(set<SortedLine>::iterator u = broom_state.begin(); u != broom_state.end(); ++u) {
		u->print_line("");
	}*/
	
	set<SortedLine>::iterator neighbor = broom_state.insert(line).first;
	set<SortedLine>::iterator neighbor2 = broom_state.insert(interline).first;
	
	/*puts("After broom");
	for(set<SortedLine>::iterator u = broom_state.begin(); u != broom_state.end(); ++u) {
		u->print_line("");
	}*/
	
	if(interline < line)
		std::swap(neighbor, neighbor2);
		
	//neighbor->print_line("neighbor");
	//neighbor2->print_line("neighbor2");
	
	bool check = true;
	if(neighbor != broom_state.begin())
		neighbor--;
	else
		check = false;
		
	bool check2 = true;
	if(++neighbor2 == broom_state.end())
		check2 = false;
	
	//neighbor->print_line("neighbor");
	//neighbor2->print_line("neighbor2");

	if(line < interline) {
		chart_new_intersection(check, line, *neighbor, events, lines, events_cloud);
		chart_new_intersection(check2, interline, *neighbor2, events, lines, events_cloud);
	} else {
		chart_new_intersection(check, interline, *neighbor, events, lines, events_cloud);
		chart_new_intersection(check2, line, *neighbor2, events, lines, events_cloud);
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
		//printf("Cloud init at: %lf %lf type: %s\n", it->x, it->y, getTextForEnum(it->type));
	}
	snapshot();

	set<Event>::iterator current_event;
	vector<Point>::iterator cloud_pointer = events_cloud.begin();
	do {
		current_event = events.begin();
		X = current_event->x;
		Point tmp(current_event->x,current_event->y);
		cloud_pointer = lower_bound(events_cloud.begin(), events_cloud.end(), tmp);
				
		assert(cloud_pointer->x == current_event->x);
		cloud_pointer->setStatus(GeoObject::Status::Active);
		//printf("event at: %lf %lf is marked active\n", current_event->x, current_event->y);
		snapshot();
		//printJSON();
		// update broom_state
		switch (current_event->type) {
			case Left : 
				handle_left_end(current_event, events, broom_state, lines, events_cloud);
				break;
			case Right:
				handle_right_end(current_event, events, broom_state, lines, events_cloud);
				break;
			case Inner:
				handle_intersection(current_event, events, broom_state, lines, events_cloud);
				break;
		}
		
		cloud_pointer = lower_bound(events_cloud.begin(), events_cloud.end(), tmp);
		assert(cloud_pointer->x == current_event->x);
		cloud_pointer->setStatus(GeoObject::Status::Processed);
		//printf("event at: %lf %lf is marked processed\n", current_event->x, current_event->y);
		
		snapshot();

		events.erase(current_event);
		
		//printJSON();
	} while (!events.empty());
	
	
}

int main(int argc, char* argv[]) {

	if(argc != 2) {
			printf("usage: %s [dataset] \n", argv[0]);
			exit(-1);
	}
	vector<SortedLine> lines;
	load_dataset(lines, argv[1]);
	//snapshot();
	sweep(lines);
	printJSON();
	
	return 0;
}
