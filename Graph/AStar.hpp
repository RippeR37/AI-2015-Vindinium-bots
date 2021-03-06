/**
 * Authors:
 * Damian Dyńdo
 * Mikołaj Nowak
*/

#ifndef GRAPH_ASTAR_HPP_INCLUDED
#define GRAPH_ASTAR_HPP_INCLUDED

#include <map>
#include <set>
#include <list>
#include <vector>
#include <functional>

#include "NodeAdapter.hpp"
#include "GraphAdapter.hpp"


namespace Graph {

    template<typename _MapType, typename _PositionType, typename _CostType = int>
    class AStar {
        public:
            typedef _MapType                        MapType;
            typedef _PositionType                   PositionType;
            typedef _CostType                       CostType;
            typedef std::list<PositionType>         PathType;

            typedef NodeAdapter<PositionType, CostType> NodeAdapterType;
            typedef GraphAdapter<MapType, PositionType, CostType> GraphAdapterType;


        public:
            AStar() {

            };

            PathType getPath(const GraphAdapterType& graphAdapter, const NodeAdapterType& start, const std::function<bool(const NodeAdapterType&)>& endCondition) const {
                PathType resultPath;

                struct NodePositionComparator {
                    bool operator() (const NodeAdapterType& lhs, const NodeAdapterType& rhs) {
                        return lhs.position < rhs.position;
                    }
                };

                std::set<NodeAdapterType> open = { start };
                std::set<NodeAdapterType, NodePositionComparator> closed;
                std::map<NodeAdapterType, NodeAdapterType, NodePositionComparator> came_from;
                typename std::set<NodeAdapterType>::iterator current;

                while(open.empty() == false) {
                    current = open.begin();

                    if(endCondition(*current) == true) {
                        // Goal found, recreating path from 'goal' node to 'start' node
                        resultPath.push_front(current->position);
                        auto pathCurrent = came_from.find(*current);
                        if(pathCurrent != came_from.end()) {
                            while(pathCurrent->second.position != start.position) {
                                resultPath.push_front(pathCurrent->second.position);
                                pathCurrent = came_from.find(pathCurrent->second);
                            }
                        }

                        return resultPath;
                    }

                    closed.insert(*current);

                    std::vector<NodeAdapterType> neighbours = graphAdapter.getNeighboursOf(*current);
                    for(NodeAdapterType& neighbour : neighbours) {
                        if(graphAdapter.isAvailable(neighbour.position)) {
                            typename std::set<NodeAdapterType>::iterator cIter, oIter;

                            cIter = closed.find(neighbour);
                            if(cIter != closed.end()) {
                                continue;
                            }

                            neighbour.g(current->g() + 1);
                            neighbour.h(graphAdapter.getHeuristicCostLeft(neighbour, neighbour));

                            oIter = open.find(neighbour);
                            if(oIter == open.end() || neighbour.g() < oIter->g()) {
                                if(oIter != open.end()) {
                                    open.erase(oIter);
                                }

                                auto cameFromIter = came_from.find(neighbour);
                                if(cameFromIter != came_from.end()) {
                                    if(cameFromIter->second.g() > neighbour.g()) {
                                        came_from.erase(cameFromIter);
                                    }
                                }
                                came_from.emplace(std::pair<NodeAdapterType, NodeAdapterType>(neighbour, *current));

                                open.insert(neighbour);
                            }
                        }
                    }

                    open.erase(current);
                }

                // If algorithm comes here, no path was found, and return value of this method will be empty vector

                return resultPath;
            }

            PathType getPath(const GraphAdapterType& graphAdapter, const NodeAdapterType& start, const NodeAdapterType& goal) const {
                PathType resultPath;

                struct NodePositionComparator {
                    bool operator() (const NodeAdapterType& lhs, const NodeAdapterType& rhs) {
                        return lhs.position < rhs.position;
                    }
                };

                std::set<NodeAdapterType> open = { start };
                std::set<NodeAdapterType, NodePositionComparator> closed;
                std::map<NodeAdapterType, NodeAdapterType, NodePositionComparator> came_from;
                typename std::set<NodeAdapterType>::iterator current;

                while(open.empty() == false) {
                    current = open.begin();

                    if(current->position == goal.position) {
                        // Goal found, recreating path from 'goal' node to 'start' node
                        resultPath.push_front(current->position);
                        auto pathCurrent = came_from.find(*current);
                        if(pathCurrent != came_from.end()) {
                            while(pathCurrent->second.position != start.position) {
                                resultPath.push_front(pathCurrent->second.position);
                                pathCurrent = came_from.find(pathCurrent->second);
                            }
                        }

                        return resultPath;
                    }

                    closed.insert(*current);

                    std::vector<NodeAdapterType> neighbours = graphAdapter.getNeighboursOf(*current);
                    for(NodeAdapterType& neighbour : neighbours) {
                        if(graphAdapter.isAvailable(neighbour.position)) {
                            typename std::set<NodeAdapterType>::iterator cIter, oIter;

                            cIter = closed.find(neighbour);
                            if(cIter != closed.end()) {
                                continue;
                            }
                            
                            neighbour.g(current->g() + 1);
                            neighbour.h(graphAdapter.getHeuristicCostLeft(neighbour, goal));

                            oIter = open.find(neighbour);
                            if(oIter == open.end() || neighbour.g() < oIter->g()) {
                                if(oIter != open.end()) {
                                    open.erase(oIter);
                                }

                                auto cameFromIter = came_from.find(neighbour);
                                if(cameFromIter != came_from.end()) {
                                    if(cameFromIter->second.g() > neighbour.g()) {
                                        came_from.erase(cameFromIter);
                                    }
                                }
                                came_from.emplace(std::pair<NodeAdapterType, NodeAdapterType>(neighbour, *current));

                                open.insert(neighbour);
                            }
                        }
                    }

                    open.erase(current);
                }

                // If algorithm comes here, no path was found, and return value of this method will be empty vector

                return resultPath;
            }
    };

}

#endif
