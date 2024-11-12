#include "SeamCarving.h"
#include <cctype>

using namespace cv;
using namespace std;

#define GREEDY 'G'
#define DYNAMIC 'D'

int main() {
    // Load the image
    Mat img = imread("../SeamCarving/Assets/pietro-de-grandi-329892-unsplash.jpg");
    if (img.empty())
    {
        cout << "Image not found!" << endl;
        return -1;
    }
    int pixelsVertical = 400; // amount of pixels to reduce horizonatally 
    int pixelsHorizontal = 0;  // amount of pixels to reduce vertically
    int targetWidth = img.cols - pixelsVertical;
    int targetHeight = img.rows - pixelsHorizontal;

    char choice;
    do {
        std::cout << "Choose algorithm for seam finding (g for Greedy, d for Dynamic Programming): ";
        std::cin >> choice;
        choice = std::toupper(choice);

        if (choice != GREEDY && choice != DYNAMIC) {
            std::cout << "Invalid input. Please enter 'g' or 'd'." << std::endl;
        }
    } while (choice != GREEDY && choice != DYNAMIC);

    while (img.cols > targetWidth || img.rows > targetHeight) {
        if (img.cols > targetWidth) {
            // Recalculate the energy map for the current image size
            Mat energyMap = calculateEnergyMap(img);

            // Find the vertical seam
            vector<int> seamVertical;

            if (choice == GREEDY)
                seamVertical = findVerticalSeamGreedy(energyMap);
            else
                seamVertical = findVerticalSeam(energyMap);

            // Optional: visualize the seam before removal
            Mat imgWithSeam = img.clone();
            drawVerticalSeam(imgWithSeam, seamVertical);
            imshow("Seam Carving", imgWithSeam);
            waitKey(100);

            // Remove the vertical seam
            img = removeVerticalSeam(img, seamVertical);
        }

        if (img.rows > targetHeight) {
            // Recalculate the energy map for the current image size
            Mat energyMap = calculateEnergyMap(img);

            // Find the horizontal seam
            vector<int> seamHorizontal;
            
            if (choice == GREEDY)
                seamHorizontal = findHorizontalSeamGreedy(energyMap);
            else
                seamHorizontal = findHorizontalSeam(energyMap);

            // Optional: visualize the seam before removal
            Mat imgWithSeam = img.clone();
            drawHorizontalSeam(imgWithSeam, seamHorizontal);
            imshow("Seam Carving", imgWithSeam);
            waitKey(100);

            // Remove the horizontal seam
            img = removeHorizontalSeam(img, seamHorizontal);
        }
    }

    destroyAllWindows();
    imshow("Final Image", img);
    waitKey(0);
    return 0;
}
