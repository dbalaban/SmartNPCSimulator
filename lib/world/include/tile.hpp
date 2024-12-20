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

class Tile;

typedef std::shared_ptr<Tile> TilePtr;
typedef std::shared_ptr<const Tile> constTilePtr;
typedef std::weak_ptr<Tile> wTilePtr;

class Tile : public Element<Tile> {
  friend class Element<Tile>;
public:
  static const size_t ElementID = 1;
  static const size_t FeatureSize = 9;

  Tile(ResourceManager resources)
    : Element<Tile>(), resources(resources) {}
  ~Tile() = default;

  std::unique_ptr<double[]> getFeatures() const override {
    std::unique_ptr<double[]> features(new double[FeatureSize]);
    features[0] = ElementID;
    features[1] = getInstanceID();
    features[2] = adjacentTiles[0] ? adjacentTiles[0]->getInstanceID() : -1;
    features[3] = adjacentTiles[1] ? adjacentTiles[1]->getInstanceID() : -1;
    features[4] = adjacentTiles[2] ? adjacentTiles[2]->getInstanceID() : -1;
    features[5] = adjacentTiles[3] ? adjacentTiles[3]->getInstanceID() : -1;
    features[6] = resources.resources.kcal;
    features[7] = resources.resourcesPerHour.kcal;
    features[8] = resources.maxResources.kcal;
    return features;
  }

  void addAdjacentTile(const TilePtr& tile) {
    adjacentTiles.push_back(tile);
  }

  const std::vector<TilePtr>& getAdjacentTiles() const {
    return adjacentTiles;
  }

  ResourceManager& getResourceManager() {
    return resources;
  }

  Resources& getResources() {
    return resources.resources;
  }

  const Resources& getResources() const {
    return resources.resources;
  }

  void update(double elapsedTime) override;

private:
  std::vector<TilePtr> adjacentTiles;
  ResourceManager resources;
};

#endif // TILE_HPP