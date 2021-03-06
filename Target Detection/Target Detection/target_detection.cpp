#include "pch.h"
#include <iostream>
#include "opencv2/opencv.hpp"
#include <string>
#include <vector>
#include <algorithm>
#include <stdio.h>


using namespace cv;
using namespace std;

String Target1_name = "target1.xml";
String Target2_name = "target2.xml";
CascadeClassifier Target1;
CascadeClassifier Target2;

int find_target(Mat image, int lower1, int lower2, int lower3, int upper1, int upper2, int upper3, int shape);
void cascade_target1(Mat image);
void cascade_target2(Mat image);
int video_mode();

int main(int argv, char** argc)
{
	/*load image*/
	Mat image = imread("square.png", CV_LOAD_IMAGE_UNCHANGED); // using cv to store the data as a class
	int blue_triangle = find_target(image, 100, 150, 0, 140, 255, 255, 3);
	int	orange_square = find_target(image, 5, 50, 50, 15, 255, 255, 4);
	if (blue_triangle == 1) {
		cout << "Find Blue Triangle" << "\n" << "Person with Haemorrhage" << endl;
	}
	else if (orange_square == 1) {
		cout << "Find Orange Square" << "\n" << "Person with Anaphylaxis" << endl;
	}
	else {
		imshow("Input Image", image);
		cout << "no target" << endl;
	}
	waitKey();

	//if (!Target1.load(Target1_name)) { printf("--(!)Error loading\n"); return -1; };
	//if (!Target2.load(Target2_name)) { printf("--(!)error loading\n"); return -1; };

	//video_mode();

}

int find_target(Mat image, int lower1, int lower2, int lower3, int upper1, int upper2, int upper3, int shape) {
	//Store image as RGB
	Mat RGB_image = image.clone();
	//Convert RGB to HSV
	Mat HSV_image;
	cvtColor(RGB_image, HSV_image, COLOR_BGR2HSV);
	//find color
	Mat HSV_target;
	inRange(HSV_image, Scalar(lower1, lower2, lower3), Scalar(upper1, upper2, upper3), HSV_target);
	//find shape
	Scalar find_color = sum(HSV_target);
	if (find_color[0] != 0) { // threshold color
		Mat edges;
		Mat gray;
		vector<vector<Point>> contours;
		vector<Vec4i> hierarchy;
		vector<Point> approx;
		Scalar color = Scalar(0, 255, 0);
		cvtColor(RGB_image, gray, COLOR_BGR2GRAY);
		Canny(gray, edges, 20, 200, 3, false);
		findContours(edges.clone(), contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
		Mat drawing = Mat::zeros(edges.size(), CV_8UC3);
		for (int i = 0; i < contours.size(); i++) {
			if (contours.size() == 4) { // threshold filled shape
				approxPolyDP(Mat(contours[i]), approx, arcLength(Mat(contours[i])*0.02, true), true);
				if (approx.size() == shape) { // threshold shape
					drawContours(drawing, contours, i, color, 2, 8, hierarchy, 0, Point());
					if (arcLength(Mat(contours[i])*0.02, true) > 0) { // threshold size
						imshow("Output image", drawing);
						imshow("Input Image", RGB_image);
						if (i == 3) { // finish for loop and return 1 that means everything pass, so target is found
							return 1;
						}
					}
				}
			}
		}
	}
	else {
		return 0;
	}
}

void cascade_target1(Mat image) {
	vector<Rect> target1;
	Mat frame_gray;
	cvtColor(image, frame_gray, CV_BGR2GRAY);
	equalizeHist(frame_gray, frame_gray);

	Target1.detectMultiScale(frame_gray, target1, 1.1, 2, 0 | CV_HAAR_SCALE_IMAGE, Size(30, 30));

	for (size_t i = 0; i < target1.size(); i++) {
		Point center(target1[i].x + target1[i].width*0.5, target1[i].y + target1[i].height*0.5);
		ellipse(image, center, Size(target1[i].width*0.5, target1[i].height*0.5), 0, 0, 360, Scalar(255, 0, 255), 4, 8, 0);
		Mat faceROI = frame_gray(target1[i]);
	}
	imshow("Output image", image);

}

void cascade_target2(Mat image) {
	vector<Rect> target2;
	Mat frame_gray;
	cvtColor(image, frame_gray, CV_BGR2GRAY);
	equalizeHist(frame_gray, frame_gray);


	Target2.detectMultiScale(frame_gray, target2, 1.1, 2, 0 | CV_HAAR_SCALE_IMAGE, Size(30, 30));

	for (size_t j = 0; j < target2.size(); j++) {
		Point center(target2[j].x + target2[j].width*0.5, target2[j].y + target2[j].height*0.5);
		ellipse(image, center, Size(target2[j].width*0.5, target2[j].height*0.5), 0, 0, 360, Scalar(255, 0, 255), 4, 8, 0);
		Mat faceROI = frame_gray(target2[j]);
	}
	imshow("Output image", image);
}

int video_mode()
{
	VideoCapture cap(0); // open the video camera no. 0

	if (!cap.isOpened())  // if not success, exit program
	{
		cout << "Cannot open the video cam" << endl;
		return -1;
	}

	double dWidth = cap.get(CV_CAP_PROP_FRAME_WIDTH); //get the width of frames of the video
	double dHeight = cap.get(CV_CAP_PROP_FRAME_HEIGHT); //get the height of frames of the video

	cout << "Frame size : " << dWidth << " x " << dHeight << endl;

	namedWindow("MyVideo", CV_WINDOW_AUTOSIZE); //create a window called "MyVideo"

	while (1)
	{
		Mat frame;
		bool bSuccess = cap.read(frame); // read a new frame from video
		if (!bSuccess) //if not success, break loop
		{
			cout << "Cannot read a frame from video stream" << endl;
			break;
		}
		imshow("MyVideo", frame); //show the frame in "MyVideo" window
		if (waitKey(30) == 27) //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
		{
			cout << "esc key is pressed by user" << endl;
			cascade_target1(frame);
			cascade_target2(frame);
			break;
		}

	}
	return 0;
}
