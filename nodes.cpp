#include "nodes.hpp"
#include <string>
#include <utility>
#include <regex>
#include <iostream>
#include "point3f.hpp"

using namespace std;

string Root::to_string() const {
    string result;
    for (const auto* child : children) {
        result += child->to_string();
    }
    return result;
}

KeyValue::KeyValue(string key, string value) : key_(std::move(key)), value_(std::move(value)) {}

KeyValue::KeyValue(const string& line) {
    size_t first_quote = line.find('"');
    size_t second_quote = line.find('"', first_quote + 1);
    size_t third_quote = line.find('"', second_quote + 1);
    size_t fourth_quote = line.find('"', third_quote + 1);
    if (first_quote != string::npos && second_quote != string::npos && third_quote != string::npos && fourth_quote != string::npos) {
        key_ = line.substr(first_quote + 1, second_quote - first_quote - 1);
        value_ = line.substr(third_quote + 1, fourth_quote - third_quote - 1);
    } else {
        throw runtime_error("Invalid KeyValue format: " + line);
    }
}

string KeyValue::to_string() const {
    return "\"" + key_ + "\" \"" + value_ + "\"\n";
}

string Container::to_string() const {
    string result;
    if (!comment.empty()) result += comment + "\n";
    if (!tag.empty()) result += tag + "\n";
    result += "{\n";
    for (const auto* child : children) {
        result += child->to_string();
    }
    result += "}\n";
    return result;
}

BrushDef::BrushDef() { tag = "brushDef"; }

BrushPrimitivePlane::BrushPrimitivePlane(const string& line) {
    std::regex re(R"(^\( (.+?) \) \( (.+?) \) \( (.+?) \) \( \( (.+?) \) \( (.+?) \) \) (.+?) (.+)$)");
    std::smatch match;
    if (std::regex_search(line, match, re)) {
        a = Point3f::from_string(match[1].str());
        b = Point3f::from_string(match[2].str());
        c = Point3f::from_string(match[3].str());
        u = Point3f::from_string(match[4].str());
        v = Point3f::from_string(match[5].str());
        texture = match[6].str();
        flags = match[7].str();
    } else {
        throw runtime_error("Invalid BrushPlane format: " + line);
    }
}

string BrushPrimitivePlane::to_string() const {
    string result;
    result += "( "; result += a.to_string(); result += " ) ";
    result += "( "; result += b.to_string(); result += " ) ";
    result += "( "; result += c.to_string(); result += " ) ";
    result += "( ";
    result += "( "; result += u.to_string(); result += " ) ";
    result += "( "; result += v.to_string(); result += " ) ";
    result += ") ";
    result += texture;
    result += " ";
    result += flags;
    result += "\n";
    return result;
}

BrushValve220Plane::BrushValve220Plane(const string& line) {
    std::regex re(R"(^\( (.+?) \) \( (.+?) \) \( (.+?) \) (\S+?) (.+)$)");
    std::smatch match;
    if (std::regex_search(line, match, re)) {
        a = Point3f::from_string(match[1].str());
        b = Point3f::from_string(match[2].str());
        c = Point3f::from_string(match[3].str());
        texture = match[4].str();
        rest = match[5].str();
    } else {
        throw runtime_error("Invalid BrushPlane format: " + line);
    }
}

string BrushValve220Plane::to_string() const {
    string result;
    result += "( "; result += a.to_string(); result += " ) ";
    result += "( "; result += b.to_string(); result += " ) ";
    result += "( "; result += c.to_string(); result += " ) ";
    result += texture;
    result += " ";
    result += rest;
    result += "\n";
    return result;
}

PatchDef2::PatchDef2() { tag = "patchDef2"; }

RawString::RawString(string  line) : content(std::move(line)) {}

string RawString::to_string() const {
    return content + "\n";
}

BrushSurface::BrushSurface(const string& line) {
    // Example: parse three floats separated by spaces using regex
    std::regex re(R"(([-+]?[0-9]*\.?[0-9]+)[ ]+([-+]?[0-9]*\.?[0-9]+)[ ]+([-+]?[0-9]*\.?[0-9]+))");
    std::smatch match;
    if (std::regex_search(line, match, re)) {
        float x = std::stof(match[1].str());
        float y = std::stof(match[2].str());
        float z = std::stof(match[3].str());
        std::cout << "Parsed floats: " << x << ", " << y << ", " << z << std::endl;
    }
}

string BrushSurface::to_string() const {
    return "BrushSurface";
}
