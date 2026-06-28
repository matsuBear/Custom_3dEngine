#include "mesh.hpp"
#include "EBO.hpp"
#include "VAO.hpp"
#include "float3.hpp"
#include "worldObject.hpp"
#include <utility>
#include <vector>

namespace meshStuff {
bool SHARP_MESH = false;
bool ORGANIC_MESH = true;
} // namespace meshStuff

Mesh::Mesh(std::vector<Mesh::Vertex> vecs, std::vector<Mesh::Triangle> tris,
           float3 color, int shininess, int GLMeshType, bool meshForm) {
  this->tris = tris;
  this->vecs = vecs;
  this->typeId = getComponentTypeId<Mesh>();
  setAllTNormal();
  this->sharedVertexesList.resize(vecs.size());
  setShared();
  if (meshForm) {
    setNormals();
  } else {
    edgeSplitting();
  }
  this->color = color / 255;
  this->shininess = shininess;
  this->meshType = GLMeshType;
  dumpTris();
  prepareRendering();
  sharedVertexesList.clear();
}

Mesh::~Mesh() { deleteRenderingObjects(); }

Mesh::Triangle::Triangle(int v1, int v2, int v3) {
  this->vertex[0] = v1;
  this->vertex[1] = v2;
  this->vertex[2] = v3;
}

Mesh::Vertex::Vertex(float3 point, float3 normal) {
  this->normal = normal;
  this->point = point;
}

void Mesh::setTNormal(Mesh::Triangle &t) {
  t.normal = cross(vecs[t.vertex[1]].point - vecs[t.vertex[0]].point,
                   vecs[t.vertex[2]].point - vecs[t.vertex[0]].point);
}

void Mesh::setAllTNormal() {
  for (Mesh::Triangle &t : tris) {
    setTNormal(t);
  }
}

void Mesh::dumpTris() {
  dumpedTris.reserve(tris.size() * 3);
  for (Mesh::Triangle t : tris) {
    dumpedTris.push_back(t.vertex[0]);
    dumpedTris.push_back(t.vertex[1]);
    dumpedTris.push_back(t.vertex[2]);
  }
}

float3 Mesh::returnNormal(Mesh::Triangle &t) {
  return cross(vecs[t.vertex[1]].point - vecs[t.vertex[0]].point,
               vecs[t.vertex[2]].point - vecs[t.vertex[0]].point);
}

void Mesh::setShared() {
  for (int j = 0; j < tris.size(); j++) {
    for (int k = 0; k < 3; k++) {
      int idx = tris[j].vertex[k];
      sharedVertexesList[idx].push_back({j, k});
    }
  }
}

void Mesh::setNormals() {
  for (int i = 0; i < vecs.size(); i++) {
    float3 v;
    int listSize = sharedVertexesList[i].size();
    for (int j = 0; j < listSize; j++) {
      v += tris[sharedVertexesList[i][j].first].normal;
      v.w = 0;
    }
    v /= listSize;
    vecs[i].normal = v;
  }
}

void Mesh::edgeSplitting() {
  std::vector<Mesh::Vertex> newVertexList;
  std::vector<std::pair<float3, int>> compareNormalsList;

  int currentGroupLength;
  int totalLength = 0;
  int currentGroupCursor;
  int vertexTotalGroupsLength;
  float3 groupAverageNormal;
  float3 compareNormal;

  std::pair<int, int> sharedHelper;

  for (int sharedIndex = 0; sharedIndex < sharedVertexesList.size();
       sharedIndex++) {

    float3 first = vecs[sharedIndex].point;

    totalLength++;
    vertexTotalGroupsLength = 1;
    currentGroupLength = 1;
    currentGroupCursor = totalLength - 1;

    sharedHelper = sharedVertexesList[sharedIndex][0];

    tris[sharedHelper.first].vertex[sharedHelper.second] = currentGroupCursor;
    groupAverageNormal = tris[sharedHelper.first].normal;
    compareNormal = tris[sharedHelper.first].normal;
    compareNormalsList.push_back({compareNormal, 1});
    newVertexList.push_back(Mesh::Vertex(first, groupAverageNormal));

    for (int currentSharedIndex = 0;
         currentSharedIndex < sharedVertexesList[sharedIndex].size();
         currentSharedIndex++) {

      sharedHelper = sharedVertexesList[sharedIndex][currentSharedIndex];

      float3 iterationNormal = tris[sharedHelper.first].normal;

      if (dot(iterationNormal, compareNormal) < 0.5f) {

        bool thereIsAGroup = false;

        for (int index = compareNormalsList.size() - 1;
             index >=
             std::max((int)compareNormalsList.size() - vertexTotalGroupsLength,
                      0);
             index--) {

          auto bucketNormal = compareNormalsList[index];

          if (dot(iterationNormal, bucketNormal.first) >= 0.5f) {

            tris[sharedHelper.first].vertex[sharedHelper.second] = index;

            newVertexList[index].normal = compareNormalsList[index].first *
                                              compareNormalsList[index].second +
                                          tris[sharedHelper.first].normal;

            normalise(newVertexList[index].normal);

            compareNormalsList[index].second++;

            thereIsAGroup = true;
            break;
          }
        }
        if (!thereIsAGroup) {

          totalLength++;
          currentGroupCursor = totalLength - 1;
          currentGroupLength = 1;
          vertexTotalGroupsLength++;

          tris[sharedHelper.first].vertex[sharedHelper.second] =
              currentGroupCursor;
          groupAverageNormal = iterationNormal;
          compareNormalsList.push_back({iterationNormal, 1});
          newVertexList.push_back(Mesh::Vertex(first, groupAverageNormal));
        }

      } else {

        tris[sharedHelper.first].vertex[sharedHelper.second] =
            currentGroupCursor;
        groupAverageNormal = groupAverageNormal * currentGroupLength +
                             tris[sharedHelper.first].normal;
        normalise(groupAverageNormal);
        newVertexList[currentGroupCursor].normal = groupAverageNormal;
        compareNormalsList.back().second++;
        currentGroupLength++;
      }
    }
  }

  vecs = newVertexList;
}

void Mesh::setVBO() { mVBO.set((GLfloat *)vecs.data(), vecs.size()); }

void Mesh::setEBO() {
  mEBO.set((GLint *)dumpedTris.data(), dumpedTris.size(), meshType);
}

void Mesh::prepareRendering() {
  mVAO.bind();
  setVBO();
  mVBO.bind();
  setEBO();
  mEBO.bind();
  mVAO.linkVBO(mVBO, 0);
  mVAO.unbind();
  mVBO.unbind();
  mEBO.unbind();
}

void Mesh::deleteRenderingObjects() {
  mVAO.exclude();
  mVBO.exclude();
  mEBO.exclude();
}

Mesh Mesh::cube(float3 origin, float size, float3 color, int shininess,
                int meshType) {
  float h = size / 2;
  std::vector<Vertex> vecs = {
      Vertex({origin.x - h, origin.y - h, origin.z + h, 1}, {}),
      Vertex({origin.x + h, origin.y - h, origin.z + h, 1}, {}),
      Vertex({origin.x + h, origin.y + h, origin.z + h, 1}, {}),
      Vertex({origin.x - h, origin.y + h, origin.z + h, 1}, {}),
      Vertex({origin.x - h, origin.y - h, origin.z - h, 1}, {}),
      Vertex({origin.x + h, origin.y - h, origin.z - h, 1}, {}),
      Vertex({origin.x + h, origin.y + h, origin.z - h, 1}, {}),
      Vertex({origin.x - h, origin.y + h, origin.z - h, 1}, {})};

  std::vector<Triangle> tris = {
      Triangle(0, 2, 1), Triangle(0, 3, 2), Triangle(5, 7, 4),
      Triangle(5, 6, 7), Triangle(4, 3, 0), Triangle(4, 7, 3),
      Triangle(1, 6, 5), Triangle(1, 2, 6), Triangle(3, 6, 2),
      Triangle(3, 7, 6), Triangle(4, 1, 5), Triangle(4, 0, 1)};

  return Mesh(vecs, tris, color, shininess, meshType, meshStuff::SHARP_MESH);
}

void Mesh::drawMesh(GLint colorLoc, GLint shineLoc, GLint modelLoc,
                    Entity *entitySceneData, bool backCulling) {
  glUniform4f(colorLoc, this->color.x, this->color.y, this->color.z,
              this->color.w);
  glUniform1f(shineLoc, this->shininess);
  mVAO.bind();
  glDrawElements(GL_TRIANGLES, dumpedTris.size(), GL_UNSIGNED_INT, (void *)0);
  mVAO.unbind();
}
