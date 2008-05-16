# Copyright (C) 2008  John Hobbs - john@velvetcache.org
#
# This file is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This file is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this file.  If not, see <http://www.gnu.org/licenses/>.

# Makefile for libcvfx
#
# Author: John Hobbs
# Homepage: http://www.velvetcache.org/

# Compiler setup
CC=g++
CFLAGS=-Wall -g
COMPILER=$(CC) $(CFLAGS)

# OpenCV
OCVCFLAGS=`pkg-config --cflags opencv`
OCVLFLAGS=`pkg-config --libs --cflags opencv`

# Program targets
cvfxTest: cvfxTest.o libcvfx.o
	$(COMPILER) $(OCVLFLAGS) cvfxTest.o libcvfx.o -o $@

clean:
	@rm -f cvfxTest
	@rm -f *.o
	@echo "Spotless!"

# Object targets
.SUFFIXES : .cpp .o

.cpp.o:
	$(COMPILER) $(OCVCFLAGS) -c $< -o $@

cvfxTest.o: cvfxTest.cpp libcvfx.h

libcvfx.o: libcvfx.cpp libcvfx.h