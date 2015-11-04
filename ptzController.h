/* 
 * Author:	Cheng-Shih, Wong
 * Date:	2015/11/04
 * E-mail:	mob5566@gmail.com
 * 
 */

#ifndef _PTZ_CONTROLLER_H_
#define _PTZ_CONTROLLER_H_

#include "SerialClass.h"

class PTZController {

private:
	// serial port connector 
	Serial* _sp;
	
public:
	// PTZ constructor & destructor
	PTZController( char *portname );
	~PTZController();

	// 
};

#endif