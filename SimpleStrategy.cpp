/**
 * Authors:
 * Damian Dyńdo
 * Mikołaj Nowak
*/

#include "SimpleStrategy.h"
#include "Path.h"

SimpleStrategy::SimpleStrategy(const Game& game) : Strategy(game) {
    int heroNumber = _game.state.next_hero_index;
    Tile playerMine;

    switch(heroNumber) {
        case 0: playerMine = MINE1; break;
        case 1: playerMine = MINE2; break;
        case 2: playerMine = MINE3; break;
        case 3: playerMine = MINE4; break;
        default: playerMine = UNKNOWN; break;
    }

    for(Tile t: {MINE, MINE1, MINE2, MINE3, MINE4}) {
        if(t != playerMine) {
            _goal.push_back(t);
        }
    }
    _scheduledUpdate = false;
}

Direction SimpleStrategy::getMove() {
    int heroNumber = _game.state.next_hero_index;
    if(_scheduledUpdate) {
        _newGoal();
        _scheduledUpdate = false;
    }

    Path::PathType path = Path::getPath(_game.state, _game.state.heroes[heroNumber].position, _goal);

    if(path.size() == 0) {
        _scheduledUpdate = true;
        return STAY;
    } else if(path.size() == 1) {
        _scheduledUpdate = true;
    }

    return Path::getDirection(_game.state.heroes[heroNumber].position, path.front());
}

void SimpleStrategy::_newGoal() {
    _goal.clear();
    int heroNumber = _game.state.next_hero_index;
    Tile playerMine;

    switch(heroNumber) {
        case 0: playerMine = MINE1; break;
        case 1: playerMine = MINE2; break;
        case 2: playerMine = MINE3; break;
        case 3: playerMine = MINE4; break;
        default: playerMine = UNKNOWN; break;
    }

    int health = _game.state.heroes[heroNumber].life;
    if(health < 50) {
        _goal.push_back(TAVERN);
    } else {
        for(Tile t: {MINE, MINE1, MINE2, MINE3, MINE4}) {
            if(t != playerMine) {
                _goal.push_back(t);
            }
        }
    }
}

