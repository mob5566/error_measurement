/* 
 * Author:	Cheng-Shih, Wong
 * Date:	2015/11/04
 * E-mail:	mob5566@gmail.com
 * 
 */

#include "ptzController.h"

#include <iostream>

using namespace std;

static char buf[1024];
const double PTZController::panStepPerDeg = 14976.0/170.0;
const double PTZController::tiltStepPerDeg = 15840.0/90.0;


//
// CONSTRUCTOR
//

// Initialize the com port to portname
// the default portname is "COM3"
PTZController::PTZController( char *portname="COM3", char *windowname="PTZ Camera", int ptznumber=1 ) {
	this->_sp = new Serial(portname);
	this->_ptzNumber = ptznumber;
	this->_power = false;
	this->_windowName = windowname;

	if( !this->_sp->IsConnected() ) {
		cerr << "Can not connect to PTZ motion controller!" << endl;
	}

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
		_power = true;
		sprintf( buf, "%c%c%c%c%c%c", 0x80+_ptzNumber, 0x01, 0x04, 0x00, 0x02, 0xFF );
		_sp->WriteData( buf, 8 );
	}
}

// Turn the PTZ camera off 
void PTZController::turnOff() {
	if( _power ) {
		_power = false;
		sprintf( buf, "%c%c%c%c%c%c", 0x80+_ptzNumber, 0x01, 0x04, 0x00, 0x03, 0xFF );
		_sp->WriteData( buf, 8 );
	}
}

//
// MOTION
//

// Move the PTZ camera with horizontal alpha degree (-170 to +170),
// and beta degree (-30 to +90)
void PTZController::moveTo( double alpha=0.0, double beta=0.0 ) {

	bool left;
	bool down;

	if( alpha < 0.0 ) {
		left = true;
		alpha = -alpha;
	} else left = false;

	if( beta < 0.0 ) {
		down = true;
		beta = -beta;
	} else down = false;

	int panStep = (int) (panStepPerDeg*alpha);
	int tiltStep = (int) (tiltStepPerDeg*beta);

	if( left ) panStep = -panStep;
	if( down ) tiltStep = -tiltStep;

	sprintf( buf, "%c%c%c%c", 0x80+_ptzNumber, 0x01, 0x06, 0x02 );		// command code
	sprintf( buf+4, "%c%c", 0x0c, 0x0c );
	sprintf( buf+6, "%c%c%c%c", (panStep&0xF000)>>12, (panStep&0xF00)>>8, (panStep&0xF0)>>4, (panStep&0xF) );
	sprintf( buf+10, "%c%c%c%c", (tiltStep&0xF000)>>12, (tiltStep&0xF00)>>8, (tiltStep&0xF0)>>4, (tiltStep&0xF) );
	sprintf( buf+14, "%c", 0xFF );

	_sp->WriteData( buf, 15 );
}

// Move the PTZ camera with horizontal alpha degree, and beta degree
// relative to the current position
void PTZController::move( double alpha=0.0, double beta=0.0 ) {

	bool left;
	bool down;

	if( alpha < 0.0 ) {
		left = true;
		alpha = -alpha;
	} else left = false;

	if( beta < 0.0 ) {
		down = true;
		beta = -beta;
	} else down = false;

	int panStep = (int) (panStepPerDeg*alpha);
	int tiltStep = (int) (tiltStepPerDeg*beta);

	if( left ) panStep = -panStep;
	if( down ) tiltStep = -tiltStep;

	sprintf( buf, "%c%c%c%c", 0x80+_ptzNumber, 0x01, 0x06, 0x03 );		// command code
	sprintf( buf+4, "%c%c", 0x0c, 0x0c );
	sprintf( buf+6, "%c%c%c%c", (panStep&0xF000)>>12, (panStep&0xF00)>>8, (panStep&0xF0)>>4, (panStep&0xF) );
	sprintf( buf+10, "%c%c%c%c", (tiltStep&0xF000)>>12, (tiltStep&0xF00)>>8, (tiltStep&0xF0)>>4, (tiltStep&0xF) );
	sprintf( buf+14, "%c", 0xFF );

	_sp->WriteData( buf, 15 );
}

//
// VIDEO
//

