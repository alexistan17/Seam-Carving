#include "SeamCarving.h"

using namespace cv;
using namespace std;

int main() {
    // Load the image
    Mat img = imread("../SeamCarving/Assets/Broadway_tower.jpg");
    if (img.empty()) 
    {
        cout << "Image not found!" << endl;
        return -1;
    }
    int pixelsVertical = 500; // amount of pixels to reduce vertically 
    int pixelsHorizontal = 200; // amount of pixels to reduce horizontally
    int targetWidth = img.cols - pixelsVertical;
    int targetHeight = img.rows - pixelsHorizontal;

    // Seam carving process
    while (img.cols > targetWidth && img.rows > targetHeight) {
        // Calculate the energy map
        Mat energyMap = calculateEnergyMap(img);

        // Find the minimum vertical seam
        vector<int> seamVertical = findVerticalSeam(energyMap);
        // Find the minimum horizontal seam
        vector<int> seamHorizontal = findHorizontalSeam(energyMap);

        // Draw the seam on the image
        Mat imgWithSeam = img.clone();  // Clone image to draw the seam without altering original
        drawVerticalSeam(imgWithSeam, seamVertical);
        drawHorizontalSeam(imgWithSeam, seamHorizontal);

        // Display the image with the seam
        imshow("Seam Carving", imgWithSeam);
        waitKey(100);  // Wait for 200 ms to see the seam

        // Remove the seam from the original image
        img = removeVerticalSeam(img, seamVertical);
        img = removeHorizontalSeam(img, seamHorizontal);
    }

    destroyAllWindows();
    imshow("Final Image", img);
    // Final result
    waitKey(0);
    return 0;
}