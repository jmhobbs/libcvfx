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
	libcvfx 0.01

	Author: John Hobbs
	Homepage: http://www.velvetcache.org/
*/
#ifndef CVFXLIB_H
#define CVFXLIB_H

#include "cv.h"
#include "highgui.h"

namespace cvfx {

	const char CVFX_VERSION[] = "0.01";

	// The effects
	void mirror (IplImage *);
	void green (IplImage *);
	void monochrome (IplImage *);
	void corners (IplImage *);
	void pixelize (IplImage *, int);
	void memory (IplImage *);
	void bitmaptrip (IplImage *);
	void hFlip (IplImage *);
	void vFlip (IplImage *);
	void oompaLoompa (IplImage *);
	void invert (IplImage *);

	// Internal stuff
	void scalarAverage (CvScalar &, CvScalar &);
}

#endif
