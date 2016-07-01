ncPrint("Creating basic file...");
BasicFile f = createBasicFileWithName("test");
if(f == NULL) {
	ncPrint("failed.\n");
	return -1;
}
ncPrint("done.\n");
uint64_t ct = 0;
while(basicFileWriteChar((char)ct, f) != EOF) {
	ct++;
}
ncPrint("Wrote ");
ncPrintDec(ct);
ncPrint("chars");

ncPrint("\nReading...");
int c;
while((c = basicFileReadChar(f)) != EOF) {
	ncPrintChar(c);
}
ncPrint("done.\nDestroying file...");
if(destroyBasicFile(f) == -1) {
	ncPrint("failed.\n");
	return -1;
}
ncPrint("done.\n");
return 0;