ncPrint("Creating basic file...");
BasicFile f = createBasicFileWithName("test");
if(f == NULL) {
	ncPrint("failed.\n");
	return -1;
}
ncPrint("done.\nWriting 'T'...");
if(basicFileWriteChar('T', f) == EOF) {
	ncPrint("failed.\n");
	return -1;
}
ncPrint("done.\nReading...");
int c = basicFileReadChar(f);
if(c == EOF) {
	ncPrint("failed.\n");
	return -1;
}
ncPrintChar(c);
ncPrint("\nDestroying file...");
if(destroyBasicFile(f) == -1) {
	ncPrint("failed.\n");
	return -1;
}
ncPrint("done.\n");
return 0;