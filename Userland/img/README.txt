For this to work properly, do the following:

1) Get any image, resize it to 150px x 150px max
2) Open it in GIMP
3) Export is as image.c (not another name) - uncheck ALL export options (no alpha channel)
-check the name in the makefile build.
-leave the name as gimp_image
4) ImageWriter will pick it up and convert it to img1.img
5) In Image/Makefile, make sure the following line is UNCOMMENTED:
	USERLAND=../Userland/userland.bin ../Userland/img/img1.img
	-deprecated

	This puts the image in the data module instead of the song

6) The following will print your image:

	Image *img = (Image *) 0x500000;
	-deprecated
	
	paintImg(img, x, y);
