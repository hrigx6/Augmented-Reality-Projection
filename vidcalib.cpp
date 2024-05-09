// Author: Hrigved Suryawanshi & Haard shah (1/19/24)
// CODE: camera calibration and 3d object projection on live feed

#include <stdio.h>
#include <opencv2/opencv.hpp>
#include "ext.h"
using namespace cv;
using namespace std;


bool readObjFile(const std::string &filename, std::vector<cv::Point3f> &vertices, std::vector<std::vector<int>> &faces) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string type;
        iss >> type;
        if (type == "v") {
            float x, y, z;
            iss >> x >> y >> z;
            vertices.push_back(cv::Point3f(x, y, z));
        } else if (type == "f") {
            std::vector<int> face;
            int idx;
            while (iss >> idx) {
                face.push_back(idx - 1); // OBJ file indices are 1-based, so subtract 1
                char c;
                iss >> c; // Skip texture and normal indices
            }
            faces.push_back(face);
        }
    }

    return true;
}


int drawOnTarget(cv::Mat &src, const cv::Mat &camera_matrix, const std::vector<double> &dist_coeff, const cv::Mat &rvec, const cv::Mat &tvec, const std::string &obj_filename) {
    // Load the OBJ file and read vertices and faces
    std::vector<cv::Point3f> object_points_3d;
    std::vector<std::vector<int>> object_faces;
    if (!readObjFile(obj_filename, object_points_3d, object_faces)) {
        std::cerr << "Error: Could not read OBJ file " << obj_filename << std::endl;
        return -1;
    }

    // Project 3D points of the object onto the image plane
    std::vector<cv::Point2f> object_points_2d;
    cv::projectPoints(object_points_3d, rvec, tvec, camera_matrix, dist_coeff, object_points_2d);

    // Draw the projected object onto the image
    for (const auto &face : object_faces) {
        std::vector<cv::Point> points;
        for (int idx : face) {
            points.push_back(object_points_2d[idx]);
        }
        const cv::Point *pts = (const cv::Point*) cv::Mat(points).data;
        int npts = cv::Mat(points).rows;
        cv::polylines(src, &pts, &npts, 1, true, cv::Scalar(0, 255, 255), 2);
    }

    return 0;
}



int main(int argc, char *argv[]) {
    cv::VideoCapture *capdev;
    cv::Mat grey;  // Declare the grey variable

    // Open the video device
    capdev = new cv::VideoCapture(0);
    if (!capdev->isOpened()) {
        printf("Unable to open video device\n");
        return -1;
    }

    // Get some properties of the image
    cv::Size refS((int)capdev->get(cv::CAP_PROP_FRAME_WIDTH),
                  (int)capdev->get(cv::CAP_PROP_FRAME_HEIGHT));
    printf("Expected size: %d %d\n", refS.width, refS.height);

    cv::namedWindow("Video", 1); // Identifies a window
    cv::Mat frame;
    cv::Mat image = cv::imread("brick.jpeg");
    std::string obj_filename = "cup.obj";

    // Definitions for calibration
    std::vector<cv::Vec3f> point_set;  // 3D world positions
    std::vector<std::vector<cv::Vec3f>> point_list;  // List of 3D world positions
    std::vector<std::vector<cv::Point2f>> corner_list;  // List of 2D image coordinates

    cv::Mat camera_matrix = cv::Mat::eye(3, 3, CV_64F); // Initialize camera matrix
    camera_matrix.at<double>(0, 2) = refS.width / 2; // Initialize center of the image
    camera_matrix.at<double>(1, 2) = refS.height / 2;

    std::vector<double> distortion_coefficients; // Define distortion coefficients
    std::vector<cv::Mat> rotations, translations;


    int colorState = 0; // Variable to track color state (if needed)
    cv::Mat blurred_frame; // New variable to store the blurred frame

    for (;;) {
        *capdev >> frame; // Get a new frame from the camera, treat as a stream

        if (frame.empty()) {
            printf("Frame is empty\n");
            break;
        }


    Mat gray;
    // Convert the image to grayscale
    cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

    // Define the chessboard size (rows x columns)
    cv::Size boardSize(6, 9);

    // Find chessboard corners
    std::vector<cv::Point2f> framePoints;
    bool found = cv::findChessboardCorners(gray, boardSize, framePoints);

    // Populate point_set with 3D world coordinates (assuming each square is 1 unit)
    point_set.clear();
    for (int i = 0; i < boardSize.height; ++i) {
        for (int j = 0; j < boardSize.width; ++j) {
            point_set.push_back(cv::Vec3f(static_cast<float>(j), static_cast<float>(-i), 0.0f));
        }
    }

    if (found) {
        // Refine corner locations for better accuracy
        cv::TermCriteria criteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 100, 0.001);
        cv::cornerSubPix(gray, framePoints, cv::Size(11, 11), cv::Size(-1, -1), criteria);

        
        ///cv::resize(image, image, cv::Size(frame.cols, frame.rows));
        //image.copyTo(frame);
        std::string imageFilename = "cloth.jpg";
        cv::Mat rot, trans;
        //drawOnTarget(frame, frame, camera_matrix, distortion_coefficients, rot, trans, imageFilename);
        
        // Draw chessboard corners on the frame
        //cv::drawChessboardCorners(frame, boardSize, framePoints, found);

        // Print information about the corners
        //std::cout << "Number of corners: " << framePoints.size() << "\n";
        //std::cout << "Coordinates of the first corner: (" << framePoints[0].x << ", " << framePoints[0].y << ")\n";

        // Estimate the pose of the target board
        cv::Mat rvec, tvec;
        double reprojectionError = cv::solvePnP(point_set, framePoints, camera_matrix, distortion_coefficients, rvec, tvec);
        std::cout << "Re-projection Error: " << reprojectionError << "\n";
        std::cout << "Rotation vector (rvec):\n" << rvec << "\n";
        std::cout << "Translation vector (tvec):\n" << tvec << "\n";

        // Call drawOnTarget function with rvec and tvec
        drawOnTarget(frame, camera_matrix, distortion_coefficients, rvec, tvec, imageFilename);


        std::vector<cv::Point2f> edges;
        std::vector<cv::Point3f> axisPoints = { {0, 0, 0}, {2, 0, 0}, {0, -2, 0}, {0, 0, 2} };
        cv::projectPoints(axisPoints, rvec, tvec, camera_matrix, distortion_coefficients, edges);

        
        // // Draw axes on the frame
        // cv::line(frame, edges[0], edges[1], cv::Scalar(0, 0, 255), 4); // X-axis (red)
        // cv::line(frame, edges[0], edges[2], cv::Scalar(0,255 , 0), 4); // Y-axis (green)
        // cv::line(frame, edges[0], edges[3], cv::Scalar(255, 0, 0), 4); // Z-axis (blue)

        
        // Define vertices of the hexagonal prism and cubes attached to it
        // Define vertices of the hexagonal prism and cubes attached to it
        // Define vertices of the hexagonal prism
        std::vector<cv::Point3f> hexPrismPoints = {
            {0, 0, 0},             // Bottom face vertices of the hexagonal prism
            {1, 0, 0},
            {1.5, 0.866, 0},
            {1, 1.732, 0},
            {0, 1.732, 0},
            {-0.5, 0.866, 0},
            {0, 0, 1},              // Top face vertices of the hexagonal prism
            {1, 0, 1},
            {1.5, 0.866, 1},
            {1, 1.732, 1},
            {0, 1.732, 1},
            {-0.5, 0.866, 1}
        };

        // Project 3D points of the hexagonal prism onto the image plane
        std::vector<cv::Point2f> hexPrismCorners;
        cv::projectPoints(hexPrismPoints, rvec, tvec, camera_matrix, distortion_coefficients, hexPrismCorners);

        // Draw lines representing the edges of the hexagonal prism
        cv::line(frame, hexPrismCorners[0], hexPrismCorners[1], cv::Scalar(0, 255, 255), 2); // Bottom face edges
        cv::line(frame, hexPrismCorners[1], hexPrismCorners[2], cv::Scalar(0, 255, 255), 2);
        cv::line(frame, hexPrismCorners[2], hexPrismCorners[3], cv::Scalar(0, 255, 255), 2);
        cv::line(frame, hexPrismCorners[3], hexPrismCorners[4], cv::Scalar(0, 255, 255), 2);
        cv::line(frame, hexPrismCorners[4], hexPrismCorners[5], cv::Scalar(0, 255, 255), 2);
        cv::line(frame, hexPrismCorners[5], hexPrismCorners[0], cv::Scalar(0, 255, 255), 2);

        cv::line(frame, hexPrismCorners[6], hexPrismCorners[7], cv::Scalar(0, 255, 255), 2); // Top face edges
        cv::line(frame, hexPrismCorners[7], hexPrismCorners[8], cv::Scalar(0, 255, 255), 2);
        cv::line(frame, hexPrismCorners[8], hexPrismCorners[9], cv::Scalar(0, 255, 255), 2);
        cv::line(frame, hexPrismCorners[9], hexPrismCorners[10], cv::Scalar(0, 255, 255), 2);
        cv::line(frame, hexPrismCorners[10], hexPrismCorners[11], cv::Scalar(0, 255, 255), 2);
        cv::line(frame, hexPrismCorners[11], hexPrismCorners[6], cv::Scalar(0, 255, 255), 2);

        cv::line(frame, hexPrismCorners[0], hexPrismCorners[6], cv::Scalar(0, 255, 255), 2); // Connecting edges
        cv::line(frame, hexPrismCorners[1], hexPrismCorners[7], cv::Scalar(0, 255, 255), 2);
        cv::line(frame, hexPrismCorners[2], hexPrismCorners[8], cv::Scalar(0, 255, 255), 2);
        cv::line(frame, hexPrismCorners[3], hexPrismCorners[9], cv::Scalar(0, 255, 255), 2);
        cv::line(frame, hexPrismCorners[4], hexPrismCorners[10], cv::Scalar(0, 255, 255), 2);
        cv::line(frame, hexPrismCorners[5], hexPrismCorners[11], cv::Scalar(0, 255, 255), 2);

        // Define vertices of the cubes attached to the hexagonal prism
        std::vector<cv::Point3f> cube1Points = {
            {0, 0, 0},     // Bottom face vertices of cube 1
            {1, 0, 0},
            {1, 1, 0},
            {0, 1, 0},
            {0, 0, 1},     // Top face vertices of cube 1
            {1, 0, 1},
            {1, 1, 1},
            {0, 1, 1}
        };

        std::vector<cv::Point3f> cube2Points = {
            {0, 0, 0},     // Bottom face vertices of cube 2
            {1, 0, 0},
            {1, 1, 0},
            {0, 1, 0},
            {0, 0, 1},     // Top face vertices of cube 2
            {1, 0, 1},
            {1, 1, 1},
            {0, 1, 1}
        };

        // Move cube 1 to one side of the hexagonal prism
        for (auto &point : cube1Points) {
            point.x += 0.75;  // Adjust the x-coordinate to attach to the side of the hexagonal prism
        }

        // Move cube 2 to the opposite side of the hexagonal prism
        for (auto &point : cube2Points) {
            point.x -= 0.75;  // Adjust the x-coordinate to attach to the opposite side of the hexagonal prism
        }

        // Project 3D points of the cubes onto the image plane
        std::vector<cv::Point2f> cube1Corners, cube2Corners;
        cv::projectPoints(cube1Points, rvec, tvec, camera_matrix, distortion_coefficients, cube1Corners);
        cv::projectPoints(cube2Points, rvec, tvec, camera_matrix, distortion_coefficients, cube2Corners);

        // Draw lines representing the edges of cube 1
        cv::line(frame, cube1Corners[0], cube1Corners[1], cv::Scalar(255, 0, 255), 2); // Bottom face edges
        cv::line(frame, cube1Corners[1], cube1Corners[2], cv::Scalar(255, 0, 255), 2);
        cv::line(frame, cube1Corners[2], cube1Corners[3], cv::Scalar(255, 0, 255), 2);
        cv::line(frame, cube1Corners[3], cube1Corners[0], cv::Scalar(255, 0, 255), 2);
        cv::line(frame, cube1Corners[4], cube1Corners[5], cv::Scalar(255, 0, 255), 2); // Top face edges
        cv::line(frame, cube1Corners[5], cube1Corners[6], cv::Scalar(255, 0, 255), 2);
        cv::line(frame, cube1Corners[6], cube1Corners[7], cv::Scalar(255, 0, 255), 2);
        cv::line(frame, cube1Corners[7], cube1Corners[4], cv::Scalar(255, 0, 255), 2);
        cv::line(frame, cube1Corners[0], cube1Corners[4], cv::Scalar(255, 0, 255), 2); // Connecting edges
        cv::line(frame, cube1Corners[1], cube1Corners[5], cv::Scalar(255, 0, 255), 2);
        cv::line(frame, cube1Corners[2], cube1Corners[6], cv::Scalar(255, 0, 255), 2);
        cv::line(frame, cube1Corners[3], cube1Corners[7], cv::Scalar(255, 0, 255), 2);

        // Draw lines representing the edges of cube 2
        cv::line(frame, cube2Corners[0], cube2Corners[1], cv::Scalar(255, 0, 255), 2); // Bottom face edges
        cv::line(frame, cube2Corners[1], cube2Corners[2], cv::Scalar(255, 0, 255), 2);
        cv::line(frame, cube2Corners[2], cube2Corners[3], cv::Scalar(255, 0, 255), 2);
        cv::line(frame, cube2Corners[3], cube2Corners[0], cv::Scalar(255, 0, 255), 2);
        cv::line(frame, cube2Corners[4], cube2Corners[5], cv::Scalar(255, 0, 255), 2); // Top face edges
        cv::line(frame, cube2Corners[5], cube2Corners[6], cv::Scalar(255, 0, 255), 2);
        cv::line(frame, cube2Corners[6], cube2Corners[7], cv::Scalar(255, 0, 255), 2);
        cv::line(frame, cube2Corners[7], cube2Corners[4], cv::Scalar(255, 0, 255), 2);
        cv::line(frame, cube2Corners[0], cube2Corners[4], cv::Scalar(255, 0, 255), 2); // Connecting edges
        cv::line(frame, cube2Corners[1], cube2Corners[5], cv::Scalar(255, 0, 255), 2);
        cv::line(frame, cube2Corners[2], cube2Corners[6], cv::Scalar(255, 0, 255), 2);
        cv::line(frame, cube2Corners[3], cube2Corners[7], cv::Scalar(255, 0, 255), 2);



       // Define vertices of the multiplanar 3D object (football) with tilt and shift
        std::vector<cv::Point3f> footballPoints;

        // Define points for the upper hemisphere with tilt and shift
        for (float theta = 0; theta <= CV_PI / 2; theta += CV_PI / 10) {
            for (float phi = 0; phi < 2 * CV_PI; phi += CV_PI / 10) {
                float tilt = 0.1; // Tilt factor towards the center and specified point
                float shift_x = 5; // Shift the football more to the right
                float shift_z = 3; // Shift the football higher in the air
                float x = cos(phi) * cos(theta) - tilt * cos(theta) + tilt * 4 + shift_x;
                float y = sin(phi) * cos(theta) - tilt * cos(theta) - tilt * 4;
                float z = sin(theta) + tilt * 4 + shift_z;
                footballPoints.push_back(cv::Point3f(x, y, z));
            }
        }

        // Define points for the lower hemisphere with tilt and shift
        for (float theta = CV_PI / 2; theta <= CV_PI; theta += CV_PI / 10) {
            for (float phi = 0; phi < 2 * CV_PI; phi += CV_PI / 10) {
                float tilt = 0.1; // Tilt factor towards the center and specified point
                float shift_x = 5; // Shift the football more to the right
                float shift_z = 3; // Shift the football higher in the air
                float x = cos(phi) * cos(theta) - tilt * cos(theta) + tilt * 4 + shift_x;
                float y = sin(phi) * cos(theta) - tilt * cos(theta) - tilt * 4;
                float z = sin(theta) + tilt * 4 + shift_z;
                footballPoints.push_back(cv::Point3f(x, y, z));
            }
        }

        // Project 3D points of the football onto the image plane
        std::vector<cv::Point2f> footballCorners;
        cv::projectPoints(footballPoints, rvec, tvec, camera_matrix, distortion_coefficients, footballCorners);

        // Draw lines representing the edges of the football
        for (size_t i = 0; i < footballCorners.size(); ++i) {
            for (size_t j = i + 1; j < footballCorners.size(); ++j) {
                cv::line(frame, footballCorners[i], footballCorners[j], cv::Scalar(0, 255, 255), 1);
            }
        }






        

        cv::Mat old_camera_matrix = camera_matrix.clone(); // Store the old camera matrix
        // Save corner locations and 3D world points when 's' is pressed
        char key = cv::waitKey(10);
        if (key == 's') {
            corner_list.push_back(framePoints);
            point_list.push_back(point_set);

            // Print saved coordinates
            std::cout << "Corner Set:\n";
            for (size_t i = 0; i < corner_list.back().size(); ++i) {
                std::cout << "(" << corner_list.back()[i].x << ", " << corner_list.back()[i].y << ") ";
            }
            std::cout << "\n";

            std::cout << "Point Set:\n";
            for (size_t i = 0; i < point_list.back().size(); ++i) {
                std::cout << "(" << point_list.back()[i][0] << ", " << point_list.back()[i][1] << ", " << point_list.back()[i][2] << ") ";
            }
            std::cout << "\n";

            // Perform camera calibration
            double error = cv::calibrateCamera(point_list, corner_list, frame.size(), camera_matrix, distortion_coefficients, rotations, translations);

            // Print camera matrix, distortion coefficients, and re-projection error
            std::cout << "Old Camera Matrix:\n" << old_camera_matrix << "\n";
            std::cout << "Camera Matrix:\n" << camera_matrix << "\n";

            std::cout << "Distortion Coefficients:\n";
            for (size_t i = 0; i < distortion_coefficients.size(); ++i) {
                std::cout << distortion_coefficients[i] << " ";
            }
            std::cout << "\n";
            std::cout << "Re-projection Error: " << error << "\n" <<"\n";


            std::cout << "rotations:\n";
            for (size_t i = 0; i < rotations.size(); ++i) {
                std::cout << "Rotation matrix " << i << ":\n" << rotations[i] << "\n";
            }

            std::cout << "translations:\n";
            for (size_t i = 0; i < translations.size(); ++i) {
                std::cout << "Translation matrix " << i << ":\n" << translations[i] << "\n";
            }

           
           

            // Save intrinsic parameters to a file
            cv::FileStorage fs("intrinsic_params.yaml", cv::FileStorage::WRITE);
            fs << "camera_matrix" << camera_matrix;
            fs << "Rotaion matrices" << rotations;

            fs << "Translation matrices" << translations;

            fs << "distortion_coefficients" << distortion_coefficients;
            fs.release();
        }

    } else {
        std::cout << "Chessboard not found in the image.\n";
    }

        imshow("Video", frame);

        char key = cv::waitKey(10);
        if (key == 'q') {
            break;  // Break the loop if 'q' key is pressed
        }
    }
    
    // Clean up resources
    delete capdev;
    return 0;
}
