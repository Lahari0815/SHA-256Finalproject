#include <iostream>
#include <string>
#include <vector>
#include <cstdint>
#include <sstream>
#include <iomanip>
#include <fstream>
using namespace std; 


class SHA256 {
private:
    uint32_t h[8];
    
    static constexpr uint32_t k[64] = {
        0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
        0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
        0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
        0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
        0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
        0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
        0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
        0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
    };

    static uint32_t rightRotate(uint32_t value, unsigned int count) {
        return (value >> count) | (value << (32 - count));
    }

    void processChunk(const uint8_t* chunk) {
        uint32_t w[64];
        for (int i = 0; i < 16; ++i) {
            w[i] = (chunk[i*4] << 24) | (chunk[i*4+1] << 16) | (chunk[i*4+2] << 8) | chunk[i*4+3];
        }

        for (int i = 16; i < 64; ++i) {
            uint32_t s0 = rightRotate(w[i-15], 7) ^ rightRotate(w[i-15], 18) ^ (w[i-15] >> 3);
            uint32_t s1 = rightRotate(w[i-2], 17) ^ rightRotate(w[i-2], 19) ^ (w[i-2] >> 10);
            w[i] = w[i-16] + s0 + w[i-7] + s1;
        }

        uint32_t a = h[0], b = h[1], c = h[2], d = h[3],
                 e = h[4], f = h[5], g = h[6], h_temp = h[7];

        for (int i = 0; i < 64; ++i) {
            uint32_t S1 = rightRotate(e, 6) ^ rightRotate(e, 11) ^ rightRotate(e, 25);
            uint32_t ch = (e & f) ^ ((~e) & g);
            uint32_t temp1 = h_temp + S1 + ch + k[i] + w[i];
            uint32_t S0 = rightRotate(a, 2) ^ rightRotate(a, 13) ^ rightRotate(a, 22);
            uint32_t maj = (a & b) ^ (a & c) ^ (b & c);
            uint32_t temp2 = S0 + maj;

            h_temp = g;
            g = f;
            f = e;
            e = d + temp1;
            d = c;
            c = b;
            b = a;
            a = temp1 + temp2;
        }

        h[0] += a; h[1] += b; h[2] += c; h[3] += d;
        h[4] += e; h[5] += f; h[6] += g; h[7] += h_temp;
    }

public:
    SHA256() {
        h[0] = 0x6a09e667;
        h[1] = 0xbb67ae85;
        h[2] = 0x3c6ef372;
        h[3] = 0xa54ff53a;
        h[4] = 0x510e527f;
        h[5] = 0x9b05688c;
        h[6] = 0x1f83d9ab;
        h[7] = 0x5be0cd19;
    }

    string hash(const string& message) {
        vector<uint8_t> padded_message(message.begin(), message.end());
        uint64_t original_length_bits = message.length() * 8;

        // Padding
        padded_message.push_back(0x80);
        while ((padded_message.size() + 8) % 64 != 0) {
            padded_message.push_back(0x00);
        }

        // Append original length
        for (int i = 7; i >= 0; --i) {
            padded_message.push_back((original_length_bits >> (i * 8)) & 0xFF);
        }

        // Process chunks
        for (size_t i = 0; i < padded_message.size(); i += 64) {
            processChunk(&padded_message[i]);
        }

        // Produce final hash value
        stringstream ss;
        for (int i = 0; i < 8; ++i) {
            ss << hex << setw(8) << setfill('0') << h[i];
        }
        return ss.str();
    }
};

// Function to read the contents of a file into a string
string readFromFile(const string& filename) {
    ifstream file(filename);
    if (!file) {
        cerr << "Unable to open file: " << filename << endl;
        return "";
    }

    stringstream buffer;
    buffer << file.rdbuf(); // Read file content into a stringstream
    return buffer.str(); // Convert stringstream into string
}


int main() {
    SHA256 sha256;

    // string input = "helloworld";

    // Read content from the file "book_of_mark.txt"
    string input = readFromFile("book_of_mark.txt");

    if (input.empty()) {
        cerr << "Failed to read file or file is empty." << endl;
        return 1; // Exit if file reading fails
    }

    string hash = sha256.hash(input);
    //cout << "SHA-256 hash of '" << input << "': " << hash << endl;
    cout << "SHA-256 hash of the entire book of Mark is : " << hash << endl;
    return 0;
}