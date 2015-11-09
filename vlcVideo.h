/*
 * Author:	Cheng-Shih, Wong
 * Date:	2015/11/09
 * Email:	mob5566@gmail.com
 *
 */ 
#ifndef _VLC_VIDEO_
#define _VLC_VIDEO_

#include <vlc/vlc.h>
#include <Windows.h>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;

class vlcVideo {

private:
	struct CTX
	{
		Mat* image;
		HANDLE mutex;
		uchar* pixels;
	};

	static void *lock(void *data, void**p_pixels);
	static void display(void *data, void *id);
	static void unlock(void *data, void *id, void *const *p_pixels);

	// VLC pointers
	libvlc_instance_t *vlcInstance;
	libvlc_media_player_t *mp;
	libvlc_media_t *media;
	CTX *context;
	
	// OpenCV parameters
	char *_windowName;
	char *_url;
	int _videoWidth;
	int _videoHeight;

public:
	vlcVideo( char *windowname, char *url, int width, int height );
	~vlcVideo();

	void videoInit();
};

#endif