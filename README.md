# Augmented-Reality-Projection and Calibration
This project focuses on camera calibration and creating an augmented reality (AR) experience by overlaying virtual objects onto real-world scenes. The system detects a checkerboard pattern as a target, estimates the camera's intrinsic parameters and lens distortion, and uses the calibration results to project virtual objects onto the target's plane.

Features
Camera Calibration
Target Detection and Corner Extraction: Utilizes OpenCV's findChessboardCorners and cornerSubPix functions to accurately detect and extract corners from the checkerboard target in various orientations and conditions.
Calibration Image Selection: Allows users to select specific frames for camera calibration by pressing a designated key (e.g., 's'). The system stores the detected corner locations and corresponding 3D world points in separate lists.
Continuous Calibration: Enables users to continuously add more frames for calibration, improving accuracy with each additional frame. The calibration process is performed using OpenCV's calibrateCamera function.
Camera Pose Estimation
Real-time Target Detection: Continuously detects the target in the video stream and estimates the camera's pose (rotation and translation) using the solvePnP function and calibration parameters.
Pose Evaluation: Prints the rotation and translation data as the camera moves, allowing users to assess the accuracy of the pose estimation based on the camera's position and orientation.
Augmented Reality
Projection of 3D Points and Axes: Projects the 3D points corresponding to the target's corners or 3D axes onto the image plane using the projectPoints function, enabling visual confirmation of the estimated pose.
Virtual Object Creation: Generates a virtual 3D object composed of interconnected lines, floating above the target board. The object's orientation is continuously updated based on the camera's pose.
Feature Detection
Harris Corner Detection: Implements a separate program to detect and display Harris corners in a video stream. Allows users to experiment with different thresholds and settings to understand feature detection and its application in augmented reality.
Getting Started
To run the project locally, follow these steps:

Clone the repository: git clone https://github.com/your-username/calibration-ar.git
Install the required dependencies (e.g., OpenCV, NumPy).
Run the desired script:
imgcalib.cpp for camera calibration on an image.
vidcalib.cpp for camera calibration and AR experience on a live video feed (Tasks 1-6).
harris.cpp for Harris corner detection (Task 7).
Usage
Print or display the provided "checkerboard.png" as the target.
For camera calibration on an image:
Run the imgcalib.cpp script.
Follow the prompts to select the calibration image(s).
For camera calibration and AR experience on a live video feed (Tasks 1-6):
Run the vidcalib.cpp script.
Use the designated key (e.g., 's') to select frames for camera calibration.
Move the camera or target to observe the real-time projection of virtual objects and pose estimation data.
For Harris corner detection (Task 7):
Run the harris.cpp script.
Experiment with different thresholds and settings to understand feature detection and its application in augmented reality.
Contributing
Contributions to the project are welcome! If you encounter any issues or have suggestions for improvements, please open an issue or submit a pull request.

Acknowledgments
We would like to express our gratitude to the following platforms for their invaluable support and resources throughout the project:

Springer
ResearchGate
GitHub
Stack Overflow
ChatGPT
Gemini
Medium
License
This project is licensed under the MIT License.

what is a MIT license?

Bot image for Claude-3-Sonnet
Claude-3-Sonnet

Poe

The MIT License is a permissive free software license originating at the Massachusetts Institute of Technology (MIT). It is one of the most popular open source licenses and has a very limited set of restrictions.

Here are some key points about the MIT License:

Permissions: The MIT License grants permissions for free use, copying, modification, merging, publishing, distribution, sublicensing, and/or selling copies of the software.
Requirements: The only requirement is to include the license's copyright notice and permission notice in all copies or substantial portions of the software.
Copyrights: The license preserves copyright holders' rights to the software.
Liability: The software is provided "as is," without any warranty of any kind, express or implied. The copyright holders are not liable for any claim, damages, or other liabilities arising from the use or distribution of the software.
Restrictions: There are minimal restrictions on how the software can be used, modified, or redistributed. The license does not require the distribution of source code.
Popularity: The MIT License is widely used for many open source projects, including software libraries, frameworks, tools, and applications, due to its permissive nature and minimal requirements.
The MIT License is considered a highly permissive license that allows free use, modification, and distribution of the licensed software, while also providing a limited disclaimer of liability for the copyright holders. Its simplicity and permissive nature make it a popular choice for many open source projects.
