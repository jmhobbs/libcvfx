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
	libcvfx 0.02 A08

    Declaration of libcvfx effects.

	Author: John Hobbs
	Homepage: http://www.velvetcache.org/
*/
#ifndef CVFXLIB_H
#define CVFXLIB_H

#include "cv.h"
#include "highgui.h"

#include <stdlib.h>

namespace cvfx {

	struct rgb {
		int red;
		int green;
		int blue;
	};

	extern rgb WHITE;
	extern rgb BLACK;

	const char CVFX_VERSION[] = "0.02 A08";

	enum cornersType {
		TOPLEFT_BOTTOMRIGHT,
		TOPRIGHT_BOTTOMLEFT,
		BOTH
	};

	enum channel {
		RED,
		GREEN,
		BLUE,
		YELLOW
	};

	// The effects
	void mirror (IplImage *);
	void vmirror (IplImage *);
	void cmirror (IplImage *);
	void hFlip (IplImage *);
	void vFlip (IplImage *);
	void channelSelect (IplImage *, channel);
	void monochrome (IplImage *);
	void corners (IplImage *, cornersType = TOPLEFT_BOTTOMRIGHT);
	void interlaceLines (IplImage *);
	void pixelize (IplImage *, int = 4);
	void memory (IplImage *, int = 5);
	void invert (IplImage *);
	void hjaggy (IplImage *, int = 15);
	void vStripFlip (IplImage *, int = 4);
	void hStripFlip (IplImage *, int = 4);
	void photoCopy (IplImage *, rgb = WHITE, rgb = BLACK, int = 10);
	void index (IplImage *, int = 100);
	void brokenTelevision (IplImage *, int = 45);
	void noise (IplImage *, int = 5);
	void composite (IplImage *, IplImage *);
	void pixelLapse (IplImage *, int = 32, bool = false);
	void quantum (IplImage *);
	void dice (IplImage *, int = 16);

	// Internal stuff
	void scalarAverage (CvScalar &, const CvScalar &);
	int getRand (int, int);
	int getFrameLuminosity (IplImage *);

}

#endif
