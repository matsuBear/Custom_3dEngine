#pragma once
#include "worldObject.hpp"
#include <array>
#include <cstddef>
#include <utility>

class BaseMemPool {
public:
  virtual ~BaseMemPool() = default;
  TypeId poolTypeId = 0;
};

template <typename Component, std::size_t Capacity>
class memPool : public BaseMemPool {
public:
  memPool<Component, Capacity>() {
    this->poolTypeId = getComponentTypeId<Component>();
  }

  std::array<Component, Capacity> data;
  size_t active = 0;

  void create(const Component &temp) {
    data[active] = temp;
    active++;
  }

  void popAndSwitch(size_t index) {
    if (index >= active)
      return;
    active--;
    data[index] = data[active];
  }
};
