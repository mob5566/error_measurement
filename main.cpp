/* 
 * Author:	Cheng-Shih, Wong
 * Date:	2015/11/03
 * E-mail:	mob5566@gmail.com
 * 
 */

#include <iostream>
#include <windows.h>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <string>
#include "DirectX.h"
#include "SerialClass.h"
#include "ptzController.h"

using namespace std;
using namespace cv;

int main( int argc, char *argv[] )
{
	
	//
	// Initialization parameters
	//
	char *ptzUrl = "rtsp://192.168.50.41:8557/h264";		// the location of the ptz stream
	char *ptzWindow = "Tracking Camera";					// the window name of ptz
	char *pnrmWindow = "Panorama Camera";					// the window name of panorama
	int pnrmResolutionIndex = 6;							// the default panorama's resolution is set to 7 (2048*1536)

	//
	// Initialization
	//

	PTZController ptz( "COM3", ptzWindow, 1 );
	
	// Open the ptz camera
	VideoCapture ptzCapture("test.avi");
	namedWindow( ptzWindow, WINDOW_AUTOSIZE );
	moveWindow( ptzWindow, 100, 100 );
	if( !ptzCapture.isOpened() ) {
		cerr << "PTZ camera is not connected!" << endl;
		return -1;
	}

	// Open the panorama camera
	namedWindow( pnrmWindow, WINDOW_AUTOSIZE );
	moveWindow( pnrmWindow, 100, 420 );
	HWND pnrmHandle = (HWND) cvGetWindowHandle( pnrmWindow );
	if( pnrmHandle == NULL ) {
		cerr << "Can't get panorama window handle." << endl;
		return -1;
	}

	HRESULT hr;

	hr = InitializeDirectX();
	if( hr != S_OK ) {
		cerr << "Please update Direct X. Version 9.0 or higher recommended." << endl;
		return -1;
	}

	hr = GetDirectXObject();
	if( hr != S_OK ) {
		cerr << "Get DirectX object failed." << endl;
		return -1;
	}

	hr = InitializePreviewVideo_SetStreamFormat( pnrmResolutionIndex, pnrmHandle );
	if( hr != S_OK ) {
		cerr << "Initialization of preview video failed." << endl;
		return -1;
	}

	hr = StartPreviewVideo();
	if( hr != S_OK ) {
		cerr << "The preview video can't start!" << endl;
		return -1;
	}

	//
	// Processing
	//
	Mat ptzFrame, tmp;

	while( true ) {
		if( !ptzCapture.read(ptzFrame) ) break;
		resize( ptzFrame, tmp, Size(ptzFrame.cols/4, ptzFrame.rows/4) );
		imshow( ptzWindow, tmp );
		waitKey(30);
	}


	// Release Resources
	hr = StopPreviewVideo();
	hr = UninitializeDirectX();
	hr = ReleaseDirectXObject();
	
	return 0;
}