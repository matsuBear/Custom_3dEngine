
#include "camera.hpp"
#include "eventHandler.hpp"
#include "float3.hpp"
#include "light.hpp"
#include "mesh.hpp"
#include "renderCache.hpp"
#include "scene.hpp"
#include "shader.hpp"
#include "worldObject.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <iostream>

int main() {

  Window w = Window("HEY LISTEN!", 1000, 800);

  Scene s(1000, 1200);

  Shader shaderProgram("shaders/vertex.glsl", "shaders/fragment.glsl");

  RenderCache rendC{};
  rendC.scenePtr = &s;

  InputHandler i;

  Entity camEntity = Entity();
  s.entityList.push_back(&camEntity);
  Camera camComponent = Camera(&camEntity);
  s.cam = &camComponent;

  Entity lightEntity = Entity();
  s.entityList.push_back(&lightEntity);
  OrbitalLight l1 = OrbitalLight(&lightEntity, float3(0.0f, 0.0f, 10, 1),
                                 getColor(255, 0, 25, 255));
  s.lightList.push_back(&l1);
  s.currentLight = s.lightList[0];

  Entity cubeEntity = Entity();
  s.entityList.push_back(&cubeEntity);
  Mesh c1 = Mesh::cube(s.worldOrigin, 2, float3(35, 107, 199, 255), 64,
                       GL_STATIC_DRAW);
  c1.setOwner(&cubeEntity);
  s.meshList.push_back(&c1);
  s.currentMesh = s.meshList[0];

  Mesh c2 = Mesh::cube(s.worldOrigin, 0.4, l1.color, 64, GL_STATIC_DRAW);
  c2.setOwner(&lightEntity);
  s.meshList.push_back(&c2);

  l1.assignCenter(&cubeEntity.pos);

  rendC.buildLightCaches();

  while (w.running) {

    s.updateTime();

    i.poll(w, s);

    w.erase();
    shaderProgram.activate();

    GLint projLoc = glGetUniformLocation(shaderProgram.id, "uProjection");
    GLint viewLoc = glGetUniformLocation(shaderProgram.id, "uView");
    GLint modelLoc = glGetUniformLocation(shaderProgram.id, "uModel");
    GLint lightPosLoc = glGetUniformLocation(shaderProgram.id, "lightPos");
    GLint lightColorLoc = glGetUniformLocation(shaderProgram.id, "lightColorL");
    GLint camPosLoc = glGetUniformLocation(shaderProgram.id, "camPos");
    GLint ambientLoc =
        glGetUniformLocation(shaderProgram.id, "ambientStrength");
    GLint colorLoc = glGetUniformLocation(shaderProgram.id, "meshColor");
    GLint shineLoc = glGetUniformLocation(shaderProgram.id, "shininess");
    GLint lightCountLoc = glGetUniformLocation(shaderProgram.id, "lightCount");

    glUniform4fv(lightPosLoc, rendC.lightSize,
                 (const GLfloat *)rendC.lightPosCache.data());
    glUniform4fv(lightColorLoc, rendC.lightSize,
                 (const GLfloat *)rendC.lightColorCache.data());
    glUniform1i(lightCountLoc, rendC.lightSize);
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, s.projection);
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, s.cam->view);
    glUniform4f(camPosLoc, s.cam->owner->pos.x, s.cam->owner->pos.y,
                s.cam->owner->pos.z, s.cam->owner->pos.w);
    glUniform1f(ambientLoc, s.ambientLight);

    w.present();
  }

  if (i.quit) {
    SDL_Quit();
  }

  return 0;
}
