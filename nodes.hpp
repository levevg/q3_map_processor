#pragma once
#include <string>
#include <vector>
#include "point3f.hpp"

using namespace std;

class Node {
public:
    virtual ~Node() = default;
    [[nodiscard]] virtual string to_string() const = 0;
    string comment;
    vector<Node*> children;
    Node* parent = nullptr;
};

class Root : public Node {
public:
    [[nodiscard]] string to_string() const override;
};

class KeyValue : public Node {
public:
    KeyValue(string key, string value);
    explicit KeyValue(const string& line);
    [[nodiscard]] string to_string() const override;
    [[nodiscard]] string getKey() const { return key_; }
    [[nodiscard]] string getValue() const { return value_; }
private:
    string key_;
    string value_;
};

class Container : public Node {
public:
    string tag;
    [[nodiscard]] string to_string() const override;
};

class BrushDef : public Container {
public:
    BrushDef();
};

class CommonBrushPlane : public Node {
public:
    Point3f a, b, c;
    string texture;
    [[nodiscard]] string to_string() const override = 0;
};

class BrushPrimitivePlane : public CommonBrushPlane {
public:
    Point3f u, v;
    string flags;
    explicit BrushPrimitivePlane(const string& line);
    [[nodiscard]] string to_string() const override;
};

class BrushValve220Plane : public CommonBrushPlane {
public:
    string rest;
    explicit BrushValve220Plane(const string& line);
    [[nodiscard]] string to_string() const override;
};

class PatchDef2 : public Container {
public:
    PatchDef2();
};

class RawString : public Node {
public:
    string content;
    explicit RawString(string  line);
    [[nodiscard]] string to_string() const override;
};

class BrushSurface : public Node {
public:
    explicit BrushSurface(const string& line);
    [[nodiscard]] string to_string() const override;
};
