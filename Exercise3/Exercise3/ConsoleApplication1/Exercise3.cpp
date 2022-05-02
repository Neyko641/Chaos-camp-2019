
#include <iostream>
#include <fstream>


class vec3 {
public:
	float x;
	float y;
	float z;

	vec3() : x(0.0), y(0.0), z(0.0) {}
	vec3(float x, float y, float z) : x(x), y(y), z(z) {}
	float dotProduct(const vec3& v0) { return x * v0.x + y * v0.y + z * v0.z; }
	vec3 crossProduct(const vec3& v0) { return vec3(y * v0.z - z * v0.y, z * v0.x - x * v0.z, x * v0.y - y * v0.x); }
	vec3 operator- (const vec3& v0) { return vec3(x - v0.x, y - v0.y, z - v0.z); }
	vec3 operator+ (const vec3& v0) { return vec3(x + v0.x, y + v0.y, z + v0.z); }
	float len() const { return sqrt(x * x + y * y + z * z); }
	vec3& normalize() { 
		float length = 1.0F / len();
		x *= length;
		y *= length;
		z *= length;

		return *this;
	}
};
class Triangle {
public: 
	vec3 v0;
	vec3 v1;
	vec3 v2;

	Triangle(const vec3& v0, const vec3& v1, const vec3& v2) : v0(v0), v1(v1), v2(v2) {}
	vec3 getNormal() {
		vec3 normal = (v1 - v0).crossProduct(v2 - v0); 
		normal.normalize();
		return normal;
	};

};


struct color {
	int red;
	int blue;
	int green;
};



int main() {
	/// Output image resolution
	static const int imageWidth = 1920;
	static const int imageHeight = 1080;

	static const int maxColorComponent = 255;
	float aspectRatio = (float) imageWidth / imageHeight;
	std::ofstream ppmFileStream("crt_output_image.ppm", std::ios::out | std::ios::binary);
	ppmFileStream << "P3\n";
	ppmFileStream << imageWidth << " " << imageHeight << "\n";
	ppmFileStream << maxColorComponent << "\n";

	for (int rowIdx = 0; rowIdx < imageHeight; ++rowIdx) {
		for (int colIdx = 0; colIdx < imageWidth; ++colIdx) {
			color RGB;
			float x = (float)colIdx + 0.5;
			float y = (float)rowIdx + 0.5;
			x /= imageWidth;
			y /= imageHeight;
			x = (2.0 * x) - 1.0;
			y = 1.0 - (2.0 * y);
			x *= aspectRatio;
			vec3 cameraRay(x, y, -1.0F);
			cameraRay.normalize();
			RGB.red = (int)(((cameraRay.x + 1) / 2.0F) * 255);
			RGB.green = (int)(((cameraRay.y + 1) / 2.0F) * 255);
			RGB.blue = (int)(((cameraRay.z + 1) / 2.0F) * 255);
			ppmFileStream << RGB.red << ' ' << RGB.green << ' ' << RGB.blue << ' ';
		}
		ppmFileStream << "\n";
	}

	ppmFileStream.close();

	return 0;
}
