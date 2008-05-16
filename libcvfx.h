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
	libcvfx 0.02 A07

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

	const char CVFX_VERSION[] = "0.02 A06";

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
	void green (IplImage *);
	void channelSelect (IplImage *, channel);
	void monochrome (IplImage *);
	void corners (IplImage *, cornersType = TOPLEFT_BOTTOMRIGHT);
	void pixelize (IplImage *, int = 4);
	void memory (IplImage *, int = 5);
	void bitmaptrip (IplImage *);
	void hFlip (IplImage *);
	void vFlip (IplImage *);
	void oompaLoompa (IplImage *);
	void invert (IplImage *);
	void vStripFlip (IplImage *, int = 4);
	void photoCopy (IplImage *, int = 10);
	void photoCopy (IplImage *, rgb, rgb, int = 10);
	void brokenTelevision (IplImage *, int = 45);
	void hStripFlip (IplImage *, int = 4);
	void noise (IplImage *, int = 5);
	void interlaceLines (IplImage *);
	void horizontalSharpen (IplImage *);
	void index (IplImage *);
	void smush (IplImage *);
	void hjaggy (IplImage *, int = 15);
	void vmirror (IplImage *);
	void cmirror (IplImage *);

	// Internal stuff
	void scalarAverage (CvScalar &, const CvScalar &);
	int getRand (int, int);
	int getFrameLuminosity (IplImage *);

}

#endif
