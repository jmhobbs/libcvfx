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
#include "libcvfx.h"

namespace cvfx {
	// Init Area

	// shared
	int h,i,j,k;
	CvScalar bgrNonPerm[10];

	// monochrome
	CvScalar monochrome_bgr;

	// pixelize
	CvScalar pixelize_bgr;

	// memory
	int memory_frameCounter = 0;
	IplImage * memory_frames[3];
	bool memory_init = false;

	// hFlip
	IplImage * hFlip_frame;
	bool hFlip_init = false;

	// vFlip
	IplImage * vFlip_frame;
	bool vFlip_init = false;

	/////////////////////////////////////////////////////////////////
	// The Effects

	/*!
		Mirrors the left side onto the right side.

		\param frame The frame to work on.
		\author John Hobbs john@velvetcache.org
	*/
	void mirror (IplImage * frame) {
		int halfFrame = frame->width/2;
		int frameBytes = frame->width*3-1;
		for(i = 0; i < frame->height; i++) {
			int offset = i*frame->width*3;
			for(j = 0; j < halfFrame; j++) {
				int jBytes = offset+frameBytes-(j*3);
				int ojBytes = offset+(j*3);
				frame->imageData[jBytes-2] = frame->imageData[ojBytes];
				frame->imageData[jBytes-1] = frame->imageData[ojBytes+1];
				frame->imageData[jBytes] = frame->imageData[ojBytes+2];
			}
		}
	}

	/*!
		Flips the image horizontally.

		\param frame The frame to work on.
		\author John Hobbs john@velvetcache.org
	*/
	void hFlip (IplImage * frame) {
		if(!hFlip_init) {
			hFlip_frame = cvCreateImage( cvGetSize(frame), frame->depth, 3 );
			hFlip_init = true;
		}

		hFlip_frame = cvCloneImage(frame);
		for(int i = 0; i < frame->height; i++) {
			for(int j = 0; j < frame->width; j++) {
				bgrNonPerm[0] = cvGet2D(hFlip_frame,i,(frame->width - j - 1));
				cvSet2D(frame,i,j,bgrNonPerm[0]);
			}
		}
	}

	/*!
		Flips the image vertically.

		\param frame The frame to work on.
		\author John Hobbs john@velvetcache.org
	*/
	void vFlip (IplImage * frame) {
		if(!vFlip_init) {
			vFlip_frame = cvCreateImage( cvGetSize(frame), frame->depth, 3 );
			vFlip_init = true;
		}

		vFlip_frame = cvCloneImage(frame);
		for(int i = 0; i < frame->height; i++) {
			for(int j = 0; j < frame->width; j++) {
				bgrNonPerm[0] = cvGet2D(vFlip_frame,(frame->height - i - 1),j);
				cvSet2D(frame,i,j,bgrNonPerm[0]);
			}
		}
	}

	/*!
		Swaps values in bgr to rbg, giving a purple/green effect.

		\param frame The frame to work on.
		\author John Hobbs john@velvetcache.org
	*/
	void oompaLoompa (IplImage * frame) {
		for(int i = 0; i < frame->height; i++) {
			for(int j = 0; j < frame->width; j++) {
				bgrNonPerm[0] = cvGet2D(frame,i,j);
				bgrNonPerm[1] = bgrNonPerm[0];
				bgrNonPerm[0].val[0] = bgrNonPerm[1].val[2];
				bgrNonPerm[0].val[1] = bgrNonPerm[1].val[0];
				bgrNonPerm[0].val[2] = bgrNonPerm[1].val[1];
				cvSet2D(frame,i,j,bgrNonPerm[0]);
			}
		}
	}

	/*!
		Makes the image green. Can get very dark.

		\param frame The frame to work on.
		\author John Hobbs john@velvetcache.org
	*/
	void green (IplImage * frame) {
		for(i = 0; i < frame->height; i++) {
			for(j = 0; j < frame->width; j++) {
				bgrNonPerm[0] = cvGet2D(frame,i,j);
				bgrNonPerm[0].val[0] = 0;
				bgrNonPerm[0].val[2] = 0;
				cvSet2D(frame,i,j,bgrNonPerm[0]);
			}
		}
	}

	/*!
		Makes the image monochrome.

		\param frame The frame to work on.
		\author John Hobbs john@velvetcache.org
	*/
	void monochrome (IplImage * frame) {
		for (i = 0; i < frame->height; i++){
			for (j = 0; j < frame->width; j++){
				monochrome_bgr = cvGet2D(frame, i, j);
				monochrome_bgr.val[0] = monochrome_bgr.val[0]*0.114 + monochrome_bgr.val[1]*0.587 + monochrome_bgr.val[2]*0.299;
				monochrome_bgr.val[1] = monochrome_bgr.val[0];
				monochrome_bgr.val[2] = monochrome_bgr.val[0];
				cvSet2D(frame,i,j,monochrome_bgr);
			}
		}
	}

	/*!
		Swaps the top left and bottom right corners.

		\param frame The frame to work on.
		\author John Hobbs john@velvetcache.org
	*/
	void corners (IplImage * frame) {
		for(int i = 0; i < frame->height/2; i++) {
			for(int j = 0; j < frame->width/2; j++) {
				bgrNonPerm[0] = cvGet2D(frame,i,j);
				bgrNonPerm[1] = cvGet2D(frame,i+(frame->height/2),j+(frame->width/2));
				cvSet2D(frame,i,j,bgrNonPerm[1]);
				cvSet2D(frame,i+(frame->height/2),j+(frame->width/2),bgrNonPerm[0]);
			}
		}
	}

	/*!
		Pixelizes the image, makes it nice and chunky.

		\param frame The frame to work on.
		\param HOWMANYPIXELS How much pixelization? Any number works, but too big will react unpredictably.
		\author John Hobbs john@velvetcache.org
	*/
	void pixelize (IplImage * frame, int HOWMANYPIXELS = 4) {
		pixelize_bgr = cvGet2D(frame,0,0);
		for(int i = 0; i < frame->height; i++) {
			for(int j = 0; j < frame->width; j++) {
				bgrNonPerm[0] = cvGet2D(frame,i,j);
				scalarAverage(pixelize_bgr,bgrNonPerm[0]);
				if(i%HOWMANYPIXELS == HOWMANYPIXELS-1 && j%HOWMANYPIXELS == HOWMANYPIXELS-1) {
					for(int r = 0; r < HOWMANYPIXELS; r++) {
						for(int t = 0; t < HOWMANYPIXELS; t++) {
							cvSet2D(frame,i-r,j-t,pixelize_bgr);
						}
					}
					pixelize_bgr = bgrNonPerm[0];
				}
			}
		}
	}

	/*!
		Keeps a few previous frames in memory, and combines them to make ghost images
		on the current frame.

		\todo Add a random factor to frame storage.

		\param frame The frame to work on.
		\author John Hobbs john@velvetcache.org
	*/
	void memory (IplImage * frame) {

		if(!memory_init) {
			for(int i = 0; i < 3; i++) {
				memory_frames[i] = cvCreateImage( cvGetSize(frame), frame->depth, 3 );
				memory_frames[i] = cvCloneImage(frame);
			}
			memory_init = true;
		}

		if(0 == memory_frameCounter)
			memory_frames[0] = cvCloneImage(frame);
		else if(2 == memory_frameCounter)
			memory_frames[1] = cvCloneImage(frame);
		else if(6 == memory_frameCounter)
			memory_frames[2] = cvCloneImage(frame);
		else if(12 <= memory_frameCounter)
			memory_frameCounter = -1;

		memory_frameCounter++;

		for(int i = 0; i < frame->height; i++) {
			for(int j =0; j < frame->width; j++) {
				bgrNonPerm[0] = cvGet2D(frame, i, j);
				bgrNonPerm[1] = bgrNonPerm[0]; // Used to add weight to the current image
				for(int k = 0; k < 3; k++) {
					bgrNonPerm[2] = cvGet2D(memory_frames[k], i, j);
					scalarAverage(bgrNonPerm[0],bgrNonPerm[2]);
				}
				scalarAverage(bgrNonPerm[0],bgrNonPerm[1]);
				cvSet2D(frame,i,j,bgrNonPerm[0]);
			}
		}
	}

	/*!
		Inverts the coloring of the image. Can be kinda freaky

		\param frame The frame to work on.
		\author John Hobbs john@velvetcache.org
		\note Idea from http://tommy.chheng.com/development/windows_development_setup.html, reimplemented using CvScalar.
	*/
	void invert (IplImage * frame) {
		for(int i = 0; i < frame->height; i++) {
			for(int j =0; j < frame->width; j++) {
				bgrNonPerm[0] = cvGet2D(frame, i, j);
				bgrNonPerm[0].val[0] = 255 - bgrNonPerm[0].val[0];
				bgrNonPerm[0].val[1] = 255 - bgrNonPerm[0].val[1];
				bgrNonPerm[0].val[2] = 255 - bgrNonPerm[0].val[2];
				cvSet2D(frame,i,j,bgrNonPerm[0]);
			}
		}
	}

		/*!
		UNSTABLE - This one accesses the pixels directly, so it could do some crazy things to
		your data, or even seg out.  It's a very strong effect when it does work though.

		\param frame The frame to work on.
		\author John Hobbs john@velvetcache.org
	*/
	void bitmaptrip (IplImage * frame) {
		for(int i = 0; i < frame->height; i++) {
			for(int j =0; j < frame->width*3; j++) {
				frame->imageData[(i*frame->width*3)+j] = frame->imageData[(i*frame->width*3)+j]*4+5;
			}
		}
	}

	// Internal Stuff

	/*!
		Convinience function to average two CvScalars.

		\param left The CvScalar to put the result in.
		\param right The other CvScalar to average in.
		\author John Hobbs john@velvetcache.org
	*/
	void scalarAverage (CvScalar & left, CvScalar & right) {
		left.val[0] = (right.val[0]+left.val[0])/2;
		left.val[1] = (right.val[1]+left.val[1])/2;
		left.val[2] = (right.val[2]+left.val[2])/2;
	}

}
