

# Augmented Reality: Projection and Calibration 

This project focuses on camera calibration and creating an augmented reality (AR) experience by overlaying virtual objects onto real-world scenes. The system detects a checkerboard pattern as a target, estimates the camera's intrinsic parameters and lens distortion, and uses the calibration results to project virtual objects onto the target's plane.

## Features

### Camera Calibration
- **Target Detection and Corner Extraction**: Utilizes OpenCV's `findChessboardCorners` and `cornerSubPix` functions to accurately detect and extract corners from the checkerboard target in various orientations and conditions.
- **Calibration Image Selection**: Allows users to select specific frames for camera calibration by pressing a designated key (e.g., 's'). The system stores the detected corner locations and corresponding 3D world points in separate lists.
- **Continuous Calibration**: Enables users to continuously add more frames for calibration, improving accuracy with each additional frame. The calibration process is performed using OpenCV's `calibrateCamera` function.

### Camera Pose Estimation
- **Real-time Target Detection**: Continuously detects the target in the video stream and estimates the camera's pose (rotation and translation) using the `solvePnP` function and calibration parameters.
- **Pose Evaluation**: Prints the rotation and translation data as the camera moves, allowing users to assess the accuracy of the pose estimation based on the camera's position and orientation.

### Augmented Reality
- **Projection of 3D Points and Axes**: Projects the 3D points corresponding to the target's corners or 3D axes onto the image plane using the `projectPoints` function, enabling visual confirmation of the estimated pose.
- **Virtual Object Creation**: Generates a virtual 3D object composed of interconnected lines, floating above the target board. The object's orientation is continuously updated based on the camera's pose.

### Feature Detection
- **Harris Corner Detection**: Implements a separate program to detect and display Harris corners in a video stream. Allows users to experiment with different thresholds and settings to understand feature detection and its application in augmented reality.

## Getting Started

To run the project locally, follow these steps:

1. Clone the repository: `git clone https://github.com/your-username/calibration-ar.git`
2. Install the required dependencies (e.g., OpenCV, NumPy).
3. Run the desired script:
   - `imgcalib.cpp` for camera calibration on an image.
   - `vidcalib.cpp` for camera calibration and AR experience on a live video feed (Tasks 1-6).
   - `harris.cpp` for Harris corner detection (Task 7).

## Usage

1. Print or display the provided "checkerboard.png" as the target.
2. For camera calibration on an image:
   - Run the `imgcalib.cpp` script.
   - Follow the prompts to select the calibration image(s).
3. For camera calibration and AR experience on a live video feed (Tasks 1-6):
   - Run the `vidcalib.cpp` script.
   - Use the designated key (e.g., 's') to select frames for camera calibration.
   - Move the camera or target to observe the real-time projection of virtual objects and pose estimation data.
4. For Harris corner detection (Task 7):
   - Run the `harris.cpp` script.
   - Experiment with different thresholds and settings to understand feature detection and its application in augmented reality.

## Contributing

Contributions to the project are welcome! If you encounter any issues or have suggestions for improvements, please open an issue or submit a pull request.

## Acknowledgments

We would like to express our gratitude to the following platforms for their invaluable support and resources throughout the project:

- Springer
- ResearchGate
- GitHub
- Stack Overflow
- ChatGPT
- Gemini
- Medium

## License

This project is licensed under the [MIT License](LICENSE).
