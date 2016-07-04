For this to work properly, do the following:

1) Get any image, open it in GIMP
2) Resize it to about 150px x 150px
3) Export is as "diranosaurio.c" (not another name) - ensure that:
	- You uncheck ALL export options (e.g. no alpha channel)
	- Leave the image name as "gimp_image"
	- The name in the makefile build.
4) ImageWriter will pick it up and convert it to "diranosaurio.img"
5) In Image/Makefile, make sure the following line is UNCOMMENTED:

	USERLAND=../Userland/userland.bin ../Userland/Songmaker/song1.bin ../Userland/img/dinosaurio.img

6) When compiling the project, the image will be loaded into a separate
data module (DATA_MODULE_IMG). Find its address.
7) The following will print your image:

	Image *img = (Image *) DATA_MODULE_IMG_ADDR;
	paintImg(img, x, y);
