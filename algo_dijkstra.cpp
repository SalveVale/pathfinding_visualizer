// #pragma once

// #include <vector>

// #include "tile.hpp"

// namespace dijkstra
// {
// std::vector<Tile> solveDijkstra(std::vector<Tile> unvisitedTiles, Tile *grid[50][50]) {
//     std::vector<Tile> neighborTiles;
//     for (int i=0; i<unvisitedTiles.size(); i++)
//     {
//       Tile *checkedTile = &unvisitedTiles[i];

//       int xCoord = checkedTile->getCoords(0);
//       int yCoord = checkedTile->getCoords(1);
      
//       if (yCoord > 0)
//       {
//         Tile *up = grid[xCoord][yCoord - 1];
//         switch (up->getState())
//         {
//           case Tile::states::end:
//             this->stateEngine.setState(StateEngine::solved, this->window);
//             break;
//           case Tile::states::empty:
//             up->setValue(checkedTile->getValue() + 1);
//             // up->setPrevTile(checkedTile);
//             neighborTiles.push_back(*up);
//             this->visitedTilesInOrder.push_back(*up);
//             break;
//           default:
//             break;
//         }
//       }

//       if (xCoord > 0)
//       {
//         Tile *left = grid[xCoord - 1][yCoord];
//         switch (left->getState())
//         {
//           case Tile::states::end:
//             this->stateEngine.setState(StateEngine::solved, this->window);
//             break;
//           case Tile::states::empty:
//             left->setValue(checkedTile->getValue() + 1);
//             neighborTiles.push_back(*left);
//             this->visitedTilesInOrder.push_back(*left);
//             break;
//           default:
//             break;
//         }
//       }

//       if (xCoord < 50)
//       {
//         Tile *right = grid[xCoord + 1][yCoord];
//         switch (right->getState())
//         {
//           case Tile::states::end:
//             this->stateEngine.setState(StateEngine::solved, this->window);
//             break;
//           case Tile::states::empty:
//             right->setValue(checkedTile->getValue() + 1);
//             neighborTiles.push_back(*right);
//             this->visitedTilesInOrder.push_back(*right);
//             break;
//           default:
//             break;
//         }
//       }

//       if (yCoord < 50)
//       {
//         Tile *down = grid[xCoord][yCoord + 1];
//         switch (down->getState())
//         {
//           case Tile::states::end:
//             this->stateEngine.setState(StateEngine::solved, this->window);
//             break;
//           case Tile::states::empty:
//             down->setValue(checkedTile->getValue() + 1);
//             neighborTiles.push_back(*down);
//             this->visitedTilesInOrder.push_back(*down);
//             break;
//           default:
//             break;
//         }
//       }

//     }
    
//     this->unvisitedTiles = neighborTiles;
    
//     // if (this->stateEngine.state != StateEngine::solved) this->solveDijkstra();
//     this->stateEngine.setState(StateEngine::solved, this->window);
//   }
// }