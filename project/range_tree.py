# -*- coding: utf-8 -*-

from collections import OrderedDict # remove dupliates preserving order

class Node:
    def __init__(self, value):
        #print "Node value: ", value
        self.value = value
        self.left = None
        self.right = None
        self.next_level = None     # multidimensional range trees
        self.my_line = None   # original line to link with 
        self.left_lines = None # this is for interval trees
        self.right_lines = None
    def isLeaf(self):
        if self.left == None: # this is enough
            return True
        else:
            return False
    def inRange(self, x_range):
        if x_range[0] <= self.value and x_range[1] >= self.value:
            return True
        else:
            return False

class SingleDimensionRangeTree:
    def __init__(self, set_of_points, is_sorted=False):
        if not is_sorted:
            set_of_points.sort(key=lambda p: p[0])
        #for line in set_of_points:
        #    print line
        self.root = self.build_tree(set_of_points)
    
    def build_tree(self, set_of_points):
        if len(set_of_points) == 1:
            leaf = Node(set_of_points[0][0])
            leaf.my_line = set_of_points[0][1]
            return leaf
        else:
            median_index = len(set_of_points) / 2
            median_value = set_of_points[median_index-1][0]
            node = Node(median_value)
            node.my_line = set_of_points[median_index-1][1]
            node.left = self.build_tree(set_of_points[:median_index])
            node.right = self.build_tree(set_of_points[median_index:])
            return node
 
    def query(self, x_range): # (-5, 0.5)
        result_set = []
        division_node = self.find_division(x_range, self.root)
        if division_node.isLeaf() and division_node.inRange(x_range):
            result_set.append(division_node.my_line)
            #print division_node.my_line
        elif not division_node.isLeaf():
            self.left_add(result_set, x_range, division_node.left)
            self.right_add(result_set, x_range, division_node.right)
        return result_set
    
    def find_division(self, x_range, root): 
        #print "value: ", root.value
        if root.isLeaf():
            return root #leaf
        elif root.value > x_range[0]:
            if root.value > x_range[1]:
                return self.find_division(x_range, root.left)
            else:
                return root
        elif root.value < x_range[1]:
            return self.find_division(x_range, root.right)
        else:
            return self.find_division(x_range, root.left) #leaf to be found in left tree
    
    def left_add(self, result_set, x_range, root):
        if root.isLeaf() and root.inRange(x_range):
            result_set.append(root.my_line)
        elif root.inRange(x_range):
            self.left_add(result_set, x_range, root.left)
            self.include_tree(result_set, root.right)
        elif not root.isLeaf() and root.value < x_range[0]:
            self.left_add(result_set, x_range, root.right)
        
    def right_add(self, result_set, x_range, root):
        if root.isLeaf() and root.inRange(x_range):
            result_set.append(root.my_line)
        elif x_range[1] >= root.value:
            self.include_tree(result_set, root.left)
            if root.value != x_range[1]:
                self.right_add(result_set, x_range, root.right)
        elif not root.isLeaf() and x_range[1] < root.value:
            self.right_add(result_set, x_range, root.left)
    
    def include_tree(self, result_set, root):
        if root.isLeaf():
            result_set.append(root.my_line)
        else:
            self.include_tree(result_set, root.left)
            self.include_tree(result_set, root.right)

class TwoDimensionalRangeTree:
    def __init__(self, set_of_points):
        x_points = list(OrderedDict.fromkeys(map(lambda p: p[0], set_of_points)))
        x_points.sort()
        y_points = list(set_of_points)
        y_points.sort(key=lambda p: p[1])
        '''for line in x_points:
            print line
        print "\n\n"
        for line in y_points:
            print line'''
        self.root = self.build_tree(x_points, y_points)
    
    def build_tree(self, points_by_x, points_by_y):
        if len(points_by_x) == 1:
            leaf = Node(points_by_x[0])
            #print "y-range ", points_by_y
            y_range = filter(lambda p: p[0] == points_by_x[0], points_by_y)
            #print "y-range ", y_range, " for ", points_by_x[0]
            leaf.next_level = SingleDimensionRangeTree\
                (map(lambda p: (p[1], p[2]), y_range), is_sorted=True)
            return leaf
        else:
            median_index = len(points_by_x) / 2
            median_value = points_by_x[median_index-1]
            #print "median value ", median_value
            node = Node(median_value)
            y_range_left = filter(lambda p: p[0] <= median_value, points_by_y)
            y_range_right = filter(lambda p: p[0] > median_value, points_by_y)
            '''print "range y left ", y_range_left
            print "range y right ", y_range_right
            print "range x left ", points_by_x[:median_index]
            print "range x right ", points_by_x[median_index:]'''
            node.left = self.build_tree(points_by_x[:median_index], y_range_left)
            node.right = self.build_tree(points_by_x[median_index:], y_range_right)
            node.next_level = SingleDimensionRangeTree\
                (map(lambda p: (p[1], p[2]), points_by_y), is_sorted=True)
            return node
 
    def query(self, x_range, y_range): # (-5, 0.5)
        result_set = []
        division_node = self.find_division(x_range, self.root)
        if division_node.isLeaf() and division_node.inRange(x_range):
            result_set += division_node.next_level.query(y_range)
            
        else:
            #print division_node.value
            self.left_add(result_set, x_range, y_range, division_node.left)
            self.right_add(result_set, x_range, y_range, division_node.right)
        return result_set

    
    def find_division(self, x_range, root): 
        #print "value: ", root.value
        if root.isLeaf():
            return root #leaf
        elif root.value > x_range[0]:
            if root.value > x_range[1]:
                return self.find_division(x_range, root.left)
            else:
                return root
        elif root.value < x_range[1]:
            return self.find_division(x_range, root.right)
        else:
            return self.find_division(x_range, root.left) #leaf to be found in left tree
    
    def left_add(self, result_set, x_range, y_range, root):
        if root.isLeaf() and root.inRange(x_range):
            result_set += root.next_level.query(y_range)
            
        elif root.inRange(x_range):
            self.left_add(result_set, x_range, y_range, root.left)
            self.include_tree(result_set, y_range, root.right)
        elif not root.isLeaf() and root.value < x_range[0]:
            self.left_add(result_set, x_range, y_range, root.right)
        
    def right_add(self, result_set, x_range, y_range, root):
        if root.isLeaf() and root.inRange(x_range):
            result_set += root.next_level.query(y_range)
        elif x_range[1] >= root.value:
            self.include_tree(result_set, y_range, root.left)
            if root.value != x_range[1]:
                self.right_add(result_set, x_range, y_range, root.right)
        elif not root.isLeaf() and x_range[1] < root.value:
            self.right_add(result_set, x_range, y_range, root.left)
    
    def include_tree(self, result_set, y_range, root):
        if root.isLeaf():
            result_set += root.next_level.query(y_range)
        else:
            self.include_tree(result_set, y_range, root.left)
            self.include_tree(result_set, y_range, root.right)



class IntervalTree:
    def __init__(self, points):
        points.sort(key=lambda p: p[0])
        #for line in set_of_points:
        #    print line
        self.root = self.build_tree(points)
    
    def build_tree(self, set_of_points):
        if len(set_of_points) == 1:
            leaf = Node(set_of_points[0][0])
            leaf.left_lines = set_of_points[0][2]
            leaf.right_lines = set_of_points[0][2]
            return leaf
        else:
            '''print "points"
            for p in set_of_points:
                print p'''
            median_index = len(set_of_points) / 2
            median_value = set_of_points[median_index-1][0]
            node = Node(median_value)
            #print "median", median_value
            
            lines_only = map(lambda p: p[2], set_of_points)
            node.left_lines = list(OrderedDict.fromkeys(
                    filter(lambda p: p[0] <= median_value and 
                   p[2] >= median_value, lines_only)))
            node.right_lines = list(node.left_lines)
            node.right_lines.sort(key=lambda p: p[2], reverse=True)
            '''print "left lines"
            for p in node.left_lines:
                print p
            print "right lines"
            for i in node.right_lines:
                print i'''
            
            L = filter(lambda p: p[2][2] <= median_value, set_of_points)
            R = filter(lambda p: p[2][0] > median_value, set_of_points)
            #print "L", L
            #print "R", R  
            
            if len(L) > 0:
                node.left = self.build_tree(L)
            if len(R) > 0:
                node.right = self.build_tree(R)
            return node

    def query(self, x_q):
        result_set = []
        self.visit(self.root, x_q, result_set)
        return result_set

    def visit(self, root, x_q, result_set):
        if root.isLeaf():
            if root.left_lines[0] <= x_q and x_q <= root.left_lines[2]:
                result_set.append(root.left_lines)
        else:
            if x_q < root.value:
                for line in root.left_lines:
                    if not (line[0] <= x_q and x_q <= line[2]):
                        break
                    result_set.append(line)
                self.visit(root.left, x_q, result_set)
            else:
                for line in root.right_lines:
                    if not (line[0] <= x_q and x_q <= line[2]):
                        break
                    result_set.append(line)
                self.visit(root.right, x_q, result_set)

class WindowingAnswerMachine:
    def __init__(self, filename):
        lines = []
        with open(filename) as data:
            for line in data:
                a, b, c, d = line.split()
                lines.append((float(a), float(b), float(c), float(d)))
        
        points = []
        for quadruple in lines:
            points.append((quadruple[0], quadruple[1], quadruple))
            points.append((quadruple[2], quadruple[3], quadruple))
            
        self.double_tree = TwoDimensionalRangeTree(points)
        self.interval_tree_horizontal = \
            IntervalTree(filter(lambda triple: triple[2][0] != triple[2][2], points))
        # self.interval_tree_vertical = \
        #     IntervalTree(filter(lambda triple: triple[2][0] == triple[2][2], points))
        
    def query(self, left_top, right_bottom):
        query_result = []
        lines_in_area = self.double_tree.query(left_top, right_bottom)
        prev = None # prevent double printing
        for line in lines_in_area:
            if prev != line:
                query_result.append(str(line[0])+" "+str(line[1])+" "+
                                    str(line[2])+" "+str(line[3]))
            prev = line   
        
        for q in self.interval_tree_horizontal.query(0):
            print q
        
        return query_result             

if __name__ == '__main__':
    cool_thing = WindowingAnswerMachine("lines.dat")
    #single_tree = SingleDimensionRangeTree([3, 3, 10, 19, 23, 30, 37, 37, 49, 59, 62, 70, 80, 89, 93, 97])
    #print single_tree.query((25, 90))
    for line in cool_thing.query((-1, 1), (-3, 0)):
        print line
    
    
    