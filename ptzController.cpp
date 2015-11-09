/* 
 * Author:	Cheng-Shih, Wong
 * Date:	2015/11/04
 * E-mail:	mob5566@gmail.com
 * 
 */

#include "ptzController.h"

#include <iostream>

using namespace std;

//
// CONSTRUCTOR
//

// Initialize the com port to portname
// the default portname is "COM3"
PTZController::PTZController( char *url, char *portname="COM3", char *windowname="PTZ Camera", int ptznumber=1 ) {
	this->_sp = new Serial(portname);
	this->_ptzNumber = ptznumber;
	this->_power = false;
	this->_url = url;
	this->_windowName = windowname;
	this->_videoPlayer = new vlcVideo( _windowName, _url, 1920, 1080 );

	if( !this->_sp->IsConnected() ) {
		cerr << "Can not connect to PTZ motion controller!" << endl;
	}

	this->_videoPlayer->videoInit();
}

// Destructor close the com port
// connection
PTZController::~PTZController() {
	if( this->_sp->IsConnected() ) {
		this->_sp->~Serial();
	}
}

//
// POWER
//

// Return the PTZ camera is on or off
bool PTZController::isOn() {
	return _power;
}

// Turn the PTZ camera on
void PTZController::turnOn() {
	if( !_power ) {
	
	}
}

// Turn the PTZ camera off 
void PTZController::turnOff() {
	if( _power ) {
	
	}
}

//
// MOTION
//

// Move the PTZ camera with horizontal alpha degree (-170 to +170),
// and beta degree (-30 to +90)
void PTZController::moveTo( double alpha=0.0, double beta=0.0 ) {
	
}

//
// VIDEO
//

