// definition of computational geometry functions
#ifndef COMPGEO_H
#define COMPGEO_H

#include<iostream>
#include<stdlib.h> 
#include<math.h>
#include<vector>
using namespace std;

#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
using namespace cv;

bool isSegmentCross(const Point &A, const Point &B, const Point &C, const Point &D);                                  //judge if segment intersect line
double getDistance(const Point p1, const Point p2);                                                                   //get distance between two points
int crossProduct(const Point &A, const Point &B);                                                                     //get cross product of two vectors
bool rapidRejectionTest(const Point &A, const Point &B, const Point &C, const Point &D);                              //rapid rejection test

#endif 