# opengl-refapp

This is a simple app that can be cross-compiled between Linux PC (X11) and RaspberryPI ( DispmanX ). It reads one texture from the provided BMP file and displays on screen. 

I've been using this for POC and performance testing across the two platforms.

To compile for X11 or RPI , change accordingly the symlink named Makefile.include to point to Makefile.include.x11 or Makefile.include.dispmanx accordingly, i.e.

make clean
rm Makefile.include
ln -s Makefile.include.rpi Makefile.include or ln -s Makefile.include.x11 Makefile.include


Then make.
