#ifndef TILE_HPP
#define TILE_HPP

#include <vector>
#include "element.hpp"

struct Resources {
  double kcal;

  // operator overloads
  Resources operator+(const Resources& rhs) const {
    return Resources{kcal + rhs.kcal};
  }

  Resources operator-(const Resources& rhs) const {
    return Resources{kcal - rhs.kcal};
  }

  Resources operator*(const double& rhs) const {
    return Resources{kcal * rhs};
  }

  Resources operator/(const double& rhs) const {
    return Resources{kcal / rhs};
  }

  Resources& operator+=(const Resources& rhs) {
    kcal += rhs.kcal;
    return *this;
  }

  Resources& operator-=(const Resources& rhs) {
    kcal -= rhs.kcal;
    return *this;
  }

  Resources& operator*=(const double& rhs) {
    kcal *= rhs;
    return *this;
  }

  Resources& operator/=(const double& rhs) {
    kcal /= rhs;
    return *this;
  }
};

struct ResourceManager {
  Resources resources;
  const Resources resourcesPerHour;
  const Resources maxResources;

  ResourceManager(Resources resources, Resources resourcesPerHour, Resources maxResources)
    : resources(resources), resourcesPerHour(resourcesPerHour), maxResources(maxResources) {};

  void update(double elapsedTime) {
    resources += resourcesPerHour * elapsedTime;
    if (resources.kcal > maxResources.kcal) {
      resources = maxResources;
    }
  }
};

class Tile : public Element<Tile> {
  friend class Element<Tile>;

  static const size_t ElementID = 0;
public:
  Tile(ResourceManager resources)
    : instanceID(InstanceCount++), resources(resources) {}
  ~Tile() = default;

  void addAdjacentTile(Tile* tile) const {
    adjacentTiles.push_back(tile);
  }

  const std::vector<Tile*>& getAdjacentTiles() const {
    return adjacentTiles;
  }

  const ResourceManager& getResources() const {
    return resources;
  }

  void update(double elapsedTime) override;

  const size_t getInstanceID() const;

private:
  mutable std::vector<Tile*> adjacentTiles;
  const size_t instanceID;
  ResourceManager resources;
};

#endif // TILE_HPP