// ConfigLoader.h
#ifndef CONFIGLOADER_H
#define CONFIGLOADER_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>

struct RobotConfig {
    std::string name;
    int x;
    int y;
};

class ConfigLoader {
private:
    std::string filename;
    int width = 20, height = 20;
    std::vector<RobotConfig> robots;

public:
    ConfigLoader(const std::string& fname) : filename(fname) {}

    bool load() {
        std::ifstream file(filename);
        if (!file.is_open()) return false;

        std::string line;
        while (getline(file, line)) {
            if (line.empty() || line[0] == '#') continue;

            if (line.find("M by N") != std::string::npos) {
                std::istringstream iss(line.substr(line.find(":") + 1));
                iss >> width >> height;
            } else if (line.find("GenericRobot") != std::string::npos) {
                std::istringstream iss(line);
                std::string type, name, xStr, yStr;
                iss >> type >> name >> xStr >> yStr;

                int x = (xStr == "random") ? rand() % width : std::stoi(xStr);
                int y = (yStr == "random") ? rand() % height : std::stoi(yStr);

                robots.push_back({name, x, y});
            }
        }

        file.close();
        return true;
    }

    int getWidth() const { return width; }
    int getHeight() const { return height; }
    const std::vector<RobotConfig>& getRobots() const { return robots; }
};

#endif // CONFIGLOADER_H
