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

	Implementation of libcvfx effects.

	Author: John Hobbs
	Homepage: http://www.velvetcache.org/
*/
#include "cvfx.h"

#include <iostream>

namespace cvfx {

	// Definitions...
	rgb WHITE = {255,255,255};
	rgb BLACK = {0,0,0};

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
	IplImage * memory_frames[3];
	bool memory_init = false;

	// hFlip
	IplImage * hFlip_frame;
	bool hFlip_init = false;

	// vFlip
	IplImage * vFlip_frame;
	bool vFlip_init = false;

	// pixelLapse
	IplImage * pixelLapse_frame;
	bool pixelLapse_init = false;
	int pixelLapse_x, pixelLapse_y;

	// quantum
	IplImage * quantum_frames[8];
	bool quantum_init = false;
	short quantum_counter;

	// dice
	IplImage * dice_frame;
	bool dice_init = false;

	// filmstrip
	IplImage * filmstrip_frame;
	bool filmstrip_init = false;

	// delayMirror
	IplImage * delayMirror_frames[24];
	int delayMirror_counter;
	bool delayMirror_init = false;

	// jitter
	IplImage * jitter_frame;
	bool jitter_init = false;

	// colorStreak
	IplImage * colorStreak_frames[4];
	int colorStreak_counter;
	bool colorStreak_init = false;

	// unnamed1
	IplImage * unnamed1_frame;
	int unnamed1_interval = 0;
	bool unnamed1_init = false;

	// test
	IplImage * test_frame;
	int test_interval = 0;
	bool test_init = false;

	/////////////////////////////////////////////////////////////////
	// The Effects

	/*!
		Mirrors the left side onto the right side.

		\param frame The frame to work on.
		\author John Hobbs john@velvetcache.org
	*/
	void mirror (IplImage * frame) {
		for(i = 0; i < frame->height; i++) {
			for(j = 0; j < frame->width/2; j++) {
				bgrNonPerm[0] = cvGet2D(frame,i,j);
				cvSet2D(frame,i,frame->width-j-1,bgrNonPerm[0]);
			}
		}
	}

	/*!
		Vertical mirror. Mirrors top onto bottom.

		\param frame The frame to work on.
		\author John Hobbs john@velvetcache.org
	*/
	void vmirror (IplImage * frame) {
		for(int i = 0; i < frame->height/2; i++) {
			for(int j =0; j < frame->width; j++) {
				bgrNonPerm[0] = cvGet2D(frame, i, j);
				cvSet2D(frame,frame->height-i-1,j,bgrNonPerm[0]);
			}
		}
	}

	/*!
		Convinience function for a kaleidescope type center mirror.

		\param frame The frame to work on.
		\author John Hobbs john@velvetcache.org
	*/
	void cmirror (IplImage * frame) {
		for(int i = 0; i < frame->height/2; i++) {
			for(int j =0; j < frame->width/2; j++) {
				bgrNonPerm[0] = cvGet2D(frame, i, j);
				// Top right
				cvSet2D(frame,i,frame->width-j-1,bgrNonPerm[0]);
				// Bottom left
				cvSet2D(frame,frame->height-i-1,j,bgrNonPerm[0]);
				// Bottom right
				cvSet2D(frame,frame->height-i-1,frame->width-j-1,bgrNonPerm[0]);
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
		Reduces the image to the selected color. Can get very dark.

		\param frame The frame to work on.
		\author John Hobbs john@velvetcache.org
	*/
	void channelSelect (IplImage * frame, channel toKeep) {
		for(i = 0; i < frame->height; i++) {
			for(j = 0; j < frame->width; j++) {
				bgrNonPerm[0] = cvGet2D(frame,i,j);
				if(toKeep == RED || toKeep == GREEN || toKeep == YELLOW)
					bgrNonPerm[0].val[0] = 0;
				if(toKeep == BLUE || toKeep == RED)
					bgrNonPerm[0].val[1] = 0;
				if(toKeep == GREEN || toKeep == BLUE)
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
		\param type The corners to work on, an enum of type "cornersType".
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

		\param frame The frame to work on.
		\author John Hobbs john@velvetcache.org
	*/
	void memory (IplImage * frame, int randomness) {

		if(!memory_init) {
			for(int i = 0; i < 3; i++) {
				memory_frames[i] = cvCreateImage( cvGetSize(frame), frame->depth, 3 );
				memory_frames[i] = cvCloneImage(frame);
			}
			memory_init = true;
		}

		if(0 == getRand(0,randomness)) {
			memory_frames[getRand(0,2)] = cvCloneImage(frame);
		}

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
		Creates a kind of jagged look by stretching pixels across greater
		horizontal spans.

		\param frame The frame to work on.
		\param jaggyness The number of pixels to stretch across.
		\author John Hobbs john@velvetcache.org
	*/
	void hjaggy (IplImage * frame, int jaggyness) {
		for(int i = 0; i < frame->height; i++) {
			for(int j =0; j < frame->width; j+= jaggyness) {
				bgrNonPerm[0] = cvGet2D(frame, i, j);
				for(int k = 0; k < jaggyness && (j+k) < frame->width; k++)
					cvSet2D(frame,i,j+k,bgrNonPerm[0]);
			}
		}
		IplImage * frameCopy = cvCreateImage( cvGetSize(frame), frame->depth, 3 );
		frameCopy = cvCloneImage(frame);
		cvSmooth(frameCopy,frame,7);
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

	/*!
		Distills the image down to two colors based on luminosity.
		Replaces over-threshold values with one color, and unser threshold with another.

		\todo Version that auto adjusts threshold.

		\param frame The frame to work on.
		\param threshold The threshold, defaults to 10, but this needs to be carefully measured.
		\param overThreshold An rgb structure for the color over the threshold.
		\param underThreshold An rgb structure for the color under the threshold.
		\author John Hobbs john@velvetcache.org
	*/
	void photoCopy (IplImage * frame, rgb overThreshold, rgb underThreshold, int threshold) {
		threshold = static_cast<int>(255.0*(threshold/100.0));
		for(int i = 0; i < frame->height; i++) {
			for(int j = 0; j < frame->width; j++) {
				bgrNonPerm[0] = cvGet2D(frame,i,j);
				if(threshold <= ((bgrNonPerm[0].val[0] + bgrNonPerm[0].val[1] + bgrNonPerm[0].val[2]) / 3)) {
					bgrNonPerm[0].val[0] = overThreshold.blue;
					bgrNonPerm[0].val[1] = overThreshold.green;
					bgrNonPerm[0].val[2] = overThreshold.red;
				}
				else {
					bgrNonPerm[0].val[0] = underThreshold.blue;
					bgrNonPerm[0].val[1] = underThreshold.green;
					bgrNonPerm[0].val[2] = underThreshold.red;
				}
				cvSet2D(frame,i,j,bgrNonPerm[0]);
			}
		}
	}

	/*!
		Distills the image down to, currently, 8 colors.
		Does so by splitting each channel to 0 or 255, based on a threshold.
		Much like "photoCopy", but measures and adjusts each channel independently.

		\param frame The frame to work on.
		\param threshold The threshold to scale the channel on. Defaults to 100 (max 255, min 0)
		\author John Hobbs john@velvetcache.org
	*/
	void index (IplImage * frame, int threshold) {
		for(int i = 0; i < frame->height; i++) {
			for(int j = 0; j < frame->width; j++) {
				bgrNonPerm[0] = cvGet2D(frame,i,j);
				if(bgrNonPerm[0].val[0] < threshold)
					bgrNonPerm[0].val[0] = 0;
				else
					bgrNonPerm[0].val[0] = 255;
				if(bgrNonPerm[0].val[1] < threshold)
					bgrNonPerm[0].val[1] = 0;
				else
					bgrNonPerm[0].val[1] = 255;
				if(bgrNonPerm[0].val[2] < threshold)
					bgrNonPerm[0].val[2] = 0;
				else
					bgrNonPerm[0].val[2] = 255;
				cvSet2D(frame,i,j,bgrNonPerm[0]);
			}
		}
	}

	/*!
		Loops the image sort of like a busted television.

		\param frame The frame to work on.
		\param speed The speed with which to move, from 1 to frame height. Defaults to 45.
		\author John Hobbs john@velvetcache.org

		\note Idea from effectv. http://effectv.sourceforge.net/
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
		BROKEN

		Meant for green screen style compositing.

		\todo Color threshold as an input.
		\todo Intensity reflected onto the replacement pixels.

		\param frame The frame to work on.
		\param replace The frame containing anything to be replaced.
		\author John Hobbs john@velvetcache.org
	*/
	void composite (IplImage * frame, IplImage * replace) {
		for(int i = 0; i < frame->height; i++) {
			for(int j = 0; j < frame->width; j++) {
				bgrNonPerm[0] = cvGet2D(frame,i,j);
				if(bgrNonPerm[0].val[1] > bgrNonPerm[0].val[0] && bgrNonPerm[0].val[1] > bgrNonPerm[0].val[2]) {
					if(bgrNonPerm[0].val[0] <= 85 && bgrNonPerm[0].val[2] <= 85) {
						cvSet2D(frame,i,j,cvGet2D(replace,i,j));
					}
				}
			}
		}
	}

	/*!
		Cool time lapse style capture. This can be a very slow effect.
		If you use the random flag it will places the blocks randomly, and be even slower.

		\param frame The frame to work on.
		\param blockSize The size of each replacement block.  The smaller the blocks the slower the render. Defaults to 32.
		\param random If true, then it doesn't go linear. It places random blocks on the screen. Can take a long time to get a full frame. Defaults to false.
		\author John Hobbs john@velvetcache.org

		\note Idea from http://www.pixel-lapse.com/
	*/
	void pixelLapse (IplImage * frame, int blockSize, bool random) {
    if(!pixelLapse_init) {
			bgrNonPerm[0].val[0] = 0;
			bgrNonPerm[0].val[1] = 0;
			bgrNonPerm[0].val[2] = 0;
			pixelLapse_frame = cvCreateImage(cvGetSize(frame), frame->depth, 3);
      for(int i = 0; i < frame->height; i++)
				for(int j = 0; j < frame->width; j++)
					cvSet2D(pixelLapse_frame,i,j,bgrNonPerm[0]);
			pixelLapse_init = true;
			pixelLapse_x = 0;
			pixelLapse_y = 0;
    }

		for(int i = 0; i < blockSize && (i+pixelLapse_y) < frame->height; i++) {
			for(int j = 0; j < blockSize && (j+pixelLapse_x) < frame->width; j++) {
				cvSet2D(pixelLapse_frame,i+pixelLapse_y,j+pixelLapse_x,cvGet2D(frame,i+pixelLapse_y,j+pixelLapse_x));
			}
		}

		if(random) {
			pixelLapse_x = getRand(0,frame->width-1);
			pixelLapse_y = getRand(0,frame->height-1);
		}
		else {
			pixelLapse_x += blockSize;
			if(pixelLapse_x >= frame->width) {
				pixelLapse_x = 0;
				pixelLapse_y += blockSize;
				if(pixelLapse_y >= frame->height)
					pixelLapse_y = 0;
			}
		}
		//! \todo Is there a better way here?
		for(int i = 0; i < frame->height; i++)
			for(int j = 0; j < frame->width; j++)
				cvSet2D(frame,i,j,cvGet2D(pixelLapse_frame,i,j));

	}

	/*!
		Does a fuzzy thing with movement.

		\param frame The frame to work on.
		\author John Hobbs john@velvetcache.org

    \note Idea and pseudo code from effectv, 'quark'. http://effectv.sourceforge.net/
	*/
	void quantum (IplImage * frame) {
		if(!quantum_init) {
			for(int i = 0; i < 8; i++) {
				quantum_frames[i] = cvCreateImage(cvGetSize(frame), frame->depth, 3);
				quantum_frames[i] = cvCloneImage(frame);
			}
			quantum_counter = 0;
			quantum_init = true;
		}
		quantum_frames[quantum_counter] = cvCloneImage(frame);

		for(int i = 0; i < frame->height; i++)
			for(int j = 0; j < frame->width; j++)
					cvSet2D(frame,i,j,cvGet2D(quantum_frames[getRand(0,7)],i,j));

		if(++quantum_counter > 7)
			quantum_counter = 0;
	}

  /*!
		BROKEN

		Cuts the image into blocks, the rotates them randomly.

		\param frame The frame to work on.
		\param bloackSize The size of the blocks.
		\author John Hobbs john@velvetcache.org

		\note Idea from effectv, 'diceTV'. http://effectv.sourceforge.net/
	*/
	void dice (IplImage * frame, int blockSize) {
		return;
		if(!dice_init) {
			dice_frame = cvCreateImage(cvGetSize(frame), frame->depth, 3);
			dice_init = true;
		}
		dice_frame = cvCloneImage(frame);
		for(int i = 0; i < frame->height; i += blockSize) {
			for(int j = 0; j < frame->width; j += blockSize) {
				int flip = getRand(1,3);
				// 0 - no, 1 - 90o, 2 - 180, 3 - 240
				if(2 == flip) {
					for(int k = 0; k < blockSize && k+i < frame->height; k++) {
						for(int l = 0; l < blockSize && l+j < frame->width; l++) {
							cvSet2D(frame,l+i,k+j,cvGet2D(dice_frame,k+i,l+j));
						}
					}
				}
				else if(1 == flip) {
					/*for(int k = 0; k < blockSize && k+i < frame->height; k++) {
						for(int l = 0; l < blockSize && l+j < frame->width; l++) {
							cvSet2D(frame,(i+blockSize)-k,(j+blockSize)-l,cvGet2D(dice_frame,k+i,l+j));
						}
					}*/
				}
				else if(3 == flip) {
					for(int k = 0; k < blockSize && k+i < frame->height; k++) {
						for(int l = 0; l < blockSize && l+j < frame->width; l++) {
							cvSet2D(frame,(i+blockSize)-k,j+l,cvGet2D(dice_frame,k+i,l+j));
						}
					}
				}
			}
		}
	}

	void filmstrip (IplImage * frame, int divisor) {
		if(!filmstrip_init) {
			filmstrip_frame = cvCreateImage(cvGetSize(frame), frame->depth, 3);
			filmstrip_init = true;
		}
		filmstrip_frame = cvCloneImage(frame);
		//cvResize( const CvArr* src, CvArr* dst, int interpolation=CV_INTER_LINEAR )
	}

	// Normal hmirror, but oneside is out of sync, old frames.
	// slightly influenced by effectv
	void delayMirror (IplImage * frame) {
		if(!delayMirror_init) {
			for(int i = 0; i < 24; i++) {
				delayMirror_frames[i] = cvCreateImage(cvGetSize(frame), frame->depth, 3);
				delayMirror_frames[i] = cvCloneImage(frame);
			}
			delayMirror_init = true;
			delayMirror_counter = 0;
		}

		if(++delayMirror_counter >= 24)
			delayMirror_counter = 0;

		delayMirror_frames[delayMirror_counter] = cvCloneImage(frame);

		for(int i = 0; i < frame->height; i++) {
			for(int j = frame->width/2; j < frame->width; j++) {
				cvSet2D(frame,i,j,cvGet2D(delayMirror_frames[(delayMirror_counter+12)%24],i,j));
			}
		}

	}

	// toss in a frame randomly from old frame buffer to make things jitter - effectv
	void jitter (IplImage * frame) {
		if(!jitter_init) {
			jitter_frame = cvCreateImage(cvGetSize(frame), frame->depth, 3);
			jitter_frame = cvCloneImage(frame);
			jitter_init = true;
		}

		if(1 == getRand(0,3)) {
			jitter_frame = cvCloneImage(frame);
		}
		else if(1 == getRand(0,3)) {
			//! \todo How come this doesn't work?
			//frame = cvCloneImage(jitter_frame);
			for(int i = 0; i < frame->height; i++)
				for(int j = 0; j < frame->width; j++)
					cvSet2D(frame,i,j,cvGet2D(jitter_frame,i,j));
		}

	}

	// from effectv - do streaky memory but tint each frame a different color
	// Slooooow.  Consider only changing "very" different pixels and doing color inline.
	void colorStreak (IplImage * frame) {
		if(!colorStreak_init) {
			for(int i = 0; i < 4; i++) {
				colorStreak_frames[i] = cvCreateImage(cvGetSize(frame), frame->depth, 3);
				colorStreak_frames[i] = cvCloneImage(frame);
			}
			colorStreak_init = true;
			colorStreak_counter = 0;
		}

		if(++colorStreak_counter >= 4)
			colorStreak_counter = 0;

		colorStreak_frames[colorStreak_counter] = cvCloneImage(frame);
		if(0 == colorStreak_counter)
			channelSelect(colorStreak_frames[colorStreak_counter],RED);
		else if(1 == colorStreak_counter)
			channelSelect(colorStreak_frames[colorStreak_counter],GREEN);
		else if(2 == colorStreak_counter)
			channelSelect(colorStreak_frames[colorStreak_counter],BLUE);
		else
			channelSelect(colorStreak_frames[colorStreak_counter],YELLOW);

		for(int i = 0; i < frame->height; i++) {
			for(int j =0; j < frame->width; j++) {
				bgrNonPerm[0] = cvGet2D(frame, i, j);
				bgrNonPerm[1] = bgrNonPerm[0]; // Used to add weight to the current image
				for(int k = 0; k < 4; k++) {
					bgrNonPerm[2] = cvGet2D(colorStreak_frames[k], i, j);
					scalarAverage(bgrNonPerm[0],bgrNonPerm[2]);
				}
				scalarAverage(bgrNonPerm[0],bgrNonPerm[1]);
				cvSet2D(frame,i,j,bgrNonPerm[0]);
			}
		}

	}

	void unnamed1 (IplImage * frame) {
  	if(!unnamed1_init) {
			unnamed1_frame = cvCreateImage(cvGetSize(frame), frame->depth, 3);
			unnamed1_init = true;
		}
		unnamed1_frame = cvCloneImage(frame);

		for(int i = 0; i < frame->height; i++) {
			unnamed1_interval = 0;
			for(int j = 0; j < frame->width; j++) {
				bgrNonPerm[0] = cvGet2D(unnamed1_frame,i,j);
				scalarAverage(bgrNonPerm[0],cvGet2D(unnamed1_frame,i,j-(unnamed1_interval/2)));
				cvSet2D(frame,i,j,bgrNonPerm[0]);
				unnamed1_interval++;
			}
		}
	}


	void test (IplImage * frame) {
  	/*if(!test_init) {
			test_frame = cvCreateImage(cvGetSize(frame), frame->depth, 3);
			test_init = true;
		}
		test_frame = cvCloneImage(frame);*/

		for(int i = 0; i < frame->height; i++) {
			for(int j = 1; j < frame->width; j++) {
				bgrNonPerm[0] = cvGet2D(frame,i,j-1);
				scalarAverage(bgrNonPerm[0],cvGet2D(frame,i,j));
				cvSet2D(frame,i,j,bgrNonPerm[0]);
			}
		}

	}

	// Internal Stuff

	/*!
		Convinience function to average two CvScalars.

		\param left The CvScalar to put the result in.
		\param right The other CvScalar to average with.
		\author John Hobbs john@velvetcache.org
	*/
	void scalarAverage (CvScalar & left, const CvScalar & right) {
		left.val[0] = (right.val[0]+left.val[0])/2;
		left.val[1] = (right.val[1]+left.val[1])/2;
		left.val[2] = (right.val[2]+left.val[2])/2;
	}

	/*!
		Convinience function to get random integer within a set of bounds.

		\param lowerBound The lower bound.
		\param upperBound The upper bound.
		\author John Hobbs john@velvetcache.org
	*/
	int getRand (int lowerBound, int upperBound) {
		return lrand48()%upperBound+lowerBound;
	}

	/*!
		BROKEN

		Return the average "weight" of the image.
		\todo This is a flawed algorithm. The method of averaging is bad.  ((5+7/2)+8)/2 != (5+7+8)/3

		\param frame The frame to measure.
		\return The luminosity from 0 - 255
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

	/*!
		Get the cannonical name for an effect.

		\param whatEffect The effect enum id of the effect.
		\return A string containing the effect name.
	*/
	std::string getEffectName(effect whatEffect) {
		switch(whatEffect) {
			case NONE:
			 return "None";
			case MIRROR:
				return "Mirror";
			case VMIRROR:
				return "Vertical Mirror";
			case CMIRROR:
				return "Central Mirror";
			case HFLIP:
				return "Horizontal Flip";
			case VFLIP:
				return "Vertical Flip";
			case CHANNELSELECT:
				return "Channel Select";
			case MONOCHROME:
				return "Monochrome";
			case CORNERS:
				return "Corner Swap";
			case INTERLACELINES:
				return "Fake Interlace";
			case PIXELIZE:
				return "Pixelize";
			case MEMORY:
				return "Memory";
			case INVERT:
				return "Invert";
			case HJAGGY:
				return "UNSUPPORTED";
			case VSTRIPFLIP:
				return "Vertical Strip Flip";
			case HSTRIPFLIP:
				return "Horizontal Strip Flip";
			case PHOTOCOPY:
			case INDEX:
			case BROKENTELEVISION:
			case NOISE:
			case COMPOSITE:
			case PIXELLAPSE:
			case QUANTUM:
			case DICE:
			default:
				return "UNKNOWN";
		}
	}

}
