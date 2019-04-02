# car_detection_counting
车辆检测与车流量检测
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
```
