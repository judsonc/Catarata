#include "util.h"

// remove the filepath from the string
char *stripFilepath(char *filepath)
{
	char *strippedFilepath = (char *) calloc(strlen(filepath), sizeof(char));

	// strip folder name from the filepath
	ushort m = strlen(filepath);
	ushort n = 0;
	while (filepath[m] != '/') {
		strippedFilepath[n] = filepath[m];
		--m;
		++n;
	}
	strippedFilepath[n] = '\0';

	// here we have to invert the string because it is reversed (Catarata.ppm is mpp.atarataC)
	for (int i = 0, j = n - 1; i < j; ++i, --j) {
		char aux = strippedFilepath[i];
		strippedFilepath[i] = strippedFilepath[j];
		strippedFilepath[j] = aux;
	}

	return strippedFilepath;
}

// generate the output filepath
char *outFilepath(char *folder, char *filename, char *toCat, char *format)
{
	// calculate the resulting size of the outFilepath
	uchar outSize = strlen(folder) + strlen(filename) + strlen(toCat);

	char *outFilepath = (char *) calloc(outSize, sizeof(char));

	// concatenate the output folder
	strcat(outFilepath, folder);

	// strip image format from the filename
	uchar i = 0;
	uchar j = strlen(outFilepath);
	while (filename[i] != '.') {
		outFilepath[j] = filename[i];
		++i;
		++j;
	}

	// concatenate the string that we want to add
	strcat(outFilepath, toCat);
	// add the '.' for the format
	outFilepath[strlen(outFilepath)] = '.';
	// concatenate the format
	strcat(outFilepath, format);

	return outFilepath;
}

// this allocates a new Pixel **
Pixel **allocatePixel(int height, int width)
{
	Pixel **pixels = (Pixel **) calloc(height, sizeof(Pixel *));
	for (int i = 0; i < height; ++i) {
		pixels[i] = (Pixel *) calloc(width, sizeof(Pixel));
	}

	return pixels;
}

// this allocates a new Img *
Img *createImg(int height, int width)
{
	Img *newImg = (Img *) calloc(1, sizeof(Img));
	newImg->height = height;
	newImg->width = width;

	newImg->pixels = allocatePixel(height, width);

	return newImg;
}
