#!/bin/sh

DEFAULT="\E[01;0m"
RED="\E[01;31m"
YELLOW="\E[01;33m"
TEAL="\E[01;36m"
BLUE="\E[01;34m"
GREEN="\E[01;32m"

if [ $(whoami) != "root" ]; then
  echo -e "${RED}This script must be run as root.${DEFAULT}"
  exit 1
fi

if [ ! -f "cvfx.h" ]; then
  echo -e "${RED}I don't think you are in the right directory.\nYou must run this script from the same directory it is in.${DEFAULT}"
  exit 1
fi

echo -e "${TEAL}This script will attempt to install (or update) libcvfx on your machine.${DEFAULT}"
echo -en "${YELLOW}Continue? ${DEFAULT}[${GREEN}y${DEFAULT}/${RED}N${DEFAULT}] "

read confirm

if [ "$confirm" != "y" ]; then
  if [ "$confirm" != "Y" ]; then
    echo -e "${TEAL}Ok, see you later then.${DEFAULT}"
    exit 1
  fi
fi

echo -e "${TEAL}Cleaning up build directory.${DEFAULT}"
make clean
echo -e "${TEAL}Building library.${DEFAULT}"
make dynlib
if [ $? -ne 0 ]; then
  echo -e "${RED}Something went wrong in the build. Check it out.${DEFAULT}"
  exit 1
fi

if [ ! -f "libcvfx.so.1.0.1" ]; then
  echo -e "${RED}Build seems to have worked, but I can't find the library. Check it out.${DEFAULT}"
  exit 1
fi

echo -e "${TEAL}Moving library.${DEFAULT}"
cp libcvfx.so.1.0.1 /usr/lib/
if [ $? -ne 0 ]; then
  echo -e "${RED}Couldn't copy the library to /usr/lib/.${DEFAULT}"
  exit 1
fi

echo -e "${TEAL}Moving header.${DEFAULT}"
cp cvfx.h /usr/include/
if [ $? -ne 0 ]; then
  echo -e "${RED}Couldn't copy the header file to /usr/include/.${DEFAULT}"
  exit 1
fi

echo -e "${TEAL}Running ldconfig.${DEFAULT}"
ldconfig -n /usr/lib/
if [ $? -ne 0 ]; then
  echo -e "${RED}Something went wrong with ldconfig.${DEFAULT}"
  exit 1
fi

if [ -f /usr/lib/libcvfx.so ]; then
	rm -f /usr/lib/libcvfx.so
	if [ $? -ne 0 ]; then
  	echo -e "${RED}Couldn't remove old symbolic link, /usr/lib/libcvfx.so.${DEFAULT}"
  	exit 1
	fi
fi

echo -e "${TEAL}Creating authoritative link.${DEFAULT}"
ln -s /usr/lib/libcvfx.so.1 /usr/lib/libcvfx.so
if [ $? -ne 0 ]; then
  echo -e "${RED}Couldn't create symbolic link /usr/lib/libcvfx.so.${DEFAULT}"
  exit 1
fi

echo -e "${GREEN}libcvfx installed successfully!${DEFAULT}"
