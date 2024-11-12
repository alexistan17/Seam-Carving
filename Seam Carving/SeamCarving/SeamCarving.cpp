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
	int rows = energyMap.rows + 1, cols = energyMap.cols+1;
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
	//Create a vector of rows with energy
	vector<int> colEnergy(cols);
	for (int i = 1; i < cols; i++) {
		colEnergy[i-1] = energyMap.at<uchar>(0, i);
	}

	//find the minimum energy row
	seam.front() = min_element(colEnergy.begin(), colEnergy.end()) - colEnergy.begin();

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
void drawVerticalSeam(Mat& img, const vector<int>& seam)
{
	for (int i = 0; i < img.rows; i++) {
		// Ensure seam[i] is within valid column indices for img
		if (seam[i] >= 0 && seam[i] < img.cols) {
			img.at<Vec3b>(i, seam[i]) = Vec3b(0, 0, 255); // Set seam pixels to red
		}
		else {
			cerr << "Warning: seam index out of bounds at row " << i << ": " << seam[i] << endl;
		}
	}
}


// Function to find the minimum horizontal seam
vector<int> findHorizontalSeam(const Mat& energyMap) {
	int rows = energyMap.rows, cols = energyMap.cols;
	vector<vector<int>> dp(rows, vector<int>(cols, 0));
	vector<vector<int>> path(rows, vector<int>(cols, 0));

	// Initialize the DP table with the first column of energy values
	for (int i = 0; i < rows; i++)
		dp[i][0] = energyMap.at<uchar>(i, 0);

	// Fill the DP table
	for (int j = 1; j < cols; j++) {
		for (int i = 0; i < rows; i++) {
			dp[i][j] = dp[i][j - 1];
			path[i][j] = i;

			if (i > 0 && dp[i - 1][j - 1] < dp[i][j]) {
				dp[i][j] = dp[i - 1][j - 1];
				path[i][j] = i - 1;
			}
			if (i < rows - 1 && dp[i + 1][j - 1] < dp[i][j]) {
				dp[i][j] = dp[i + 1][j - 1];
				path[i][j] = i + 1;
			}
			dp[i][j] += energyMap.at<uchar>(i, j);
		}
	}

	// Trace back the path of the minimum seam
	int minSeam = min_element(dp.begin(), dp.end(),
		[&](const vector<int>& a, const vector<int>& b) { return a[cols - 1] < b[cols - 1]; }) - dp.begin();
	vector<int> seam(cols);
	for (int j = cols - 1; j >= 0; j--) {
		seam[j] = minSeam;
		minSeam = path[minSeam][j];
	}
	return seam;
}


// Greedy algorithm to find a horizontal seam
vector<int> findHorizontalSeamGreedy(const Mat& energyMap) {
	int rows = energyMap.rows, cols = energyMap.cols;
	vector<int> seam(cols);

	//Create a vector of rows with energy
	vector<int> rowEnergy(rows);
	for (int i = 0; i < rows; i++) {
		rowEnergy[i] = energyMap.at<uchar>(i, 0);
	}

	//find the minimum energy row
	seam.front() = min_element(rowEnergy.begin(), rowEnergy.end()) - rowEnergy.begin();

	//seam.front() = min_element(energyMap.ptr<uchar>(0), energyMap.ptr<uchar>(rows)) - energyMap.ptr<uchar>(0);

	for (int j = 1; j < cols; j++) {
		int prevRow = seam[j - 1];
		seam[j] = prevRow;

		// Check above
		if (prevRow > 0 && energyMap.at<uchar>(prevRow - 1, j) < energyMap.at<uchar>(seam[j], j)) {
			seam[j] = prevRow - 1;
		}

		// Check below
		if (prevRow < rows - 1 && energyMap.at<uchar>(prevRow + 1, j) < energyMap.at<uchar>(seam[j], j)) {
			seam[j] = prevRow + 1;
		}
	}
	return seam;
}

// Function to remove a horizontal seam from the image
Mat removeHorizontalSeam(const Mat& img, const vector<int>& seam) {
	Mat output(img.rows - 1, img.cols, img.type());

	for (int j = 0; j < img.cols; ++j) {
		int row = seam[j];
		for (int i = 0; i < row; ++i) {
			output.at<Vec3b>(i, j) = img.at<Vec3b>(i, j);
		}
		for (int i = row + 1; i < img.rows; ++i) {
			output.at<Vec3b>(i - 1, j) = img.at<Vec3b>(i, j);
		}
	}

	return output;
}

// Function to draw a horizontal seam on the image
void drawHorizontalSeam(Mat& img, const vector<int>& seam) {
	for (int j = 0; j < img.cols; j++) {
		// Ensure seam[j] is within valid row indices for img
		if (seam[j] >= 0 && seam[j] < img.rows) {
			img.at<Vec3b>(seam[j], j) = Vec3b(0, 0, 255); // Set seam pixels to red
		}
		else {
			cerr << "Warning: seam index out of bounds at column " << j << ": " << seam[j] << endl;
		}
	}
}
