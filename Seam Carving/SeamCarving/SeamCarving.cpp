#include "SeamCarving.h"
#include <iostream>
#include <vector>
#include <limits>

using namespace cv;
using namespace std;
// Function to calculate the energy map using the Sobel filter
Mat calculateEnergyMap(const Mat& img) {
    Mat gray, grad_x, grad_y, abs_grad_x, abs_grad_y, energyMap;

    // Convert to grayscale
    cvtColor(img, gray, COLOR_BGR2GRAY);

    // Compute gradients along the x and y directions
    Sobel(gray, grad_x, CV_16S, 1, 0, 3);
    Sobel(gray, grad_y, CV_16S, 0, 1, 3);

    // Convert gradients to absolute values
    convertScaleAbs(grad_x, abs_grad_x);
    convertScaleAbs(grad_y, abs_grad_y);

    // Combine the gradients to get the energy map
    addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, energyMap);

    return energyMap;
}

// Function to find the minimum vertical seam
vector<int> findVerticalSeam(const Mat& energyMap) {
    int rows = energyMap.rows, cols = energyMap.cols;
    vector<vector<int>> dp(rows, vector<int>(cols, 0));
    vector<vector<int>> path(rows, vector<int>(cols, 0));

    // Initialize the DP table with the first row of energy values
    for (int j = 0; j < cols; j++)
        dp[0][j] = energyMap.at<uchar>(0, j);

    // Fill the DP table
    for (int i = 1; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            dp[i][j] = dp[i - 1][j];
            path[i][j] = j;

            if (j > 0 && dp[i - 1][j - 1] < dp[i][j])
            {
                dp[i][j] = dp[i - 1][j - 1];
                path[i][j] = j - 1;
            }
            if (j < cols - 1 && dp[i - 1][j + 1] < dp[i][j])
            {
                dp[i][j] = dp[i - 1][j + 1];
                path[i][j] = j + 1;
            }
            dp[i][j] += energyMap.at<uchar>(i, j);
        }
    }

    // Trace back the path of the minimum seam
    int minSeam = min_element(dp[rows - 1].begin(), dp[rows - 1].end()) - dp[rows - 1].begin();
    vector<int> seam(rows);
    for (int i = rows - 1; i >= 0; i--) {
        seam[i] = minSeam;
        minSeam = path[i][minSeam];
    }
    return seam;
}

// Greedy algorithm to find a vertical seam
vector<int> findVerticalSeamGreedy(const Mat& energyMap) {
    int rows = energyMap.rows, cols = energyMap.cols;
    vector<int> seam(rows);
    seam[0] = min_element(energyMap.ptr<uchar>(0), energyMap.ptr<uchar>(0) + cols) - energyMap.ptr<uchar>(0);

    for (int i = 1; i < rows; i++) {
        int prevCol = seam[i - 1];
        seam[i] = prevCol;
        if (prevCol > 0 && energyMap.at<uchar>(i, prevCol - 1) < energyMap.at<uchar>(i, seam[i]))
            seam[i] = prevCol - 1;
        if (prevCol < cols - 1 && energyMap.at<uchar>(i, prevCol + 1) < energyMap.at<uchar>(i, seam[i]))
            seam[i] = prevCol + 1;
    }
    return seam;
}

// Function to remove a vertical seam from the image
Mat removeVerticalSeam(const Mat& img, const vector<int>& seam) {
    Mat output(img.rows, img.cols - 1, img.type());

    for (int i = 0; i < img.rows; ++i)
    {
        int col = seam[i];
        for (int j = 0; j < col; ++j)
        {
            output.at<Vec3b>(i, j) = img.at<Vec3b>(i, j);
        }
        for (int j = col + 1; j < img.cols; ++j) {
            output.at<Vec3b>(i, j - 1) = img.at<Vec3b>(i, j);
        }
    }

    return output;
}

// Function to draw a vertical seam on the image
void drawVerticalSeam(Mat& img, const vector<int>& seam) {
    for (int i = 0; i < img.rows; i++) {
        img.at<Vec3b>(i, seam[i]) = Vec3b(0, 0, 255); // Set seam pixels to red
    }
}