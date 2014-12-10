# -*- coding: utf-8 -*-

import bisect, math

class SingleDimensionRangeTree:
    def __init__(self, set_of_points): # already sorted
        set_of_points.sort()
        #for line in set_of_points:
        #    print line
        self.root = self.build_tree(set_of_points)
    
    def build_tree(self, set_of_points):
        if len(set_of_points) == 1:
            leaf = Node(set_of_points[0])
            return leaf
        else:
            median_index = len(set_of_points) / 2
            median_value = set_of_points[median_index-1]
            node = Node(median_value)
            node.left = self.build_tree(set_of_points[:median_index])
            node.right = self.build_tree(set_of_points[median_index:])
            return node
 
    def query(self, x_range): # (-5, 0.5)
        result_set = []
        division_node = self.find_division(x_range, self.root)
        if division_node.isLeaf() and division_node.inRange(x_range):
            result_set.append(division_node.value)
        else:
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
            result_set.append(root.value)
        elif root.inRange(x_range):
            self.left_add(result_set, x_range, root.left)
            self.include_tree(result_set, root.right)
        elif not root.isLeaf() and root.value < x_range[0]:
            self.left_add(result_set, x_range, root.right)
        
    def right_add(self, result_set, x_range, root):
        if root.isLeaf() and root.inRange(x_range):
            result_set.append(root.value)
        elif x_range[1] >= root.value:
            self.include_tree(result_set, root.left)
            if root.value != x_range[1]:
                self.right_add(result_set, x_range, root.right)
        elif not root.isLeaf() and x_range[1] < root.value:
            self.right_add(result_set, x_range, root.left)
    
    def include_tree(self, result_set, root):
        if root.isLeaf():
            result_set.append(root.value)
        else:
            self.include_tree(result_set, root.left)
            self.include_tree(result_set, root.right)

  
class Node:
    def __init__(self, value):
        #print "Node value: ", value
        self.value = value
        self.left = None
        self.right = None
        self.next_level = None       
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

class Tree:
    def __init__(self, filename):
        self.lines = []
        with open(filename) as data:
            for line in data:
                a, b, c, d = line.split()
                self.lines.append((a, b, c, d))
        #print self.lines
        
        self.points = []
        for quadruple in self.lines:
            self.points.append((float(quadruple[0]), float(quadruple[1])))
            self.points.append((float(quadruple[2]), float(quadruple[3])))
            
        x_points = list(set(map(lambda p: p[0], self.points)))
        self.single_tree = SingleDimensionRangeTree(x_points)

    def build_tree(self, set_of_points):
        self.points.sort()
        for line in self.points:
            print line
        self.by_x = range(len(self.points)) #map(lambda x: x[0], self.points)
        self.by_y = zip(self.points, range(len(self.points)))
        self.by_y.sort(key=lambda x: x[0][1])
        self.by_y = map(lambda x: x[1], self.by_y)
        
        print self.by_x
        print self.by_y

    def query(self, left_top, right_bottom):
        return self.single_tree.query(left_top)

if __name__ == '__main__':
    tree = Tree("lines.dat")
    print tree.query((-8, -1), (5, 3.5))
    