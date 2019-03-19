#define _CRT_SECURE_NO_WARNINGS

#include<iostream>
#include<opencv2\opencv.hpp>

using namespace std;
using namespace cv;

#define BASE_POS 270

void SkinColor(Mat& frame,Mat& output);
void FindTheCounters(Mat& input, Mat& output, vector<Point>& max_contours);
void TexMapping(Mat& tex, Mat& dst, int threshold_value, Point pos);
void Animation(bool& clock, Mat& wave, Mat& output, Mat& character, vector<Point>& contours);

int main() {
	VideoCapture cap(0);
	if (!cap.isOpened()) {
		cout << "ERROR VideoCapture" << endl;
		return -1;
	}

	Mat frame;
	Mat skin;
	Mat output;
	Mat wave;
	Mat character;
	bool clock = true;
	vector<Point> contours_return;


	while (true) {
		cap >> frame;
		if (frame.empty()) {
			break;
		}
		output = imread("bkground.png");
		wave = imread("wave_burned.png");
		character = imread("character.png");
		
		SkinColor(frame,skin);
		contours_return.clear();
		FindTheCounters(skin,output, contours_return);
		for (int i = 0; i < contours_return.size(); i++) {
			cout << contours_return[i] << endl;
		}
		
		Animation(clock, wave, output, character, contours_return);
		imshow("game", output);
		imshow("frame", frame);

		waitKey(30);
	}

	return 0;
}

void SkinColor(Mat& frame,Mat& skin_output) {
	cvtColor(frame, skin_output, CV_BGR2HSV);
	inRange(skin_output, Scalar(0, 58, 40), Scalar(35, 174, 255), skin_output);
}

void FindTheCounters(Mat& input, Mat& output,vector<Point>& contours_return){
	int largest_contour_index = -1;
	int largest_area = 0;
	Rect bounding_rect;
	vector<vector<Point>> contours;
	vector<vector<Point>> convexHull_contours(1);
	vector<Vec4i> hierarchy;
	findContours(input, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	for (int i = 0; i < contours.size(); i++) {
		double a = contourArea(contours[i], false);
		if (a > largest_area) {
			largest_area = a;
			largest_contour_index = i;
			bounding_rect = boundingRect(contours[i]);
		}
	}
	if (largest_contour_index >= 0) {
		Scalar color(0, 0, 255);
		convexHull(contours[largest_contour_index], convexHull_contours[0]);
		drawContours(output, convexHull_contours, 0, color, 5);
		for (int i = 0; i < convexHull_contours[0].size(); ++i)
			contours_return.push_back(convexHull_contours[0][i]);
	}

}

void TexMapping(Mat& tex,Mat& dst,int threshold_value,Point pos){
	Mat mask;
	cvtColor(tex, mask, CV_BGR2GRAY);
	threshold(mask, mask, threshold_value, 255, THRESH_BINARY_INV);
	for (int j = 0; j < mask.rows; j++) {
		for (int i = 0; i < mask.cols; i++) {
			if (mask.at<uchar>(j, i) == 255) {
				if (((pos.y + j) >= 0) && ((pos.y + j) < dst.rows) && ((pos.x + i) >= 0) && ((pos.x + i) < dst.cols)) {
					dst.at<Vec3b>(pos.y + j, pos.x + i)[0] = tex.at<Vec3b>(j, i)[0];
					dst.at<Vec3b>(pos.y + j, pos.x + i)[1] = tex.at<Vec3b>(j, i)[1];
					dst.at<Vec3b>(pos.y + j, pos.x + i)[2] = tex.at<Vec3b>(j, i)[2];
				}
			}
		}
	}

}

void Animation(bool& clock, Mat& wave, Mat& output, Mat& character, vector<Point>& contours) {
	int wave_hight = BASE_POS;
	for (int i = 0; i < contours.size(); i++) {
		if ((contours[i].x <= 150) && contours[i].y < wave_hight) {
			wave_hight = contours[i].y;
		}
	}

	if (clock == true) {
		TexMapping(wave, output, 150, Point(0, 371));
		TexMapping(character, output, 245, Point(0, wave_hight-10));
		clock = false;
	}
	else {
		TexMapping(wave, output, 150, Point(-50, 371));
		TexMapping(character, output, 245, Point(0, wave_hight));
		clock = true;
	}
	
}
