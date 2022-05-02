#include <iostream>


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


int main()
{
    std::cout << "Hello World!\n";
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
