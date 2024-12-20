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
	vector<vector<int>> weighted_map(rows, vector<int>(cols, 0));
	vector<vector<int>> path_table(rows, vector<int>(cols, 0));

	// Initialize the weighted_map table with the first row of energy values
	for (int j = 0; j < cols; j++)
		weighted_map[0][j] = energyMap.at<uchar>(0, j);

	// Fill the weighted_map table
	for (int i = 1; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			weighted_map[i][j] = weighted_map[i - 1][j];
			path_table[i][j] = j;

			if (j > 0 && weighted_map[i - 1][j - 1] < weighted_map[i][j])
			{
				weighted_map[i][j] = weighted_map[i - 1][j - 1];
				path_table[i][j] = j - 1;
			}
			if (j < cols - 1 && weighted_map[i - 1][j + 1] < weighted_map[i][j])
			{
				weighted_map[i][j] = weighted_map[i - 1][j + 1];
				path_table[i][j] = j + 1;
			}
			weighted_map[i][j] += energyMap.at<uchar>(i, j);
		}
	}

	// Trace back the path of the minimum seam
	int minSeam = min_element(weighted_map[rows - 1].begin(), weighted_map[rows - 1].end()) - weighted_map[rows - 1].begin();
	vector<int> seam(rows);
	for (int i = rows - 1; i >= 0; i--) {
		seam[i] = minSeam;
		minSeam = path_table[i][minSeam];
	}
	return seam;
}

// Greedy algorithm to find a vertical seam
vector<int> findVerticalSeamGreedy(const Mat& energyMap) {
	int rows = energyMap.rows, cols = energyMap.cols;
	vector<int> seam(rows);

	// Start with the minimum energy pixel in the first row, excluding the first and last columns
	int minSeam = min_element(energyMap.ptr<uchar>(0) + 1, energyMap.ptr<uchar>(0) + cols - 1) - energyMap.ptr<uchar>(0);
	seam[0] = minSeam;

	// Iterate over each row to greedily choose the next pixel in the seam
	for (int i = 1; i < rows; i++) {
		int prevCol = seam[i - 1];
		int minEnergy = INT_MAX;
		int minCol = prevCol;

		// Check the pixel directly above, and the ones to the left and right (if within bounds)
		for (int j = max(1, prevCol - 1); j <= min(prevCol + 1, cols - 2); j++) {
			if (energyMap.at<uchar>(i, j) < minEnergy) {
				minEnergy = energyMap.at<uchar>(i, j);
				minCol = j;
			}
		}

		seam[i] = minCol; // Record the column of the chosen pixel
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
	vector<vector<int>> weighted_map(rows, vector<int>(cols, 0));
	vector<vector<int>> path_table(rows, vector<int>(cols, 0));

	// Initialize the weighted_map table with the first column of energy values
	for (int i = 0; i < rows; i++)
		weighted_map[i][0] = energyMap.at<uchar>(i, 0);

	// Fill the weighted_map table
	for (int j = 1; j < cols; j++) {
		for (int i = 0; i < rows; i++) {
			weighted_map[i][j] = weighted_map[i][j - 1];
			path_table[i][j] = i;

			if (i > 0 && weighted_map[i - 1][j - 1] < weighted_map[i][j]) {
				weighted_map[i][j] = weighted_map[i - 1][j - 1];
				path_table[i][j] = i - 1;
			}
			if (i < rows - 1 && weighted_map[i + 1][j - 1] < weighted_map[i][j]) {
				weighted_map[i][j] = weighted_map[i + 1][j - 1];
				path_table[i][j] = i + 1;
			}
			weighted_map[i][j] += energyMap.at<uchar>(i, j);
		}
	}

	// Trace back the path of the minimum seam
	int minSeam = min_element(weighted_map.begin(), weighted_map.end(),
		[&](const vector<int>& a, const vector<int>& b) { return a[cols - 1] < b[cols - 1]; }) - weighted_map.begin();
	vector<int> seam(cols);
	for (int j = cols - 1; j >= 0; j--) {
		seam[j] = minSeam;
		minSeam = path_table[minSeam][j];
	}
	return seam;
}


// Greedy algorithm to find a horizontal seam
vector<int> findHorizontalSeamGreedy(const Mat& energyMap) {
	int rows = energyMap.rows, cols = energyMap.cols;
	vector<int> seam(cols);

	// Start with the minimum energy pixel in the first column, excluding the first and last rows
	int minSeam = min_element(energyMap.col(0).ptr<uchar>() + 1, energyMap.col(0).ptr<uchar>() + rows - 1) - energyMap.col(0).ptr<uchar>();
	seam[0] = minSeam;

	// Iterate over each column to greedily choose the next pixel in the seam
	for (int j = 1; j < cols; j++) {
		int prevRow = seam[j - 1];
		int minEnergy = INT_MAX;
		int minRow = prevRow;

		// Check the pixel directly left, and the ones above and below (if within bounds)
		for (int i = max(1, prevRow - 1); i <= min(prevRow + 1, rows - 2); i++) {
			if (energyMap.at<uchar>(i, j) < minEnergy) {
				minEnergy = energyMap.at<uchar>(i, j);
				minRow = i;
			}
		}

		seam[j] = minRow; // Record the row of the chosen pixel
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
