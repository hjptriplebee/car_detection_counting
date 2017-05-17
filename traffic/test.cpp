// main.cpp
#include "Blob.h"
#define SHOW_FLAG true
int main() 
{
	VideoCapture capture;
	Mat frame1, frame2;           //two frames for substract
	Point crossingLine[2];        //represent counting line
	vector<Blob> blobs;	          //blobs
	int frameCnt = 2;             //number of frames
	int cnt = 0;                  //number of cars
	char keyValue = 0;            //key you press
	bool firstFlag = true;        //is first frame?
	//capture.open("E:\\traffic\\交通路口流量统计\\交通路口流量统计\\华池街.wmv");
	capture.open("E:\\traffic\\OpenCV_3_Car_Counting_Cpp-master\\CarsDrivingUnderBridge.mp4");
	//capture.open("E:\\traffic\\交通路口流量统计\\交通视频-金科\\xx路与xx路交叉口—上午7.30至8.30—视频左侧为西进口.mp4");
	if (!capture.isOpened()) //cannot open the video
	{                                                
		cout << "无法打开视频！" << endl;      
		system("pause");
		return 0;                                                           
	}

	capture.read(frame1);
	capture.read(frame2);

	int countingLine = (int)round(frame1.rows * 0.5); //calculate the position of counting line	
	crossingLine[0].x = 0;
	crossingLine[0].y = countingLine;
	crossingLine[1].x = frame1.cols - 1;
	crossingLine[1].y = countingLine;	

	while (capture.isOpened() && keyValue != 27) 
	{

		vector<Blob> currentBlobs;             //blob in current frame

		Mat frame1Copy = frame1.clone();       //copy completely
		Mat frame2Copy = frame2.clone(); 
		Mat imgDifference;                     //frame difference
		Mat imgThresh;                         //binarization
		
		cvtColor(frame1Copy, frame1Copy, CV_BGR2GRAY);                    //to gray
		cvtColor(frame2Copy, frame2Copy, CV_BGR2GRAY);
		GaussianBlur(frame1Copy, frame1Copy, Size(5, 5), 0);              //GaussianBlur for smoothing
		GaussianBlur(frame2Copy, frame2Copy, Size(5, 5), 0);
		absdiff(frame1Copy, frame2Copy, imgDifference);                   //frame difference
		threshold(imgDifference, imgThresh, 30, 255.0, CV_THRESH_BINARY); //binarization

		if (SHOW_FLAG) imshow("imgThresh", imgThresh);

		Mat structuringElement3x3   = getStructuringElement(MORPH_RECT, Size(3, 3)); //definition of different kernels, big kernel for noisy image
		Mat structuringElement5x5   = getStructuringElement(MORPH_RECT, Size(5, 5));
		//Mat structuringElement7x7   = getStructuringElement(MORPH_RECT, Size(7, 7));
		//Mat structuringElement15x15 = getStructuringElement(MORPH_RECT, Size(15, 15));

		for (int i = 0; i < 2; i++)
		{
			dilate(imgThresh, imgThresh, structuringElement3x3, Point(-1, -1), 3);
			erode(imgThresh, imgThresh, structuringElement5x5);			
		}

		Mat imgThreshCopy = imgThresh.clone();
		imshow("demo", imgThreshCopy);
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
			    possibleBlob.center.back().x > frame2Copy.cols * leftBounding &&                                      
				possibleBlob.center.back().x < frame2Copy.cols * rightBounding  &&                                 
				possibleBlob.center.back().y > frame2Copy.rows * upBounding &&                                      
				possibleBlob.center.back().y < frame2Copy.cols * bottomBounding &&                                       
				(contourArea(possibleBlob.contour) / (double)possibleBlob.boundingBox.area()) > 0.50        //contour area / rect area 
			) 
				currentBlobs.push_back(possibleBlob);
		}
		if (SHOW_FLAG) showContours(imgThresh.size(), currentBlobs, "imgcurrentBlobs");

		if (firstFlag == true) //is first frame
			for (auto &currentBlob : currentBlobs) blobs.push_back(currentBlob);
		else 
			matchBlobs(blobs, currentBlobs);

		if (SHOW_FLAG) showContours(imgThresh.size(), blobs, "imgBlobs");

		frame2Copy = frame2.clone();         
		drawBlob(blobs, frame2Copy);
		bool crossLineFlag = isCrossLine(blobs, countingLine, cnt);
		if (crossLineFlag == true) 
			line(frame2Copy, crossingLine[0], crossingLine[1], GREEN, 2);
		else 
			line(frame2Copy, crossingLine[0], crossingLine[1], RED, 2);

		drawCnt(cnt, frame2Copy);

		imshow("frame2Copy", frame2Copy);
		//prepare for the next iteration
		currentBlobs.clear();
		frame1 = frame2.clone();    
		if ((capture.get(CV_CAP_PROP_POS_FRAMES) + 1) < capture.get(CV_CAP_PROP_FRAME_COUNT)) //has next frame
			capture.read(frame2);
		else 
			break;
		firstFlag = false;
		frameCnt++;
		keyValue = waitKey(1);
		//waitKey(0);
	}
	cout << "按任意键退出！" << endl;
	if (keyValue != 27) waitKey(0);                         //don't press ESC
	return 0;
}