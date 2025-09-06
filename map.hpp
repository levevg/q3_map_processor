#pragma once
#include <string>
#include <iostream>
#include "nodes.hpp"

using namespace std;

string trim(const string& s);

class Map {
public:
    explicit Map(istream& input);
    void save(ostream& output) const;
    Root root = Root();
private:
    void parse(istream& input);
};
