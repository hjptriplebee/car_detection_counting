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
const int maxBlobArea = 10000;
const int minBlobWidth = 10;
const int maxBlobWidth = 150;
const int minBlobheight = 10;
const int maxBlobheight = 150;
const int minBlobDiagonal = 10;
const int maxBlobDiagonal = 150;
const int maxBlobNum = 500;                      //skip noisy frame
const double minBlobRatio = 0.2;
const double maxBlobRatio = 4;
const double leftBoundingCoefficient = 0;
const double rightBoundingCoefficient = 1;
const double upBoundingCoefficient = 0;
const double bottomBoundingCoefficient = 1;
const double IOUThreshold = 0.7;
const double resizeHeightCoefficient = 1;
const double resizeWidthCoefficient = 1;

class Blob 
{
private:
	vector<Point> contour;                       //contour
	Rect boundingBox;                            //bounding box	
	vector<Point> center;                        //center point
	double diagonalLength;                       //diagonal length
	double ratio;                                //width / height
	bool isCurrentBlob;                          //is current blob£¿
	vector<bool> isCounted;                      //is counted by lines?
	int numOfCrossedCountingLine;                //num of crossed counting line
	Point nextCenter;                            //next center
	Scalar boxColor;                             //boundingbox color
	int notMatchedFrameCnt;                      //how long it has't been matched	

public:		
	Blob(vector<Point> c, int countingLineNum);  //init
	vector<Point> getCenter();                   //get center
	vector<Point> getContour();                  //get contour
	vector<bool> getIsCounted();                 //get is counted
	double getDiagonalLength();                  //get diagonal length
	double getRatio();                           //get ratio
	bool getIsCurrentBlob();                     //get is current blob
	int getNotMatchedFrameCnt();                 //get not matched frame cnt
	Rect getBoundingBox();                       //get boundingbox
	Point getNextCenter();                       //get next center
	Scalar getBoxColor();                        //get box color
	int getNumOfCrossedCountingLine();           //get num of crossed counting line

	void addNumOfCrossedCountingLine(int add);   //add num of crossed counting line
	void changeIsCountedToTrue(int index);       //is counted to true
	void changeIsCurrentBlobToFalse();           //is current blob to false
	void addNotMatchedFrameCnt();                //add not matched frame cnt
	void zeroNotMatchedFrameCnt();               //set not matched frame cnt to zero
	void predictNextCenter();                    //predict next center  
	void updateBlob(Blob &currentBlob);          //update blob by current blob
};

void matchBlobs(vector<Blob> &existingBlobs, vector<Blob> &currentBlobs, Mat &frame2Copy);                //match existing blob and current blob
void showContours(Size size, vector<vector<Point> > contours, string windowName);                         //show contours in point vector
void showContours(Size size, vector<Blob> blobs, string windowName);                                      //show contours in blobs
bool isCrossLine(vector<Blob> &blobs, Point start, Point end, int &cnt, int &index, vector<vector<int>> &total);       //judge if the blob has crossed the counting line
void drawBlob(vector<Blob> &blobs, Mat &frame2Copy);                                                      //draw bounding box
void drawCnt(vector<int> &cnt, Mat &frame2Copy, vector<Scalar> &crossingLineColor);                                                          //draw counting result
bool isOverlapped(Rect box1, Rect box2);                                                                  //judge if bounding box is overlapped

#endif 