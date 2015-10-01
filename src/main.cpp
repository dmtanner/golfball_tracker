#include <opencv2/opencv.hpp>
#include <vector>
#include "Processor.h"

using namespace cv;
using namespace std;

int main(int argc, char** argv)
{
	vector<Mat> pics;
	if(argc < 3)
	{
		cout << "not enough arguments" << endl;
		exit(0);
	}

	
	pics.push_back(imread(argv[1], 0));
	pics.push_back(imread(argv[2], 0));
	Processor processor(pics);

	cout << "Speed: " << processor.getSpeed() << " mph" << endl;	
	cout << "Direction: " << processor.getDirection() << " degrees" << endl;	
	cout << "Launch Angle: " << processor.getLaunchAngle() << " degrees" << endl;	
	return 0;
}

