#include <iostream>
#include <fstream>
#include <map>
#include <string>

using namespace std;

extern "C" {

    // Function to convert .bin file into .txt file through decompression
    __declspec(dllexport) void decompress_file(const char* in_file, const char* out_file) {
        // read from binary file
    ifstream input_file(in_file, ios::binary);

    if (!input_file.is_open()) {
            cerr << "Error opening file\n";
            return; // Exit program, failure to open
    }

    char ch;
    string line;
    while (input_file.get(ch)) {
        line += ch;
    }    
    
    map<string, char> map;
    bool new_flag = false;
    string bits;
    int i = 0;
    while (!line.empty()) {
        char value = line[i];   // char value
        // if there was a newline in text file
        if (value == '\n' && line[i + 1] == '\n') {
            // double newline represents end of char counts 
            break;
        }
        ++i;
        // get rid of junk characters like stx and nul
        while (line[i] != '0' && line[i] != '1') {
            ++i;
        }
        // read 0's and 1s into string for use in map
        while (line[i] == '0' || line[i] == '1') {
            char bit = line[i];
            bits += bit;
            ++i;
        }
        // add key and value to map: <char, string>
        map[bits] = value;
        bits.clear();
        ++i;
    }
    // go to line with all the chars of 8 bits (0's and 1's)
    while (line[i] == '\n') {
        ++i;
    }

    ofstream output_file(out_file);

    if (!output_file) {
        cerr << "Error opening file\n";
        return; // Exit program, failure to open
    }

    string sequence;
    while (i < line.length()) {
        ch = line[i];
        for (int j = 7; j >= 0; --j) {
            // Shift the character to the right by i bits and check the least significant bit
            bool bit = (ch & (1 << j));
           // Convert bool to character and append to sequence
            sequence += (bit ? '1' : '0');
            auto it = map.find(sequence);
            if (it != map.end()) {
                if (map[sequence] == 3) {   // EOF stored as 3 on ASCII table
                    break;
                }
                output_file << map[sequence];   // outputs char from original file
                sequence.clear();    // clear string for next searching of binary code
            }
        }
        ++i;    // go to next byte of data to be read
    }
    }

}