// definition of blob class and some functions
#ifndef BLOB_H
#define BLOB_H

#include<iostream>
#include<stdlib.h> 
#include<math.h>
#include<vector>
using namespace std;

#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
using namespace cv;

const Scalar BLACK = Scalar(  0,   0,   0);
const Scalar WHITE = Scalar(255, 255, 255);
const Scalar GREEN = Scalar(  0, 255,   0);
const Scalar RED   = Scalar(  0,   0, 255);
const int lineThickness = 3;
const int minBlobArea = 100;
const int maxBlobArea = 100000;
const int minBlobWidth = 20;
const int maxBlobWidth = 500;
const int minBlobheight = 20;
const int maxBlobheight = 500;
const int minBlobDiagonal = 60;
const int maxBlobDiagonal = 1000;
const double minBlobRatio = 0.2;
const double maxBlobRatio = 4;
const double leftBoundingCoefficient = 0.1;
const double rightBoundingCoefficient = 1;
const double upBoundingCoefficient = 0.6;
const double bottomBoundingCoefficient = 1;
const double IOUThreshold = 0.7;
const double resizeHeightCoefficient = 2;
const double resizeWidthCoefficient = 2;

class Blob 
{
public:
	vector<Point> contour;                       //contour
	Rect boundingBox;                            //bounding box
	vector<Point> center;                        //center point
	double diagonalLength;                       //diagonal length
	double ratio;                                //width / height
	bool isCurrentBlob;                          //is current blob£¿
	vector<bool> isCounted;                      //is counted by lines?
	int notMatchedFrameCnt;                      //how long it has't been matched
	Point nextCenter;                            //next center
	Scalar boxColor;                             //boundingbox color

	Blob(vector<Point> c, int countingLineNum);  //init
	void predictNextCenter();                    //predict next center  
};

void matchBlobs(vector<Blob> &existingBlobs, vector<Blob> &currentBlobs, Mat &frame2Copy);                //match existing blob and current blob
void showContours(Size size, vector<vector<Point> > contours, string windowName);                         //show contours in point vector
void showContours(Size size, vector<Blob> blobs, string windowName);                                      //show contours in blobs
bool isCrossLine(vector<Blob> &blobs, Point start, Point end, int &cnt, int &index);                      //judge if the blob has crossed the counting line
void drawBlob(vector<Blob> &blobs, Mat &frame2Copy);                                                      //draw bounding box
void drawCnt(vector<int> &cnt, Mat &frame2Copy);                                                          //draw counting result
bool isOverlapped(Rect box1, Rect box2);                                                                  //judge if bounding box is overlapped

#endif 