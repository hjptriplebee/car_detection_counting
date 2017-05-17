// definition of blob class and some functions
#ifndef BLOB
#define BLOB

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
const int minBlobArea = 500;
const int maxBlobArea = 100000;
const int minBlobWidth = 30;
const int maxBlobWidth = 500;
const int minBlobheight = 30;
const int maxBlobheight = 500;
const int minBlobDiagonal = 60;
const int maxBlobDiagonal = 1000;
const double minBlobRatio = 0.2;
const double maxBlobRatio = 4;
const double leftBounding = 0.25;
const double rightBounding = 0.75;
const double upBounding = 0.25;
const double bottomBounding = 1;

class Blob 
{
public:
	vector<Point> contour;                       //contour
	Rect boundingBox;                            //bounding box
	vector<Point> center;                        //center point
	double diagonalLength;                       //diagonal length
	double ratio;                                //width / height
	bool isCurrentBlob;                          //is current blob£¿
	int notMatchedFrameCnt;                      //how long it has't been matched
	Point nextCenter;                            //next center

	Blob(vector<Point> c);                       //init
	void predictNextCenter();                    //predict next center  
};

double getDistance(Point point1, Point point2);                                      //cal distance between two points
void matchBlobs(vector<Blob> &existingBlobs, vector<Blob> &currentBlobs);            //match existing blob and current blob
void updateBlob(Blob &currentBlob, vector<Blob> &existingBlobs, int &index);         //update existing blob
void addBlob(Blob &currentBlob, vector<Blob> &existingBlobs);                        //add new blob
void showContours(Size size, vector<vector<Point> > contours, string windowName);    //show contours in point vector
void showContours(Size size, vector<Blob> blobs, string windowName);                 //show contours in blobs
bool isCrossLine(vector<Blob> &blobs, int &countingLine, int &cnt);                  //judge if the blob has crossed the counting line
void drawBlob(vector<Blob> &blobs, Mat &frame2Copy);                                 //draw bounding box
void drawCnt(int &cnt, Mat &frame2Copy);                                             //draw counting result

#endif 


