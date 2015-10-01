#include "Processor.h"

Processor::Processor(vector<Mat> pics)
{
	threshold = 18;
	images = pics;

	undistort();

	extractGolfballs();
}

float Processor::getSpeed()
{
	float pixels_per_inch = 34;
	float ips_to_mph = 1.0/17.6;
	float seconds = 1.0/750.0;
	return ((golfballs[1].distance - golfballs[0].distance)/pixels_per_inch/seconds)*ips_to_mph;
}

float Processor::getDirection()
{
	float horizontal_change = golfballs[1].horizontal_position - golfballs[0].horizontal_position;
	float distance_change = golfballs[1].distance - golfballs[0].distance;
	return atan(horizontal_change/distance_change)*180/3.14159;
}

float Processor::getLaunchAngle()
{
	float distance_change = golfballs[1].distance - golfballs[0].distance;
	float vertical_change = golfballs[1].height - golfballs[0].height;
	return atan(vertical_change/distance_change)*180/3.14159;
}

float Processor::getDistance()
{
	return 0;	
}

float Processor::getHeight()
{
	return 0;
}

void Processor::undistort()
{
	Point2f inputQuad[4], outputQuad[4];

    // These four pts are the sides of the rect box used as input 
	inputQuad[0] = Point2f( 390,124);
    inputQuad[1] = Point2f( 746,106);
    inputQuad[2] = Point2f( 748,422);
    inputQuad[3] = Point2f( 388,412);  
    // The 4 points where the mapping is to be done , from top-left in clockwise order
    outputQuad[0] = Point2f( 389, 106);
    outputQuad[1] = Point2f( 747, 106);
    outputQuad[2] = Point2f( 747, 422);
    outputQuad[3] = Point2f( 389, 422);

	Mat perspective = getPerspectiveTransform(inputQuad, outputQuad);

	for(unsigned int i = 0; i < images.size(); i++)
	{
		warpPerspective(images[i], images[i], perspective, Size(808, 608));
	}
}

void Processor::extractGolfballs()
{
	for(unsigned int i = 0; i < images.size(); i++)
	{
		vector<vector<Point> > contours;
		vector<Vec4i> hierarchy;

		// Find Contours
		Mat white_area;
		inRange(images[i], Scalar(threshold), Scalar(255), white_area);
		findContours( white_area, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );

		// Find Largest Contour
		Rect brect;
		int max_diameter = 0;
		int max_idx = 0;
		for(unsigned int j = 0; j< contours.size(); j++)
		{
			drawContours( white_area, contours, j, Scalar(200), 1, 8, hierarchy, 0, Point() );
			brect = boundingRect(contours.at(j));
			if (brect.height > max_diameter && brect.y < 350 && brect.y > 50)
			{
				max_diameter = brect.height;
				max_idx = j;
			}
		}
		if(contours.size() > 0)
		{
			brect = boundingRect(contours.at(max_idx));
			rectangle(white_area, brect, Scalar(255));
			cout << "Diameter: " << brect.height << " X: " << (747 - brect.x) << endl;
			Golfball golfball;
			golfball.diameter = brect.height;
			golfball.distance = images[i].size().width - brect.x;
			golfball.horizontal_position = brect.y + brect.height/2.0;
			golfball.height = brect.height - 63;
			golfballs.push_back(golfball);
		}

	  
		float alpha = 0.75;
		addWeighted(images[i], alpha, white_area, 1.0-alpha, 0.0, white_area);
		imshow( "Source", white_area);

		waitKey(0);
	}
}
