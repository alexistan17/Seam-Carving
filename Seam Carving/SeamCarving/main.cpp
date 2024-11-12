#include "SeamCarving.h"
#include <cctype>

using namespace cv;
using namespace std;

#define GREEDY 'G'
#define DYNAMIC 'D'

int main() {
    // Load the image
    Mat img = imread("../SeamCarving/Assets/pietro.jpg");
    if (img.empty())
    {
        cout << "Image not found!" << endl;
        return -1;
    }
    int targetWidth, targetHeight;

    while (true) {
        cout << "Please enter the desired target width and height (in pixels)." << endl;
        cout << "The maximum dimensions are " << img.cols << " (width) by " << img.rows << " (height)." << endl;
        cout << "Enter width and height separated by a space: ";

        cin >> targetWidth >> targetHeight;

        if (cin.fail()) {
            // Clear error state and ignore invalid input
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input. Please enter numeric values for width and height." << endl;
            continue;
        }

        if (targetWidth > 0 && targetWidth <= img.cols && targetHeight > 0 && targetHeight <= img.rows) {
            // Valid input, break the loop
            break;
        }
        else {
            cout << "Invalid dimensions. Ensure width and height are positive and do not exceed the original dimensions." << endl;
        }
    }

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
