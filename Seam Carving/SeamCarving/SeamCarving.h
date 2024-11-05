#pragma once
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "SeamCarving.h"
#include <iostream>
#include <vector>
#include <limits>

using namespace cv;
using namespace std;

Mat calculateEnergyMap(const Mat& img);
vector<int> findVerticalSeam(const Mat& energyMap);
vector<int> findVerticalSeamGreedy(const Mat& energyMap);
Mat removeVerticalSeam(const Mat& img, const vector<int>& seam);
void drawVerticalSeam(Mat& img, const vector<int>& seam);
