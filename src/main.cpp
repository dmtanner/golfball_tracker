#include <opencv2/opencv.hpp>
#include <opencv2/nonfree/features2d.hpp>
#include <opencv2/nonfree/features2d.hpp>
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
	Mat pre;
	addWeighted(pics[0], 0.5, pics[1], 0.5, 0.0, pre);
	Processor processor(pics, imread(argv[3], 0));

	cout << "Speed: " << processor.getSpeed() << " mph" << endl;	
	cout << "Direction: " << processor.getDirection() << " degrees" << endl;	
	cout << "Launch Angle: " << processor.getLaunchAngle() << " degrees" << endl;
	
	Mat post, final;
	float alpha = 0.5;
	addWeighted(pics[0], alpha, pics[1], 1.0-alpha, 0.0, post);
	addWeighted(pre, 0.5, post, 0.5, 0.0, final);
	imshow("source", post);
	waitKey(0);

	return 0;
}

