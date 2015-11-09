/*
 * Author:	Cheng-Shih, Wong
 * Date:	2015/11/09
 * Email:	mob5566@gmail.com
 *
 */ 

#include "vlcVideo.h"

void *vlcVideo::lock( void *data, void **p_pixels )
// void *lock( void *data, void **p_pixels )
{
	CTX *ctx = (CTX *)data;
	
	WaitForSingleObject(ctx->mutex, INFINITE);
	
	// pixel will be stored on image pixel space
	*p_pixels = ctx->pixels;   

	return NULL;

}

void vlcVideo::display(void *data, void *id) {
// void display(void *data, void *id) {
	(void) data;
	assert(id == NULL);
}

void vlcVideo::unlock(void *data, void *id, void *const *p_pixels) {
// void unlock(void *data, void *id, void *const *p_pixels) {
   
	// get back data structure 
	CTX *ctx = (CTX *)data;
	
	/* VLC just rendered the video, but we can also render stuff */
	// show rendered image
	imshow( "test", *ctx->image);
	puts("hi");
	
	ReleaseMutex(ctx->mutex);
}

vlcVideo::vlcVideo( char *windowname, char *url, int width, int height ) {

	const char * const vlc_args[] = {
		"-I", "dummy", // Don't use any interface
		"--ignore-config", // Don't use VLC's config
		"--extraintf=logger", // Log anything
		"--verbose=2", // Be much more verbose then normal for debugging purpose
	};
	
	_windowName = windowname;
	_url = url;
	_videoWidth = width;
	_videoHeight = height;
	vlcInstance = libvlc_new(sizeof(vlc_args) / sizeof(vlc_args[0]), vlc_args);
		
	
	// int ii = 0;
	// int key = 0;
	// while(key != 27)
	// // {
		// ii++;
		// if (ii > 5)
		// {
		// }
		// float fps =  libvlc_media_player_get_fps(mp);
		// printf("fps:%f\r\n",fps);
		// key = waitKey(100); // wait 100ms for Esc key
	// }
}

vlcVideo::~vlcVideo() {
	libvlc_media_player_stop(mp);
}

void vlcVideo::videoInit() {
	
	// Read a distant video stream
	media = libvlc_media_new_location(vlcInstance, _url );
	// Read a local video file
	// media = libvlc_media_new_path(vlcInstance, "test.mp4");
	
	mp = libvlc_media_player_new_from_media(media);
	
	libvlc_media_release(media);
	
	context = (CTX *) malloc( sizeof(CTX) );
	context->mutex = CreateMutex(NULL, FALSE, NULL);
	context->image = new Mat(_videoHeight, _videoWidth, CV_8UC3);
	context->pixels = (unsigned char *)context->image->data;   

	// show blank image
	imshow( "test", *context->image);
	
	libvlc_video_set_callbacks(mp, lock, unlock, display, context);
	libvlc_video_set_format(mp, "RV24", _videoWidth, _videoWidth, _videoWidth * 24 / 8); // pitch = width * BitsPerPixel / 8
	
	libvlc_media_player_play(mp);
}