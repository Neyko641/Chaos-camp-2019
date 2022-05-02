#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <fstream>
#include <string>
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

class mat3 {
public:
	mat3() {
		for (int i = 0; i < 9; ++i) {
			a[i] = 0.0F;
		}
	}
	mat3(float a0, float a1, float a2, float a3, float a4, float a5, float a6, float a7, float a8) {
		a[0] = a0;
		a[1] = a1;
		a[2] = a2;
		a[3] = a3;
		a[4] = a4; 
		a[5] = a5;
		a[6] = a6;
		a[7] = a7;
		a[8] = a8;
	}
	vec3 operator*(const vec3& v) const {
		return vec3(
			a[0] * v.x + a[1] * v.y + a[2] * v.z,
			a[3] * v.x + a[4] * v.y + a[5] * v.z,
			a[6] * v.x + a[7] * v.y + a[8] * v.z);
	}
	mat3 operator*(const mat3& m) const {
		vec3 row0(a[0], a[1], a[2]);
		vec3 row1(a[3], a[4], a[5]);
		vec3 row2(a[6], a[7], a[8]);
		vec3 col0(m.a[0], m.a[3], m.a[6]);
		vec3 col1(m.a[1], m.a[4], m.a[7]);
		vec3 col2(m.a[2], m.a[5], m.a[8]);
		return mat3(row0.dotProduct(col0), row0.dotProduct(col1), row0.dotProduct(col2),
			row1.dotProduct(col0), row1.dotProduct(col1), row1.dotProduct(col2),
			row2.dotProduct(col0), row2.dotProduct(col1), row2.dotProduct(col2));
	}
	mat3 Transpose() {
		return mat3(a[0], a[3], a[6], a[1], a[4], a[7], a[2], a[5], a[8]);
	}
	float a[9];
};


class Ray {
public:
	vec3 Origin;
	vec3 Dir;
	Ray(const vec3& origin, const vec3& dir) : Origin(origin), Dir(dir) { Dir.normalize(); }
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
				vec3 p = ray.Dir * t + ray.Origin;
				bool test1 = normal.dotProduct((v1 - v0).crossProduct(p - v0)) > 0.0F;
				bool test2 = normal.dotProduct((v2 - v1).crossProduct(p - v1)) > 0.0F;
				bool test3 = normal.dotProduct((v0 - v2).crossProduct(p - v2)) > 0.0F;
				return test1 && test2 && test3;
			}
		}
		return false;
	}
};
class Camera {
public:
	Camera() {
		orient.a[0] = 1.0F;
		orient.a[4] = 1.0F;
		orient.a[8] = 1.0F;
	}
	mat3 orient;
	vec3 origin;
	void dolly(float distance) {
		vec3 fwdVec = vec3(-orient.a[2], -orient.a[5], -orient.a[8]);
		origin = origin + fwdVec.normalize() * distance;
	}
	void truck(float distance) {
		vec3 rightVec = vec3(orient.a[0], orient.a[3], orient.a[6]);
		origin = origin + rightVec.normalize() * distance;
	}
	void pedestal(float distance) {
		vec3 upVec = vec3(orient.a[1], orient.a[4], orient.a[7]);
		origin = origin + upVec.normalize() * distance;
	}
	void pan(float angle) {
		float cos = cosf(angle * M_PI / 180.0F);
		float sin = sinf(angle * M_PI / 180.0F);
		mat3 rotation(cos, 0.0F, sin, 0.0F, 1.0F, 0.0F, -sin, 0.0F, cos);
		orient = rotation * orient;
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

	std::vector<Triangle> triangles = { Triangle(vec3(-1.5, 0, -3), vec3(0, -1, -3), vec3(0, 1, -3)),
										Triangle(vec3(0, -1, -3), vec3(1.5, 0, -3), vec3(0, 1, -3)) };
	Camera cam;
	cam.pan(-30.0F);
	for (auto frames = 0; frames < 10; ++frames) {
		std::ofstream ppmFileStream("frames" + std::to_string(frames) + ".ppm", std::ios::out | std::ios::binary);
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
				Ray cameraRay(cam.origin, cam.orient * cameraRayDir);
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
		cam.pan(6.0F);
	}
	return 0;
}
