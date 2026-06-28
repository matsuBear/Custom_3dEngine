#include "Geometry.cpp"
#include "Window.cpp"
#include "Window.hpp"
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_mouse.h>
#include <SDL2/SDL_scancode.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <vector>

class Camera {
public:
  Vec4 origin;
  Vec4 target;
  Vec4 xAxis;
  Vec4 yAxis;
  Vec4 zAxis;

  Matrix view;

  float pendingXRot = 0;
  float pendYRot = 0;

  Camera() = default;
  Camera(Vec4 origin, Vec4 target) {
    this->origin = origin;
    this->target = target;
    this->xAxis = Vec4(1, 0, 0);
    this->yAxis = Vec4(0, 1, 0);
    this->zAxis = Vec4(0, 0, 1);
    view = Matrix();
    viewTransform(target);
  }

  void viewTransform(Vec4 target) {
    Vec4 worldUp(0, 1, 0);

    zAxis = target - origin;
    normalise(zAxis);

    xAxis = cross(worldUp, zAxis);
    normalise(xAxis);

    yAxis = cross(zAxis, xAxis);
    normalise(yAxis);

    view = Matrix((float[]){xAxis.x, xAxis.y, xAxis.z, -dot(origin, xAxis),
                            yAxis.x, yAxis.y, yAxis.z, -dot(origin, yAxis),
                            zAxis.x, zAxis.y, zAxis.z, -dot(origin, zAxis), 0,
                            0, 0, 1});
  };

  void cameraSpinCenter(Vec4 axis, Vec4 center, float angle) {
    pointSpinCenter(axis, center, origin, angle);
    viewTransform(target);
    printf("Cam Pos: %f, %f, %f\n", origin.x, origin.y, origin.z);
  }
};

class Mesh {
public:
  std::vector<Vec4> vecs;
  std::vector<Vec4> vNormals;
  std::vector<std::vector<int>> sharedVertexesList;

  struct Triangle {
  public:
    int vertexes[3];
    Vec4 normal;

    Triangle(int v1, int v2, int v3) {
      vertexes[0] = v1;
      vertexes[1] = v2;
      vertexes[2] = v3;

      normal = Vec4();
    }
  };

  std::vector<Triangle> tris;
  std::vector<Vec4> vectorBuffer;
  std::vector<bool> transCheck;
  std::vector<Vec4> normalBuffer;

  uint32_t color;

  Vec4 center;

  Mesh() = default;

  Vec4 centerCalc() {
    Vec4 u(0, 0, 0);
    if (vecs.empty())
      return u;

    for (const Vec4 &v : vecs) {
      u.x += v.x;
      u.y += v.y;
      u.z += v.z;
    }

    float n = static_cast<float>(vecs.size());
    u.x /= n;
    u.y /= n;
    u.z /= n;
    return u;
  }

  Vec4 returnTNormal(Triangle t, std::vector<Vec4> vs) {
    Vec4 line1 = vs[t.vertexes[1]] - vs[t.vertexes[0]];
    Vec4 line2 = vs[t.vertexes[2]] - vs[t.vertexes[0]];
    Vec4 u = cross(line1, line2);
    normalise(u);
    return u;
  }

  void calcTNormal(Triangle &t, std::vector<Vec4> vs) {
    Vec4 line1 = vs[t.vertexes[1]] - vs[t.vertexes[0]];
    Vec4 line2 = vs[t.vertexes[2]] - vs[t.vertexes[0]];
    t.normal = cross(line1, line2);
    normalise(t.normal);
  }

  void setVNormals() {
    for (int i = 0; i < vecs.size(); i++) {
      Vec4 acc;
      int j = 0;
      while (j < sharedVertexesList[i].size()) {
        acc = acc + tris[sharedVertexesList[i][j]].normal;
        j++;
      }
      acc /= j;
      acc.w = 0;
      vNormals.push_back(acc);
    }
  }

  void setShared() {
    sharedVertexesList.assign(vecs.size(), std::vector<int>());
    for (int j = 0; j < tris.size(); j++) {
      for (int i = 0; i < 3; i++) {
        int vertexIdx = tris[j].vertexes[i];
        sharedVertexesList[vertexIdx].push_back(j);
      }
    }
  }

  void correctTWinding(Triangle &t) {
    if (dot(t.normal, vecs[t.vertexes[0]] - center) < 0) {
      int swap = t.vertexes[0];
      t.vertexes[0] = t.vertexes[1];
      t.vertexes[1] = swap;

      calcTNormal(t, vecs);
    }
  }

  Triangle createTriangle(int v1, int v2, int v3) {
    Triangle t(v1, v2, v3);
    return t;
  }

  int indexOfVec(Vec4 target) {
    int counter = 0;
    for (Vec4 v : vecs) {
      if (v == target) {
        return counter;
      }
      counter++;
    }
    return -1;
  }

  void completeMesh(std::vector<Vec4> vList, std::vector<Triangle> tList,
                    bool correct) {
    tris = tList;
    vecs = vList;
    this->center = centerCalc();
    for (Triangle &t : tris) {
      calcTNormal(t, vecs);
      correctTWinding(t);
    }
    this->setShared();
    this->setVNormals();
    this->transCheck.resize(vecs.size());
  }

  void completeMesh(std::vector<Vec4> vList, std::vector<Triangle> tList,
                    Vec4 center, bool correct) {
    tris = tList;
    vecs = vList;
    this->center = center;
    for (Triangle &t : tris) {
      calcTNormal(t, vecs);
      correctTWinding(t);
    }
    this->setShared();
    this->setVNormals();
    this->transCheck.resize(vecs.size());
  }

  void tMatrixMult(Triangle t, std::vector<Vec4> &vs, Matrix m) {
    for (int i = 0; i < 3; i++) {
      vs[t.vertexes[i]] = m * vs[t.vertexes[i]];
    }
  }

  void tProjecMultAndOffset(Triangle t, std::vector<Vec4> &vs, Matrix p,
                            int width, int height) {
    for (int i = 0; i < 3; i++) {
      if (!transCheck[t.vertexes[i]]) {
        vs[t.vertexes[i]] = p * vs[t.vertexes[i]];
        vs[t.vertexes[i]] /= std::abs(vs[t.vertexes[i]].w);
        vs[t.vertexes[i]].w = 1;
        vs[t.vertexes[i]].x = (1 + vs[t.vertexes[i]].x) * width / 2;
        vs[t.vertexes[i]].y = (1 - vs[t.vertexes[i]].y) * height / 2;
        transCheck[t.vertexes[i]] = true;
      }
    }
  }

  void meshSpin(Vec4 axis, float angle) {
    for (int i = 0; i < vecs.size(); i++) {
      pointSpin(axis, vecs[i], angle);
      pointSpin(axis, vNormals[i], angle);
    }
  }

  void meshTranslate(Vec4 u) {
    for (Vec4 &v : vecs) {
      v += u;
    }
  }

  void meshPointSpin(Vec4 axis, Vec4 center, float angle) {
    for (int i = 0; i < vecs.size(); i++) {
      pointSpinCenter(axis, center, vecs[i], angle);
      pointSpinCenter(axis, center, vNormals[i], angle);
    }
  }
};

Mesh createCube(float unit, uint32_t color, Vec4 center) {
  float s = unit / 2.0;

  Mesh cube;

  // 1. Define the 8 unique vertices
  std::vector<Vec4> vertices = {
      Vec4(center.x - s, center.y - s, center.z - s),
      Vec4(center.x + s, center.y - s, center.z - s),
      Vec4(center.x + s, center.y + s, center.z - s),
      Vec4(center.x - s, center.y + s, center.z - s), // Back
      Vec4(center.x - s, center.y - s, center.z + s),
      Vec4(center.x + s, center.y - s, center.z + s),
      Vec4(center.x + s, center.y + s, center.z + s),
      Vec4(center.x - s, center.y + s, center.z + s) // Front
  };

  // 2. Define triangles using integer indices (referencing the list above)
  std::vector<Mesh::Triangle> triangles;
  // Front (Z+)
  triangles.push_back(cube.createTriangle(4, 5, 6));
  triangles.push_back(cube.createTriangle(4, 6, 7));
  // Back (Z-)
  triangles.push_back(cube.createTriangle(1, 0, 3));
  triangles.push_back(cube.createTriangle(1, 3, 2));
  // Left (X-)
  triangles.push_back(cube.createTriangle(0, 4, 7));
  triangles.push_back(cube.createTriangle(0, 7, 3));
  // Right (X+)
  triangles.push_back(cube.createTriangle(5, 1, 2));
  triangles.push_back(cube.createTriangle(5, 2, 6));
  // Top (Y+)
  triangles.push_back(cube.createTriangle(3, 7, 6));
  triangles.push_back(cube.createTriangle(3, 6, 2));
  // Bottom (Y-)
  triangles.push_back(cube.createTriangle(4, 0, 1));
  triangles.push_back(cube.createTriangle(4, 1, 5));

  cube.completeMesh(vertices, triangles, center, true);

  cube.color = color;

  return cube;
}

Mesh icosahedronGen(float radius, Vec4 origin, int passes, uint32_t color) {
  Mesh icosahedron;
  // The Golden Ratio
  const float phi = (1.0f + std::sqrt(5.0f)) / 2.0f;

  // 1. Define the 12 base vertices of a regular icosahedron
  std::vector<Vec4> vertices = {
      Vec4(-1, phi, 0), Vec4(1, phi, 0), Vec4(-1, -phi, 0), Vec4(1, -phi, 0),
      Vec4(0, -1, phi), Vec4(0, 1, phi), Vec4(0, -1, -phi), Vec4(0, 1, -phi),
      Vec4(phi, 0, -1), Vec4(phi, 0, 1), Vec4(-phi, 0, -1), Vec4(-phi, 0, 1)};

  // Normalize and scale to the requested radius, then offset by origin
  for (auto &v : vertices) {
    normalise(v);
    v *= radius;
    v = v + origin;
  }

  // 2. Define the 20 base triangles (indices)
  std::vector<Mesh::Triangle> triangles = {
      {0, 11, 5}, {0, 5, 1},  {0, 1, 7},   {0, 7, 10}, {0, 10, 11},
      {1, 5, 9},  {5, 11, 4}, {11, 10, 2}, {10, 7, 6}, {7, 1, 8},
      {3, 9, 4},  {3, 4, 2},  {3, 2, 6},   {3, 6, 8},  {3, 8, 9},
      {4, 9, 5},  {2, 4, 11}, {6, 2, 10},  {8, 6, 7},  {9, 8, 1}};

  icosahedron.completeMesh(vertices, triangles, false);
  icosahedron.color = color;
  return icosahedron;
}

Mesh icosphereHelper() {}

class LightCube {
public:
  Mesh cube;
  Vec4 light;

  LightCube(Vec4 l, float unit) {
    cube = createCube(unit, 0xFFFFFFFF, l);
    light = l;
  }

  void lightCubeOrbit(Vec4 axis, Vec4 center, float angle) {
    pointSpinCenter(axis, center, light, angle);
    cube.meshPointSpin(axis, center, angle);
  }

  void lcTranslate(Vec4 u) {
    cube.meshTranslate(u);
    light += u;
  }

  void lcReset(Vec4 origin) {
    cube.meshTranslate(origin - light);
    light = origin;
  }
};

const int height = 1200;
const int width = 1500;
const float Pi = 2 * acos(0);
float fNear = 1;
float fFar = 1000;
float FOV = Pi * 1 / 4;
float aspectRatio = (float)width / height;
float deltaTime = 0;

Vec4 worldOrigin = Vec4(0, 0, 0);
Vec4 worldX = Vec4(1, 0, 0);
Vec4 worldY = Vec4(0, 1, 0);
Vec4 worldZ = Vec4(0, 0, 1);
Vec4 light = Vec4(0, 0, -6);
float ambientLight = 0.2;

std::string title = "Engine";
Camera cam(Vec4(0, 0, -5), worldOrigin);
Window window(title, width, height);

std::vector<Mesh> meshes;
int meshIndex = 0;
Mesh *currentMesh;

class Renderer {
public:
  Matrix projec;
  Vec4 normalBuffer;
  bool rendPause = false;

  Renderer()
      : projec(Matrix((float[]){
            1 / (std::tan(FOV) * aspectRatio), 0, 0, 0, 0, 1 / std::tan(FOV), 0,
            0, 0, 0, (fFar + fNear) / (fFar - fNear),
            -2 * fNear * fFar / (fFar - fNear), 0, 0, 1, 0})) {};

  float edgeFunc(Vec4 a, Vec4 b, Vec4 p) {
    return (p.x - a.x) * (b.y - a.y) - (p.y - a.y) * (b.x - a.x);
  }

  bool inEdge(Vec4 a, Vec4 b, Vec4 p) {
    if (edgeFunc(a, b, p) >= 0) {
      return true;
    } else {
      return false;
    }
  }

  void fillTriangle(Vec4 a, Vec4 b, Vec4 c, float ia, float ib, float ic,
                    uint32_t color) {
    int maxX = std::max(std::max(a.x, b.x), c.x);
    int maxY = std::max(std::max(a.y, b.y), c.y);
    int minX = std::min(std::min(a.x, b.x), c.x);
    int minY = std::min(std::min(a.y, b.y), c.y);

    maxX = std::min(maxX, width - 1);
    maxY = std::min(maxY, height - 1);
    minX = std::max(minX, 0);
    minY = std::max(minY, 0);

    for (int i = minY; i <= maxY; i++) {
      for (int j = minX; j <= maxX; j++) {
        Vec4 fillingVec(j, i, 0);
        float p0 = edgeFunc(a, b, fillingVec);
        float p1 = edgeFunc(b, c, fillingVec);
        float p2 = edgeFunc(c, a, fillingVec);
        float area = edgeFunc(a, b, c);
        float wA = p1 / area;
        float wB = p2 / area;
        float wC = p0 / area;
        if (p0 <= 0 && p1 <= 0 && p2 <= 0) {
          float z = (wA)*a.z + (wB)*b.z + (wC)*c.z;
          if (i < 0 || i >= height || j < 0 || j >= width)
            continue;

          int bufferIndex = i * width + j;

          if (window.m_zBuffer[bufferIndex] >= z) {
            window.m_zBuffer[bufferIndex] = z;

            float currentIntensity =
                ambientLight +
                (1 - ambientLight) * ((wA * ia) + (wB * ib) + (wC * ic));

            uint8_t r = static_cast<uint8_t>((float)((color >> 16) & 0xFF) *
                                             currentIntensity);
            uint8_t g = static_cast<uint8_t>((float)((color >> 8) & 0xFF) *
                                             currentIntensity);
            uint8_t b =
                static_cast<uint8_t>((float)(color & 0xFF) * currentIntensity);

            uint32_t shadedColor = (0xFF << 24) | (r << 16) | (g << 8) | b;
            window.setPixel(j, i, shadedColor);
          }
        }
      }
    }
  }

  void drawMesh(Mesh &mesh, Camera &c, Vec4 l) {
    mesh.vectorBuffer.clear();
    mesh.normalBuffer.clear();
    mesh.transCheck.assign(mesh.vecs.size(), false);
    for (int i = 0; i < mesh.vecs.size(); i++) {
      mesh.vectorBuffer.push_back(c.view * mesh.vecs[i]);
      mesh.normalBuffer.push_back(returnNormalised(c.view * mesh.vNormals[i]));
    }
    std::vector<Vec4> screenBuffer = mesh.vectorBuffer;
    Vec4 lightCamSpace = c.view * l;
    for (Mesh::Triangle t : mesh.tris) {
      if (mesh.vectorBuffer[t.vertexes[0]].z < fNear ||
          mesh.vectorBuffer[t.vertexes[1]].z < fNear ||
          mesh.vectorBuffer[t.vertexes[2]].z < fNear)
        continue;
      normalBuffer = mesh.returnTNormal(t, mesh.vectorBuffer);
      if (dot(normalBuffer, mesh.vectorBuffer[t.vertexes[0]]) < 0) {
        float i1 = std::max(
            (float)0, dot(returnNormalised(lightCamSpace -
                                           mesh.vectorBuffer[t.vertexes[0]]),
                          mesh.normalBuffer[t.vertexes[0]]));
        float i2 = std::max(
            (float)0, dot(returnNormalised(lightCamSpace -
                                           mesh.vectorBuffer[t.vertexes[1]]),
                          mesh.normalBuffer[t.vertexes[1]]));
        float i3 = std::max(
            (float)0, dot(returnNormalised(lightCamSpace -
                                           mesh.vectorBuffer[t.vertexes[2]]),
                          mesh.normalBuffer[t.vertexes[2]]));
        mesh.tProjecMultAndOffset(t, screenBuffer, projec, width, height);
        fillTriangle(screenBuffer[t.vertexes[0]], screenBuffer[t.vertexes[1]],
                     screenBuffer[t.vertexes[2]], i1, i2, i3, mesh.color);
      }
    }
  }

  void fillTriangleNoShade(Vec4 a, Vec4 b, Vec4 c, uint32_t color) {
    int maxX = std::max(std::max(a.x, b.x), c.x);
    int maxY = std::max(std::max(a.y, b.y), c.y);
    int minX = std::min(std::min(a.x, b.x), c.x);
    int minY = std::min(std::min(a.y, b.y), c.y);

    maxX = std::min(maxX, width - 1);
    maxY = std::min(maxY, height - 1);
    minX = std::max(minX, 0);
    minY = std::max(minY, 0);

    for (int i = minY; i <= maxY; i++) {
      for (int j = minX; j <= maxX; j++) {
        Vec4 fillingVec(j, i, 0);
        float p0 = edgeFunc(a, b, fillingVec);
        float p1 = edgeFunc(b, c, fillingVec);
        float p2 = edgeFunc(c, a, fillingVec);
        float area = edgeFunc(a, b, c);
        float wA = p1 / area;
        float wB = p2 / area;
        float wC = p0 / area;
        if (p0 <= 0 && p1 <= 0 && p2 <= 0) {
          float z = (wA)*a.z + (wB)*b.z + (wC)*c.z;
          if (i < 0 || i >= height || j < 0 || j >= width)
            continue;

          int bufferIndex = i * width + j;

          if (window.m_zBuffer[bufferIndex] >= z) {
            window.m_zBuffer[bufferIndex] = z;
            window.setPixel(j, i, color);
          }
        }
      }
    }
  }

  void drawMeshNoShade(Mesh &mesh, Camera &c) {
    mesh.vectorBuffer.clear();
    mesh.normalBuffer.clear();
    mesh.transCheck.assign(mesh.vecs.size(), false);
    for (int i = 0; i < mesh.vecs.size(); i++) {
      mesh.vectorBuffer.push_back(c.view * mesh.vecs[i]);
    }
    std::vector<Vec4> screenBuffer = mesh.vectorBuffer;
    for (Mesh::Triangle t : mesh.tris) {
      if (mesh.vectorBuffer[t.vertexes[0]].z < fNear ||
          mesh.vectorBuffer[t.vertexes[1]].z < fNear ||
          mesh.vectorBuffer[t.vertexes[2]].z < fNear)
        continue;
      normalBuffer = mesh.returnTNormal(t, mesh.vectorBuffer);
      if (dot(normalBuffer, mesh.vectorBuffer[t.vertexes[0]]) < 0) {
        mesh.tProjecMultAndOffset(t, screenBuffer, projec, width, height);
        fillTriangleNoShade(screenBuffer[t.vertexes[0]],
                            screenBuffer[t.vertexes[1]],
                            screenBuffer[t.vertexes[2]], mesh.color);
      }
    }
  }
};

void cubeRoutine(Mesh &cube, double delta) {
  cube.meshSpin(cube.tris[11].normal, 0.3 * delta);
  cube.meshSpin(cube.tris[5].normal, 1 * delta);
  cube.meshSpin(cube.tris[0].normal, 0.5 * delta);
}

void meshSelection(Mesh *&ptr, std::vector<Mesh> &meshes, int &currentIndex,
                   bool up) {
  if (up) {
    currentIndex = (currentIndex + 1 >= meshes.size()) ? 0 : currentIndex + 1;
    ptr = &meshes[currentIndex];
  } else {
    currentIndex = (currentIndex - 1 < 0) ? currentIndex = meshes.size() - 1
                                          : currentIndex - 1;
    ptr = &meshes[currentIndex];
  }
}

void eventHandler(Renderer &r, Camera &c, LightCube &lc) {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    if (event.type == SDL_QUIT) {
      window.running = false;
    } else {
      if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.scancode) {
        case SDL_SCANCODE_ESCAPE:
          window.running = false;
          break;
        case SDL_SCANCODE_2:
          if (event.key.repeat == 0) {
            meshSelection(currentMesh, meshes, meshIndex, true);
          }
          break;
        case SDL_SCANCODE_1:
          if (event.key.repeat == 0) {
            meshSelection(currentMesh, meshes, meshIndex, false);
          }
          break;
        case SDL_SCANCODE_P:
          if (event.key.repeat == 0) {
            r.rendPause = (r.rendPause) ? false : true;
          }
          break;
        case SDL_SCANCODE_L:
          c.origin.x = 0;
          c.origin.y = 0;
          c.origin.z = -5;
          c.viewTransform(c.target);
          lc.lcReset(worldOrigin);
          break;
        }
      }
      if (event.type == SDL_MOUSEMOTION) {
        switch (event.button.button) {
        case SDL_BUTTON_LEFT:
          int mouseX, mouseY;
          Uint32 mouseState = SDL_GetMouseState(&mouseX, &mouseY);
          if (mouseState & SDL_BUTTON_LMASK) {
            c.pendingXRot += window.mouseSense * event.motion.xrel;
            c.pendYRot += window.mouseSense * event.motion.yrel;
          }
          break;
        }
      }
      if (event.type == SDL_MOUSEWHEEL) {
        Vec4 newOrigin = c.origin + (c.zAxis * (float)event.wheel.y);
        newOrigin.print();
        Vec4 distance = c.target - newOrigin;
        std::cout << distance.norm() << '\n';
        if (distance.norm() > 0.5f) {
          c.origin = newOrigin;
          c.viewTransform(c.target);
        }
      }
    }
  }
}

void continousInput(Renderer &r, Camera &c, LightCube &lc) {
  SDL_Event event;
  const Uint8 *keys = SDL_GetKeyboardState(NULL);
  if (keys[SDL_SCANCODE_LEFT]) {
    if (keys[SDL_SCANCODE_LSHIFT]) {
      lc.lcTranslate(worldX * (-0.01));
    } else {
      lc.lightCubeOrbit(worldY, worldOrigin, 0.03);
    }
  }
  if (keys[SDL_SCANCODE_RIGHT]) {
    if (keys[SDL_SCANCODE_LSHIFT]) {
      lc.lcTranslate(worldX * (0.1));
    } else {
      lc.lightCubeOrbit(worldY, worldOrigin, -0.03);
    }
  }
  if (keys[SDL_SCANCODE_UP]) {
    if (keys[SDL_SCANCODE_LSHIFT]) {
      lc.lcTranslate(worldZ * (0.1));
    } else if (keys[SDL_SCANCODE_LCTRL]) {
      lc.lcTranslate(worldY * 0.1);
    } else {
      lc.lightCubeOrbit(worldX, worldOrigin, 0.03);
    }
  }
  if (keys[SDL_SCANCODE_DOWN]) {
    if (keys[SDL_SCANCODE_LSHIFT]) {
      lc.lcTranslate(worldZ * (-0.1));
    } else if (keys[SDL_SCANCODE_LCTRL]) {
      lc.lcTranslate(worldY * (-0.1));
    } else {
      lc.lightCubeOrbit(worldX, worldOrigin, -0.03);
    }
  }
}

void updateCamera(Camera &c) {
  if (c.pendingXRot != 0) {
    c.cameraSpinCenter(worldY, c.target, c.pendingXRot);
  }
  if (c.pendYRot != 0) {
    c.cameraSpinCenter(c.xAxis, c.target, c.pendYRot);
  }
  c.pendingXRot = 0;
  c.pendYRot = 0;
}

int main() {
  Renderer r;

  Mesh cube = createCube(2, 0xFF0AC0C5, worldOrigin);
  Mesh cube2 = createCube(1.5, 0xFFF33138, worldOrigin);
  Mesh ico = icosahedronGen(2, worldOrigin, 1, 0x2AF719);
  LightCube lc(light, 0.3);

  meshes.push_back(cube);
  meshes.push_back(cube2);
  meshes.push_back(ico);

  currentMesh = &meshes[meshIndex];

  uint64_t currentTime = SDL_GetTicks();
  uint64_t timeElapsed = 0;

  Window &win = window;

  while (window.running) {
    win.clear(0xFF000000);
    eventHandler(r, cam, lc);
    continousInput(r, cam, lc);
    updateCamera(cam);
    r.drawMesh(*currentMesh, cam, lc.light);
    r.drawMeshNoShade(lc.cube, cam);
    currentTime = SDL_GetTicks();
    deltaTime = (double)(currentTime - timeElapsed) / 1000;
    timeElapsed = currentTime;
    if (r.rendPause) {
      cubeRoutine(*currentMesh, deltaTime);
    }
    win.present();
  }

  return 0;
}