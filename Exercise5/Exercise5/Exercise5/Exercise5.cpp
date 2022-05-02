
#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>

const float epsilon = 0.00001F;

class vec3 {
public:
	float x;
	float y;
	float z;

	vec3() : x(0.0), y(0.0), z(0.0) {}
	vec3(float x, float y, float z) : x(x), y(y), z(z) {}
	float dotProduct(const vec3& v0) const { return x * v0.x + y * v0.y + z * v0.z; }
	vec3 crossProduct(const vec3& v0) const { return vec3(y * v0.z - z * v0.y, z * v0.x - x * v0.z, x * v0.y - y * v0.x); }
	vec3 operator- (const vec3& v0) const { return vec3(x - v0.x, y - v0.y, z - v0.z); }
	vec3 operator+ (const vec3& v0) const { return vec3(x + v0.x, y + v0.y, z + v0.z); }
	vec3 operator* (float s) const { return vec3(x * s, y * s, z * s); }
	float len() const { return sqrt(x * x + y * y + z * z); }
	vec3& normalize() {
		float length = 1.0F / len();
		x *= length;
		y *= length;
		z *= length;

		return *this;
	}
};
class Ray {
public:
	vec3 Origin;
	vec3 Dir;
	Ray(const vec3& origin, const vec3& dir) : Origin(origin), Dir(dir) { Dir.normalize();}
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
	bool intersection(const Ray& ray) {
		vec3 normal = getNormal();
		if (std::abs(normal.dotProduct(ray.Dir)) > epsilon) {
			if ((v0 - ray.Origin).dotProduct(normal) < -epsilon) {
				float rpDist = (ray.Origin - v0).dotProduct(normal);
				float rProj = std::abs((ray.Dir).dotProduct(normal));
				float t = rpDist / rProj;
				vec3 p = ray.Dir * t;
				bool test1 = normal.dotProduct((v1 - v0).crossProduct(p - v0)) > 0.0F;
				bool test2 = normal.dotProduct((v2 - v1).crossProduct(p - v1)) > 0.0F;
				bool test3 = normal.dotProduct((v0 - v2).crossProduct(p - v2)) > 0.0F;
				return test1 && test2 && test3;
			}
		}
		return false;
	}
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
	float aspectRatio = (float)imageWidth / imageHeight;

	std::vector<Triangle> triangles = { Triangle (vec3(-1.5, 0, -3), vec3(0, -1, -3), vec3(0, 1, -3)),
									    Triangle (vec3(0, -1, -3), vec3(1.5, 0, -3), vec3(0, 1, -3))};
	
	std::ofstream ppmFileStream("rombcho.ppm", std::ios::out | std::ios::binary);
	ppmFileStream << "P3\n";
	ppmFileStream << imageWidth << " " << imageHeight << "\n";
	ppmFileStream << maxColorComponent << "\n";

	for (int rowIdx = 0; rowIdx < imageHeight; ++rowIdx) {
		for (int colIdx = 0; colIdx < imageWidth; ++colIdx) {
			float x = (float)colIdx + 0.5;
			float y = (float)rowIdx + 0.5;
			x /= imageWidth;
			y /= imageHeight;
			x = (2.0 * x) - 1.0;
			y = 1.0 - (2.0 * y);
			x *= aspectRatio;
			vec3 cameraRayDir(x, y, -1.0F);
			cameraRayDir.normalize();
			Ray cameraRay(vec3(0.0F, 0.0F, 0.0F), cameraRayDir);
			if (triangles[0].intersection(cameraRay)) {
				ppmFileStream << 255 << ' ' << 255 << ' ' << 255 << ' ';
			}
			else if (triangles[1].intersection(cameraRay)) {
				ppmFileStream << 0 << ' ' << 255 << ' ' << 255 << ' ';
			}
			else {
				ppmFileStream << 255 << ' ' << 180 << ' ' << 50 << ' ';
			}
		}
		ppmFileStream << "\n";
	}

	ppmFileStream.close();

	return 0;
}
