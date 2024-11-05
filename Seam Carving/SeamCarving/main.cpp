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

    int targetWidth = img.cols - 500;

    // Seam carving process
    while (img.cols > targetWidth) {
        // Step 1: Calculate the energy map
        Mat energyMap = calculateEnergyMap(img);

        // Step 2: Find the minimum vertical seam
        vector<int> seam = findVerticalSeam(energyMap);

        // Draw the seam on the image
        Mat imgWithSeam = img.clone();  // Clone image to draw the seam without altering original
        drawVerticalSeam(imgWithSeam, seam);

        // Display the image with the seam
        imshow("Seam Carving - Current Seam", imgWithSeam);
        waitKey(100);  // Wait for 200 ms to see the seam

        // Remove the seam from the original image
        img = removeVerticalSeam(img, seam);
    }
    destroyAllWindows();
    imshow("Final Image", img);
    // Final result
    waitKey(0);
    return 0;
}