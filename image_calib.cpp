// Author: Hrigved Suryawanshi & Haard shah (1/19/24)
// CODE: camera calibration on an image


#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;

int main(int argc, char *argv[]) {
    cv::Mat frame;

    // Define the path to the image
    //std::string imagePath = "chess.JPEG";
    std::string imagePath = "cal5.jpeg"; 

    // Load the image
    frame = cv::imread(imagePath);

    if (frame.empty()) {
        printf("Unable to load image: %s\n", imagePath.c_str());
        return -1;
    }

    // Get image size
    cv::Size refS(frame.cols, frame.rows);
    std::cout << "Image size: " << refS.width << " " << refS.height << std::endl;


    // Resize the image to fit within the screen
    cv::Size screenSize(1280, 820);  // Adjust the size according to your screen resolution
    cv::resize(frame, frame, screenSize);

    Mat gray;
    // Convert the image to grayscale
    cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

    // Define the chessboard size (rows x columns)
    cv::Size boardSize(6, 9);

    // Find chessboard corners
    std::vector<cv::Point2f> framePoints;
    bool found = cv::findChessboardCorners(gray, boardSize, framePoints);

    if (found) {
        // Refine corner locations for better accuracy
        cv::TermCriteria criteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 100, 0.001);
        cv::cornerSubPix(gray, framePoints, cv::Size(11, 11), cv::Size(-1, -1), criteria);

        // Draw chessboard corners on the frame
        cv::drawChessboardCorners(frame, boardSize, framePoints, found);

        // Print information about the corners
        std::cout << "Number of corners: " << framePoints.size() << "\n";
        std::cout << "Coordinates of the first corner: (" << framePoints[0].x << ", " << framePoints[0].y << ")\n";
    } else {
        std::cout << "Chessboard not found in the image.\n";
    }

    // Display the image with detected corners
    cv::imshow("Image", frame);
    cv::waitKey(0);

    return 0;
}
