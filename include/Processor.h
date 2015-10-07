#include <opencv2/opencv.hpp>
#include <vector>
#include <sys/time.h>
#include "Golfball.h"

using namespace cv;
using namespace std;

class Processor
{
	public:
		Processor(vector<Mat>& pics, Mat chessboard);
		float getSpeed();
		float getDirection();
		float getLaunchAngle();

	private:
		vector<Mat> images;
		vector<Golfball> golfballs;
		Mat chessboard;
		float pixels_per_inch;
		int threshold;
		void extractGolfballs();
		void undistort();
		float getHeight();
		float getDistance();
		float getSubPixelHeight(unsigned int index, vector<Point> contour);


};
