#include "config.hpp"

using namespace std;

Config* global_config = nullptr;

void loadGlobalConfig(istream& input) {
    global_config = new Config(input);
}

Config& globalConfig() {
    return *global_config;
}

Config::Config(istream& input) : map_data(input) {
    if (map_data.root.children.size() != 1) {
        throw runtime_error("Config file must contain exactly one root container.");
    }
    max_angle = -1;
    wait = -2;
    for (const auto node: map_data.root.children[0]->children) {
        if (auto* kv = dynamic_cast<KeyValue*>(node)) {
            if (kv->getKey() == "max_angle") {
                try {
                    max_angle = stof(kv->getValue());
                } catch (...) {
                    throw runtime_error("max_angle should be a float number.");
                }
            } else if (kv->getKey() == "wait") {
                wait = kv->getValue();
            } else if (kv->getKey() == "target_name") {
                target_name = kv->getValue();
            } else if (kv->getKey() == "shader") {
                shaders.push_back(kv->getValue());
            } else {
                throw runtime_error("Unknown key: " + kv->getKey());
            }
        } else {
            throw runtime_error("Unexpected node in config: " + node->to_string());
        }
    }
    if (max_angle < 0 || max_angle > 90) {
        throw runtime_error("max_angle should be between 0 and 90.");
    }
    if (target_name.empty()) {
        throw runtime_error("target_name is required.");
    }
    if (shaders.empty()) {
        throw runtime_error("At least one shader is required.");
    }

    cout << "Config loaded:"  << endl
            << "  max_angle = " << max_angle << endl
            << "  target_name = " << target_name << endl
            << "  wait = " << wait << endl
            << "  shaders:" << endl;
    for (const auto &shader : shaders) {
        cout << "    " << shader << endl;
    }
    cout << endl;
}
