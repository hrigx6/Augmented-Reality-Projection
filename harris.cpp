#include <opencv2/opencv.hpp>

int main() {
    cv::VideoCapture cap(0); // Open the default camera
    if (!cap.isOpened()) {
        std::cerr << "Error: Unable to open the camera" << std::endl;
        return -1;
    }

    cv::namedWindow("Harris Corners", 1);
    
    while (true) {
        cv::Mat frame;
        cap >> frame; // Capture frame from the camera

        if (frame.empty()) {
            std::cerr << "Error: Blank frame captured" << std::endl;
            break;
        }

        // Convert the frame to grayscale
        cv::Mat gray;
        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

        // Detect Harris corners
        cv::Mat corners;
        double threshold = 0.01;
        cv::cornerHarris(gray, corners, 2, 3, threshold);

        // Normalize the corner response
        cv::normalize(corners, corners, 0, 255, cv::NORM_MINMAX, CV_32FC1, cv::Mat());

        // Draw circles around the detected corners
        for (int i = 0; i < corners.rows; ++i) {
            for (int j = 0; j < corners.cols; ++j) {
                if ((int)corners.at<float>(i, j) > 100) { // Adjust the threshold as needed
                    cv::circle(frame, cv::Point(j, i), 5, cv::Scalar(230, 230, 100), 1.5);
                }
            }
        }

        cv::imshow("Harris Corners", frame);

        char key = cv::waitKey(10);
        if (key == 'q') {
            break;
        }
    }

    cap.release();
    cv::destroyAllWindows();

    return 0;
}

