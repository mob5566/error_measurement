/* 
 * Author:	Cheng-Shih, Wong
 * Date:	2015/11/04
 * E-mail:	mob5566@gmail.com
 * 
 */

#include "ptzController.h"

#include <iostream>

using namespace std;

// initialize the com port to portname
// the default portname is "COM3"
PTZController::PTZController( char *portname="COM3" ) {
	this->_sp = new Serial(portname);

	if( !this->_sp->IsConnected() ) {
		cerr << "Can not connect to PTZ controller!" << endl;
	}
}

//
//
PTZController::~PTZController() {
	if( this->_sp->IsConnected() ) {
		this->_sp->~Serial();
	}
}

