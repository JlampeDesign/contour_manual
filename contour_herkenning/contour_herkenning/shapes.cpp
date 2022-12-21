#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;

Mat imgGray, imgBlur, imgCanny, imgDil, imgErode;

Mat preprocessing(Mat img) {

	cvtColor(img, imgGray, COLOR_BGR2GRAY);
	GaussianBlur(imgGray, imgBlur, Size(3, 3), 3, 0);
	Canny(imgBlur, imgCanny, 25, 75);

	Mat kernel = getStructuringElement(MORPH_RECT, Size(3, 3));
	dilate(imgCanny, imgDil, kernel);

	return(imgDil);
}

void getContours(Mat imgDil, Mat img) {

	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;

	findContours(imgDil, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

	vector<vector<Point>> conPoly(contours.size());
	vector<Rect> boundRect(contours.size());

	for (int i = 0; i < contours.size(); i++) {

		int area = contourArea(contours[i]);
		string objectType;

		if (area > 1000){

			float peri = arcLength(contours[i], true);
			approxPolyDP(contours[i], conPoly[i], 0.02 * peri, true);

			boundRect[i] = boundingRect(conPoly[i]);
			int objCor = (int)conPoly[i].size();

			if (objCor == 3) { objectType = "Triangle"; }

			else if (objCor == 4){

				float aspRatio = (float)boundRect[i].width / (float)boundRect[i].height;

					if (aspRatio > 0.95 && aspRatio < 1.05) { objectType = "Square";}
					else { objectType = "Rectangle"; }

			} else if (objCor > 4) { objectType = "Circle"; }

			drawContours(img, conPoly, i, Scalar(255, 0, 255), 2);
			rectangle(img, boundRect[i].tl(), boundRect[i].br(), Scalar(0, 255, 0), 5);
			putText(img, objectType, { boundRect[i].x,boundRect[i].y - 5 }, FONT_HERSHEY_PLAIN, 1, Scalar(0, 69, 255), 1.2);

		}
	}
}


/// main function ///

int main() {

	string path = "images/shapes.png";
	Mat img = imread(path);

	// Preprocessing
	preprocessing(img);

	// shape detecting
	getContours(imgDil, img);

	imshow("Shapes", img);
	waitKey(0);
}