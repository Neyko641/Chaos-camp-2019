#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "rapidjson/document.h";
#include "rapidjson/istreamwrapper.h";
#include <assert.h>
#include <time.h>

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
	bool intersection(const Ray& ray, float& distance) {
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
				distance = t;
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

struct TriangleIdxes {
	int v0;
	int v1;
	int v2;
	TriangleIdxes(int v0, int v1, int v2) : v0(v0), v1(v1), v2(v2) {}
};

class objects {
public:
	std::vector<vec3> vertices;
	std::vector<TriangleIdxes> indicies;
};

class scene {
public:
	std::vector<objects> items;
	Camera camera;
	color bgc; // background color;
	int imageWidth;
	int imageHeight;
	std::string filename;
	void ParseScene(const std::string& filename) {
		rapidjson::Document doc = getJsonDoc(filename);
		const rapidjson::Value& settingsVal = doc.FindMember("settings")->value;

		if (!settingsVal.IsNull() && settingsVal.IsObject()) {

			const rapidjson::Value& bg = settingsVal.FindMember("background_color")->value;
			assert(!bg.IsNull() && bg.IsArray());
			vec3 bgColVec = loadVec(bg.GetArray());
			bgc.red = bgColVec.x * 255;
			bgc.green = bgColVec.y * 255;
			bgc.blue = bgColVec.z * 255;

			const rapidjson::Value& imgSettings = settingsVal.FindMember("image_settings")->value;
			assert(!imgSettings.IsNull() && imgSettings.IsObject());
			const rapidjson::Value& width = imgSettings.FindMember("width")->value;
			const rapidjson::Value& height = imgSettings.FindMember("height")->value;
			assert(!width.IsNull() && width.IsInt() && !height.IsNull() && height.IsInt());
			imageWidth = width.GetInt();
			imageHeight = height.GetInt();
			const rapidjson::Value& cam = doc.FindMember("camera")->value;
			if (!cam.IsNull() && cam.IsObject()) {
				const rapidjson::Value& pos = cam.FindMember("position")->value;
				assert(!pos.IsNull() && pos.IsArray());
				camera.origin = loadVec(pos.GetArray());

				const rapidjson::Value& orient = cam.FindMember("matrix")->value;
				assert(!orient.IsNull() && orient.IsArray());
				camera.orient = loadMat(orient.GetArray());
			}
			const rapidjson::Value& obj = doc.FindMember("objects")->value;
			if (!obj.IsNull() && obj.IsArray()) {

				for (int i = 0; i < obj.Size(); i++) {
					const rapidjson::Value& vert = obj[i].FindMember("vertices")->value;
					assert(!vert.IsNull() && vert.IsArray());

					items.push_back(objects());

					for (int j = 0; j < vert.GetArray().Size() / 3; j++) {
						items.back().vertices.emplace_back(
							vert.GetArray()[3 * j].GetFloat(),
							vert.GetArray()[3 * j + 1].GetFloat(),
							vert.GetArray()[3 * j + 2].GetFloat());
					}
					const rapidjson::Value& tri = obj[i].FindMember("triangles")->value;
					assert(!tri.IsNull() && tri.IsArray());
					for (int j = 0; j < tri.GetArray().Size() / 3; j++) {
						items.back().indicies.emplace_back(
							tri.GetArray()[3 * j].GetInt(),
							tri.GetArray()[3 * j + 1].GetInt(),
							tri.GetArray()[3 * j + 2].GetInt());
					}
				}
			}
		}

	}
private:
	rapidjson::Document getJsonDoc(const std::string& filename) {
		std::ifstream input(filename);
		assert(input.is_open());

		rapidjson::IStreamWrapper inStream(input);
		rapidjson::Document doc;
		doc.ParseStream(inStream);

		if (doc.HasParseError()) {
			std::cout << "Error Parsing: " << doc.GetParseError() << "\n";
			std::cout << "Error Getting Offset: " << doc.GetErrorOffset() << "\n";
			assert(false);
		}

		assert(doc.IsObject());
		return doc;
	}
	vec3 loadVec(const rapidjson::Value::ConstArray& val) {
		vec3 vec(val[0].GetFloat(), val[1].GetFloat(), val[2].GetFloat());
		return vec;
	}
	mat3 loadMat(const rapidjson::Value::ConstArray& val) {
		return mat3(
			val[0].GetFloat(), val[3].GetFloat(), val[6].GetFloat(),
			val[1].GetFloat(), val[4].GetFloat(), val[7].GetFloat(),
			val[2].GetFloat(), val[5].GetFloat(), val[8].GetFloat());
	}
};

int main() {
	scene scene1;
	scene1.ParseScene("scene4.crtscene");
	srand(time(NULL));
	color colors[5] = {
  {rand() % 256, rand() % 256, rand() % 256},
  {rand() % 256, rand() % 256, rand() % 256},
  {rand() % 256, rand() % 256, rand() % 256},
  {rand() % 256, rand() % 256, rand() % 256},
  {rand() % 256, rand() % 256, rand() % 256}
	};

	static const int maxColorComponent = 255;
	float aspectRatio = (float)scene1.imageWidth / scene1.imageHeight;

	std::ofstream ppmFileStream("scene4.ppm", std::ios::out | std::ios::binary);
	ppmFileStream << "P3\n";
	ppmFileStream << scene1.imageWidth << " " << scene1.imageHeight << "\n";
	ppmFileStream << maxColorComponent << "\n";
	for (int rowIdx = 0; rowIdx < scene1.imageHeight; ++rowIdx) {
		for (int colIdx = 0; colIdx < scene1.imageWidth; ++colIdx) {
			float x = (float)colIdx + 0.5;
			float y = (float)rowIdx + 0.5;
			x /= scene1.imageWidth;
			y /= scene1.imageHeight;
			x = (2.0 * x) - 1.0;
			y = 1.0 - (2.0 * y);
			x *= aspectRatio;
			vec3 cameraRayDir(x, y, -1.0F);
			cameraRayDir.normalize();
			Ray cameraRay(scene1.camera.origin, scene1.camera.orient * cameraRayDir);
			float closestDistance = FLT_MAX;
			int renderItemIdx = -1;
			int renderTriangleIdx = -1;
			for (auto itemIdx = 0; itemIdx < scene1.items.size(); ++itemIdx) {
				for (auto triangleIdx = 0; triangleIdx < scene1.items[itemIdx].indicies.size(); ++triangleIdx) {
					TriangleIdxes triangleIndex = scene1.items[itemIdx].indicies[triangleIdx];
					Triangle triangle(scene1.items[itemIdx].vertices[triangleIndex.v0],
						scene1.items[itemIdx].vertices[triangleIndex.v1],
						scene1.items[itemIdx].vertices[triangleIndex.v2]);
					float distance = FLT_MAX;
					if (triangle.intersection(cameraRay, distance)) {
						if (distance < closestDistance) {
							closestDistance = distance;
							renderItemIdx = itemIdx;
							renderTriangleIdx = triangleIdx;
						}
					}
				}
			}
			color triColor;
			if (renderTriangleIdx != -1) {
				triColor = colors[renderTriangleIdx % 5];
			}
			else {
				triColor = scene1.bgc;
			}
			ppmFileStream << triColor.red << ' ' << triColor.green << ' ' << triColor.blue << ' ';
		}
		ppmFileStream << "\n";
	}
	ppmFileStream.close();
	return 0;
}
