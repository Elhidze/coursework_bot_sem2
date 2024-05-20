#include "Agent.h"

void Agent::assign_mark(Mark player_mark) {
    _mark = player_mark;
    _opponent_mark = (player_mark == Mark::Cross) ? Mark::Zero : Mark::Cross;
}

Point Agent::play(const GameView& game) {
    return find_best_move(game);
}

Point Agent::find_best_move(const GameView& game) {
    const GameState& state = game.get_state();
    Boundary b = game.get_settings().field_size;
    Point best_move = {0, 0};
    int best_score = -1;

    for (int x = b.min.x; x <= b.max.x; ++x) {
        for (int y = b.min.y; y <= b.max.y; ++y) {
            Point move = {x, y};
            if (state.field->get_value(move) == Mark::None) {
                int score = evaluate_move(game, move);
                if (score > best_score) {
                    best_score = score;
                    best_move = move;
                }
            }
        }
    }
    return best_move;
}

int Agent::evaluate_move(const GameView& game, const Point& move) {
    const GameState& state = game.get_state();
    Boundary b = game.get_settings().field_size;
    GameState hypothetical_state = state;
    hypothetical_state.field->set_value(move, _mark);

    int score = 0;

    // 1) Check if the move wins the game
    if (count_line_length(hypothetical_state, move, {1, 0}, _mark, b) >= 5 ||  // Horizontal
        count_line_length(hypothetical_state, move, {0, 1}, _mark, b) >= 5 ||  // Vertical
        count_line_length(hypothetical_state, move, {1, 1}, _mark, b) >= 5 ||  // Diagonal
        count_line_length(hypothetical_state, move, {1, -1}, _mark, b) >= 5) {  // Diagonal
        return 1000;
    }

    // 2) Check if the move extends the player's line
    score += count_line_length(hypothetical_state, move, {1, 0}, _mark, b);
    score += count_line_length(hypothetical_state, move, {0, 1}, _mark, b);
    score += count_line_length(hypothetical_state, move, {1, 1}, _mark, b);
    score += count_line_length(hypothetical_state, move, {1, -1}, _mark, b);

    // 3) Check if the move blocks the opponent's line
    hypothetical_state.field->set_value(move, _opponent_mark);
    if (count_line_length(hypothetical_state, move, {1, 0}, _opponent_mark, b) >= 4 ||  // Horizontal
        count_line_length(hypothetical_state, move, {0, 1}, _opponent_mark, b) >= 4 ||  // Vertical
        count_line_length(hypothetical_state, move, {1, 1}, _opponent_mark, b) >= 4 ||  // Diagonal
        count_line_length(hypothetical_state, move, {1, -1}, _opponent_mark, b) >= 4) {  // Diagonal
        score += 500;
    }

    // Additional metrics (add 3 more)
    // 4) Center control (higher score for center positions)
    Point center = {(b.max.x - b.min.x) / 2, (b.max.y - b.min.y) / 2};
    score += 40 - (abs(move.x - center.x) + abs(move.y - center.y));

    // 5) Corner control (higher score for corner positions)
    if ((move.x == b.min.x && move.y == b.min.y) ||
        (move.x == b.min.x && move.y == b.max.y) ||
        (move.x == b.max.x && move.y == b.min.y) ||
        (move.x == b.max.x && move.y == b.max.y)) {
        score += 100;
    }

    // 6) Proximity to existing marks (favor moves adjacent to other marks)
    for (int dx = -1; dx <= 1; ++dx) {
        for (int dy = -1; dy <= 1; ++dy) {
            if (dx == 0 && dy == 0) continue;
                Point adjacent = {move.x + dx, move.y + dy};
                if (is_within_bounds(adjacent, b)) {
                    if (state.field->get_value(adjacent) == _mark) {
                    score += 10;
                    }
                }
        }
    }

return score;
}

int Agent::count_line_length(const GameState& state, const Point& start, const Point& direction, Mark mark, const Boundary& b) {
    int length = 0;
    Point current = start;

    while (is_within_bounds(current, b) && state.field->get_value(current) == mark) {
        length++;
        current.x += direction.x;
        current.y += direction.y;
    }

    current = start;
    current.x -= direction.x;
    current.y -= direction.y;

    while (is_within_bounds(current, b) && state.field->get_value(current) == mark) {
        length++;
        current.x -= direction.x;
        current.y -= direction.y;
    }

    return length;
}

bool Agent::is_within_bounds(const Point& point, const Boundary& b) {
    return point.x >= b.min.x && point.x <= b.max.x &&
           point.y >= b.min.y && point.y <= b.max.y;
}
