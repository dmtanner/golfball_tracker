#include <opencv2/opencv.hpp>
#include <vector>
#include "Golfball.h"

using namespace cv;
using namespace std;

class Processor
{
	public:
		Processor(vector<Mat>& pics);
		float getSpeed();
		float getDirection();
		float getLaunchAngle();

	private:
		vector<Mat> images;
		vector<Golfball> golfballs;
		float pixels_per_inch;
		int threshold;
		void extractGolfballs();
		void undistort();
		float getHeight();
		float getDistance();


};
