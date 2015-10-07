#include "Processor.h"

Processor::Processor(vector<Mat>& pics, Mat chessboard)
{
	threshold = 18;
	pixels_per_inch = 34;
	images = pics;
	this->chessboard = chessboard;

	undistort();

	extractGolfballs();
}

float Processor::getSpeed()
{
	float ips_to_mph = 1.0/17.6;
	float seconds = 1.0/750.0;
	float distance = sqrt(pow((golfballs[1].distance - golfballs[0].distance), 2) + pow((golfballs[1].horizontal_position - golfballs[0].horizontal_position), 2) + pow((golfballs[1].height - golfballs[0].height), 2));
	return (distance/pixels_per_inch/seconds)*ips_to_mph;
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
	vector<Point2f> corners;
	int x_dim = 9;
	int y_dim = 6;
	Size size(x_dim, y_dim);
	bool found = findChessboardCorners(chessboard, size, corners, CV_CALIB_CB_ADAPTIVE_THRESH); 
	drawChessboardCorners(chessboard, size, Mat(corners), found);
	namedWindow("win", 1);
	imshow("win", chessboard);
	waitKey(0);

	Point2f inputQuad[4], outputQuad[4];


    // These four pts are the sides of the rect box used as input 
	inputQuad[0] = corners[0];
    inputQuad[1] = corners[x_dim - 1];
    inputQuad[2] = corners[corners.size() - 1];
    inputQuad[3] = corners[corners.size() - x_dim]; 

	float edge_length_vert = inputQuad[2].y - inputQuad[1].y;	
	float edge_length_horiz = edge_length_vert*(x_dim-1)/(y_dim-1);
    // The 4 points where the mapping is to be done , from top-left in clockwise order
	// The top right corner is used as the reference point, everything is rotated around it
    outputQuad[0] = Point2f( inputQuad[1].x - edge_length_horiz, inputQuad[1].y);
    outputQuad[1] = inputQuad[1];
    outputQuad[2] = Point2f(inputQuad[1].x, inputQuad[2].y);
    outputQuad[3] = Point2f( inputQuad[1].x - edge_length_horiz, inputQuad[2].y);


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
	warpPerspective(chessboard, chessboard, perspective, chessboard.size());
	imshow("win", chessboard);
	waitKey(0);
	for(unsigned int i = 0; i < images.size(); i++)
	{
		warpPerspective(images[i], images[i], perspective, images[i].size(), INTER_LINEAR, BORDER_CONSTANT, 5);
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
			//drawContours( images[i], contours, j, Scalar(200), 1, 8, hierarchy, 0, Point() );
			brect = boundingRect(contours.at(j));
			if (brect.height > max_diameter && brect.height < 150 && brect.y < 400 && brect.y > 100)
			{
				max_diameter = brect.height;
				max_idx = j;
			}
		}
		if(contours.size() > 0)
		{
			brect = boundingRect(contours.at(max_idx));
			RotatedRect golf_ellipse = fitEllipse(contours.at(max_idx));
				

			Golfball golfball;
			golfball.diameter = getSubPixelHeight(i, contours.at(max_idx));
			golfball.distance = images[i].size().width - brect.x; 	//From right of image to left of golfball
			golfball.horizontal_position = images[i].size().height - (brect.y + brect.height/2.0); 	// From top of image to middle of golfball
			golfball.height = golfball.diameter * 2;
			golfballs.push_back(golfball);

			cout << "Diameter: " << golfball.diameter << " Distance: " << golfball.distance << " Horizontal: " << golfball.horizontal_position << " Height: " << golfball.height << endl;
			rectangle(images[i], brect, Scalar(255));
			cout << "ellipse size: " << golf_ellipse.size << "\t";
			cout << "aspect ratio: " << golf_ellipse.size.height/golf_ellipse.size.width << "\t";
			cout << "angle: " << golf_ellipse.angle << endl;
			ellipse(images[i], golf_ellipse, Scalar(255), 2, 8);
		}

	}
}

float Processor::getSubPixelHeight(unsigned int index, vector<Point> contour)
{
	vector<Point2f> edge_tips;
	//Find min/max
	Point max_y_point = contour.at(0);
	Point min_y_point = contour.at(0);
	Point max_x_point = contour.at(0);
	Point min_x_point = contour.at(0);
	for(Point p: contour)
	{
		if(p.y > max_y_point.y) max_y_point = p;
		if(p.y < min_y_point.y) min_y_point = p;
		if(p.x > max_x_point.x) max_x_point = p;
		if(p.x < min_x_point.x) min_x_point = p;
	}
	edge_tips.push_back(max_y_point);
	edge_tips.push_back(min_y_point);
	edge_tips.push_back(max_x_point);
	edge_tips.push_back(min_x_point);

	//Find sub-pixel tips
	Size winSize = Size( 3, 3 );
	Size zeroZone = Size( -1, -1 );
	TermCriteria criteria = TermCriteria( CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 50, 0.001 );
	cornerSubPix(images[index], edge_tips, winSize, zeroZone, criteria);
	
	//cout << "width: " << edge_tips[2].x - edge_tips[3].x << endl;
	//cout << "height: " << edge_tips[0].y - edge_tips[1].y << endl;

	return (edge_tips[0].y - edge_tips[1].y);
}

