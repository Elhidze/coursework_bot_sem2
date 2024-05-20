#pragma once
#include <string>
#include <vector>
#include "player.h"
#include "game_state.h"

class Agent : public Player {
    std::string _name;
    Mark _mark;
    Mark _opponent_mark;

public:
    Agent(const std::string& name) : _name(name) {}

    std::string get_name() const override { return _name; }
    void assign_mark(Mark player_mark) override;
    Point play(const GameView& game) override;
    void notify(const GameView&, const Event&) override {}

private:
    Point find_best_move(const GameView& game);
    int evaluate_move(const GameView& game, const Point& move);
    int count_line_length(const GameState& state, const Point& start, const Point& direction, Mark mark, const Boundary& b);
    bool is_within_bounds(const Point& point, const Boundary& b);
};
