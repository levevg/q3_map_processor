#pragma once
#include <iostream>
#include "map.hpp"

using namespace std;

class Config {
public:
    explicit Config(istream& input);
    [[nodiscard]] float getMaxAngle() const { return max_angle; }
    [[nodiscard]] string getWait() const { return wait; }
    [[nodiscard]] string getTargetName() const { return target_name; }
    [[nodiscard]] const vector<string>& getShaders() const { return shaders; }
private:
    Map map_data;
    float max_angle;
    string wait;
    string target_name;
    vector<string> shaders;
};

void loadGlobalConfig(istream& input);
Config& globalConfig();
