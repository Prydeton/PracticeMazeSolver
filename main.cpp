#include <iostream>
#include <fstream>
#include <stdio.h>
#include <vector>
#include <tuple>
#include<string.h>
#include <unistd.h>

void printMaze(char **maze) {
    for (int i = 0; i < 20; i++)
    {
        for (int j = 0; j < 20; j++)
        {
            std::cout << maze[i][j] << ' ';
        }
        std::cout << std::endl;
    }
    std::cout << "\n" << std::endl;
}

char** readMaze() {
    std::ifstream file{ "maze.txt" };
    char** maze = new char*[20];
    for (int x = 0; x < 20; x++) {
        maze[x] = new char[20];
        for (int y = 0; y < 20; y++) {
            file >> maze[x][y]; 
        }
    }
    return maze;
}

char getItemAtCords(char** maze, int x, int y) {
    return maze[x][y];
}

int * findObject(char object, char** maze) {
    static int cords[2] = {0, 0}; 
    for (int x = 0; x < 20; x++) {
        for (int y = 0; y < 20; y++) {
            if (maze[x][y] == object) {
                cords[0] = x;
                cords[1] = y;
                return cords;
            } 
        }
    }
    return NULL;
}

int findRobotIndex(char* robots, char searching) {
    for (int i = 0; i < 4; i++) {
        if (robots[i] == searching) {
            return i;
        }
    }
    return 0;
}

char rotateRobot(char current, char* robots, std::string direction) {
    int currentIndex = findRobotIndex(robots, current);
    int newIndex;
    if (direction.compare("right") == 0) {
        if (currentIndex + 1 > 3) {
            newIndex = 0;
        } else {
            newIndex = currentIndex + 1;
        }
    } else {
        if (currentIndex - 1 < 0) {
            newIndex = 3;
        } else {
            newIndex = currentIndex - 1;
        }
    }
    return robots[newIndex];
}

    char getInFront(char** maze, char robot, std::tuple<int, int, char> current_position) {
        char itemInFront;
        if (robot == '^') {
            itemInFront = getItemAtCords(maze, std::get<0>(current_position) - 1, std::get<1>(current_position));
        } else if (robot == '>') {
            itemInFront = getItemAtCords(maze, std::get<0>(current_position), std::get<1>(current_position) + 1);
        } else if (robot == 'v') {
            itemInFront = getItemAtCords(maze, std::get<0>(current_position) + 1, std::get<1>(current_position));
        } else if  (robot == '<') {
            itemInFront = getItemAtCords(maze, std::get<0>(current_position), std::get<1>(current_position)  - 1);
        }
        return itemInFront;
    }

std::tuple<int, int, char> moveForward(std::tuple<int, int, char> current_position) {
    std::tuple<int, int, char> new_position;
    if (std::get<2>(current_position) == '^') {
        new_position = std::make_tuple(std::get<0>(current_position) - 1, std::get<1>(current_position), std::get<2>(current_position) );
    } else if (std::get<2>(current_position) == '>') {
        new_position = std::make_tuple(std::get<0>(current_position), std::get<1>(current_position) + 1, std::get<2>(current_position));
    } else if (std::get<2>(current_position) == 'v') {
        new_position = std::make_tuple(std::get<0>(current_position) + 1, std::get<1>(current_position), std::get<2>(current_position));
    } else if  (std::get<2>(current_position) == '<') {
        new_position = std::make_tuple(std::get<0>(current_position), std::get<1>(current_position) - 1, std::get<2>(current_position));
    }
    return new_position;
} 

char** updateMazeProgress(char** maze, char** mazeWithProgress, std::vector<std::tuple<int, int, char>> trail) {
    for (std::tuple<int, int, char> move : trail) {
        mazeWithProgress[std::get<0>(move)] [std::get<1>(move)] = std::get<2>(move);
    }
    return  mazeWithProgress;
}

int main() {
    char** maze = readMaze();
    char** updatedMaze = readMaze();
    printMaze(maze);

    std::vector<std::tuple<int, int, char>> trail;
    std::tuple<int, int> goal = std::make_tuple(findObject('G', maze)[0], findObject('G', maze)[1]);

    char robots[] = {'^', '>', 'v', '<'};

    std::tuple<int, int, char> r;
    
    for (char c : robots) {
        int* location = findObject(c, maze);
        if  (location) {
            r = std::make_tuple(location[0], location[1], c);
            trail.push_back (r);
            break;
        }
    }

    bool solved = false;
    while (!solved) {
        char robot = rotateRobot(std::get<2>(trail.back()), robots, "right");
        char to_right = getInFront(maze, robot, trail.back());
        if  (to_right == '=')  {
            char infront = getInFront(maze, std::get<2>(trail.back()), trail.back());
            if  (infront != '=')  {
                trail.push_back(moveForward(trail.back()));
            } else {
                char robot = rotateRobot(std::get<2>(trail.back()), robots, "left");
                trail.back() = std::make_tuple(std::get<0>(trail.back()), std::get<1>(trail.back()), robot);
            }
        } else {
            trail.back() = std::make_tuple(std::get<0>(trail.back()), std::get<1>(trail.back()), robot);
            trail.push_back(moveForward(trail.back()));    
        }
        if (std::get<0>(trail.back()) == std::get<0>(goal) && std::get<1>(trail.back()) == std::get<1>(goal)) {
            std::cout << "SUCCESS" << std::endl;
            solved = true;
        } else {
            updatedMaze = updateMazeProgress(maze, updatedMaze, trail);
            printMaze(updatedMaze);
        }
        sleep(.5);
    }
    return 0;
}