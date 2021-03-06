/*
	Copyright (C) 2008  John Hobbs - john@velvetcache.org

	This file is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This file is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this file.  If not, see <http://www.gnu.org/licenses/>.
*/
/*
	libcvfx 1.0.1 A01

	cvfxTest is a simple application based on the demonstration code provided
	by the OpenCV group. It is a testbed for creating and demonstrating different
	methods with OpenCV and libcvfx. It is also a bit of a mess.

	Author: John Hobbs
	Homepage: http://www.velvetcache.org/
*/
#include "cv.h"
#include "highgui.h"
#include <cvfx.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <iostream>
#include <sstream>

using namespace std;
#define TOTALEFFECTS 29

string getFilterName(int _effectCounter) {
	switch (_effectCounter % TOTALEFFECTS) {
		case 1:
			return "channelSelect";
			break;
		case 2:
			return "mirror";
			break;
		case 3:
			return "monochrome";
			break;
		case 4:
			return "corners";
			break;
		case 5:
			return "pixelize";
			break;
		case 6:
			return "memory";
			break;
		case 7:
			return "hFlip";
			break;
		case 8:
			return "vFlip";
			break;
		case 9:
			return "quantum";
			break;
		case 10:
			return "invert";
			break;
		case 11:
			return "vStripFlip";
			break;
		case 12:
			return "photoCopy";
			break;
		case 13:
			return "brokenTelevision";
			break;
		case 14:
			return "hStripFlip";
			break;
		case 15:
			return "noise";
			break;
		case 16:
			return "interlaceLines";
			break;
		case 17:
			return "index";
			break;
		case 18:
			return "pixelLapse";
			break;
		case 19:
			return "hjaggy";
			break;
		case 20:
			return "vmirror";
			break;
		case 21:
			return "cmirror";
			break;
		case 22:
			return "dice";
			break;
		case 23:
			return "delayMirror";
			break;
		case 24:
			return "jitter";
			break;
		case 25:
			return "colorStreak";
			break;
		case 26:
			return "unnamed1";
			break;
		case 27:
			return "randomShift";
			break;
		case 28:
			return "test";
			break;
		default:
			return "none";
			break;
	}
}

int main(int argc, char * argv[]) {

	// Stupid hack so you can go through effects in reverse right away
	int effectCounter = 15*TOTALEFFECTS;

	time_t seconds;
	stringstream utl;

	CvCapture* capture = cvCaptureFromCAM( CV_CAP_ANY );
	if( !capture ) {
		cerr << "ERROR: Failed to start capture device." << endl;
		return -1;
	}

	cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH, 640);
	cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT, 480);

	cout << "\nUsing libcvfx version " << cvfx::CVFX_VERSION
			 << "\n\nPress 'c' to capture and save an image.\n"
			 << "Press the 'w' and 's' to switch effects.\n"
			 << "Press 'ESC' or 'q' to quit.\n\n"
			 << "Try not to quit using the UI, it won't shut down properly.\n\n" << endl;

	cvNamedWindow( "cvfxTest", CV_WINDOW_AUTOSIZE );

	IplImage * frame;

	while( 1 ) {

		frame = cvQueryFrame( capture );

		if( !frame ) {
			cerr << "ERROR: Couldn't get a frame." << endl;
			break;
		}

		switch (effectCounter % TOTALEFFECTS) {
			case 1:
				cvfx::channelSelect(frame,cvfx::GREEN);
				break;
			case 2:
				cvfx::mirror(frame);
				break;
			case 3:
				cvfx::monochrome(frame);
				break;
			case 4:
				cvfx::corners(frame);
				break;
			case 5:
				cvfx::pixelize(frame);
				break;
			case 6:
				cvfx::memory(frame);
				break;
			case 7:
				cvfx::hFlip(frame);
				break;
			case 9:
				cvfx::quantum(frame);
				break;
			case 8:
				cvfx::vFlip(frame);
				break;
			case 10:
				cvfx::invert(frame);
				break;
			case 11:
				cvfx::vStripFlip(frame);
				break;
			case 12:
				cvfx::photoCopy(frame);
				break;
			case 13:
				cvfx::brokenTelevision(frame);
				break;
			case 14:
				cvfx::hStripFlip(frame);
				break;
			case 15:
				cvfx::noise(frame);
				break;
			case 16:
				cvfx::interlaceLines(frame);
				break;
			case 17:
				cvfx::index(frame);
				break;
			case 18:
				cvfx::pixelLapse(frame);
				break;
			case 19:
				cvfx::hjaggy(frame);
				break;
			case 20:
				cvfx::vmirror(frame);
				break;
			case 21:
				cvfx::cmirror(frame);
				break;
			case 22:
				cvfx::dice(frame);
				break;
			case 23:
				cvfx::delayMirror(frame);
				break;
			case 24:
				cvfx::jitter(frame);
				break;
			case 25:
				cvfx::colorStreak(frame);
				break;
			case 26:
				cvfx::unnamed1(frame);
				break;
			case 27:
				cvfx::randomShift(frame,3);
				break;
			case 28:
				cvfx::test(frame);
				break;
			default:
				break;
		}

		cvShowImage( "cvfxTest", frame );

		int keypress = (cvWaitKey(10) & 255);
		if(keypress == 255) {
			// Do nothing, no keys pressed.
		}
		else if(keypress == 27 || keypress == 113) {
			// ESC or q pressed, bail out
			break;
		}
		else if(keypress == 99) {
			// c pressed, capture an image
			seconds = time (NULL);
			utl.str("");
			utl << "cvfxTest_capture_" << seconds <<  "_" << getFilterName(effectCounter) << ".jpg";
			if(!cvSaveImage(utl.str().c_str(),frame))
				cout << "Could not save file: " << utl.str() << endl;
			else
				cout << "Saved file as: " << utl.str() << endl;
		}
		else if (keypress == 119/*82*/) {
			cout << getFilterName(++effectCounter) << endl;
		}
		else if (keypress == 115/*84*/) {
			cout << getFilterName(--effectCounter) << endl;
		}
		else {
			// Other key pressed, inform the user
			cout << "You pushed..." << keypress << endl;
		}
	}

	// Release the capture device housekeeping
	cvReleaseCapture( &capture );
	cvDestroyWindow( "cvfxTest" );

	return 0;
}
