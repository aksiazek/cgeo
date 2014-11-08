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
			double x_range = fabs(point2.x - point1.x);
			bool reverse_on_saved = false;
			double saved;
			Line* reverse_partner = nullptr;
		
			bool operator<(const SortedLine &that) const {
				
				double r = fabs(point2.x - X) / x_range;
				//printf("r: %lf\n", r);
				double y = r*point1.y + (1-r)*point2.y;
				
				double r_that = fabs(that.point2.x - X) / that.x_range;
				double y_that = r_that*that.point1.y + (1-r_that)*that.point2.y;
				
				/*print_line("< this");
				printf("y: %lf\n", y);
				that.print_line("< that");
				printf("y_that: %lf\n", y_that);*/
				
				bool ret = y < y_that;
				if(reverse_on_saved && (X == saved) && (that == *reverse_partner)) { 
					
					/*if(ret)
						print_line("< this is smaller");
					else
						that.print_line("< that is smaller");
					puts("reverse <");
					printf("X: %lf\n", X);*/
					return !ret; 
				} else {
					
					/*if(ret)
						print_line("< this is smaller");
					else
						that.print_line("< that is smaller");
					puts("<");
					printf("X: %lf\n", X);*/
					return ret;
				}
			}
			using Line::Line;
			
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
	printf("Possible savepoint: %lf %lf\n", savepoint.x, savepoint.y);
	
	std::vector<double> ys;
	ys.push_back(a.point1.y);
	ys.push_back(a.point2.y);
	ys.push_back(b.point1.y);
	ys.push_back(b.point2.y);
	sort(ys.begin(), ys.end());
	
	if((savepoint.x < (1-EPSILON)*std::max(a.point1.x, b.point1.x)) || 
		savepoint.x > (1+EPSILON)*std::min(a.point2.x, b.point2.x) 
		|| (savepoint.y < (1-EPSILON)*ys[1]) || (savepoint.y > (1+EPSILON)*ys[2])) {
		puts("bounds fail");
		return false;
	}
	
	return true;
}

bool lines_order_by_x(const SortedLine& lhs, const SortedLine& rhs) { 
	return lhs.point1.x < rhs.point1.x; 
}

void handle_left_end
	(set<Event>::iterator& current_event, set<Event>& events, set<SortedLine>& broom_state, 
	vector<SortedLine>& lines, vector<Point>& events_cloud) 
{
	vector<Point>::iterator cloud_pointer;
	
	SortedLine& line = lines[current_event->line_index];
	
	printf("Left end at: %lf %lf\n", current_event->x, current_event->y);
	set<SortedLine>::iterator neighbor 
		= broom_state.insert(line).first;
	line.setStatus(GeoObject::Status::Active);
	snapshot();
	//printJSON();
	line.print_line("Broom includes line");
		
	bool check = true;
	if(neighbor != broom_state.begin())
		neighbor--;
	else
		check = false;
		
	//printf("check bool: %d\n", check);
	Point possible(0,0);
	if(check && intersection(line, *neighbor, possible)) {
		Event inter(possible, Inner, current_event->line_index);
		vector<SortedLine>::iterator sect 
			= lower_bound(lines.begin(), lines.end(), *neighbor, lines_order_by_x); 
		inter.intersected = sect;
		events.insert(inter);
		
		assert(lines[inter.line_index] == line);
		assert(*inter.intersected == *neighbor);
		
		line.print_line("Intersecting line one");
		neighbor->print_line("Intersecting line two");
		printf("new event intersection: %lf %lf\n", 
			possible.x, possible.y);
		
		cloud_pointer = lower_bound(events_cloud.begin(), events_cloud.end(), possible);
		//printf("lb intersection: %lf %lf\n", cloud_pointer->x, cloud_pointer->y);
		cloud_pointer = events_cloud.insert(cloud_pointer, possible);	
		//printf("cloud pointer point: %lf %lf\n", cloud_pointer->x, cloud_pointer->y);
		assert(*cloud_pointer == possible);
		/*for(vector<Point>::iterator it = events_cloud.begin(); it != events_cloud.end(); ++it)
				printf("cloud point: %lf %lf\n", it->x, it->y);*/
		
		snapshot();
		
		//printJSON();
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
		vector<SortedLine>::iterator sect = lower_bound(lines.begin(), lines.end(), *neighbor2, lines_order_by_x); 
		
		inter.intersected = sect;
		events.insert(inter);
		
		assert(lines[inter.line_index] == line);
		assert(*inter.intersected == *neighbor2);

		line.print_line("Intersecting line one");
		neighbor2->print_line("Intersecting line two");
		
		printf("new event intersection: %lf %lf\n", 
			maybe.x, maybe.y);
			
		cloud_pointer = lower_bound(events_cloud.begin(), events_cloud.end(), maybe);
		printf("lb intersection: %lf %lf\n", cloud_pointer->x, cloud_pointer->y);
		cloud_pointer = events_cloud.insert(cloud_pointer, maybe);
		printf("cloud pointer point: %lf %lf\n", cloud_pointer->x, cloud_pointer->y);
		/*for(vector<Point>::iterator it = events_cloud.begin(); it != events_cloud.end(); ++it)
				printf("cloud point: %lf %lf\n", it->x, it->y);*/
		assert(*cloud_pointer == maybe);
		
		
		snapshot();
	}
}

void handle_right_end
	(set<Event>::iterator& current_event, set<Event>& events, set<SortedLine>& broom_state, 
	vector<SortedLine>& lines, vector<Point>& events_cloud) 
{
	vector<Point>::iterator cloud_pointer;
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
	line.print_line("Broom loses line");
	//printJSON();
	
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
		
		inter.intersected = line_2;
		events.insert(inter);
		
		assert(lines[inter.line_index] == *neighbor);
		assert(*inter.intersected == *neighbor2);
		
		neighbor->print_line("Intersecting line one");
		neighbor2->print_line("Intersecting line one");
		
		printf("new event intersection: %lf %lf\n", 
			maybe.x, maybe.y);
			
		cloud_pointer = lower_bound(events_cloud.begin(), events_cloud.end(), maybe);
		printf("lb intersection: %lf %lf\n", cloud_pointer->x, cloud_pointer->y);
		cloud_pointer = events_cloud.insert(cloud_pointer, maybe);
		printf("cloud pointer point: %lf %lf\n", cloud_pointer->x, cloud_pointer->y);
		assert(*cloud_pointer == maybe);
		for(vector<Point>::iterator it = events_cloud.begin(); it != events_cloud.end(); ++it)
				printf("cloud point: %lf %lf\n", it->x, it->y);
		
		snapshot();
	}				
}

void sweep(vector<SortedLine>& lines) {
	set<SortedLine> broom_state;
	set<Event> events;
	vector<Point> events_cloud;
	
	//std::ios_base::sync_with_stdio(false);
		
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
		printf("Cloud init at: %lf %lf type: %s\n", it->x, it->y, getTextForEnum(it->type));
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
		printf("event at: %lf %lf is marked active\n", current_event->x, current_event->y);
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
				printf("Intersection at: %lf %lf\n", current_event->x, current_event->y);
					
				puts("Before broom");
				for(set<SortedLine>::iterator u = broom_state.begin(); u != broom_state.end(); ++u) {
					u->print_line("");
				}
					
				SortedLine& line = lines[current_event->line_index];
				SortedLine& interline = *(current_event->intersected);
				unsigned a = broom_state.size();
				line.print_line("please rem");
				interline.print_line("please rem");
				broom_state.erase(line);
				broom_state.erase(interline);
				assert(broom_state.size() == (a-2));
				
				puts("After erase");
				for(set<SortedLine>::iterator u = broom_state.begin(); u != broom_state.end(); ++u) {
					u->print_line("");
				}
				
				
				line.reverse_on_saved = true;
				line.saved = X;
				line.reverse_partner = &interline;
				
				interline.reverse_on_saved = true;
				interline.saved = X;
				interline.reverse_partner = &line;
				
				broom_state.insert(line);
				broom_state.insert(interline);
				
				// may break really...
				
				puts("After broom");
				for(set<SortedLine>::iterator u = broom_state.begin(); u != broom_state.end(); ++u) {
					u->print_line("");
				}
				
				// more
				
				break;
		}
		
		cloud_pointer = lower_bound(events_cloud.begin(), events_cloud.end(), tmp);
		assert(cloud_pointer->x == current_event->x);
		cloud_pointer->setStatus(GeoObject::Status::Processed);
		printf("event at: %lf %lf is marked processed\n", current_event->x, current_event->y);
		
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
