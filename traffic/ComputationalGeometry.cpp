// implement of computational geometry functions
#include "ComputationalGeometry.h"

double getDistance(const Point p1, const Point p2)
{
	return sqrt((double)(p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y));
}

int crossProduct(const Point &A, const Point &B)
{
	return A.x * B.y - A.y * B.x;
}

bool rapidRejectionTest(const Point &A, const Point &B, const Point &C, const Point &D)
{
	return min(A.x, B.x) <= max(C.x, D.x) && min(C.x, D.x) <= max(A.x, B.x) &&
		min(A.y, B.y) <= max(C.y, D.y) && min(C.y, D.y) <= max(A.y, B.y);
}

bool isSegmentCross(const Point &A, const Point &B, const Point &C, const Point &D)
{
	if (!rapidRejectionTest(A, B, C, D)) return false;	
	Point AB(B.x - A.x, B.y - A.y);
	Point AC(C.x - A.x, C.y - A.y);
	Point AD(D.x - A.x, D.y - A.y);
	Point CA(A.x - C.x, A.y - C.y);
	Point CB(B.x - C.x, B.y - C.y);
	Point CD(D.x - C.x, D.y - C.y);
	if (crossProduct(AB, AC) * crossProduct(AB, AD) < 0 || crossProduct(CD, CA) * crossProduct(CD, CB)) return true;
	return false;
}
