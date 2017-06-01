// function of blob
#include "Blob.h"
#include "ComputationalGeometry.h"

void addBlob(Blob &currentBlob, vector<Blob> &existingBlobs);                                             //add new blob

Blob::Blob(vector<Point> c, int countingLineNum)
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
	vector<bool> temp(countingLineNum, false);
	isCounted = temp;
}

/*== == == == == == == == == == = get function of blob == == == == == == == == == == == == == == =*/

Rect Blob::getBoundingBox()
{
	return boundingBox;
}

vector<Point> Blob::getCenter()
{
	return center;
}

vector<Point> Blob::getContour()
{
	return contour;
}

double Blob::getDiagonalLength()
{
	return diagonalLength;
}

double Blob::getRatio()
{
	return ratio;
}

bool Blob::getIsCurrentBlob()
{
	return isCurrentBlob;
}

vector<bool> Blob::getIsCounted()
{
	return isCounted;
}

Point Blob::getNextCenter()
{
	return nextCenter;
}

Scalar Blob::getBoxColor()
{
	return boxColor;
}

int Blob::getNotMatchedFrameCnt()
{
	return notMatchedFrameCnt;
}

/*== == == == == == == == == == = change function of blob == == == == == == == == == == == == == == =*/
void Blob::changeIsCountedToTrue(int index)
{
	isCounted[index] = true;
}

void Blob::changeIsCurrentBlobToFalse()
{
	isCurrentBlob = false;
}

void Blob::addNotMatchedFrameCnt()
{
	notMatchedFrameCnt++;
}

void Blob::zeroNotMatchedFrameCnt()
{
	notMatchedFrameCnt = 0;
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

void Blob::updateBlob(Blob &currentBlob)
{
	contour = currentBlob.contour;
	boundingBox = currentBlob.boundingBox;
	center.push_back(currentBlob.center.back());
	diagonalLength = currentBlob.diagonalLength;
	ratio = currentBlob.ratio;
	isCurrentBlob = true;
	return;
}

/*== == == == == == == == == == = outer function == == == == == == == == == == == == == == =*/
void matchBlobs(vector<Blob> &existingBlobs, vector<Blob> &currentBlobs, Mat &frame2Copy)
{
	for (auto &existingBlob : existingBlobs)
	{
		existingBlob.changeIsCurrentBlobToFalse();
		existingBlob.predictNextCenter();
	}
	for (auto &currentBlob : currentBlobs)
	{
		int index = 0;
		double minDistance = 1000000.0;	
		for (int i = 0; i < existingBlobs.size(); i++)  //get the closest blob
		{
			double dblDistance = getDistance(currentBlob.getCenter().back(), existingBlobs[i].getNextCenter());
			if (dblDistance < minDistance)
			{
				minDistance = dblDistance;
				index = i;
			}
		}
		if (minDistance < currentBlob.getDiagonalLength() * 0.4)  //old blob
			existingBlobs[index].updateBlob(currentBlob);
		else //new blob
		{
			vector<Point> currentBlobCenter = currentBlob.getCenter();
			if (currentBlobCenter.back().x > frame2Copy.cols * leftBoundingCoefficient &&            //in detection area
				currentBlobCenter.back().x < frame2Copy.cols * rightBoundingCoefficient  &&
				currentBlobCenter.back().y > frame2Copy.rows * upBoundingCoefficient &&
				currentBlobCenter.back().y < frame2Copy.rows * bottomBoundingCoefficient
			)
			addBlob(currentBlob, existingBlobs);
		}
	}
	for (auto itr = existingBlobs.begin(); itr != existingBlobs.end();)
	{
		if (itr->getIsCurrentBlob() == false) 
			itr->addNotMatchedFrameCnt(); 
		else
			itr->zeroNotMatchedFrameCnt();
		if (itr->getNotMatchedFrameCnt() >= 5) //erase disappeared blob
			itr = existingBlobs.erase(itr);
		else
			itr++;
	}
	return;
}

void addBlob(Blob &currentBlob, vector<Blob> &existingBlobs)
{
	currentBlob.changeIsCurrentBlobToFalse();
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
	for (auto &blob : blobs) contours.push_back(blob.getContour());
	drawContours(image, contours, -1, WHITE, -1);
	imshow(windowName, image);
	return;
}

bool isCrossLine(vector<Blob> &blobs, Point start, Point end, int &cnt, int &index)
{
	start.x = (int)(start.x / resizeWidthCoefficient);
	start.y = (int)(start.y / resizeHeightCoefficient);
	end.x = (int)(end.x / resizeWidthCoefficient);
	end.y = (int)(end.y / resizeHeightCoefficient);
	bool flag = false;
	for (auto &blob : blobs)
	{
		vector<bool> blobIsCounted = blob.getIsCounted();
		if (blob.getCenter().size() >= 2 && !blobIsCounted[index] && blob.getIsCurrentBlob())
		{
			vector<Point> blobCenter = blob.getCenter();
			Point preFrameCenter = blobCenter[blobCenter.size() - 2];
			Point curFrameCenter = blobCenter[blobCenter.size() - 1];
			if (isSegmentCross(start, end, preFrameCenter, curFrameCenter)) 
			{
				cnt++;
				blob.changeIsCountedToTrue(index);
				flag = true;
			}
		}
	}
	return flag;
}

void drawBlob(vector<Blob> &blobs, Mat &frame2Copy)
{
	for (auto blob : blobs)
	{
		if (!blob.getIsCurrentBlob()) continue;
		Rect r = blob.getBoundingBox();
		r.x = (int)(r.x * resizeWidthCoefficient);
		r.width = (int)(r.width * resizeWidthCoefficient);
		r.y = (int)(r.y * resizeHeightCoefficient);
		r.height = (int)(r.height * resizeHeightCoefficient);
		rectangle(frame2Copy, r, blob.getBoxColor(), lineThickness);
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

int mergeContour(const vector<std::vector<cv::Point>> srcContour, vector<std::vector<cv::Point>> &dstContour, double gap, int size) 
{
	vector<std::vector<cv::Point>> tempDst;
	int needAgain(0);
	int srcSize = srcContour.size();
	if (srcSize == 0) 
	{
		return -1;
	}
	tempDst.push_back(srcContour[0]);

	for (int srcIndex = 1; srcIndex < srcSize; srcIndex++) 
	{
		int isMerge(0);
		if (contourArea(srcContour[srcIndex]) < size)
			continue;
		for (int dstIndex = 0; ((dstIndex < tempDst.size()) && (isMerge != 1)); dstIndex++) 
		{
			for (int srcPoint = 0; ((srcPoint < srcContour[srcIndex].size()) && (isMerge != 1)); srcPoint++) 
			{				
				for (int dstPoint = 0; ((dstPoint < tempDst[dstIndex].size()) && (isMerge != 1)); dstPoint++) 
				{
					if (pointPolygonTest(tempDst[dstIndex], srcContour[srcIndex][srcPoint], false) != -1) 
					{
						isMerge = 1;
						break;
					}
					double srcX = srcContour[srcIndex][srcPoint].x;
					double srcY = srcContour[srcIndex][srcPoint].y;
					double dstX = tempDst[dstIndex][dstPoint].x;
					double dstY = tempDst[dstIndex][dstPoint].y;
					if (sqrt(pow(abs(dstX - srcX), 2) + pow(abs(dstY - srcY), 2)) < gap)
					{
						isMerge = 1;
						break;
					}
				} //dstPoint
			}  //srcPoint

			if (isMerge == 1) 
			{
				for (int srcPointCount = 0; srcPointCount < srcContour[srcIndex].size(); srcPointCount++) 
				{
					tempDst[dstIndex].push_back(srcContour[srcIndex][srcPointCount]);
				}
				std::vector<cv::Point>temp;
				convexHull(tempDst[dstIndex], temp);
				tempDst[dstIndex].clear();
				tempDst[dstIndex].assign(temp.begin(), temp.end());
				needAgain = 1;
				break;
			}
		}  //dst index
		if (isMerge == 0) 
		{
			tempDst.push_back(srcContour[srcIndex]);
		}
	} //src index
	if (needAgain) 
	{
		mergeContour(tempDst, dstContour, gap, size);
	}
	else 
	{
		dstContour = tempDst;
	}
	return 1;
}
