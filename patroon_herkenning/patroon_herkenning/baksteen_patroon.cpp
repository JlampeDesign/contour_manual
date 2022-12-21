#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;

Mat imgGray, imgBlur, imgCanny, imgDil, imgErode, smallDil;

Mat preprocessing(Mat img) {

	cvtColor(img, imgGray, COLOR_BGR2GRAY);
	GaussianBlur(imgGray, imgBlur, Size(3, 3), 3, 0);
	Canny(imgBlur, imgCanny, 25, 75);

	Mat kernel = getStructuringElement(MORPH_RECT, Size(3, 3));
	dilate(imgCanny, imgDil, kernel);

	return(imgDil);
}

void getContours(Mat img) {

	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;

	findContours(img, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	//drawContours(img, contours, -1, Scalar(0, 0, 0), 2);

	vector<vector<Point>> conPoly(contours.size());
	vector<Rect> boundRect(contours.size());

	for (int i = 0; i < contours.size(); i++) {

		int area = contourArea(contours[i]);
		string objectType;

		if (area > 1000)
		{
			float peri = arcLength(contours[i], true);
			approxPolyDP(contours[i], conPoly[i], 0.02 * peri, true);

			if (conPoly[i].size() == 4) {

				string brickText;

				float topLocaX = conPoly[i][0].x;
				float btmLocaY = conPoly[i][2].x;

				if (topLocaX < btmLocaY) {
					brickText = "pattern 1";
					drawContours(smallDil, conPoly, i, Scalar(255, 0, 255), 3);
					putText(smallDil, brickText, { conPoly[i][0].x + 40, conPoly[i][0].y - 20 }, FONT_HERSHEY_PLAIN, 1, Scalar(0, 0, 0), 1);

				}
				else {
					brickText = "pattern 2";
					drawContours(smallDil, conPoly, i, Scalar(0, 255, 0), 3);
					putText(smallDil, brickText, { conPoly[i][2].x + 40, conPoly[i][2].y + 20 }, FONT_HERSHEY_PLAIN, 1, Scalar(0, 0, 0), 1);
				}
			}
		}
	}
}


/// main function ///

int main() {

	string path = "images/brickPattern.png";
	Mat img = imread(path);
	resize(img, smallDil, Size(), 0.8, 0.8);

	// Preprocessing
	preprocessing(smallDil);

	// shape detecting
	getContours(imgDil);

	imshow("Image", smallDil);
	waitKey(0);
}