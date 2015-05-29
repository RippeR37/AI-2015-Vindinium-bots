#pragma once

#include <vector>

#include "Strategy.h"

class SimpleStrategy : public Strategy {
    public:
        SimpleStrategy(Game& game);
        Direction getMove();
    private:
        void _newGoal();
        std::vector<Tile> _goal;
};
