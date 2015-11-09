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
#include <vlc/vlc.h>

using namespace std;
using namespace cv;

struct CTX
{
   Mat* image;
   HANDLE mutex;
   uchar*    pixels;
};

void *lock(void *data, void**p_pixels);
void display(void *data, void *id);
void unlock(void *data, void *id, void *const *p_pixels);

int main( int argc, char *argv[] )
{
	
	//
	// Initialization parameters
	//
	char *ptzUrl = "rtsp://192.168.50.41:8557/h264";		// the location of the ptz stream
	char *ptzWindow = "Tracking Camera";					// the window name of ptz
	char *pnrmWindow = "Panorama Camera";					// the window name of panorama
	int pnrmResolutionIndex = 6;							// the default panorama's resolution is set to 7 (2048*1536)
	int ptzWidth = 1920;									// the width of ptz video
	int ptzHeight = 1080;									// the height of ptz video

	//
	// Initialization
	//

	// VLC PTZ video

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
	media = libvlc_media_new_location(vlcInstance, "rtsp://192.168.50.41:8557/h264");
	// Read a local video file
	// media = libvlc_media_new_path(vlcInstance, "test.mp4");
	
	mp = libvlc_media_player_new_from_media(media);
	
	libvlc_media_release(media);
	
	CTX *context = (CTX *)malloc( sizeof( *context ) );
	context->mutex = CreateMutex(NULL, FALSE, NULL);
	context->image = new Mat(ptzHeight, ptzWidth, CV_8UC3);
	context->pixels = (unsigned char *)context->image->data;   
	// show blank image
	// imshow("test", *context->image);

	libvlc_video_set_callbacks(mp, lock, unlock, display, context);
	libvlc_video_set_format(mp, "RV24", ptzWidth, ptzHeight, ptzWidth * 24 / 8); // pitch = width * BitsPerPixel / 8

	libvlc_media_player_play(mp);
	
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
		// if( !ptzCapture.read(ptzFrame) ) break;
		// resize( ptzFrame, tmp, Size(ptzFrame.cols/4, ptzFrame.rows/4) );
		// imshow( ptzWindow, tmp );
		waitKey(30);
	}


	// Release Resources
	hr = StopPreviewVideo();
	hr = UninitializeDirectX();
	hr = ReleaseDirectXObject();

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
	Mat tmp(*ctx->image);
	resize( tmp, tmp, Size(tmp.cols/4, tmp.rows/4) );
	imshow( "Tracking Camera", tmp );

	ReleaseMutex(ctx->mutex);
}

