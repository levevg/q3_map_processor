#include "generate_triggers.hpp"
#include <algorithm>
#include <cmath>
#include <stdexcept>
#include "map.hpp"
#include "config.hpp"

using namespace std;

struct Plane {
    Point3f normal;
    float c;
    bool hasTexture;
    string sourceString;

    explicit Plane(CommonBrushPlane* plane);
    [[nodiscard]] float distance(const Point3f& point) const;
};

Plane::Plane(CommonBrushPlane* plane) {
    const auto& textures = globalConfig().getShaders();
    hasTexture = std::find(textures.begin(), textures.end(), plane->texture) != textures.end();
    normal = (plane->a - plane->b).cross(plane->c - plane->b);
    float length = sqrt(normal.dot(normal));
    if (length == 0) {
        throw std::runtime_error("Invalid plane: zero length normal vector.");
    }
    normal = Point3f(normal.x / length, normal.y / length, normal.z / length);
    c = -normal.dot(plane->b);
    sourceString = plane->to_string();
}

float Plane::distance(const Point3f& point) const {
    return normal.dot(point) + c;
}

void add_raw_string(Container *container, const string& content) {
    auto *raw_string = new RawString(content);
    raw_string->parent = container;
    container->children.push_back(raw_string);
}

bool intersect(const Plane& p1, const Plane& p2, const Plane& p3, Point3f &out) {
    float det = p1.normal.x * (p2.normal.y * p3.normal.z - p2.normal.z * p3.normal.y)
               -p1.normal.y * (p2.normal.x * p3.normal.z - p2.normal.z * p3.normal.x)
               +p1.normal.z * (p2.normal.x * p3.normal.y - p2.normal.y * p3.normal.x);
    if (det == 0) return false;
    out.x = (- p1.normal.y * (p2.normal.z * p3.c - p2.c * p3.normal.z)
             + p1.normal.z * (p2.normal.y * p3.c - p2.c * p3.normal.y)
                    - p1.c * (p2.normal.y * p3.normal.z - p3.normal.y * p2.normal.z))/det;
    out.y = (+ p1.normal.x * (p2.normal.z * p3.c - p2.c * p3.normal.z)
             - p1.normal.z * (p2.normal.x * p3.c - p2.c * p3.normal.x)
                    + p1.c * (p2.normal.x * p3.normal.z - p2.normal.z * p3.normal.x))/det;
    out.z = (- p1.normal.x * (p2.normal.y * p3.c - p2.c * p3.normal.y)
             + p1.normal.y * (p2.normal.x * p3.c - p2.c * p3.normal.x)
                   - p1.c * (p2.normal.x * p3.normal.y - p2.normal.y * p3.normal.x))/det;
    return true;
}

void fix_order(vector <Point3f>& vertices) {
    Point3f center(0, 0, 0);
    for (const auto& vertex : vertices) {
        center = center + vertex;
    }
    center = center * (1.0f / (float)vertices.size());
    auto compare_angle = [&center](const Point3f& a, const Point3f& b) {
        float angle_a = atan2(a.y - center.y, a.x - center.x);
        float angle_b = atan2(b.y - center.y, b.x - center.x);
        return angle_a > angle_b;
    };
    sort(vertices.begin(), vertices.end(), compare_angle);
}

string point_to_patch_string(const Point3f &p, float u, float v) {
    return "( " + p.to_string() + " " + float_to_string(u) + " " + float_to_string(v) + " )";
}

string points_to_patch_string(const Point3f &p1, const Point3f &p2, float u1, float v1, float u2, float v2) {
    return "( " + point_to_patch_string(p1, u1, v1) + " " + point_to_patch_string((p1+p2)*0.5, (u1+u2)*0.5f, (v1+v2)*0.5f) + " " + point_to_patch_string(p2, u2, v2) + " )";
}

void add_patch_for_quad(Container *container, const Plane *plane, const vector<Point3f> &quad) {
    Point3f o = plane->normal;
    Point3f a = quad[0] + o, b = quad[1] + o, c = quad[2] + o, d = quad[3] + o;

    auto* brush = new Container();
    brush->parent = container;
    container->children.push_back(brush);
    auto *patch = new PatchDef2();
    patch->parent = brush;
    brush->children.push_back(patch);
    add_raw_string(patch, "common/trigger");
    add_raw_string(patch, "( 3 3 0 0 0 )");
    add_raw_string(patch, "(");
    add_raw_string(patch, points_to_patch_string(a, b, 0, 0, 1, 0));
    add_raw_string(patch, points_to_patch_string((a+d)*0.5, (b+c)*0.5, 0, 0.5, 1, 0.5));
    add_raw_string(patch, points_to_patch_string(d, c, 0, 1, 1, 1));
    add_raw_string(patch, ")");
}

void add_triggers_for_solid(const vector<Plane>& planes, Container *container) {
    float angle_threshold = globalConfig().getMaxAngle();
    for (const auto& plane : planes) {
        if (!plane.hasTexture) continue;
        if (plane.normal.dot(Point3f(0, 0, 1)) < cos(M_PI*angle_threshold/180.0)) continue;
        auto top = &plane;

        vector<Point3f> vertices;
        for (int i = 0; i < planes.size(); ++i) {
            for (int j = i + 1; j < planes.size(); ++j) {
                for (int k = j + 1; k < planes.size(); ++k) {
                    Point3f intersection;
                    if (intersect(planes[i], planes[j], planes[k], intersection) && abs(top->distance(intersection)) < 0.001f) {
                        vertices.push_back(intersection);
                    }
                }
            }
        }

        vertices.erase(std::remove_if(vertices.begin(), vertices.end(), [&planes](const Point3f& v) {
            for (auto &plane: planes)
                if (plane.distance(v) > 0.001f) return true;
            return false;
        }), vertices.end());

        if (vertices.size() < 3) return;

        fix_order(vertices);

        if (vertices.size() == 3) {
            vertices.insert(vertices.begin() + 1, (vertices[0] + vertices[1]) * 0.5f);
        }

        if (vertices.size() % 2 == 1) {
            vertices.insert(vertices.begin() + 3, (vertices[2] + vertices[3]) * 0.5f);
        }

        while (vertices.size() >= 4) {
            auto from = vertices.begin();
            auto to = from + 4;
            add_patch_for_quad(container, top, vector<Point3f>(from, to));
            vertices.erase(from + 1, to - 1);
        }
    }
}

void add_triggers_for_brush(BrushDef* brush, Container *container) {
    vector<Plane> planes;
    for (Node* child : brush->children) {
        if (dynamic_cast<CommonBrushPlane*>(child)) {
            auto* plane = (CommonBrushPlane *)child;
            planes.emplace_back(plane);
        }
    }
    add_triggers_for_solid(planes, container);
}

void add_triggers_for_node(Node* node, Container *container) {
    for (Node* child : node->children) {
        if (dynamic_cast<BrushDef*>(child)) {
            add_triggers_for_brush((BrushDef *)child, container);
            continue;
        }
        if (dynamic_cast<Container*>(child)) {
            BrushDef valve220Brush;
            for (auto &b: child->children) {
                if (dynamic_cast<BrushValve220Plane *>(b)) {
                    valve220Brush.children.push_back(b);
                }
            }
            if (!valve220Brush.children.empty()) {
                add_triggers_for_brush(&valve220Brush, container);
                continue;
            }
        }
        add_triggers_for_node(child, container);
    }
}

void generate_triggers(Map& map) {
    string comment = "// generated triggers";
    for (Node* child : map.root.children) {
        if (child->comment == comment) {
            throw std::runtime_error("Generated triggers already exist in the input file. Recheck the input file.");
        }
    }
    auto* triggers = new Container();
    triggers->comment = comment;
    triggers->children.push_back(new KeyValue("classname", "trigger_multiple"));
    triggers->children.push_back(new KeyValue("target", globalConfig().getTargetName()));
    triggers->children.push_back(new KeyValue("wait", globalConfig().getWait()));
    triggers->parent = &map.root;

    int size_before = triggers->children.size();
    add_triggers_for_node(&map.root, triggers);
    cout << (triggers->children.size() - size_before) << " trigger patches added." << endl;

    map.root.children.push_back(triggers);
}
