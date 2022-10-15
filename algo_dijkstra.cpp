#pragma once

#include "tile.hpp"
namespace dijkstra
{
Tile *currentTile = nullptr;
void solve(Tile *grid[50][50]) {
  if (currentTile != nullptr)
  {
    for (int i=0; i<50; i++)
    {
      for (int j=0; j<50; j++)
      {
        if (grid[i][j]->getState() == Tile::states::start) currentTile = grid[i][j];
        currentTile->setState(Tile::states::visited);
        break;
      }
    }
  }
  
  // int currentCoords[2] = { currentTile->getCoords(0), currentTile->getCoords(1) };
  // grid[currentTile->getCoords(1)+1][currentTile->getCoords(0)]->setState(Tile::visited);
  // currentTile = grid[currentCoords[0]+1][currentCoords[1]];
}
}
