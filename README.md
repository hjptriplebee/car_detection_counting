# car_detection_counting
车辆检测与车流量统计

An algorithm for car detection and counting, which is based on frame difference. Demo video can be found [here](https://www.youtube.com/watch?v=TaqK1ez3Pw8)

![here](https://raw.githubusercontent.com/hjptriplebee/car_detection_counting/master/demo.gif)
# Requirement
- C++
- OpenCV3.0

# Usage
The path of video can be set in "test.cpp".
```cpp
capture.open("E:\traffic\苏交科视频\无人机\无人机2\视频\9.MP4");
```
Also, the number and position of counting lines can be set in "test.cpp".
```cpp
//line1
pair<double, double> downOutStart(0.43, 0.75);
pair<double, double> downOutEnd(0.455, 0.75);
crossingLineStart.push_back(downOutStart);
crossingLineEnd.push_back(downOutEnd);
crossingLineColor.push_back(Scalar(rand() % 255, rand() % 255, rand() & 255));
```
Debug, show intermediate result, save result.
```cpp
#define SHOW_FLAG true
#define DEBUG_BY_STEP false
#define SAVE_VIDEO false
```
Adjust parameters to handle videos with different view, resolution.
The parameters can be set in "Blob.h".

```cpp
const int minBlobArea = 100;                     // min area of an object
const int maxBlobArea = 10000;                   // max area of an object
const int minBlobWidth = 10;                     // min width
const int maxBlobWidth = 150;                    // max width
const int minBlobheight = 10;                    // min height
const int maxBlobheight = 150;                   // max height
const int minBlobDiagonal = 10;                  // min diagonal
const int maxBlobDiagonal = 150;                 // max diagonal
const int maxBlobNum = 500;        // max number of objects in a frame, if a frame contains too many objeects, it may be a noisy frame
const double minBlobRatio = 0.2;                 // min ratio of width and height
const double maxBlobRatio = 4;                   // max ratio of width and height
// bounding coefficient is used to get ROI. Objects which are not in ROI will not be detected. For example, if leftBoundingCoefficient = 0.1 and the width of a frame is 1080, then the left bounding of ROI start from 1080 * 0.1 = 108. Default value means ROI is the whole frame.
const double leftBoundingCoefficient = 0;      
const double rightBoundingCoefficient = 1;
const double upBoundingCoefficient = 0;
const double bottomBoundingCoefficient = 1;
const double IOUThreshold = 0.7; 
// resize coefficient is used to resize frame. If resize width coefficient = 0.5 and the width of a frame is 1080, then the width of the frame will be resized to 1080 * 0.5 = 540. Resize to a smaller frame can avoid some noise and increase the speed of our algorithm. Default value means no resize.
const double resizeHeightCoefficient = 1;  
const double resizeWidthCoefficient = 1;
```
