/* 
 * Author:	Cheng-Shih, Wong
 * Date:	2015/11/03
 * E-mail:	mob5566@gmail.com
 * 
 */

#include <iostream>
#include <windows.h>
#include <opencv2/opencv.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <string>
#include "DirectX.h"
#include "SerialClass.h"

using namespace std;
using namespace cv;

int main( int argc, char *argv[] )
{
	
	//
	// Initialization parameters
	//
	string ptzUrl = "rtsp://192.168.50.41:8557/h264";		// the location of the ptz stream
	string ptzWindow = "Tracking Camera";					// the window name of ptz
	string pnrmWindow = "Panorama Camera";					// the window name of panorama
	int pnrmResolutionIndex = 7;							// the default panorama's resolution is set to 7 (2048*1536)

	//
	// Initialization
	//

	// Open the ptz camera
	VideoCapture ptzCapture(ptzUrl);
	namedWindow( ptzWindow );
	if( !ptzCapture.isOpened() ) {
		cerr << "PTZ camera is not connected!" << endl;
		return -1;
	}

	/*
	// Open the panorama camera
	namedWindow( pnrmWindow, CV_WINDOW_NORMAL|CV_GUI_NORMAL );
	wstring tmpwstr = wstring( pnrmWindow.begin(), pnrmWindow.end() );
	HWND pnrmHandle = ::FindWindow( 0, tmpwstr.c_str() );
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
	*/

	//
	// Processing
	//
	Mat ptzFrame;

	while( true ) {
		if( !ptzCapture.read(ptzFrame) ) break;
		imshow( ptzWindow, ptzFrame );
		waitKey(30);
	}

	// Release Resources

	/*
	hr = StopPreviewVideo();
	hr = UninitializeDirectX();
	hr = ReleaseDirectXObject();
	*/

	
	return 0;
}