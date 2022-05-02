
#include <iostream>
#include <fstream>
#include <time.h>
#include <array>

struct color {
	int red;
	int blue;
	int green;
};
 
int main()
{
	srand(time(NULL));
	const int canvasWidth = 256;
	const int canvasHeight = 256;
	const int maxColorComponent = 255;
	const int rectH = 4;
	const int rectW = 4;
	const int pixelsPerHRect = canvasHeight / rectH;
	const int pixelsPerWRect = canvasWidth / rectW;
	std::array<color, rectH * rectW> data;
	for (auto amountOfRectanglesH = 0; amountOfRectanglesH < rectH; ++amountOfRectanglesH) {
		for (auto amountOfRectangleW = 0; amountOfRectangleW < rectW; ++amountOfRectangleW) {
			color RGB;
			RGB.red = rand() % 256;
			RGB.blue = rand() % 256;
			RGB.green = rand() % 256;
			data[amountOfRectanglesH * rectW + amountOfRectangleW] = RGB;

		}
	}

	std::ofstream ppmFileStream("image.ppm", std::ios::out | std::ios::binary);
	ppmFileStream << "P3\n";
	ppmFileStream << canvasWidth << ' ' << canvasHeight << '\n';
	ppmFileStream << maxColorComponent << '\n';
	for (auto pixelsY = 0; pixelsY < canvasHeight; ++pixelsY) {
		for (auto pixelsX = 0; pixelsX < canvasWidth; ++pixelsX) {
			int rectIdxH = pixelsY / pixelsPerHRect;
			int rectIdxW = pixelsX / pixelsPerWRect;
			color RGB = data[rectIdxH * rectW + rectIdxW];
			ppmFileStream << RGB.red << ' ' << RGB.green << ' ' << RGB.blue << '\n';
		}
	}

	 ppmFileStream.close();
 }