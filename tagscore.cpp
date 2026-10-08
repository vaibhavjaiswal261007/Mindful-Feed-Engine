#include "tagscore.h"
#include <iostream>
#include <fstream>
#include <unordered_set>
#include <cctype>

using namespace std;

string HashFilter::normalizeTag(const string& tag) {
    int start = 0;
    int end = (int)tag.size() - 1;

    // To remove leading spaces
    while (start <= end && isspace(tag[start]))
    {
        start++;
    }
    // To remove spaces from the end
    while (end >= start && isspace(tag[end])) 
    {
        end--;
    }
    // To remove  # from the start 
    if (start <= end && tag[start] == '#')
        start++;
    // clean is the new string that contains the string in lower char
    string clean;
    for (int i = start; i <= end; ++i) {
        clean += tolower(tag[i]);
    }

    return clean;
}

void HashFilter::loadTagScores(const string& filename) {
    ifstream file(filename);

    if (!file.is_open()) {
        cout << "Could not open " << filename << endl;
        return;
    }
    // line will store each line of the file 
    string line;

    while (getline(file, line)) {
        int comma = line.find(',');//  line.find(',') will return iterator of ,

        string tag = normalizeTag(line.substr(0, comma));

        if (tag.empty())
            continue;
        tagScores[tag] = stoi(line.substr(comma + 1)); // stoi is used to convert string into int
        
    }
}


void HashFilter::displayHashMap() {
    cout << "\n===== Tag Score Hash Map =====\n";

    if (tagScores.empty()) {
        cout << "Hash map is empty.\n";
        return;
    }

    for (const auto& entry : tagScores) {
        cout << "#" << entry.first << " : " << entry.second << endl;
    }

    cout << "==============================\n";
}

void createSampleFile(const string& filename) {
    ofstream file(filename);
    if (file.is_open()) {
        file << "#coding, 100\n";
        file << "  C++ , 75 \n";       // Testing leading/trailing spaces
        file << "#Gaming, 50\n";      // Testing uppercase conversion
        file << "invalid_line, 98\n";     // Testing missing comma
        file << "music,20 \n";          // Testing missing score
        file << "  #study  , 90\n";
        file.close();
        cout << "Created sample file: " << filename << "\n";
    }
}

int main() {
    string filename = "tags_data.csv";

    // 1. Create a file to read from
    createSampleFile(filename);

    // 2. Create an instance of your HashFilter class
    HashFilter filter;

    // 3. Call the load function
    cout << "Loading data into Hash Map...\n";
    filter.loadTagScores(filename);

    // 4. Call the display function to print the hash map
    filter.displayHashMap();

    return 0;
}