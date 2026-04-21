#ifndef SRC_HPP
#define SRC_HPP
#include <iostream>
#include <utility>
#include <deque>

struct Snake;
struct Map;

enum class instruction { UP, DOWN, LEFT, RIGHT, NONE };

bool is_food(Map* map, int x, int y);
bool is_wall(Map* map, int x, int y);
void eat_food(Map* map, int x, int y);
int get_height(Map* map);
int get_width(Map* map);

const int MaxWidth = 25;

struct Map {
    bool wall[MaxWidth][MaxWidth];
    bool food[MaxWidth][MaxWidth];
    int width, height;

    int get_height() { return height; }
    int get_width() { return width; }
    bool is_food(int x, int y) {
        if (x < 0 || x >= height || y < 0 || y >= width) return false;
        return food[x][y];
    }
    void eat_food(int x, int y) {
        if (x >= 0 && x < height && y >= 0 && y < width) food[x][y] = false;
    }
    bool is_wall(int x, int y) {
        if (x < 0 || x >= height || y < 0 || y >= width) return true;
        return wall[x][y];
    }

    void initialize(Snake* snake);
    void print(Snake* snake);
};

struct Snake {
    std::deque<std::pair<int, int>> body;
    instruction current_dir;

    void initialize(int x, int y, instruction ins) {
        body.clear();
        body.push_back({ x, y });
        current_dir = ins;
    }

    int get_length() {
        return body.size();
    }

    bool move(Map* map, instruction ins) {
        if (ins == instruction::NONE) {
            ins = current_dir;
        }

        if ((ins == instruction::UP && current_dir == instruction::DOWN) ||
            (ins == instruction::DOWN && current_dir == instruction::UP) ||
            (ins == instruction::LEFT && current_dir == instruction::RIGHT) ||
            (ins == instruction::RIGHT && current_dir == instruction::LEFT)) {
            return false;
        }

        current_dir = ins;
        std::pair<int, int> head = body.front();
        int next_x = head.first;
        int next_y = head.second;

        if (ins == instruction::UP) next_x--;
        else if (ins == instruction::DOWN) next_x++;
        else if (ins == instruction::LEFT) next_y--;
        else if (ins == instruction::RIGHT) next_y++;

        if (next_x < 0 || next_x >= map->get_height() || next_y < 0 || next_y >= map->get_width()) {
            return false;
        }

        if (map->is_wall(next_x, next_y)) {
            return false;
        }

        bool eating = map->is_food(next_x, next_y);

        for (size_t i = 0; i < body.size(); ++i) {
            if (body[i].first == next_x && body[i].second == next_y) {
                return false;
            }
        }

        if (!eating) {
            body.pop_back();
        } else {
            map->eat_food(next_x, next_y);
        }

        body.push_front({ next_x, next_y });
        return true;
    }

    std::pair<int, std::pair<int, int>*> get_snake() {
        int len = body.size();
        std::pair<int, int>* arr = new std::pair<int, int>[len];
        for (int i = 0; i < len; ++i) {
            arr[i] = body[i];
        }
        return { len, arr };
    }
};

inline void Map::initialize(Snake* snake) {
    if (!(std::cin >> height >> width)) return;
    int head_x = -1, head_y = -1;
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            char c;
            std::cin >> c;
            wall[i][j] = (c == '#');
            food[i][j] = (c == '*');
            if (c == '@') {
                head_x = i;
                head_y = j;
            }
        }
    }
    char dir_char;
    std::cin >> dir_char;
    instruction ins;
    if (dir_char == 'U') ins = instruction::UP;
    else if (dir_char == 'D') ins = instruction::DOWN;
    else if (dir_char == 'L') ins = instruction::LEFT;
    else if (dir_char == 'R') ins = instruction::RIGHT;
    else ins = instruction::NONE;

    snake->initialize(head_x, head_y, ins);
}

inline void Map::print(Snake* snake) {
    auto snake_info = snake->get_snake();
    int len = snake_info.first;
    std::pair<int, int>* body_arr = snake_info.second;

    char display[MaxWidth][MaxWidth];
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            if (wall[i][j]) display[i][j] = '#';
            else if (food[i][j]) display[i][j] = '*';
            else display[i][j] = '.';
        }
    }

    for (int i = 0; i < len; ++i) {
        if (i == 0) display[body_arr[i].first][body_arr[i].second] = '@';
        else display[body_arr[i].first][body_arr[i].second] = 'o';
    }

    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            std::cout << display[i][j];
        }
        std::cout << std::endl;
    }
    delete[] body_arr;
}

struct Game {
    Map* map;
    Snake* snake;
    int score;
    int round;
    void initialize() {
        map = new Map();
        snake = new Snake();
        map->initialize(snake);
        score = 0;
        round = 0;
    }
    bool step() {
        char str[MaxWidth];
        if (!(std::cin >> str)) return false;
        instruction ins;
        switch (str[0]) {
        case 'U': ins = instruction::UP; break;
        case 'D': ins = instruction::DOWN; break;
        case 'L': ins = instruction::LEFT; break;
        case 'R': ins = instruction::RIGHT; break;
        case 'N': ins = instruction::NONE; break;
        default: ins = instruction::NONE;
        }
        if (snake->move(map, ins)) {
            score++;
        } else {
            return false;
        }
        return true;
    }
    void print() {
        std::cout << "Round: " << round << std::endl;
        map->print(snake);
    }
    void play() {
        while (step()) {
            round++;
            print();
        }
        score += snake->get_length();
        std::cout << "Game Over" << std::endl;
        std::cout << "Score: " << score << std::endl;
    }
};

#endif
