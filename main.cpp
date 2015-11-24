/* 
 * Author:	Cheng-Shih, Wong
 * Date:	2015/11/03
 * E-mail:	mob5566@gmail.com
 * 
 */

#include <iostream>
#include <fstream>
#include <windows.h>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <string>
#include "DirectX.h"
#include "SerialClass.h"
#include "ptzController.h"
#include <vlc/vlc.h>

#define PI acos(-1.0)
#define sqr(x) ((x)*(x))

using namespace std;
using namespace cv;

struct CTX
{
   Mat* image;
   HANDLE mutex;
   uchar* pixels;
};

void *lock(void *data, void**p_pixels);
void display(void *data, void *id);
void unlock(void *data, void *id, void *const *p_pixels);

int main( int argc, char *argv[] )
{
	
	//
	// Initialization parameters
	//
	char *ptzUrl = "rtsp://192.168.100.150:8557/h264";		// the location of the ptz stream
	char *ptzWindow = "Tracking Camera";					// the window name of ptz
	char *pnrmWindow = "Panorama Camera";					// the window name of panorama
	int pnrmResolutionIndex = 0;							// the default panorama's resolution is set to 7 (2048*1536)
	int ptzWidth = 1920;									// the width of ptz video
	int ptzHeight = 1080;									// the height of ptz video
	int pnrmWidth = 1024;									// the width of panorama video
	int pnrmHeight = 768;									// the height of panorama video
	string outputDataName = "dism";							// the location of the output data file
	double pnrmFL = 3.74;									// the focal length of panorama camera
	double ptzFL = 5.0;									// the focal length of panorama camera
	double pnrmSensorWidth = 4.48;							// the width of panorama sensor in millimeters
	double pnrmSensorHeight = 3.36;							// the height of panorama sensor in millimeters
	double ptzSensorWidth = 5.12;							// the width of ptz sensor in millimeters
	double ptzSensorHeight = 2.88;							// the height of ptz sensor in millimeters
	bool errorCorrection = true;							// if errorCorrection is true, the error correction is turn on

	// parameters for calibration
	double circleRadius = 154.0/2;							// the real radius of circle in millimeters
	double pnrmDis;											// the distance between panorama camera and circle center
	double ptzDis;											// the distance between ptz camera and circle center
	double baseline = 300.0;								// the distance between ptz camera and panorama camera
	double theta;											// the angle between ptz camera and panorama camera


	//
	// Initialization
	//

	// Open the vlc PTZ camera 

	namedWindow( ptzWindow, WINDOW_AUTOSIZE );
	moveWindow( ptzWindow, 100, 100 );

	// VLC pointers
	libvlc_instance_t *vlcInstance;
	libvlc_media_player_t *mp;
	libvlc_media_t *media;

	const char * const vlc_args[] = {
		"-I", "dummy", // Don't use any interface
		"--ignore-config", // Don't use VLC's config
		"--extraintf=logger", // Log anything
		"--verbose=2", // Be much more verbose then normal for debugging purpose
	};
	
	vlcInstance = libvlc_new(sizeof(vlc_args) / sizeof(vlc_args[0]), vlc_args);
	
	// Read a distant video stream
	media = libvlc_media_new_location(vlcInstance, ptzUrl);
	// Read a local video file
	// media = libvlc_media_new_path(vlcInstance, "test.mp4");
	
	mp = libvlc_media_player_new_from_media(media);
	
	libvlc_media_release(media);
	
	CTX *context = (CTX *)malloc( sizeof( *context ) );
	context->mutex = CreateMutex(NULL, FALSE, NULL);
	context->image = new Mat(ptzHeight, ptzWidth, CV_8UC3);
	context->pixels = (unsigned char *)context->image->data;   

	libvlc_video_set_callbacks(mp, lock, unlock, display, context);
	libvlc_video_set_format(mp, "RV24", ptzWidth, ptzHeight, ptzWidth * 24 / 8); // pitch = width * BitsPerPixel / 8

	libvlc_media_player_play(mp);

	// Setup the connection to controller PTZ
	PTZController ptzMotion( "COM3", ptzWindow, 1 );
	
	// Open the panorama camera
	namedWindow( pnrmWindow, WINDOW_AUTOSIZE );
	moveWindow( pnrmWindow, 100, 420 );

	HWND pnrmHandle = (HWND) cvGetWindowHandle( pnrmWindow );
	if( pnrmHandle == NULL ) {
		cerr << "Can't get panorama window handle." << endl;
		return -1;
	}

	if( InitializeDirectX() != S_OK ) {
		cerr << "Please update Direct X. Version 9.0 or higher recommended." << endl;
		return -1;
	}

	if( GetDirectXObject() != S_OK ) {
		cerr << "Get DirectX object failed." << endl;
		return -1;
	}

	if( InitializePreviewVideo_SetStreamFormat( pnrmResolutionIndex, pnrmHandle ) != S_OK ) {
		cerr << "Initialization of preview video failed." << endl;
		return -1;
	}

	if( StartPreviewVideo() != S_OK ) {
		cerr << "The preview video can't start!" << endl;
		return -1;
	}

	//
	// Processing
	//
	Mat ptzFrame, pnrmFrame;
	Mat tmpFrame;
	unsigned char *imgBuffer;
	imgBuffer = (unsigned char *) malloc( pnrmWidth*pnrmHeight*3*sizeof(unsigned char) );
	Point2d trackCenter = Point();
	Point pnrmCenter, ptzCenter;
	int radius;
	double dx, dy, alpha, beta;
	char buf[1024];

	namedWindow( "Track Circle" );
	moveWindow( "Track Circle", 700, 100 );

	int frameCnt = 0;
	ofstream fout( "data/"+outputDataName+".txt" );

	waitKey( 2000 );

	while( true ) {

		// get ptz frame
		ptzFrame = Mat(*context->image).clone();

		// get panorama frame
		if( CaptureStillImageToFile(imgBuffer)!=S_OK ) {
			cerr << "Can't get panorama frame!" << endl;
			return -1;
		}
		pnrmFrame = Mat( Size(pnrmWidth,pnrmHeight), CV_8UC3, imgBuffer ).clone();

		flip( pnrmFrame, pnrmFrame, 1 );
		// using HoughCircle to find the circle at panorama video
		cvtColor( pnrmFrame, tmpFrame, CV_BGR2GRAY );
		threshold( tmpFrame, tmpFrame, 50, 255, THRESH_BINARY_INV );

		GaussianBlur( tmpFrame, tmpFrame, Size(9,9), 2, 2 );
		vector<Vec3f> circles;
		HoughCircles( tmpFrame, circles, CV_HOUGH_GRADIENT, 2, tmpFrame.rows, 200, 100 );

		for( size_t i=0; i<circles.size(); ++i ) {

			// assume that we will get only one circle
			pnrmCenter = Point( cvRound(circles[0][0]), cvRound(circles[0][1]) );
			radius = cvRound(circles[0][2]);

			dx = pnrmCenter.x-(pnrmWidth/2);
			dy = -pnrmCenter.y+(pnrmHeight/2);
	
			dx = dx*(pnrmSensorWidth/pnrmWidth);
			dy = dy*(pnrmSensorHeight/pnrmHeight);
	
			alpha = atan(dx/pnrmFL)*(180.0/PI);
			beta = atan(dy/pnrmFL)*(180.0/PI);
			pnrmDis = circleRadius*pnrmFL/(radius*(pnrmSensorHeight/pnrmHeight));
			
			if( frameCnt%10 == 0 ) {
				fout << "The " << frameCnt/10 << " round" << endl;

				fout << "Panorama:" << endl;
				fout << "\tcenter (" << pnrmCenter.x << ", " << pnrmCenter.y << ")" << endl;
				fout << "\tradius in pixels " << radius << endl;
				fout << "\tdiff in degrees (" << alpha << ", " << beta << ")" << endl;
				// theta = beta;
				
				fout << "\tdistance to center " << pnrmDis << endl;
			}

			if( errorCorrection ) {
				theta = 90.0-beta;
			
				ptzDis = sqrt( sqr(pnrmDis)+sqr(baseline)-2*pnrmDis*baseline*cos(theta/180*PI) );
				theta = acos( (sqr(ptzDis)+sqr(baseline)-sqr(pnrmDis))/(2*ptzDis*baseline) )*180/PI;

				beta = theta-90.0;
			}

			if( frameCnt%10 == 0 ) {
				fout << "\tptz Distance is " << ptzDis << endl;
				fout << "\tbeta offset " << beta << endl;
			}

			if( abs(alpha-trackCenter.x)>1.0 || abs(beta-trackCenter.y)>1.0 )
					trackCenter = Point2d(alpha,beta);
			ptzMotion.moveTo( trackCenter.x, trackCenter.y );

			circle( pnrmFrame, pnrmCenter, 3, Scalar(0,255,0), -1, 8, 0 );
			line( pnrmFrame, Point(pnrmWidth/2-10,pnrmHeight/2), Point(pnrmWidth/2+10,pnrmHeight/2), Scalar(0,0,255), 3, 8, 0);
			line( pnrmFrame, Point(pnrmWidth/2,pnrmHeight/2-10), Point(pnrmWidth/2,pnrmHeight/2+10), Scalar(0,0,255), 3, 8, 0);

			if( frameCnt%10 == 0 ) {
				sprintf( buf, "pnrm.%s.%d.jpg", outputDataName.c_str(), frameCnt/10 );
				imwrite( "imgs/"+string(buf), pnrmFrame );
			}
		}

		resize(pnrmFrame, tmpFrame, Size(pnrmFrame.cols/2,pnrmFrame.rows/2));
		imshow( "Track Circle", tmpFrame );

		// if( frameCnt%10 == 0 ) {
			// using HoughCircle to find the circle at ptz video
			cvtColor( ptzFrame, tmpFrame, CV_BGR2GRAY );
			threshold( tmpFrame, tmpFrame, 50, 255, THRESH_BINARY_INV );
	
			GaussianBlur( tmpFrame, tmpFrame, Size(9,9), 2, 2 );
			circles.clear();
			HoughCircles( tmpFrame, circles, CV_HOUGH_GRADIENT, 2, tmpFrame.rows, 200, 100 );

			for( size_t i=0; i < circles.size(); ++i ) {
				ptzCenter = Point( cvRound(circles[0][0]), cvRound(circles[0][1]) );
				radius = cvRound(circles[0][2]);
		
				dx = ptzCenter.x-(ptzWidth/2);
				dy = -ptzCenter.y+(ptzHeight/2);

				if( frameCnt%10 == 0 ) {
					fout << "PTZ:" << endl;
					fout << "\tcenter (" << ptzCenter.x << ", " << ptzCenter.y << ")" << endl;
					fout << "\tdiff in pixels (" << dx << ", " << dy << ")" << endl;
				}

				dx = dx*(ptzSensorWidth/ptzWidth);
				dy = dy*(ptzSensorHeight/ptzHeight);

				alpha = atan(dx/ptzFL)*(180.0/PI);
				beta = atan(dy/ptzFL)*(180.0/PI);
				// theta -= beta;

				// if( frameCnt == 10 )
					// ptzMotion.move( alpha, beta );
				ptzDis = ptzFL*circleRadius/(radius*(ptzSensorHeight/ptzHeight));

				if( frameCnt%10 == 0 ) {
					fout << "\tdiff in degrees (" << alpha << ", " << beta << ")" << endl;
					fout << "\tradius in pixels " << radius << endl;
					fout << "\tdistance to center " << ptzDis << endl;

				/*
				fout << "//////////////////////////////////////////////////////////////" << endl;

				fout << "theta = " << theta << endl;
				baseline = sqrt(pnrmDis*pnrmDis+ptzDis*ptzDis-2*pnrmDis*ptzDis*cos(theta*PI/180));
				fout << "base line = " << baseline << endl;
				*/
		
					fout << endl << endl;
				}

				circle( ptzFrame, ptzCenter, 3, Scalar(0,255,0), -1, 8, 0 );
				line( ptzFrame, Point(ptzWidth/2-10,ptzHeight/2), Point(ptzWidth/2+10,ptzHeight/2), Scalar(0,0,255), 3, 8, 0);
				line( ptzFrame, Point(ptzWidth/2,ptzHeight/2-10), Point(ptzWidth/2,ptzHeight/2+10), Scalar(0,0,255), 3, 8, 0);

				if( frameCnt%10 == 0 ) {
					sprintf( buf, "ptz.%s.%d.jpg", outputDataName.c_str(), frameCnt/10 );
					imwrite( "imgs/"+string(buf), ptzFrame );
				}
			}
		// }

		resize( ptzFrame, tmpFrame, Size(ptzFrame.cols/4, ptzFrame.rows/4) );
		imshow( ptzWindow, tmpFrame );

		char key = waitKey(30);

		if( key==27 ) break;
		cout << "frame cnt :" << frameCnt << endl;
		++frameCnt;
	}

	fout.close();

	ptzMotion.moveTo( 0.0, 0.0 );

	// Release Resources
	StopPreviewVideo();
	UninitializeDirectX();
	ReleaseDirectXObject();

	libvlc_media_player_stop(mp);
	
	return 0;
}

void *lock(void *data, void**p_pixels)
{
	CTX *ctx = (CTX *)data;

	WaitForSingleObject(ctx->mutex, INFINITE);

	// pixel will be stored on image pixel space
	*p_pixels = ctx->pixels;   

	return NULL;
}

void display(void *data, void *id){
	(void) data;
	assert(id == NULL);
}

void unlock(void *data, void *id, void *const *p_pixels){
   
	// get back data structure 
	CTX *ctx = (CTX *)data;

	/* VLC just rendered the video, but we can also render stuff */
	// show rendered image

	// Mat tmpFrame(*ctx->image);
	// resize( tmpFrame, tmpFrame, Size(tmpFrame.cols/4, tmpFrame.rows/4) );
	// imshow( "Tracking Camera", tmpFrame );

	ReleaseMutex(ctx->mutex);
}
