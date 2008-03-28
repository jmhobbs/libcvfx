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
	libcvfx 0.02 A05
	
	Implementation of libcvfx effects.

	Author: John Hobbs
	Homepage: http://www.velvetcache.org/
*/
#include "libcvfx.h"

namespace cvfx {
	// Init Area

	// shared
	int h,i,j,k;
	CvScalar bgrNonPerm[10];

	// brokenTelevision
	int brokenTelevision_scanlines = 0;

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
		Swaps the selected corners.

		\param frame The frame to work on.
		\author John Hobbs john@velvetcache.org
	*/
	void corners (IplImage * frame, cornersType type) {
		for(int i = 0; i < frame->height/2; i++) {
			for(int j = 0; j < frame->width/2; j++) {
				if(type == TOPLEFT_BOTTOMRIGHT || type == BOTH) {
					bgrNonPerm[0] = cvGet2D(frame,i,j);
					bgrNonPerm[1] = cvGet2D(frame,i+(frame->height/2),j+(frame->width/2));
					cvSet2D(frame,i,j,bgrNonPerm[1]);
					cvSet2D(frame,i+(frame->height/2),j+(frame->width/2),bgrNonPerm[0]);
				}
				if(type == TOPRIGHT_BOTTOMLEFT || type == BOTH) {
					bgrNonPerm[0] = cvGet2D(frame,i,j+(frame->width/2));
					bgrNonPerm[1] = cvGet2D(frame,i+(frame->height/2),j);
					cvSet2D(frame,i,j+(frame->width/2),bgrNonPerm[1]);
					cvSet2D(frame,i+(frame->height/2),j,bgrNonPerm[0]);
				}
			}
		}
	}

	/*!
		Adds simulated 'interlace' lines.

		\param frame The frame to work on.
		\author John Hobbs john@velvetcache.org
	*/
	void interlaceLines (IplImage * frame) {
		for(int i = 0; i < 3; i++)
			bgrNonPerm[0].val[i] = 0;

		for(int i = 1; i < frame->height; i += 2)
			for(int j = 0; j < frame->width; j++)
				cvSet2D(frame,i,j,bgrNonPerm[0]);
	}

	/*!
		BROKEN
		Sharpen the image.

		\param frame The frame to work on.
		\author John Hobbs john@velvetcache.org
	*/
	void horizontalSharpen (IplImage * frame) {
		IplImage * oFrame = cvCreateImage( cvGetSize(frame), frame->depth, 3 );
		oFrame = cvCloneImage(frame);

		for(int i = 0; i < frame->height; i++) {
			for(int j = 1; j < frame->width; j++) {
				bgrNonPerm[0] = cvGet2D(oFrame,i,j-1);
				bgrNonPerm[1] = cvGet2D(oFrame,i,j);
				bgrNonPerm[2] = cvGet2D(oFrame,i,j+1);
				// if above a bound do double diff up, below do double diff down?
				cvSet2D(frame,i,j,bgrNonPerm[0]);
			}
		}
	}

	/*!
		Pixelizes the image, makes it nice and chunky.

		\param frame The frame to work on.
		\param HOWMANYPIXELS How much pixelization? Any number works, but too big will react unpredictably.
		\author John Hobbs john@velvetcache.org
	*/
	void pixelize (IplImage * frame, int HOWMANYPIXELS) {
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
		Inverts the coloring of the image.

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
		Cuts image into vertical sections, every other section is vertically flipped.

		\param frame The frame to work on.
		\param strips The number of strips to create.
		\author John Hobbs john@velvetcache.org
	*/
	void vStripFlip (IplImage * frame, int strips) {
		int stripWidth = frame->width/strips;
		for(int i = 0; i < frame->height/2; i++) {
			for(int k = 1; k < strips; k+=2) {
				for(int j = stripWidth*k; j < stripWidth*(k+1); j++) {
					bgrNonPerm[0] = cvGet2D(frame,i,j);
					bgrNonPerm[1] = cvGet2D(frame,frame->height-i-1,j);
					cvSet2D(frame,i,j,bgrNonPerm[1]);
					cvSet2D(frame,frame->height-i-1,j,bgrNonPerm[0]);
				}
			}
		}
	}

	/*!
		Distills the image down to two colors based on luminosity.
		Replaces over-threshold values with white by default.
		If you want something other than white, you may specify.

		\param frame The frame to work on.
		\param threshold The threshold, defaults to 10, but this needs to be carefully measured.
		\param blue The blue aspect of the over threshold color.
		\param green The green aspect of the over threshold color.
		\param red The red aspect of the over threshold color.
		\author John Hobbs john@velvetcache.org
	*/
	void photoCopy (IplImage * frame, int threshold, int blue, int green, int red) {
		threshold = static_cast<int>(255.0*(threshold/100.0));
		for(int i = 0; i < frame->height; i++) {
			for(int j = 0; j < frame->width; j++) {
				bgrNonPerm[0] = cvGet2D(frame,i,j);
				if(threshold <= ((bgrNonPerm[0].val[0] + bgrNonPerm[0].val[1] + bgrNonPerm[0].val[2]) / 3)) {
					bgrNonPerm[0].val[0] = blue;
					bgrNonPerm[0].val[1] = green;
					bgrNonPerm[0].val[2] = red;
				}
				else {
					bgrNonPerm[0].val[0] = 0;
					bgrNonPerm[0].val[1] = 0;
					bgrNonPerm[0].val[2] = 0;
				}
				cvSet2D(frame,i,j,bgrNonPerm[0]);
			}
		}
	}

	/*!
		UNSTABLE
		This one accesses the pixels directly, so it could do some crazy things to
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

	/*!
		Loops the image sort of like a busted television.

		\param frame The frame to work on.
		\param speed The speed with which to move, from 1 to frame height. Defaults to 45.
		\author John Hobbs john@velvetcache.org
	*/
	void brokenTelevision (IplImage * frame, int speed) {

		CvScalar white = cvGet2D(frame,1,1);
		white.val[0] = 255;
		white.val[1] = 255;
		white.val[2] = 255;

		IplImage * frameCopy = cvCreateImage( cvGetSize(frame), frame->depth, 3 );
		frameCopy = cvCloneImage(frame);

		brokenTelevision_scanlines += speed;

		for(int i = 0; i < frame->height; i++) {
			int offset = (brokenTelevision_scanlines+i)%frame->height;
			for(int j = 0; j < frame->width; j++) {
					bgrNonPerm[0] = cvGet2D(frameCopy,i,j);
					cvSet2D(frame,offset,j,bgrNonPerm[0]);
			}
		}

	}

	/*!
		BROKEN
		Supposed to inject some random noise, but doesn't do a very good job of it.

		\param frame The frame to work on.
		\param ratio Supposed to be the signal to noise ratio. Isn't.
		\author John Hobbs john@velvetcache.org
	*/
	void noise (IplImage * frame, int ratio) {
		for(int i = 0; i < frame->height; i++) {
			for(int j = 0; j < frame->width; j++) {
				if(getRand(0,1000) < ratio) {
					bgrNonPerm[0] = cvGet2D(frame,i,j);
					bgrNonPerm[1] = cvGet2D(frame,i,j);
					bgrNonPerm[0].val[0] = getRand(0,255);
					bgrNonPerm[0].val[1] = getRand(0,255);
					bgrNonPerm[0].val[2] = getRand(0,255);
					scalarAverage(bgrNonPerm[0],bgrNonPerm[1]);
					cvSet2D(frame,i,j,bgrNonPerm[0]);
				}
			}
		}
	}

	/*!
		Cuts image into horizontal sections, every other section is horizontally flipped.

		\param frame The frame to work on.
		\param strips The number of strips to create.
		\author John Hobbs john@velvetcache.org
	*/
	void hStripFlip (IplImage * frame, int strips) {
		int stripHeight = frame->height/strips;
		for(int k = 1; k < strips; k += 2) {
			for(int i = k*stripHeight; i < (k+1)*stripHeight; i++) {
				for(int j = 0; j < frame->width/2; j++) {
					bgrNonPerm[0] = cvGet2D(frame,i,j);
					bgrNonPerm[1] = cvGet2D(frame,i,frame->width-1-j);
					cvSet2D(frame,i,j,bgrNonPerm[1]);
					cvSet2D(frame,i,frame->width-1-j,bgrNonPerm[0]);
				}
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

	/*!
		Convinience function to get random digit within a set of bounds.

		\param lowerBound The lower bound.
		\param upperBound The upper bound.
		\author John Hobbs john@velvetcache.org
	*/
	int getRand (int lowerBound, int upperBound) {
		return rand()%upperBound+lowerBound;
	}

	/*!
		Return the average "weight" of the image.

		\param frame The frame to measure.
		\return value The luminosity from 0 - 255
	*/
	int getFrameLuminosity (IplImage * frame) {
		bgrNonPerm[0] = cvGet2D(frame, 1, 1);
		for(int i = 0; i < frame->height; i++) {
			for(int j =0; j < frame->width; j++) {
				bgrNonPerm[1] = cvGet2D(frame, i, j);
				scalarAverage(bgrNonPerm[0],bgrNonPerm[1]);
			}
		}
		return static_cast<int>((bgrNonPerm[0].val[0] + bgrNonPerm[0].val[1] + bgrNonPerm[0].val[2])/3);
	}

}
