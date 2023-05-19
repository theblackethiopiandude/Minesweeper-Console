#include <iostream>
#include <chrono>
#include <random>
#include <set>

struct Level{
    short ROW=0, COLUMN=0, BOMBS=0;
};
struct Position{
    int row;
    int column;
    bool operator<(const Position& other) const{
        if (row != other.row)
            return row < other.row;
        else
            return column < other.column;
    }

    bool operator==(const Position &other){
        return (other.row == row) && (other.column == column);
    }
};
std::ostream& operator<<(std::ostream &os, const Position &pos) {
    os << "(" << pos.row << ", " << pos.column << ")";
    return os;
}
_inline bool check(char ch){
    return ((ch >= '0') && (ch <= '9'));
}
Level difficulty(const short level){
    Level l{};
    switch (level){
        case 1:
            l.ROW=8;
            l.COLUMN=10;
            l.BOMBS=10;
            break;
        case 2:
            l.ROW=14;
            l.COLUMN=18;
            l.BOMBS=40;
            break;
        case 3:
            l.ROW=20;
            l.COLUMN=25;
            l.BOMBS=99;
            break;
        default:
            l.ROW=-1;
            l.COLUMN=-1;
            l.BOMBS=-1;
            break;
    }
    return l;
}
void draw(const Level level, const std::set<Position> &positions){
    if(level.ROW < 0 && level.COLUMN < 0 && level.BOMBS < 0)
        return;

    char ch[level.ROW][level.COLUMN];
    for (int row = 0; row < level.ROW; ++row) {
        for (int column = 0; column < level.COLUMN; ++column) {
            ch[row][column] = '0';
        }
    }
    const short LAST_ROW = level.ROW - 1;
    const short LAST_COLUMN = level.COLUMN - 1;
    for(const auto &value:positions) {
        const short PREVIOUS_ROW = value.row - 1;
        const short CURRENT_ROW = value.row;
        const short NEXT_ROW = value.row + 1;
        const short PREVIOUS_COLUMN = value.column - 1;
        const short CURRENT_COLUMN = value.column;
        const short NEXT_COLUMN = value.column + 1;

        ch[CURRENT_ROW][CURRENT_COLUMN] = '*';
        if(CURRENT_ROW == 0 && CURRENT_COLUMN == 0){
            if(check(ch[NEXT_ROW][CURRENT_COLUMN]))
                ch[NEXT_ROW][CURRENT_COLUMN] += 1;
            if(check(ch[NEXT_ROW][NEXT_COLUMN]))
                ch[NEXT_ROW][NEXT_COLUMN] += 1;
            if(check(ch[CURRENT_ROW][NEXT_COLUMN]))
                ch[CURRENT_ROW][NEXT_COLUMN] += 1;
        }else if(CURRENT_ROW == LAST_ROW && CURRENT_COLUMN == 0){
            if(check(ch[PREVIOUS_ROW][CURRENT_COLUMN]))
                ch[PREVIOUS_ROW][CURRENT_COLUMN] += 1;
            if(check(ch[PREVIOUS_ROW][NEXT_COLUMN]))
                ch[PREVIOUS_ROW][NEXT_COLUMN] += 1;
            if(check(ch[CURRENT_ROW][NEXT_COLUMN]))
                ch[CURRENT_ROW][NEXT_COLUMN] += 1;
        }else if(CURRENT_ROW == LAST_ROW && CURRENT_COLUMN == LAST_COLUMN){
            if(check(ch[PREVIOUS_ROW][PREVIOUS_COLUMN]))
                ch[PREVIOUS_ROW][PREVIOUS_COLUMN] += 1;
            if(check(ch[PREVIOUS_ROW][CURRENT_COLUMN]))
                ch[PREVIOUS_ROW][CURRENT_COLUMN] += 1;
            if(check(ch[CURRENT_ROW][PREVIOUS_COLUMN]))
                ch[CURRENT_ROW][PREVIOUS_COLUMN] += 1;
        }else if(CURRENT_ROW == 0 && CURRENT_COLUMN == LAST_COLUMN){
            if(check(ch[NEXT_ROW][PREVIOUS_COLUMN]))
                ch[NEXT_ROW][PREVIOUS_COLUMN] += 1;
            if(check(ch[NEXT_ROW][CURRENT_COLUMN]))
                ch[NEXT_ROW][CURRENT_COLUMN] += 1;
            if(check(ch[CURRENT_ROW][PREVIOUS_COLUMN]))
                ch[CURRENT_ROW][PREVIOUS_COLUMN] += 1;
        }else if(CURRENT_ROW == 0){
            if(check(ch[NEXT_ROW][PREVIOUS_COLUMN]))
                ch[NEXT_ROW][PREVIOUS_COLUMN] += 1;
            if(check(ch[NEXT_ROW][CURRENT_COLUMN]))
                ch[NEXT_ROW][CURRENT_COLUMN] += 1;
            if(check(ch[NEXT_ROW][NEXT_COLUMN]))
                ch[NEXT_ROW][NEXT_COLUMN] += 1;
            if(check(ch[CURRENT_ROW][PREVIOUS_COLUMN]))
                ch[CURRENT_ROW][PREVIOUS_COLUMN] += 1;
            if(check(ch[CURRENT_ROW][NEXT_COLUMN]))
                ch[CURRENT_ROW][NEXT_COLUMN] += 1;
        }else if(CURRENT_ROW == LAST_ROW){
            if(check(ch[CURRENT_ROW][PREVIOUS_COLUMN]))
                ch[CURRENT_ROW][PREVIOUS_COLUMN] += 1;
            if(check(ch[CURRENT_ROW][NEXT_COLUMN]))
                ch[CURRENT_ROW][NEXT_COLUMN] += 1;
            if(check(ch[PREVIOUS_ROW][PREVIOUS_COLUMN]))
                ch[PREVIOUS_ROW][PREVIOUS_COLUMN] += 1;
            if(check(ch[PREVIOUS_ROW][CURRENT_COLUMN]))
                ch[PREVIOUS_ROW][CURRENT_COLUMN] += 1;
            if(check(ch[PREVIOUS_ROW][NEXT_COLUMN]))
                ch[PREVIOUS_ROW][NEXT_COLUMN] += 1;
        }else if(CURRENT_COLUMN == 0){
            if(check(ch[NEXT_ROW][CURRENT_COLUMN]))
                ch[NEXT_ROW][CURRENT_COLUMN] += 1;
            if(check(ch[NEXT_ROW][NEXT_COLUMN]))
                ch[NEXT_ROW][NEXT_COLUMN] += 1;
            if(check(ch[PREVIOUS_ROW][CURRENT_COLUMN]))
                ch[PREVIOUS_ROW][CURRENT_COLUMN] += 1;
            if(check(ch[PREVIOUS_ROW][NEXT_COLUMN]))
                ch[PREVIOUS_ROW][NEXT_COLUMN] += 1;
            if(check(ch[CURRENT_ROW][NEXT_COLUMN]))
                ch[CURRENT_ROW][NEXT_COLUMN] += 1;
        }else if(CURRENT_COLUMN == LAST_COLUMN){
            if(check(ch[NEXT_ROW][CURRENT_COLUMN]))
                ch[NEXT_ROW][CURRENT_COLUMN] += 1;
            if(check(ch[NEXT_ROW][PREVIOUS_COLUMN]))
                ch[NEXT_ROW][PREVIOUS_COLUMN] += 1;
            if(check(ch[CURRENT_ROW][PREVIOUS_COLUMN]))
                ch[CURRENT_ROW][PREVIOUS_COLUMN] += 1;
            if(check(ch[PREVIOUS_ROW][PREVIOUS_COLUMN]))
                ch[PREVIOUS_ROW][PREVIOUS_COLUMN] += 1;
            if(check(ch[PREVIOUS_ROW][CURRENT_COLUMN]))
                ch[PREVIOUS_ROW][CURRENT_COLUMN] += 1;
        }else{
            if(check(ch[NEXT_ROW][PREVIOUS_COLUMN]))
                ch[NEXT_ROW][PREVIOUS_COLUMN] += 1;
            if(check(ch[NEXT_ROW][CURRENT_COLUMN]))
                ch[NEXT_ROW][CURRENT_COLUMN] += 1;
            if(check(ch[NEXT_ROW][NEXT_COLUMN]))
                ch[NEXT_ROW][NEXT_COLUMN] += 1;
            if(check(ch[CURRENT_ROW][PREVIOUS_COLUMN]))
                ch[CURRENT_ROW][PREVIOUS_COLUMN] += 1;
            if(check(ch[CURRENT_ROW][NEXT_COLUMN]))
                ch[CURRENT_ROW][NEXT_COLUMN] += 1;
            if(check(ch[PREVIOUS_ROW][PREVIOUS_COLUMN]))
                ch[PREVIOUS_ROW][PREVIOUS_COLUMN] += 1;
            if(check(ch[PREVIOUS_ROW][CURRENT_COLUMN]))
                ch[PREVIOUS_ROW][CURRENT_COLUMN] += 1;
            if(check(ch[PREVIOUS_ROW][NEXT_COLUMN]))
                ch[PREVIOUS_ROW][NEXT_COLUMN] += 1;
        }
    }
    for (int row = 0; row < level.ROW; ++row) {
        for (int column = 0; column < level.COLUMN; ++column) {
            std::cout << ch[row][column] << ' ';
        }
        std::cout << std::endl;
    }
}


void generate(const Level level, std::set<Position> &positions){ //Set that holds the positions, to make sure every position is unique
    if(level.ROW < 0 && level.COLUMN < 0 && level.BOMBS < 0)
        return;
    Position xy{};
    for (int i = 0; i < level.BOMBS; ++i) {
    std::srand(std::time(nullptr));//improve the quality of randomness
    random_generation:
        xy.row = rand() % level.ROW;
        xy.column = rand() % level.COLUMN;
        if (positions.count(xy)) //positions.find(xy) != positions.end()
            goto random_generation;
    positions.insert(xy);

    }
}
int main() {
    auto startTime = std::chrono::high_resolution_clock::now();

    std::set<Position> positions{};
    int level = 2;
    generate(difficulty(level), positions);
    draw(difficulty(level),positions);


    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
    std::cout << "\nExecution time: " << duration << " milliseconds" << std::endl;
    return 0;
}


