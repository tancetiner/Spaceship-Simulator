// from the private repo of Alp Cihan (https://github.com/alpcihan)
#pragma once
#include "GameObj3D.hpp"

bool intersect(GameObj3D& a, GameObj3D& b) {
  return (a.collider.getMinX() <= b.collider.getMaxX() && a.collider.getMaxX() >= b.collider.getMinX()) &&
         (a.collider.getMinY() <= b.collider.getMaxY() && a.collider.getMaxY() >= b.collider.getMinY()) &&
         (a.collider.getMinZ() <= b.collider.getMaxZ() && a.collider.getMaxZ() >= b.collider.getMinZ());
}
