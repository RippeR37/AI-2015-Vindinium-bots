/**
 * Authors:
 * Damian Dyńdo
 * Mikołaj Nowak
*/

#include "Path.h"
#include "Graph/AStar.hpp"

#include <cmath>
#include <climits>
#include <algorithm>

/*** Typedefs ***/
typedef Graph::GraphAdapter<State, Position, double> BaseGraphAdapter;

/*** Simple A* graph adapter for getPath(state, start, end) method ***/
class SimpleMapAdapter : public BaseGraphAdapter {
    public:
        SimpleMapAdapter(const State& state, const Position& goal) : BaseGraphAdapter(state), _goal(goal) {

        }

        bool isAvailable(const Position& position) const {
            bool isAvailable = false;

            if(position == _goal)
                isAvailable = true;
            else if(_graph.get_tile_from_background_border_check(position) == Tile::EMPTY)
                isAvailable = true;

            return isAvailable;
        }

        std::vector<NodeAdapterType> getNeighboursOf(const NodeAdapterType& node) const {
            std::vector<NodeAdapterType> neighbours;

            std::vector<std::pair<int, int>> posDiffs = {
                {  0, -1 }, {  0,  1 }, { -1,  0 }, {  1,  0 }
            };

            for(const std::pair<int, int>& posDiff : posDiffs) {
                neighbours.push_back(
                    NodeAdapterType(
                        Position(
                            node.position.x + posDiff.first,
                            node.position.y + posDiff.second
                        )
                    )
                );
            }

            return neighbours;
        }

        virtual double getHeuristicCostLeft(const NodeAdapterType& currentNode, const NodeAdapterType& goal) const {
            double dx, dy;

            dx = currentNode.position.x - goal.position.x;
            dy = currentNode.position.y - goal.position.y;

            return std::sqrt(dx*dx + dy*dy);
        }

    private:
        const Position& _goal;

};

/*** Advanced A* graph adapter for getPath(state, start, tileTypes) method ***/
class AdvancedMapAdapter : public BaseGraphAdapter {
    public:
        AdvancedMapAdapter(const State& state, const std::vector<Tile>& goalTypes, const std::vector<Tile>& avoidTypes)
                : BaseGraphAdapter(state), _tileTypes(goalTypes), _avoidTypes(avoidTypes) {

        }

        bool isAvailable(const Position& position) const {
            bool isAvailable = false;
            Tile tileOnPosition;

            tileOnPosition = _graph.get_tile_from_background_border_check(position);
            if(tileOnPosition == Tile::EMPTY)
                isAvailable = true;

            for(unsigned int i = 0; i < _tileTypes.size(); ++i) {
                if(tileOnPosition == _tileTypes[i]) {
                    isAvailable = true;
                    break;
                }
            }

            return isAvailable;
        }

        std::vector<NodeAdapterType> getNeighboursOf(const NodeAdapterType& node) const {
            std::vector<NodeAdapterType> neighbours;

            std::vector<std::pair<int, int>> posDiffs = {
                {  0, -1 }, {  0,  1 }, { -1,  0 }, {  1,  0 }
            };

            for(const std::pair<int, int>& posDiff : posDiffs) {
                neighbours.push_back(
                    NodeAdapterType(
                        Position(
                            node.position.x + posDiff.first,
                            node.position.y + posDiff.second
                        )
                    )
                );
            }

            return neighbours;
        }

        double getHeuristicCostLeft(const NodeAdapterType& currentNode, const NodeAdapterType& currentNodeCpy) const {
            static std::vector<PositionType> tavernPositions = _getTavernPositions();
            static std::vector<PositionType> minesPositions = _getMinesPositions();

            double result = INT_MAX;
            PositionType position;
            PositionType currentPosition = currentNode.position;
            double distance, diffx, diffy;

            for(Tile t: _tileTypes) {
                switch(t) {
                    case HERO1:
                        position = _graph.heroes[0].position;
                        diffx = currentPosition.x - position.x;
                        diffy = currentPosition.y - position.y;
                        distance = diffx * diffx + diffy * diffy;
                        result = std::min(result, distance);
                        break;
                    case HERO2:
                        position = _graph.heroes[1].position;
                        diffx = currentPosition.x - position.x;
                        diffy = currentPosition.y - position.y;
                        distance = diffx * diffx + diffy * diffy;
                        result = std::min(result, distance);
                        break;
                    case HERO3:
                        position = _graph.heroes[2].position;
                        diffx = currentPosition.x - position.x;
                        diffy = currentPosition.y - position.y;
                        distance = diffx * diffx + diffy * diffy;
                        result = std::min(result, distance);
                        break;
                    case HERO4:
                        position = _graph.heroes[3].position;
                        diffx = currentPosition.x - position.x;
                        diffy = currentPosition.y - position.y;
                        distance = diffx * diffx + diffy * diffy;
                        result = std::min(result, distance);
                        break;
                    case TAVERN:
                        for(PositionType pos: tavernPositions) {
                            diffx = currentPosition.x - pos.x;
                            diffy = currentPosition.y - pos.y;
                            distance = diffx * diffx + diffy * diffy;
                            result = std::min(result, distance);
                        }
                        break;
                    case MINE:
                        for(PositionType pos: minesPositions) {
                            bool owned = false;
                            for(int i = 0; i < 4; ++i) {
                                owned = (owned || (_graph.heroes[i].mine_positions.find(pos) != _graph.heroes[i].mine_positions.end()));
                            }
                            if(!owned) {
                                diffx = currentPosition.x - pos.x;
                                diffy = currentPosition.y - pos.y;
                                distance = diffx * diffx + diffy * diffy;
                                result = std::min(result, distance);
                            }
                        }
                        break;
                    case MINE1:
                        for(PositionType pos: _graph.heroes[0].mine_positions) {
                            diffx = currentPosition.x - pos.x;
                            diffy = currentPosition.y - pos.y;
                            distance = diffx * diffx + diffy * diffy;
                            result = std::min(result, distance);
                        }
                        break;
                    case MINE2:
                        for(PositionType pos: _graph.heroes[1].mine_positions) {
                            diffx = currentPosition.x - pos.x;
                            diffy = currentPosition.y - pos.y;
                            distance = diffx * diffx + diffy * diffy;
                            result = std::min(result, distance);
                        }
                        break;
                    case MINE3:
                        for(PositionType pos: _graph.heroes[2].mine_positions) {
                            diffx = currentPosition.x - pos.x;
                            diffy = currentPosition.y - pos.y;
                            distance = diffx * diffx + diffy * diffy;
                            result = std::min(result, distance);
                        }
                        break;
                    case MINE4:
                        for(PositionType pos: _graph.heroes[3].mine_positions) {
                            diffx = currentPosition.x - pos.x;
                            diffy = currentPosition.y - pos.y;
                            distance = diffx * diffx + diffy * diffy;
                            result = std::min(result, distance);
                        }
                        break;
                    default: break;
                }
            }

            result = getCostWithAvoidance(currentNode, result);

            return ( (result == INT_MAX) ? 0 : result );
        }

        double getCostWithAvoidance(const NodeAdapterType& currentNode, double oldResult) const {
            static std::vector<PositionType> tavernPositions = _getTavernPositions();
            static std::vector<PositionType> minesPositions = _getMinesPositions();

            double newResult = oldResult;
            PositionType position;
            PositionType currentPosition = currentNode.position;
            int diffx, diffy;

            int maxDXDY = 2;

            for(Tile t: _avoidTypes) {
                switch(t) {
                    case HERO1:
                        position = _graph.heroes[0].position;
                        diffx = std::abs(currentPosition.x - position.x);
                        diffy = std::abs(currentPosition.y - position.y);
                        if(diffx + diffy <= maxDXDY) {
                            newResult = oldResult + 99999.0;
                        }
                        break;
                    case HERO2:
                        position = _graph.heroes[1].position;
                        diffx = std::abs(currentPosition.x - position.x);
                        diffy = std::abs(currentPosition.y - position.y);
                        if(diffx + diffy <= maxDXDY) {
                            newResult = oldResult + 99999.0;
                        }
                        break;
                    case HERO3:
                        position = _graph.heroes[2].position;
                        diffx = std::abs(currentPosition.x - position.x);
                        diffy = std::abs(currentPosition.y - position.y);
                        if(diffx + diffy <= maxDXDY) {
                            newResult = oldResult + 99999.0;
                        }
                        break;
                    case HERO4:
                        position = _graph.heroes[3].position;
                        diffx = std::abs(currentPosition.x - position.x);
                        diffy = std::abs(currentPosition.y - position.y);
                        if(diffx + diffy <= maxDXDY) {
                            newResult = oldResult + 99999.0;
                        }
                        break;
                    case TAVERN:
                        for(PositionType pos: tavernPositions) {
                            diffx = std::abs(currentPosition.x - pos.x);
                            diffy = std::abs(currentPosition.y - pos.y);
                            if(diffx + diffy <= maxDXDY) {
                                newResult = oldResult + 99999.0;
                            }
                        }
                        break;
                    case MINE:
                        for(PositionType pos: minesPositions) {
                            bool owned = false;
                            for(int i = 0; i < 4; ++i) {
                                owned = (owned || (_graph.heroes[i].mine_positions.find(pos) != _graph.heroes[i].mine_positions.end()));
                            }
                            if(!owned) {
                                diffx = std::abs(currentPosition.x - pos.x);
                                diffy = std::abs(currentPosition.y - pos.y);
                                if(diffx + diffy <= maxDXDY) {
                                    newResult = oldResult + 99999.0;
                                }
                            }
                        }
                        break;
                    case MINE1:
                        for(PositionType pos: _graph.heroes[0].mine_positions) {
                            diffx = std::abs(currentPosition.x - pos.x);
                            diffy = std::abs(currentPosition.y - pos.y);
                            if(diffx + diffy <= maxDXDY) {
                                newResult = oldResult + 99999.0;
                            }
                        }
                        break;
                    case MINE2:
                        for(PositionType pos: _graph.heroes[1].mine_positions) {
                            diffx = std::abs(currentPosition.x - pos.x);
                            diffy = std::abs(currentPosition.y - pos.y);
                            if(diffx + diffy <= maxDXDY) {
                                newResult = oldResult + 99999.0;
                            }
                        }
                        break;
                    case MINE3:
                        for(PositionType pos: _graph.heroes[2].mine_positions) {
                            diffx = std::abs(currentPosition.x - pos.x);
                            diffy = std::abs(currentPosition.y - pos.y);
                            if(diffx + diffy <= maxDXDY) {
                                newResult = oldResult + 99999.0;
                            }
                        }
                        break;
                    case MINE4:
                        for(PositionType pos: _graph.heroes[3].mine_positions) {
                            diffx = std::abs(currentPosition.x - pos.x);
                            diffy = std::abs(currentPosition.y - pos.y);
                            if(diffx + diffy <= maxDXDY) {
                                newResult = oldResult + 99999.0;
                            }
                        }
                        break;
                    default: break;
                }
            }

            return newResult;
        }


    private:
        std::vector<PositionType> _getTavernPositions() const {
            int mapSize = _graph.get_tiles_full().shape()[0];
            std::vector<PositionType> positions;
            for(int i = 0; i < mapSize; ++i) {
                for(int j = 0; j < mapSize; ++j) {
                    PositionType position(i, j);
                    if(_graph.get_tile_from_background(position) == TAVERN) {
                        positions.push_back(position);
                    }
                }
            }
            return positions;
        }

        std::vector<PositionType> _getMinesPositions() const {
            int mapSize = _graph.get_tiles_full().shape()[0];
            std::vector<PositionType> positions;
            for(int i = 0; i < mapSize; ++i) {
                for(int j = 0; j < mapSize; ++j) {
                    PositionType position(i, j);
                    Tile t = _graph.get_tile_from_background(position);
                    if(t == MINE || t == MINE1 || t == MINE2 || t == MINE3 || t == MINE4) {
                        positions.push_back(position);
                    }
                }
            }
            return positions;
        }

        const std::vector<Tile>& _tileTypes;
        const std::vector<Tile>& _avoidTypes;
};


Path::PathType Path::getPath(const State& state, const Position& start, const Position& end) {
    PathType result;
    Graph::AStar<State, Position, double> myAStar;
    SimpleMapAdapter myMapAdapter(state, end);
    SimpleMapAdapter::NodeAdapterType nodeStart(start);
    SimpleMapAdapter::NodeAdapterType nodeGoal(end);

    result = myAStar.getPath(myMapAdapter, nodeStart, nodeGoal);

    return result;
}


Path::PathType Path::getPath(const State& state, const Position& start, Tile tileType) {
    PathType result;
    std::vector<Tile> tileTypes;
    tileTypes.push_back(tileType);

    result = getPath(state, start, tileTypes);

    return result;
}


Path::PathType Path::getPath(const State& state, const Position& start, const std::vector<Tile>& tileTypes) {
    std::vector<Tile> avoidTypes; // empty
    PathType result;
    Graph::AStar<State, Position, double> myAStar;
    AdvancedMapAdapter myMapAdapter(state, tileTypes, avoidTypes);
    AdvancedMapAdapter::NodeAdapterType nodeStart(start);

    std::function<bool(const AdvancedMapAdapter::NodeAdapterType&)> endCondition = [&](const AdvancedMapAdapter::NodeAdapterType& currentNode) -> bool {
        bool isGoal = false;
        Tile tileType;

        tileType = state.get_tile_from_background_border_check(currentNode.position);
        for(unsigned int i = 0; i < tileTypes.size(); ++i) {
            if(tileTypes[i] == tileType) {
                isGoal = true;
                break;
            }
        }

        return isGoal;
    };

    result = myAStar.getPath(myMapAdapter, nodeStart, endCondition);

    return result;
}

Path::PathType Path::getPath(const State& state, const Position& start, const std::vector<Tile>& goalTypes, const std::vector<Tile>& avoidTypes) {
    PathType result;
    Graph::AStar<State, Position, double> myAStar;
    AdvancedMapAdapter myMapAdapter(state, goalTypes, avoidTypes);
    AdvancedMapAdapter::NodeAdapterType nodeStart(start);

    std::function<bool(const AdvancedMapAdapter::NodeAdapterType&)> endCondition = [&](const AdvancedMapAdapter::NodeAdapterType& currentNode) -> bool {
        bool isGoal = false;
        Tile tileType;

        tileType = state.get_tile_from_background_border_check(currentNode.position);
        for(unsigned int i = 0; i < goalTypes.size(); ++i) {
            if(goalTypes[i] == tileType) {
                isGoal = true;
                break;
            }
        }

        return isGoal;
    };

    result = myAStar.getPath(myMapAdapter, nodeStart, endCondition);

    return result;
}


Direction Path::getDirection(const Position& pos1, const Position& pos2) {
    Direction result = Direction::STAY;

    // Be advised - game uses bed signatures for x/y and EW/SN, so it may not look logical here
    if(pos2.x > pos1.x) {
        result = Direction::SOUTH;
    } else if(pos2.x < pos1.x) {
        result = Direction::NORTH;
    } else {
        if(pos2.y > pos1.y) {
            result = Direction::EAST;
        } else if(pos2.y < pos1.y) {
            result = Direction::WEST;
        }
    }

    return result;
}
