// function of blob
#include "Blob.h"

Blob::Blob(vector<Point> c) 
{
	contour = c;
	boundingBox = boundingRect(contour);
	Point currentCenter;
	currentCenter.x = (boundingBox.x * 2 + boundingBox.width) / 2;
	currentCenter.y = (boundingBox.y * 2 + boundingBox.height) / 2;
	center.push_back(currentCenter);
	diagonalLength = sqrt(pow(boundingBox.width, 2) + pow(boundingBox.height, 2));
	ratio = 1.0 * boundingBox.width / boundingBox.height;
	isCurrentBlob = true;
	notMatchedFrameCnt = 0;
}

void Blob::predictNextCenter() 
{
	int num = (int)center.size();
	int xShifting = 0, yShifting = 0;
	int weightSum = (num - 1) * num / 2 > 0 ? (num - 1) * num / 2 : 1;
	int limit = num > 4 ? 4 : num - 1;
	cout << "num = " << num << endl;
	cout << "weightSum = " << weightSum << endl;
	cout << "limit = " << limit << endl;
	for (int weight = 1; weight <= limit; weight += 1)
	{
		xShifting += weight * (center[num - limit + weight - 1].x - center[num - limit + weight - 2].x);
		yShifting += weight * (center[num - limit + weight - 1].y - center[num - limit + weight - 2].y);
	}
	nextCenter.x = center.back().x + (int)round((float)xShifting / weightSum);
	nextCenter.y = center.back().y + (int)round((float)yShifting / weightSum);
}

double getDistance(Point p1, Point p2)
{
	return sqrt((double)(p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y));
}

void matchBlobs(vector<Blob> &existingBlobs, vector<Blob> &currentBlobs)
{
	for (auto &existingBlob : existingBlobs)
	{
		existingBlob.isCurrentBlob = false;
		existingBlob.predictNextCenter();
	}
	for (auto &currentBlob : currentBlobs)
	{
		int index = 0;
		double minDistance = 100000.0;	
		for (int i = 0; i < existingBlobs.size(); i++)  //get the closest blob
		{
			double dblDistance = getDistance(currentBlob.center.back(), existingBlobs[i].nextCenter);
			if (dblDistance < minDistance)
			{
				minDistance = dblDistance;
				index = i;
			}
		}
		if (minDistance < currentBlob.diagonalLength * 0.5)  //old blob
			updateBlob(currentBlob, existingBlobs, index);
		else //new blob
			addBlob(currentBlob, existingBlobs);
	}
	for (auto itr = existingBlobs.begin(); itr != existingBlobs.end();)
	{
		if (itr->isCurrentBlob == false) itr->notMatchedFrameCnt++; 
		if (itr->notMatchedFrameCnt >= 5) //erase disappeared blob
			itr = existingBlobs.erase(itr);
		else
			itr++;
	}
}

void updateBlob(Blob &currentBlob, vector<Blob> &existingBlobs, int &index)
{
	existingBlobs[index].contour = currentBlob.contour;
	existingBlobs[index].boundingBox = currentBlob.boundingBox;
	existingBlobs[index].center.push_back(currentBlob.center.back());
	existingBlobs[index].diagonalLength = currentBlob.diagonalLength;
	existingBlobs[index].ratio = currentBlob.ratio;
	existingBlobs[index].isCurrentBlob = true;
}

void addBlob(Blob &currentBlob, vector<Blob> &existingBlobs)
{
	currentBlob.isCurrentBlob = true;
	existingBlobs.push_back(currentBlob);
}

void showContours(Size size, vector<vector<Point> > contours, string windowName)
{
	Mat image(size, CV_8UC3, BLACK);
	drawContours(image, contours, -1, WHITE, -1);
	imshow(windowName, image);
}

void showContours(Size size, vector<Blob> blobs, string windowName)
{
	Mat image(size, CV_8UC3, BLACK);
	vector<vector<Point> > contours;
	for (auto &blob : blobs) contours.push_back(blob.contour);
	drawContours(image, contours, -1, WHITE, -1);
	imshow(windowName, image);
}

bool isCrossLine(vector<Blob> &blobs, int &countingLine, int &cnt)
{
	for (auto blob : blobs)
	{
		if (blob.center.size() >= 2)
		{
			int prevFrameIndex = (int)blob.center.size() - 2;
			int currFrameIndex = (int)blob.center.size() - 1;
			if (blob.center[prevFrameIndex].y > countingLine && blob.center[currFrameIndex].y <= countingLine)
			{
				cnt++;
				return true;
			}
		}
	}
	return false;
}

void drawBlob(vector<Blob> &blobs, Mat &frame2Copy)
{
	for (int i = 0; i < blobs.size(); i++) rectangle(frame2Copy, blobs[i].boundingBox, RED, lineThickness);
}

void drawCnt(int &cnt, Mat &frame2Copy)
{
	double fontScale = (frame2Copy.rows * frame2Copy.cols) / 300000.0;
	int fontThickness = (int)round(fontScale * 1.5);
	Size textSize = getTextSize(to_string(cnt), CV_FONT_HERSHEY_SIMPLEX, fontScale, fontThickness, 0);
	Point bottomLeftPosition;
	bottomLeftPosition.x = frame2Copy.cols - 1 - (int)((double)textSize.width * 1.25);
	bottomLeftPosition.y = (int)((double)textSize.height * 1.25);
	putText(frame2Copy, to_string(cnt), bottomLeftPosition, CV_FONT_HERSHEY_SIMPLEX, fontScale, GREEN, fontThickness);
}