#pragma once
#include "EBO.hpp"
#include "VAO.hpp"
#include "VBO.hpp"
#include "float3.hpp"
#include "worldObject.hpp"
#include <utility>
#include <vector>

class Mesh : public Component {
public:
  class Vertex {
  public:
    Vertex(float3 point, float3 normal);

    float3 point;
    float3 normal;
  };

  class Triangle {
  public:
    Triangle(int v1, int v2, int v3);

    int vertex[3];
    float3 normal;
  };

  Mesh(std::vector<Vertex> vecs, std::vector<Triangle>, float3 color,
       int shininess, int GLMeshType, bool meshForm);

  Mesh() = default;
  ~Mesh();

  void setTNormal(Triangle &t);
  void setAllTNormal();
  void dumpTris();
  float3 returnNormal(Triangle &t);
  void setShared();
  void setNormals();
  void edgeSplitting();
  void setVBO();
  void setEBO();
  void setVAO();
  void prepareRendering();
  void deleteRenderingObjects();
  void drawMesh(GLint colorLoc, GLint shineLoc, GLint modelLoc,
                Entity *entitySceneData, bool backCulling);
  static Mesh cube(float3 origin, float size, float3 color, int shininess,
                   int meshType);

  std::vector<Vertex> vecs;
  std::vector<Triangle> tris;
  std::vector<GLuint> dumpedTris;
  std::vector<std::vector<std::pair<int, int>>> sharedVertexesList;
  float3 color;
  int shininess;
  int meshType;

  int id;
  TypeId typeId;

  VBO mVBO;
  EBO mEBO;
  VAO mVAO;
};