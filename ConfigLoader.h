/**********|**********|**********|
Program: CongfigLoader.h
Course: Data Structures and Algorithms
Trimester: 2410
Name: Jason Hean Qi Shen
ID: 242UC244FW
Lecture Section: TC3L
Tutorial Section: TT12L
Email: jason.hean.qi@studen.mmu.edu.my
Phone: 016-5556355
**********|**********|**********/
#ifndef CONFIGLOADER_H
#define CONFIGLOADER_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>

// Structure to hold robot configuration data from the config file
struct RobotConfig {
    std::string name;  // Robot identifier
    int x;             // Starting x-coordinate
    int y;             // Starting y-coordinate
};

class ConfigLoader {
private:
    std::string filename;  // Configuration file to load
    int width = 20, height = 20;  // Default battlefield dimensions
    int steps = 10;                // Default simulation steps
    std::vector<RobotConfig> robots;  // List of robots to create

public:
    // Constructor initializes random number generator
    ConfigLoader(const std::string& fname) : filename(fname) {
        srand(static_cast<unsigned>(time(nullptr)));
    }

    // Loads and parses the configuration file
    bool load() {
        std::ifstream file(filename);
        if (!file.is_open()) return false;

        std::string line;
        while (getline(file, line)) {
            if (line.empty() || line[0] == '#') continue;  // Skip empty lines and comments

            // Parse battlefield dimensions (M by N)
            if (line.find("M by N") != std::string::npos) {
                std::istringstream iss(line.substr(line.find(":") + 1));
                iss >> width >> height;
            } 
            // Parse number of simulation steps
            else if (line.find("steps") != std::string::npos) {
                std::istringstream iss(line.substr(line.find(":") + 1));
                iss >> steps;
            } 
            // Parse robot configurations
            else if (line.find("GenericRobot") != std::string::npos) {
                std::istringstream iss(line);
                std::string type, name, xStr, yStr;
                iss >> type >> name >> xStr >> yStr;

                // Handle random positions if specified
                int x = (xStr == "random") ? rand() % width : std::stoi(xStr);
                int y = (yStr == "random") ? rand() % height : std::stoi(yStr);

                robots.push_back({name, x, y});
            }
        }

        file.close();
        return true;
    }

    // Accessors for configuration data
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    int getSteps() const { return steps; }
    const std::vector<RobotConfig>& getRobots() const { return robots; }
};

#endif // CONFIGLOADER_H