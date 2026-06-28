
#include <algorithm>
#include <bits/types/cookie_io_functions_t.h>
#include <cmath>
#include <iostream>
#include <ostream>

class Vec4 {
public:
  union {
    float coords[4];
    struct {
      float x, y, z, w;
    };
  };

  Vec4() : x(0), y(0), z(0), w(1) {}

  Vec4(float x, float y, float z) : x(x), y(y), z(z), w(1) {}

  Vec4 operator/=(float q) {
    if (q != 0) {
      for (int i = 0; i < 3; i++) {
        coords[i] /= q;
      }
    }
    return *this;
  }

  Vec4 operator*=(float q) {
    for (int i = 0; i < 3; i++) {
      coords[i] *= q;
    }
    return *this;
  }

  Vec4 operator*(float q) {
    Vec4 u;
    for (int i = 0; i < 3; i++) {
      u.coords[i] = coords[i] * q;
    }
    return u;
  }

  Vec4 operator+(Vec4 v) { return Vec4(x + v.x, y + v.y, z + v.z); }
  Vec4 operator-(Vec4 v) { return Vec4(x - v.x, y - v.y, z - v.z); }

  void operator+=(Vec4 v) {
    for (int i = 0; i < 3; i++) {
      this->coords[i] += v.coords[i];
    }
  }

  void operator-=(Vec4 v) {
    for (int i = 0; i < 3; i++) {
      this->coords[i] -= v.coords[i];
    }
  }

  bool operator==(Vec4 v) {
    for (int i = 0; i < 3; i++) {
      if (this->coords[i] != v.coords[i]) {
        return false;
      }
    }
    return true;
  }

  float norm() { return std::sqrt(x * x + y * y + z * z); }

  void print() {
    std::cout << "{" << x << ", " << y << ", " << z << "}" << std::endl;
  }
};

float dot(Vec4 v, Vec4 u) {
  float val = 0;
  for (int i = 0; i < 3; i++) {
    val += v.coords[i] * u.coords[i];
  }
  return val;
}

Vec4 cross(Vec4 v, Vec4 u) {
  return Vec4(v.y * u.z - v.z * u.y, v.z * u.x - v.x * u.z,
              v.x * u.y - v.y * u.x);
}

void normalise(Vec4 &v) {
  float size = v.norm();
  for (int i = 0; i < 3; i++) {
    v.coords[i] /= size;
  }
}

Vec4 returnNormalised(Vec4 v) {
  Vec4 u = v;
  float size = u.norm();
  for (int i = 0; i < 3; i++) {
    u.coords[i] /= size;
  }
  u.w = 0;
  return u;
}

class Matrix {
public:
  float values[16];

  Matrix() {
    std::fill(values, values + 16, 0);
    values[0] = values[5] = values[10] = values[15] = 1;
  };

  Matrix(float arr[16]) {
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
        values[i * 4 + j] = arr[i * 4 + j];
      }
    };
  }

  float &index(int i, int j) { return values[4 * i + j]; }

  Vec4 operator*(Vec4 v) {
    Vec4 u;
    u.coords[3] = 0;
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
        u.coords[i] += index(i, j) * v.coords[j];
      }
    }
    return u;
  }
};

class Quaternion {
public:
  float r;
  float x;
  float y;
  float z;

  Quaternion() = default;
  Quaternion(float r, float x, float y, float z) {
    this->r = r;
    this->x = x;
    this->y = y;
    this->z = z;
  }

  static Quaternion inverse(Quaternion q) {
    return Quaternion(q.r, -q.x, -q.y, -q.z);
  }

  static Quaternion angleAxis(Vec4 axis, float angle) {
    return Quaternion(std::cos(angle / 2), std::sin(angle / 2) * axis.x,
                      std::sin(angle / 2) * axis.y,
                      std::sin(angle / 2) * axis.z);
  }

  Quaternion operator*(Quaternion q) {
    return Quaternion(r * q.r - x * q.x - y * q.y - z * q.z,
                      r * q.x + x * q.r + y * q.z - z * q.y,
                      r * q.y - x * q.z + y * q.r + z * q.x,
                      r * q.z + x * q.y - y * q.x + z * q.r);
  }
};

void pointSpin(Vec4 axis, Vec4 &point, float angle) {
  Quaternion p(0, point.x, point.y, point.z);
  Quaternion q = Quaternion::angleAxis(axis, angle);
  Quaternion result = (q * p) * Quaternion::inverse(q);
  point.x = result.x;
  point.y = result.y;
  point.z = result.z;
}

void pointSpinCenter(Vec4 axis, Vec4 center, Vec4 &point, float angle) {
  Quaternion p(0, point.x - center.x, point.y - center.y, point.z - center.z);
  Quaternion q = Quaternion::angleAxis(axis, angle);
  Quaternion result = (q * p) * Quaternion::inverse(q);
  point.x = result.x;
  point.y = result.y;
  point.z = result.z;
}

// int main() {
//   Vec4 v(1, 5, 2);
//   Vec4 u(5, 3, 5);

//   std::cout << dot(v, u) << '\n';
//   u * 3;
//   u.print();
//   return 0;
// }