#include <iostream>
#include <fstream>
int main()
{
    const int imageWidth = 256;
    const int imageHeight = 256;
    const int Rad = 50;
    const int maxColorComponent = 255;
    std::ofstream ppmFileStream("image.ppm", std::ios::out | std::ios::binary);
    ppmFileStream << "P3\n";
    ppmFileStream << imageWidth << ' ' << imageHeight << '\n';
    ppmFileStream << maxColorComponent << '\n';
    for (int rowIdx = 0; rowIdx < imageHeight; ++rowIdx) {
        for (int colIdx = 0; colIdx < imageWidth; ++colIdx) {
            float newXIdx = colIdx - (imageWidth / 2);
            float newYIdx = (imageHeight / 2) - 1 - rowIdx;
            if (((newXIdx * newXIdx) + (newYIdx * newYIdx)) < (Rad * Rad)) {
                ppmFileStream << 255 << ' ' << 255 << ' ' << 0 << '\n';
            }
            else {
                ppmFileStream << 255 << ' ' << 255 << ' ' << 255 << '\n';
            }
        }
    }
}
