
#include "g_states_file_reader.hpp"

std::vector<GStateInfo> GStatesFileReader::read_file(std::filesystem::path path) {
    using namespace std;
    if (VERBOSE) {
        cout << "GStatesFileReader::read_file: reading " << path << endl;
    }
    vector<GStateInfo> v;

    ifstream in(path.c_str());
    stringstream ss;
    ss << in.rdbuf();
    if (VERBOSE) {
        cout << "Contents: " << endl;
        cout << ss.str();
        cout << endl;
    }

    string line;

    bool foundName = false; // name of state
    bool foundFrames = false; // number of frames
    bool foundImage = false; // image file name
    bool foundShader = false; // name of shader

    GStateInfo gsi;

    while (getline(ss, line)) { // until there are no more lines
        stringstream linestream; // make a streamstream with the line and read the first word
        linestream << line;
        string str;
        linestream >> str;
        if (str == "name:") { // read the property, see if it matches what we're expecting ("name:" , etc.)
            foundName = true;
            linestream >> gsi.name;  // then get the value from right after the property name
            if (foundName && foundFrames && foundImage && foundShader) {
                foundName = false;
                foundFrames = false;
                foundImage = false;
                foundShader = false;
                v.push_back(gsi); // add to the result if a complete set of state information is read
                // (a copy of the gsi on the stack gets pushed into the vector we're returning)
            }
            continue; // get next line
        }
        else if (str == "frames:") {
            foundFrames = true;
            linestream >> gsi.frames;
            if (foundName && foundFrames && foundImage && foundShader) {
                foundName = false;
                foundFrames = false;
                foundImage = false;
                foundShader = false;
                v.push_back(gsi);
            }
            continue; // get next line
        }
        else if (str == "image:") {
            foundImage = true;
            linestream >> gsi.image;
            if (foundName && foundFrames && foundImage && foundShader) {
                foundName = false;
                foundFrames = false;
                foundImage = false;
                foundShader = false;
                v.push_back(gsi);
            }
            continue; // get next line
        }
        else if (str == "shader:") {
            foundShader = true;
            linestream >> gsi.shader;
            if (foundName && foundFrames && foundImage && foundShader) {
                foundName = false;
                foundFrames = false;
                foundImage = false;
                foundShader = false;
                v.push_back(gsi);
            }
            continue; // get next line
        }
    }

    return v;
}