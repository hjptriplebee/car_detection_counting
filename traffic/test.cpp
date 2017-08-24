// main.cpp
#include "Blob.h"
#define SHOW_FLAG true
#define DEBUG_BY_STEP false
#define SAVE_VIDEO false

void carCounting(vector<pair<double, double>> &crossingLineStart, vector<pair<double, double>> &crossingLineEnd, vector<Scalar> &crossingLineColor, vector<vector<int>> &total)
{
	/*== == == == == == == == == == = step1 Variable definition and Initialization == == == == == == == == == == == == == == =*/
	VideoCapture capture;
	Mat frame1, frame2;                                                 //two frames for substract	 
	vector<Blob> blobs;	                                                //blobs
	vector<Point> lineStart, lineEnd;                                   //crossing line start points and end points
	vector<int> cnt(crossingLineStart.size(), 0);                       //number of cars cross each line
	int frameCnt = 2;                                                   //number of frames	
	char keyValue = 0;                                                  //key you press

	//capture.open("E:\\traffic\\交通路口流量统计\\交通路口流量统计\\华池街.wmv");
	//capture.open("E:\\traffic\\OpenCV_3_Car_Counting_Cpp-master\\CarsDrivingUnderBridge.mp4");	
	capture.open("E:\\traffic\\苏交科视频\\无人机\\无人机2\\视频\\9.MP4");
	if (!capture.isOpened()) //cannot open the video
	{                                                
		cout << "can't open the video！" << endl;      
		system("pause");
		return;                                                           
	}
	//for (int i = 0; i < 1000; i++)
	//	capture.read(frame1);
	capture.read(frame1);
	capture.read(frame2);

	VideoWriter writer("E:\\traffic\\OpenCV_3_Car_Counting_Cpp-master\\demox.avi", CV_FOURCC('D', 'I', 'V', 'X'), 29, Size(frame2.cols, frame2.rows), true);

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
		threshold(imgDifference, imgThresh, 20, 255.0, CV_THRESH_BINARY); //binarization
		//adaptiveThreshold(imgDifference, imgThresh, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY_INV, 25, 9);
		if (SHOW_FLAG) imshow("imgThresh", imgThresh);

		Mat structuringElement3x3   = getStructuringElement(MORPH_RECT, Size(3, 3)); //definition of different kernels, big kernel for noisy image
		Mat structuringElement5x5   = getStructuringElement(MORPH_RECT, Size(5, 5));
		//Mat structuringElement7x7   = getStructuringElement(MORPH_RECT, Size(7, 7));
		//Mat structuringElement15x15 = getStructuringElement(MORPH_RECT, Size(15, 15));

		for (int i = 0; i < 3; i++)
		{
			//dilate(imgThresh, imgThresh, structuringElement3x3, Point(-1, -1), 2);
			//erode(imgThresh, imgThresh, structuringElement3x3);			
		}
		dilate(imgThresh, imgThresh, structuringElement3x3, Point(-1, -1), 2);
		//for (int i = 0; i < 1; i++)
		//	erode(imgThresh, imgThresh, structuringElement3x3);

		Mat imgThreshCopy = imgThresh.clone();
		vector<vector<Point> > contours; //contours
		findContours(imgThreshCopy, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
		if (SHOW_FLAG) showContours(imgThresh.size(), contours, "imgContours");

		vector<vector<Point> > convexHulls(contours.size());

		for (int i = 0; i < contours.size(); i++) convexHull(contours[i], convexHulls[i]);

		if (SHOW_FLAG) showContours(imgThresh.size(), convexHulls, "imgConvexHulls");
		for (auto &convexHull : convexHulls) //filter with heuristic knowledge 
		{
			Blob possibleBlob(convexHull, (int)lineStart.size());
			Rect possibleBoundingBox = possibleBlob.getBoundingBox();
			if (
				possibleBoundingBox.area() > minBlobArea &&
				possibleBoundingBox.area() < maxBlobArea &&
				possibleBlob.getRatio() > minBlobRatio &&
				possibleBlob.getRatio() < maxBlobRatio &&
				possibleBoundingBox.width > minBlobWidth &&
				possibleBoundingBox.width < maxBlobWidth &&
				possibleBoundingBox.height > minBlobheight &&
				possibleBoundingBox.height < maxBlobheight &&
				possibleBlob.getDiagonalLength() > minBlobDiagonal &&
				possibleBlob.getDiagonalLength() < maxBlobDiagonal &&
				(contourArea(possibleBlob.getContour()) / (double)possibleBoundingBox.area()) > 0.50        //contour area / rect area 
			)
				tempBlobs.push_back(possibleBlob);	
		}
		
		for (int i = 0, j ; i < tempBlobs.size(); i++) //filter with inclusion
		{
			for (j = 0; j < tempBlobs.size(); j++) 
				if (j != i && isOverlapped(tempBlobs[i].getBoundingBox(), tempBlobs[j].getBoundingBox())) break; //is covered
			if (j == tempBlobs.size())
				currentBlobs.push_back(tempBlobs[i]);			
		}
		//if (currentBlobs.size() > maxBlobNum) continue;
		if (SHOW_FLAG) showContours(imgThresh.size(), currentBlobs, "imgcurrentBlobs");

		matchBlobs(blobs, currentBlobs, frame2Copy);
		if (SHOW_FLAG) showContours(imgThresh.size(), blobs, "imgBlobs");

		/*== == == == == == == == == == = step2.3 counting and draw blobs == == == == == == == == == == == == == == =*/
		frame2Copy = frame2.clone();
		drawBlob(blobs, frame2Copy);
		for (int i = 0; i < lineStart.size(); i++)
		{
			if (isCrossLine(blobs, lineStart[i], lineEnd[i], cnt[i], i, total)) //some blob has crossed the line
				line(frame2Copy, lineStart[i], lineEnd[i], GREEN, lineThickness);
			else
				line(frame2Copy, lineStart[i], lineEnd[i], crossingLineColor[i], lineThickness);
		}
		if (SHOW_FLAG)
		{
			for (auto t : total)
			{
				cout << "right：" << t[0] << "  straight：" << t[1] << "  left：" << t[2] << endl;
			}
			cout << endl;
		}
		drawCnt(cnt, frame2Copy, crossingLineColor);
		
		if (SAVE_VIDEO) writer << frame2Copy;
		///////temp
		Mat frame2Copy2;
		resize(frame2Copy, frame2Copy2, Size((int)(frame1.cols / 2), (int)(frame1.rows / 2))); //resize
		imshow("frame2Copy", frame2Copy2);

		/*== == == == == == == == == == = step2.4 prepare for the next iteration == == == == == == == == == == == == == == =*/
		currentBlobs.clear();
		frame1 = frame2.clone();    
		if ((capture.get(CV_CAP_PROP_POS_FRAMES) + 1) < capture.get(CV_CAP_PROP_FRAME_COUNT)) //has next frame
			capture.read(frame2);
		else 
			break;

		frameCnt++;
		keyValue = waitKey(1);
		if (DEBUG_BY_STEP) waitKey(0);
	}
	if (SAVE_VIDEO) writer.release();
	cout << "按ESC退出！" << endl;
	if (keyValue != 27) waitKey(0);                         // press ESC to escape
	return;
}

int main()
{
	vector<pair<double, double>> crossingLineStart;                      //represent counting line
	vector<pair<double, double>> crossingLineEnd;
	vector<Scalar> crossingLineColor;  
	vector<int> threeDirection(3, 0);
	vector<vector<int>> total(4, threeDirection);
	//line1
	pair<double, double> downOutStart(0.43, 0.75);
	pair<double, double> downOutEnd(0.455, 0.75);
	crossingLineStart.push_back(downOutStart);
	crossingLineEnd.push_back(downOutEnd);
	crossingLineColor.push_back(Scalar(rand() % 255, rand() % 255, rand() & 255));
	//line2
	pair<double, double> downInStart(0.46, 0.61);
	pair<double, double> downInEnd(0.50, 0.59);
	crossingLineStart.push_back(downInStart);
	crossingLineEnd.push_back(downInEnd);
	crossingLineColor.push_back(Scalar(rand() % 255, rand() % 255, rand() & 255));

	//line3
	pair<double, double> rightOutStart(0.54, 0.444);
	pair<double, double> rightOutEnd(0.55, 0.49);
	crossingLineStart.push_back(rightOutStart);
	crossingLineEnd.push_back(rightOutEnd);
	crossingLineColor.push_back(Scalar(rand() % 255, rand() % 255, rand() & 255));
	//line4
	pair<double, double> rightInStart(0.515, 0.385);
	pair<double, double> rightInEnd(0.525, 0.45);
	crossingLineStart.push_back(rightInStart);
	crossingLineEnd.push_back(rightInEnd);
	crossingLineColor.push_back(Scalar(rand() % 255, rand() % 255, rand() & 255));

	//line5
	pair<double, double> upOutStart(0.515, 0.385);
	pair<double, double> upOutEnd(0.47, 0.385);
	crossingLineStart.push_back(upOutStart);
	crossingLineEnd.push_back(upOutEnd);
	crossingLineColor.push_back(Scalar(rand() % 255, rand() % 255, rand() & 255));
	//line6
	pair<double, double> upInStart(0.464, 0.40);
	pair<double, double> upInEnd(0.42, 0.42);
	crossingLineStart.push_back(upInStart);
	crossingLineEnd.push_back(upInEnd);
	crossingLineColor.push_back(Scalar(rand() % 255, rand() % 255, rand() & 255));

	//line7
	pair<double, double> leftOutStart(0.34, 0.51);
	pair<double, double> leftOutEnd(0.35, 0.56);
	crossingLineStart.push_back(leftOutStart);
	crossingLineEnd.push_back(leftOutEnd);
	crossingLineColor.push_back(Scalar(rand() % 255, rand() % 255, rand() & 255));
	//line8
	pair<double, double> leftInStart(0.40, 0.53);
	pair<double, double> leftInEnd(0.41, 0.60);
	crossingLineStart.push_back(leftInStart);
	crossingLineEnd.push_back(leftInEnd);
	crossingLineColor.push_back(Scalar(rand() % 255, rand() % 255, rand() & 255));
	

	carCounting(crossingLineStart, crossingLineEnd, crossingLineColor, total);
	return 0;
}