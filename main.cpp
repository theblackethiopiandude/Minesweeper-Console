#include <iostream>
#include <chrono>
#include <random>
#include <set>
#include <vector>
#include <iomanip>
#include <fstream>

struct Level{
    short ROW=0;
    short COLUMN=0;
    short BOMBS=0;
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

    bool operator==(const Position &other)const{
        return (other.row == row) && (other.column == column);
    }
};
std::ostream& operator<<(std::ostream &os, const Position &pos) {
    os << "(" << pos.row << ", " << pos.column << ")";
    return os;
}
short start_game();
void generate_game(const Level level, const std::set<Position> &positions);
void generate(const Level level, std::set<Position> &positions);
void draw_box(const Level level,const std::vector<std::vector<char>> &gameValues, const std::set<Position> &bombPosition);
void new_game();
void open_game_file();
_inline bool check(char ch){
    return ((ch >= '0') && (ch <= '9'));
}
int main() {
    auto startTime = std::chrono::high_resolution_clock::now();
    new_game();

    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
    std::cout << "\nExecution time: " << duration << " milliseconds" << std::endl;
    system("pause");
    return 0;
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
void draw_box(const Level level,const std::vector<std::vector<char>> &gameValues, const std::set<Position> &bombPosition){
    bool gameOver = false, won = false;
    std::string box = "[ ]";
    /*for(const auto &value: bombPosition) // little back door to know where the bombs are
        std::cout << value << " ";*/
    std::cout << std::endl;
    for (int row = 0; row < level.ROW; ++row) {
        for (int column = 0; column < level.COLUMN; ++column) {
            if(row == 0 && column == 0){
                for (int column = 0; column < level.COLUMN; ++column){
                    if (column == 0)
                        std::cout << "    ";
                    std::cout << std::setw(3) << column << ' ';
                }
                std::cout << std::endl;
                for (int column = 0; column < level.COLUMN; ++column){
                    if (column == 0)
                        std::cout << "    ";
                    std::cout << std::setw(4) << "_ ";
                }
                std::cout << std::endl;
            }
            if(column == 0)
                std::cout << std::setw(3) << row << "| ";
            std::cout << std::setw(3) << "[ ]" << ' ';
        }
        std::cout << std::endl;
    }
    auto startTime = std::chrono::steady_clock::now();
    Position xy{};
    short choice{};
    std::set<Position> flagged, revealed;
    while (not gameOver) {
        gamechoice:
        std::cout << std::endl;
        std::cout << "\t \t Available Flags: " << level.BOMBS - flagged.size() << '\n';
        std::cout << "1. Reveal \n";
        std::cout << "2. Flag \n";
        std::cout << "3. New Game \n";
        std::cout << "4. Save \n";
        std::cout << "> ";
        std::cin >> choice;
        if(not (choice >= 1 && choice <= 4))
            //goto gamechoice;
        switch (choice){
            case 3:
                new_game();
                return;
                break;
        }
        if(choice == 4){
            std::ofstream outputFile("gameValues.txt");
            if (!outputFile) {
                std::cerr << "Failed to open output file: gameValues.txt" << std::endl;
                return;
            }
            for (const std::vector<char>& row : gameValues) {
                for (char element : row) {
                    outputFile << element;
                }
                outputFile << '\n';
            }
            outputFile.close();
            std::cout << "Vector saved to file: gameValues.txt" << std::endl;

            outputFile.open("level.bin", std::ios::binary);
            if (!outputFile) {
                std::cerr << "Failed to open output file: level.bin" << std::endl;
                return;
            }
            outputFile.write(reinterpret_cast<const char*>(&level), sizeof(level));// Write the structure to the file
            outputFile.close();

            std::cout << "Structure written to file: level.bin" << std::endl;

            outputFile.open("bombPosition.txt");
            if (!outputFile) {
                std::cerr << "Failed to open output file: bombPosition.txt" << std::endl;
                return;
            }

            for (const Position& pos : bombPosition) {
                // Write each structure to the file
                outputFile << pos.row << ' ' << pos.column << '\n';
            }
            outputFile.close();

            std::cout << "Set written to file: bombPosition.txt" << std::endl;
            break;
        }
        game_axis:
            std::cout << "Enter Position (xy separated by space): ";
            std::cin >> xy.row >> xy.column;
            if((not (xy.row < level.ROW && xy.row >= 0) ) || (not (xy.column < level.COLUMN && xy.column >= 0))){
                std::cout << "Invalid Position: OUT OF BOUND!\n";
                goto game_axis;
            }else if(revealed.count(xy)){
                std::cout << "Position Already Revealed!\n";
                goto game_axis;
            }

        switch (choice){
            case 1:
                if(flagged.count(xy)){
                    std::cout << "Position is Flagged!\n";
                    goto game_axis;
                }
                if(bombPosition.count(xy))
                    gameOver = true;
                else
                    revealed.insert(xy);
                break;
            case 2:
                if(flagged.count(xy))
                    flagged.erase(xy);
                else if(flagged.size() >= level.BOMBS) {
                    std::cout << "Out of Available Flags, remove some flags to add new flags\n";
                    goto gamechoice;
                }
                else
                    flagged.insert(xy);
                break;
        }
        Position loop{};
        std::cout << std::endl;
        if(flagged.size() == level.BOMBS){
            if(flagged == bombPosition){
                won = true;
                goto win;
            }

        }

        for (int row = 0; row < level.ROW; ++row) {
            for (int column = 0; column < level.COLUMN; ++column) {
                loop.row = row;
                loop.column = column;
                if(row == 0 && column == 0){
                    for (int column = 0; column < level.COLUMN; ++column){
                        if (column == 0)
                            std::cout << "    ";
                        std::cout << std::setw(3) << column << ' ';
                    }
                    std::cout << std::endl;
                    for (int column = 0; column < level.COLUMN; ++column){
                        if (column == 0)
                            std::cout << "    ";
                        std::cout << std::setw(4) << "_ ";
                    }
                    std::cout << std::endl;
                }
                if(column == 0)
                    std::cout << std::setw(3) << row << "| ";

                if(gameOver){
                    if (bombPosition.count(loop)) // shows bombs because the game is over
                        box[1] = gameValues[row][column];
                    for(auto it = flagged.begin(); it != flagged.end();){ // erase positions that are not bombs
                        if (!bombPosition.count(*it))
                            it = flagged.erase(it);
                        else
                            it++;
                    }
                }
                if (revealed.count(loop))
                    box[1] = gameValues[row][column];
                if(flagged.count(loop))
                    box = "[B]";
                std::cout << std::setw(3) << box << ' ';
                box = "[ ]";
            }
            std::cout << std::endl;
        }

        win:
            if(won){
                for (int row = 0; row < level.ROW; ++row) {
                    for (int column = 0; column < level.COLUMN; ++column) {
                        loop.row = row;
                        loop.column = column;
                        if (row == 0 && column == 0) {
                            for (int column = 0; column < level.COLUMN; ++column) {
                                if (column == 0)
                                    std::cout << "    ";
                                std::cout << std::setw(3) << column << ' ';
                            }
                            std::cout << std::endl;
                            for (int column = 0; column < level.COLUMN; ++column) {
                                if (column == 0)
                                    std::cout << "    ";
                                std::cout << std::setw(4) << "_ ";
                            }
                            std::cout << std::endl;
                        }
                        if (column == 0)
                            std::cout << std::setw(3) << row << "| ";
                        box[1] = gameValues[row][column];
                        if (flagged.count(loop))
                            box = "[B]";
                        std::cout << std::setw(3) << box << ' ';
                        box = "[ ]";
                    }
                    std::cout << std::endl;
                }
            }
            std::chrono::seconds elapsedTime;
            if (gameOver || won) {
                auto endTime = std::chrono::steady_clock::now();
                elapsedTime = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime);
                std::cout << "\nElapsed time: " << elapsedTime.count() << " seconds" << std::endl;
            }
                if (gameOver) {
                    std::cout << "Game Over!" << std::endl;
                }
                if (won) {
                    std::cout << "You Have Won!" << std::endl;
                    std::string name{}, difficulty{}, line_from_file{};
                    std::cout << "Enter Your Name: ";
                    std::cin >> name;
                    switch (level.BOMBS) {
                        case 10:
                            difficulty = "Easy";
                            break;
                        case 40:
                            difficulty = "Medium";
                            break;
                        case 99:
                            difficulty = "Hard";
                            break;
                    }
                    std::ofstream out("Highscore.txt", std::ios::app);
                    if (!out.fail()) {
                        out << std::left << std::setw(10) << name << std::setw(10) << difficulty << elapsedTime.count() << " seconds\n";
                    }
                    break;
                }

    }
}
void generate_game(const Level level, const std::set<Position> &positions){
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
    std::vector<std::vector<char>> gameValues(level.ROW, std::vector<char>(level.COLUMN));
    for (int row = 0; row < level.ROW; ++row) {
        for (int column = 0; column < level.COLUMN; ++column) {
            gameValues[row][column] = ch[row][column];
        }
    }
    draw_box(level, gameValues, positions);
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
short start_game(){
    short level{};
    std::cout << "High Scores Of Previous Players!\n";
    std::ifstream in("Highscore.txt");
    if (!in.fail()) {
        for (char ch;in.get(ch);)
            std::cout << ch;
    }

    std::cout << "\nWelcome to Minesweeper!\n";
    Difficulty:
    std::cout << "\tChoose Difficulty:\n";
    std::cout << "\t\t1.Easy\n";
    std::cout << "\t\t2.Medium\n";
    std::cout << "\t\t3.Hard\n";
    std::cout << "\n\t\t4.Continue\n";
    std::cout << "\t>> ";
    std::cin >> level;
    if(level == 4)
        open_game_file();
    if(not ((level >= 1) && (level <=3))){
        std::cout << "Invalid Difficulty!\n\n";
        goto Difficulty;
    }
    return level;
}
void new_game(){
    std::set<Position> positions{};
    Level level = difficulty(start_game());
    generate(level, positions);
    generate_game(level, positions);
}
void open_game_file(){
    std::vector<std::vector<char>> gameValues;
    std::ifstream inputFile("gameValues.txt");
    if (!inputFile) {
        std::cerr << "Failed to open input file: gameValues.txt"<< std::endl;
        return;
    }

    std::string line;
    while (std::getline(inputFile, line)) {
        std::vector<char> row;
        for (char element : line) {
            row.push_back(element);
        }
        gameValues.push_back(row);
    }
    inputFile.close();
    std::cout << "Vector loaded from file: gameValues.txt"<< std::endl;

    Level level;
    inputFile.open("level.bin", std::ios::binary);
    if (!inputFile) {
        std::cerr << "Failed to open input file: level.bin"<< std::endl;
        return;
    }

    // Read the structure from the file
    inputFile.read(reinterpret_cast<char*>(&level), sizeof(level));
    inputFile.close();

    std::cout << "Structure read from file: level.bin" << std::endl;

    std::set<Position> bombPositions;

    inputFile.open("bombPosition.txt");
    if (!inputFile) {
        std::cerr << "Failed to open input file: bombPosition.txt" << std::endl;
        return;
    }

    int row;
    int column;
    while (inputFile >> row >> column) {
        // Create a Person structure from the file data
        Position pos = {row, column};
        // Add the structure to the set
        bombPositions.insert(pos);
    }

    inputFile.close();

    std::cout << "Set read from file: bombPosition.txt" << std::endl;
    draw_box(level,gameValues, bombPositions);


}
