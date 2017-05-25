// function of blob
#include "Blob.h"
#include "ComputationalGeometry.h"

void updateBlob(Blob &currentBlob, vector<Blob> &existingBlobs, int &index);                              //update existing blob
void addBlob(Blob &currentBlob, vector<Blob> &existingBlobs);                                             //add new blob

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
	boxColor = Scalar(rand() % 255, rand() % 255, rand() & 255);
}

void Blob::predictNextCenter() 
{
	int num = (int)center.size();
	int xShifting = 0, yShifting = 0;
	int weightSum = (num - 1) * num / 2 > 0 ? (num - 1) * num / 2 : 1;
	int limit = num > 4 ? 4 : num - 1;
	for (int weight = 1; weight <= limit; weight += 1)
	{
		xShifting += weight * (center[num - limit + weight - 1].x - center[num - limit + weight - 2].x);
		yShifting += weight * (center[num - limit + weight - 1].y - center[num - limit + weight - 2].y);
	}
	nextCenter.x = center.back().x + (int)round((float)xShifting / weightSum);
	nextCenter.y = center.back().y + (int)round((float)yShifting / weightSum);
	return;
}

void matchBlobs(vector<Blob> &existingBlobs, vector<Blob> &currentBlobs, Mat &frame2Copy)
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
		{
			if (currentBlob.center.back().x > frame2Copy.cols * leftBoundingCoefficient &&            //in detection area
				currentBlob.center.back().x < frame2Copy.cols * rightBoundingCoefficient  &&
				currentBlob.center.back().y > frame2Copy.rows * upBoundingCoefficient &&
				currentBlob.center.back().y < frame2Copy.rows * bottomBoundingCoefficient
			)
			addBlob(currentBlob, existingBlobs);
		}
	}
	for (auto itr = existingBlobs.begin(); itr != existingBlobs.end();)
	{
		if (itr->isCurrentBlob == false) itr->notMatchedFrameCnt++; 
		if (itr->notMatchedFrameCnt >= 5) //erase disappeared blob
			itr = existingBlobs.erase(itr);
		else
			itr++;
	}
	return;
}

void updateBlob(Blob &currentBlob, vector<Blob> &existingBlobs, int &index)
{
	existingBlobs[index].contour = currentBlob.contour;
	existingBlobs[index].boundingBox = currentBlob.boundingBox;
	existingBlobs[index].center.push_back(currentBlob.center.back());
	existingBlobs[index].diagonalLength = currentBlob.diagonalLength;
	existingBlobs[index].ratio = currentBlob.ratio;
	existingBlobs[index].isCurrentBlob = true;
	return;
}

void addBlob(Blob &currentBlob, vector<Blob> &existingBlobs)
{
	currentBlob.isCurrentBlob = true;
	existingBlobs.push_back(currentBlob);
	return;
}

void showContours(Size size, vector<vector<Point> > contours, string windowName)
{
	Mat image(size, CV_8UC3, BLACK);
	drawContours(image, contours, -1, WHITE, -1);
	imshow(windowName, image);
	return;
}

void showContours(Size size, vector<Blob> blobs, string windowName)
{
	Mat image(size, CV_8UC3, BLACK);
	vector<vector<Point> > contours;
	for (auto &blob : blobs) contours.push_back(blob.contour);
	drawContours(image, contours, -1, WHITE, -1);
	imshow(windowName, image);
	return;
}

bool isCrossLine(vector<Blob> &blobs, Point start, Point end, int &cnt)
{
	start.x = (int)(start.x / resizeWidthCoefficient);
	start.y = (int)(start.y / resizeHeightCoefficient);
	end.x = (int)(end.x / resizeWidthCoefficient);
	end.y = (int)(end.y / resizeHeightCoefficient);
	for (auto blob : blobs)
	{
		if (blob.center.size() >= 2)
		{
			Point preFrameCenter = blob.center[blob.center.size() - 2];
			Point curFrameCenter = blob.center[blob.center.size() - 1];
			if (isSegmentCross(start, end, preFrameCenter, curFrameCenter)) 
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
	for (auto blob : blobs)
	{
		Rect r = blob.boundingBox;
		r.x = (int)(r.x * resizeWidthCoefficient);
		r.width = (int)(r.width * resizeWidthCoefficient);
		r.y = (int)(r.y * resizeHeightCoefficient);
		r.height = (int)(r.height * resizeHeightCoefficient);
		rectangle(frame2Copy, r, blob.boxColor, lineThickness);
	}
	return;
}

void drawCnt(vector<int> &cnt, Mat &frame2Copy)
{
	double fontScale = (frame2Copy.rows * frame2Copy.cols) / 800000.0;
	int fontThickness = (int)round(fontScale * 1.5);
	for (int i = 0; i < cnt.size(); i++)
	{
		string text("line " + to_string(i + 1) + ": " + to_string(cnt[i]));
		Size textSize = getTextSize(text, CV_FONT_HERSHEY_SIMPLEX, fontScale, fontThickness, 0);
		Point position;
		position.x = frame2Copy.cols - 1 - (int)((double)textSize.width * 1.1);
		position.y = (int)((double)textSize.height * 1.25 * (i + 1));
		putText(frame2Copy, text, position, CV_FONT_HERSHEY_SIMPLEX, fontScale, GREEN, fontThickness);
	}
	return;
}

bool isOverlapped(Rect box1, Rect box2)
{
	Point tl1 = box1.tl(), br1 = box1.br();
	Point tl2 = box2.tl(), br2 = box2.br();
	if (tl1.x > br2.x) return false; //disjoint from each other
	if (tl1.y > br2.y) return false;
	if (tl2.x > br1.x) return false;
	if (tl2.y > br1.y) return false;
	int colInt = min(br1.x, br2.x) - max(tl1.x, tl2.x);
	int rowInt = min(br1.y, br2.y) - max(tl1.y, tl2.y);
	int intersection = colInt * rowInt;
	if ((float)intersection / box1.area() > IOUThreshold) return true;

	return false;
}
