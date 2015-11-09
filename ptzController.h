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
	Serial *_sp;			// serial port connector 
	int _ptzNumber;			// the number of PTZ camera
	bool _power;			// the power status
	char *_windowName;		// the Opencv window name
	
public:
	// PTZ CONSTRUCTOR & DESTRUCTOR
	PTZController( char *portname, char *windowname, int ptznumber );
	~PTZController();

	// PTZ POWER CONTROLLER
	bool isOn();
	void turnOn();
	void turnOff();

	// PTZ MOTION CONTROLLER
	void moveTo( double alpha, double beta );

	// PTZ VIDEO CONTROLLER
};

#endif