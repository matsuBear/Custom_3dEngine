#pragma once
#include "float3.hpp"
#include "worldObject.hpp"
#pragma once
class Model;

class Model : public Component, public Matrix4x4 {};
