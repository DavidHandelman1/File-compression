#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <string>

using namespace std;

struct Node {
    char ch;
    int count;
    Node* l_ptr = nullptr;
    Node* r_ptr = nullptr;
};

extern "C" {

    // Function to insert a new element into min heap
    void insert_min_heap(vector<Node*>& heap, char ch, int count)
    {
        // Add the new element to the end of the heap
        Node* new_node = new Node{ ch, count };
        heap.push_back(new_node);
        // Get the index of the last element
        int index = heap.size() - 1;
        // Compare the new element with its parent and swap if necessary
        while (index > 0 && heap[(index - 1) / 2]->count > heap[index]->count) {
            swap(heap[index], heap[(index - 1) / 2]);
            // Move up the tree to the parent of the current element
            index = (index - 1) / 2;
        }
    }

    // Function to remove_node Node at specified index
    void remove_node(vector<Node*>& heap, int index) {
        // Move Node at index to end of heap
        heap[index] = heap[heap.size() - 1];
        // Delete last Node in heap
        heap.pop_back();

        // Heapify the tree starting from the element at the deleted index
        while (true) {
            int left_child = 2 * index + 1;
            int right_child = 2 * index + 2;
            int smallest = index;
            if (left_child < heap.size()
                && heap[left_child]->count < heap[smallest]->count) {
                smallest = left_child;
            }
            if (right_child < heap.size()
                && heap[right_child]->count < heap[smallest]->count) {
                smallest = right_child;
            }
            if (smallest != index) {
                swap(heap[index], heap[smallest]);
                index = smallest;
            }
            else {
                break;
            }
        }
    }

    // Function to transform min heap into huffman binary tree
    Node* build_huffmanTree(vector<Node*>& heap) {
        while (heap.size() > 1) {
            // get min twice and adjust heap to maintain min heap properties
            Node* left = heap[0];
            remove_node(heap, 0);
            Node* right = heap[0];
            remove_node(heap, 0);

            // create node with using 3 for NOCHAR and add min values
            Node* new_node = new Node{ '\0', left->count + right->count };
            new_node->l_ptr = left;
            new_node->r_ptr = right;
            // push new node into bottom of min heap
            heap.push_back(new_node);
        }

        return heap[0]; // return root of tree
    }

    // Function to delete all allocated memory from Nodes in heap
    void freeNodes(Node* root) {
        if (root == nullptr) {
            return;
        }
        Node* left = root->l_ptr;
        Node* right = root->r_ptr;
        // delete Node
        delete root;
        // recursively go to children to delete all Nodes
        freeNodes(left);
        freeNodes(right);
    }

    // Function to print all chars and counts in huffman tree
    void encode(Node* root, map<char, string>& encode_map, string code = "", char c = '-') {
        if (root == nullptr) {
            return;
        }

        if (root->ch != '\0') {
            // each character (and EOF) will have a corresponding string code of bits
            encode_map[root->ch] = code;
        }

        encode(root->l_ptr, encode_map, code + "0", root->ch);
        encode(root->r_ptr, encode_map, code + "1", root->ch);
    }

    // Function to convert string of bits from encode map and append it to vector of bools
    void string_to_bool(const string& bits, vector<bool>& compressed_bits) {
        for (int i = 0; i < bits.size(); ++i) {
            compressed_bits.push_back(bits[i] == '1' ? true : false);
        }
    }

    // Function to write a string to a binary file
    void writeChar(ofstream& out, const char ch) {
        // Write the character to the binary file
        out.write(reinterpret_cast<const char*>(&ch), sizeof(ch));
    }  

    // Function to write an int to a binary file
    void writeString(ofstream& out, const string& str) {
        size_t length = str.size();
        out.write(reinterpret_cast<const char*>(&length), sizeof(length)); // Write the length of the string
        out.write(str.c_str(), length); // Write the string itself
    }  

    // Function to output encoded map and compressed bits to output file
    void compress(ofstream& out, map<char, string>& encode_map, const string& text) {
        vector<bool> compressed_bits;
        string bits;
        char newline = '\n';

        int i = 0, digit = 0;
        while (text[i]) {
            string_to_bool(encode_map[text[i]], compressed_bits);
            ++i;
        }
        // append EOF which is 3
        string_to_bool(encode_map[3], compressed_bits);

        // if number of bits != multiple of 8, append false (0) to fix
        int remainder = compressed_bits.size() % 8;
        if (remainder != 0) {
            remainder = 8 - remainder;
            for (int i = 0; i < remainder; ++i) {
                compressed_bits.push_back(false);
            }
        }

        for (const auto& pair : encode_map) {
            writeChar(out, pair.first);  // Write the key
            writeString(out, pair.second); // Write the value
            out.write(&newline, sizeof(newline));
        }

        out.write(&newline, sizeof(newline));
        out.write(&newline, sizeof(newline));

        int index = 0;
        while (index < compressed_bits.size()) {
            unsigned char byte = 0;
            for (int i = 0; i < 8; ++i) {
                // fancy bit manipulation to pack 8 bits into 1 byte
                byte = (byte << 1) | static_cast<unsigned char>(compressed_bits[index + i]);
            }
            
            // Write byte to binary file
            out.write(reinterpret_cast<const char*>(&byte), sizeof(byte));
            // go to next 8 bits in vector of booleans
            index += 8;
        }

        out.write(&newline, sizeof(newline));
        out.write(&newline, sizeof(newline));
    }

    // Function to convert .txt file into .bin file through compression
    __declspec(dllexport) void compress_file(const char* in_file, const char* out_file) {
        ifstream input_file(in_file);

        if (!input_file.is_open()) {
            cerr << "Error opening file\n";
            return; // Exit program, failure to open
        }

        map<char, int> hashmap;
        string text;
        char ch;
        while (input_file.get(ch)) {
            text += ch;
            hashmap[ch] += 1;
        }
        hashmap[3] = 1;  // EOF file represented by 3 on ASCII table

        // create min heap 
        vector<Node*> minHeap;
        for (const auto& pair : hashmap) {
            insert_min_heap(minHeap, pair.first, pair.second);
        }

        Node* root = build_huffmanTree(minHeap);

        // map each character to the binary code as a string of 0's and 1's
        map<char, string> encode_map;
        encode(minHeap[0], encode_map);
        // dynamically delete memory in min heap tree
        freeNodes(minHeap[0]);

        ofstream output_file(out_file, ios::binary);

        if (!output_file) {
            cerr << "Error opening file\n";
            return; // Exit program, failure to open
        }

        string line;

        // will output encoded map char codes, as well as bits compressed
        compress(output_file, encode_map, text);

        input_file.close();
        output_file.close();
    }
    
}