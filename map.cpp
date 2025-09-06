#include "map.hpp"
#include <string>
#include <vector>
#include <iostream>
#include "nodes.hpp"

using namespace std;

string trim(const string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == string::npos) return "";
    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

Map::Map(istream& input) {
    parse(input);
}

void Map::save(ostream& output) const {
    output.seekp(0);
    output.clear();
    output << root.to_string();
}

void Map::parse(istream& input) {
    string line;
    Node *current_node = &root;
    string next_comment;
    while (getline(input, line)) {
        line = trim(line);
        if (line.empty()) continue;
        if (line[0] == '/' && line[1] == '/') {
            next_comment = trim(line);
            continue;
        }
        Node *created_node = nullptr;
        if (line == "brushDef") {
            created_node = new BrushDef();
            getline(input, line); line = trim(line);
        }
        if (line == "patchDef2") {
            created_node = new PatchDef2();
            getline(input, line); line = trim(line);
        }
        if (line[0] == '{') {
            Node *node = created_node ?: new Container();
            node->parent = current_node;
            current_node->children.push_back(node);
            current_node = node;
            if (!next_comment.empty()) {
                node->comment = next_comment;
                next_comment.clear();
            }
            continue;
        }
        if (line[0] == '}') {
            current_node = current_node->parent;
            continue;
        }
        if (line[0] == '"') {
            Node *node = new KeyValue(line);
            node->parent = current_node;
            current_node->children.push_back(node);
            continue;
        }
        if (dynamic_cast<BrushDef*>(current_node)) {
            auto *plane = new BrushPrimitivePlane(line);
            plane->parent = current_node;
            current_node->children.push_back(plane);
            continue;
        }
        if (dynamic_cast<PatchDef2*>(current_node)) {
            auto* string = new RawString(line);
            string->parent = current_node;
            current_node->children.push_back(string);
            continue;
        }
        if (dynamic_cast<Container*>(current_node)) {
            try {
                auto *plane = new BrushValve220Plane(line);
                plane->parent = current_node;
                current_node->children.push_back(plane);
                continue;
            } catch(const exception &e) { }
            continue;
        }
        throw runtime_error("Unexpected line format: " + line);
    }
}
