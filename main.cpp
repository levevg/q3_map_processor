#include <iostream>
#include <fstream>
#include <string>
#include "map.hpp"
#include "generate_triggers.hpp"
#include "config.hpp"

using namespace std;

bool file_exists(const string& filename) {
    ifstream file(filename);
    return file.good();
}

bool copy_file_portable(const string& source, const string& dest) {
    ifstream src(source, ios::binary);
    if (!src) return false;

    ofstream dst(dest, ios::binary);
    if (!dst) return false;

    dst << src.rdbuf();
    return dst.good() && src.good();
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cerr << "Usage: q3mp input_file.map output_file.map" << endl;
        return 1;
    }

    const auto config_file = "q3mp.cfg";
    try {
        ifstream file(config_file);
        if (!file) throw runtime_error("File not found");
        loadGlobalConfig(file);
        file.close();
    } catch(const exception& e) {
        cerr << "Config file " << config_file << " error: " << e.what() << endl;
        return 1;
    }

    string input_filename = argv[1];
    string output_filename = argv[2];

    if (input_filename == output_filename) {
        cerr << "Input and output files must be different." << endl;
        return 1;
    }

    ifstream infile(input_filename);
    if (!infile) {
        cerr << "Error opening input file: " << input_filename << endl;
        return 1;
    }

    if (file_exists(output_filename)) {
        string backup_filename = output_filename + ".q3mpbk";
        if (copy_file_portable(output_filename, backup_filename)) {
            cout << "Created backup: " << backup_filename << endl << endl;
        } else {
            cerr << "Warning: Could not create backup file" << endl << endl;
        }
    }

    ofstream outfile(output_filename);
    if (!outfile) {
        cerr << "Error opening output file: " << output_filename << endl;
        return 1;
    }

    try {
        Map map(infile);
        infile.close();
        cout << "Map file parsed successfully." << endl << endl;

        generate_triggers(map);
        map.save(outfile);
        outfile.close();
    } catch (const exception& e) {
        cerr << "Error parsing map file: " << e.what() << endl;
        return 1;
    }

    infile.close();
    outfile.close();

    return 0;
}
