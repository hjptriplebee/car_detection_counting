// main.cpp
#include "Blob.h"
#define SHOW_FLAG false
#define DEBUG_BY_STEP false
void carCounting(vector<pair<double, double>> &crossingLineStart, vector<pair<double, double>> &crossingLineEnd)
{
	/*== == == == == == == == == == = step1 Variable definition and Initialization == == == == == == == == == == == == == == =*/
	VideoCapture capture;
	Mat frame1, frame2;                                                 //two frames for substract	 
	vector<Blob> blobs;	                                                //blobs
	vector<Point> lineStart, lineEnd;                                   //crossing line start points and end points
	vector<int> cnt(crossingLineStart.size(), 0);                       //number of cars cross each line
	int frameCnt = 2;                                                   //number of frames	
	char keyValue = 0;                                                  //key you press
	bool firstFlag = true;                                              //is first frame?

	//capture.open("E:\\traffic\\交通路口流量统计\\交通路口流量统计\\华池街.wmv");
	//capture.open("E:\\traffic\\OpenCV_3_Car_Counting_Cpp-master\\CarsDrivingUnderBridge.mp4");
	capture.open("E:\\traffic\\交通路口流量统计\\交通视频-金科\\[0001070]-[2017-02-13_18_40_13]-[2017-02-13_19_10_13].mkv");
	if (!capture.isOpened()) //cannot open the video
	{                                                
		cout << "can't open the video！" << endl;      
		system("pause");
		return;                                                           
	}
	capture.read(frame1);
	capture.read(frame2);

	for (int i = 0; i < crossingLineStart.size(); i++)       //scale to coordinate
	{
		int x = (int)((frame1.cols - 1) * crossingLineStart[i].first);
		int y = (int)((frame1.rows - 1) * crossingLineStart[i].second);
		Point start(x, y);
		lineStart.push_back(start);

		x = (int)((frame1.cols - 1) * crossingLineEnd[i].first);
		y = (int)((frame1.rows - 1) * crossingLineEnd[i].second);
		Point end(x, y);

		lineEnd.push_back(end);
	}

	/*== == == == == == == == == == = step2 process of car counting == == == == == == == == == == == == == == =*/
	while (capture.isOpened() && keyValue != 27) 
	{
		/*== == == == == == == == == == = step2.1 Variable definition and Initialization == == == == == == == == == == == == == == =*/
		vector<Blob> currentBlobs;             //blob in current frame
		vector<Blob> tempBlobs;                //temp blob      
		Mat frame1Copy, frame2Copy;            //copy completely
		Mat imgDifference;                     //frame difference
		Mat imgThresh;                         //binarization

		/*== == == == == == == == == == = step2.2 denoise and find contours == == == == == == == == == == == == == == =*/
		resize(frame1, frame1Copy, Size((int)(frame1.cols / resizeWidthCoefficient), (int)(frame1.rows / resizeHeightCoefficient))); //resize
		resize(frame2, frame2Copy, Size((int)(frame2.cols / resizeWidthCoefficient), (int)(frame2.rows / resizeHeightCoefficient)));
		cvtColor(frame1Copy, frame1Copy, CV_BGR2GRAY);                    //to gray
		cvtColor(frame2Copy, frame2Copy, CV_BGR2GRAY);
		GaussianBlur(frame1Copy, frame1Copy, Size(5, 5), 0);              //GaussianBlur for smoothing
		GaussianBlur(frame2Copy, frame2Copy, Size(5, 5), 0);
		absdiff(frame1Copy, frame2Copy, imgDifference);                   //frame difference
		//GaussianBlur(imgDifference, imgDifference, Size(5, 5), 0);
		//threshold(imgDifference, imgThresh, 30, 255.0, CV_THRESH_BINARY); //binarization
		adaptiveThreshold(imgDifference, imgThresh, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY_INV, 25, 10);
		if (SHOW_FLAG) imshow("imgThresh", imgThresh);

		Mat structuringElement3x3   = getStructuringElement(MORPH_RECT, Size(3, 3)); //definition of different kernels, big kernel for noisy image
		Mat structuringElement5x5   = getStructuringElement(MORPH_RECT, Size(5, 5));
		//Mat structuringElement7x7   = getStructuringElement(MORPH_RECT, Size(7, 7));
		//Mat structuringElement15x15 = getStructuringElement(MORPH_RECT, Size(15, 15));

		for (int i = 0; i < 2; i++)
		{
			dilate(imgThresh, imgThresh, structuringElement3x3, Point(-1, -1), 2);
			erode(imgThresh, imgThresh, structuringElement5x5);			
		}

		Mat imgThreshCopy = imgThresh.clone();
		vector<vector<Point> > contours; //contours
		findContours(imgThreshCopy, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
		if (SHOW_FLAG) showContours(imgThresh.size(), contours, "imgContours");

		vector<vector<Point> > convexHulls(contours.size());

		for (int i = 0; i < contours.size(); i++) convexHull(contours[i], convexHulls[i]);

		if (SHOW_FLAG) showContours(imgThresh.size(), convexHulls, "imgConvexHulls");

		for (auto &convexHull : convexHulls) //filter with heuristic knowledge 
		{
			Blob possibleBlob(convexHull);
			if (
				possibleBlob.boundingBox.area() > minBlobArea &&
				possibleBlob.boundingBox.area() < maxBlobArea &&
				possibleBlob.ratio > minBlobRatio &&
				possibleBlob.ratio < maxBlobRatio &&
				possibleBlob.boundingBox.width > minBlobWidth &&
				possibleBlob.boundingBox.width < maxBlobWidth &&
				possibleBlob.boundingBox.height > minBlobheight &&
				possibleBlob.boundingBox.height < maxBlobheight &&
				possibleBlob.diagonalLength > minBlobDiagonal &&
				possibleBlob.diagonalLength < maxBlobDiagonal &&
				(contourArea(possibleBlob.contour) / (double)possibleBlob.boundingBox.area()) > 0.50        //contour area / rect area 
			)
				tempBlobs.push_back(possibleBlob);	
		}

		for (int i = 0, j ; i < tempBlobs.size(); i++) //filter with inclusion
		{
			for (j = 0; j < tempBlobs.size(); j++) 
				if(j != i && isOverlapped(tempBlobs[i].boundingBox, tempBlobs[j].boundingBox)) break; //is covered
			if (j == tempBlobs.size())
				currentBlobs.push_back(tempBlobs[i]);			
		}
		if (SHOW_FLAG) showContours(imgThresh.size(), currentBlobs, "imgcurrentBlobs");

		if (firstFlag == true) //is first frame
			for (auto &currentBlob : currentBlobs) blobs.push_back(currentBlob);
		else 
			matchBlobs(blobs, currentBlobs, frame2Copy);
		if (SHOW_FLAG) showContours(imgThresh.size(), blobs, "imgBlobs");

		/*== == == == == == == == == == = step2.3 counting and draw blobs == == == == == == == == == == == == == == =*/
		frame2Copy = frame2.clone();
		drawBlob(blobs, frame2Copy);
		
		for (int i = 0; i < lineStart.size(); i++)
		{
			if (isCrossLine(blobs, lineStart[i], lineEnd[i], cnt[i])) //some blob has crossed the line
				line(frame2Copy, lineStart[i], lineEnd[i], GREEN, lineThickness);
			else
				line(frame2Copy, lineStart[i], lineEnd[i], RED, lineThickness);
		}
		drawCnt(cnt, frame2Copy);

		imshow("frame2Copy", frame2Copy);

		/*== == == == == == == == == == = step2.4 prepare for the next iteration == == == == == == == == == == == == == == =*/
		currentBlobs.clear();
		frame1 = frame2.clone();    
		if ((capture.get(CV_CAP_PROP_POS_FRAMES) + 1) < capture.get(CV_CAP_PROP_FRAME_COUNT)) //has next frame
			capture.read(frame2);
		else 
			break;
		firstFlag = false;
		frameCnt++;
		keyValue = waitKey(1);
		if (DEBUG_BY_STEP) waitKey(0);
	}
	cout << "按任意键退出！" << endl;
	if (keyValue != 27) waitKey(0);                         //don't press ESC
	return;
}

int main()
{
	vector<pair<double, double>> crossingLineStart;                      //represent counting line
	vector<pair<double, double>> crossingLineEnd;
	pair<double, double> straightStart(0.25, 0.5);
	pair<double, double> straightEnd(0.75, 0.5);
	pair<double, double> leftStart(0.25, 0);
	pair<double, double> leftEnd(0.25, 0.5);
	pair<double, double> rightStart(0.75, 0);
	pair<double, double> rightEnd(0.75, 0.5);

	crossingLineStart.push_back(straightStart);
	crossingLineStart.push_back(leftStart);
	crossingLineStart.push_back(rightStart);
	crossingLineEnd.push_back(straightEnd);
	crossingLineEnd.push_back(leftEnd);
	crossingLineEnd.push_back(rightEnd);

	carCounting(crossingLineStart, crossingLineEnd);
	return 0;
}