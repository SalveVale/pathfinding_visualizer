#pragma once

#include "tile.hpp"

#include <iostream>

namespace dijkstra
{
Tile *solve(Tile *grid[50][50], Tile *currentTile) {
  if (currentTile == nullptr)
  {
    for (int i=0; i<50; i++)
    {
      for (int j=0; j<50; j++)
      {
        if (grid[i][j]->getState() == Tile::states::start) currentTile = grid[i][j];
        break;
      }
    }
  }
  
  // int currentCoords[2] = { currentTile->getCoords(0), currentTile->getCoords(1) };
  // grid[currentTile->getCoords(0)+1][currentTile->getCoords(1)]->setState(Tile::visited);
  // currentTile = grid[currentCoords[0]+1][currentCoords[1]];

  return currentTile;
}
}
