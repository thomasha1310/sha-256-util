#include <cstdint>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

const uint32_t K[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};

uint32_t H[8] = {
    0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a, 0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19};

/**
 * Rotates the input x to the right by n places.
 * in:  10111000001110000101001001|001101
 * out: 001101|10111000001110000101001001
 */
uint32_t rotr(uint32_t x, uint32_t n)
{
    return (x >> n) | (x << (32 - n));
}

/**
 * The x input determines whether the output comes from y or z.
 *
 * x: 00001111
 * y: 00110011
 * z: 01010101
 *    --------
 *    01010011
 */
uint32_t choose(uint32_t x, uint32_t y, uint32_t z)
{
    return (x & y) ^ (~x & z);
}

/**
 * An output bit is 0 if two or more corresponding input bits are 0 and is 1 if two or more corresponding input bits are 1.
 *
 * x: 00001111
 * y: 00110011
 * z: 01010101
 *    --------
 *    00010111
 */
uint32_t majority(uint32_t x, uint32_t y, uint32_t z)
{
    return (x & y) ^ (x & z) ^ (y & z);
}

uint32_t Σ0(uint32_t x)
{
    return rotr(x, 2) ^ rotr(x, 13) ^ rotr(x, 22);
}

uint32_t Σ1(uint32_t x)
{
    return rotr(x, 6) ^ rotr(x, 11) ^ rotr(x, 25);
}

uint32_t ς0(uint32_t x)
{
    return rotr(x, 7) ^ rotr(x, 18) ^ (x >> 3);
}

uint32_t ς1(uint32_t x)
{
    return rotr(x, 17) ^ rotr(x, 19) ^ (x >> 10);
}

vector<uint8_t> padMessage(const string &message)
{
    vector<uint8_t> paddedMessage(message.begin(), message.end());

    // Append the '1' bit followed by 7 '0' bits.
    paddedMessage.push_back(0x80);

    // Append '00000000' until the length is 448 mod 512
    while ((paddedMessage.size() * 8 + 64) % 512 != 0)
    {
        paddedMessage.push_back(0x00);
    }

    // Append original message length byte-by-byte as a 64-bit big-endian integer.
    uint64_t messageLength = message.length() * 8;
    for (int i = 7; i >= 0; i--)
    {
        paddedMessage.push_back((messageLength >> (i * 8)) & 0xFF);
    }

    return paddedMessage;
}

// Expects a pointer to the first byte in a 64-byte block.
// *block
// ↓
// [00000001] [00000010] [00000100] [00001000] [00010000] [00100000] [01000000] [10000000] ...
// ↑          ↑          ↑          ↑
// i * 4      i * 4 + 1  i * 4 + 2  i * 4 + 3 ...
// for i = 0..15

void processBlock(const uint8_t *block)
{
    // Each word is a 32-bit (4-byte) piece of the block.
    uint32_t W[64];

    // Copy the 16 32-bit words in the block into the first 16 words of the message schedule.
    for (int i = 0; i < 16; i++)
    {
        W[i] = (block[i * 4] << 24) | (block[i * 4 + 1] << 16) | (block[i * 4 + 2] << 8) | (block[i * 4 + 3]);
    }

    // Expand the 16 words into 64 words.
    for (int i = 16; i < 64; i++)
    {
        W[i] = ς1(W[i - 2]) + W[i - 7] + ς0(W[i - 15]) + W[i - 16];
    }

    // Initialize working variables.
    uint32_t a = H[0], b = H[1], c = H[2], d = H[3], e = H[4], f = H[5], g = H[6], h = H[7];

    // Main compression loop.
    // ς1 Σ
    for (int i = 0; i < 64; i++)
    {
        int32_t temp1 = h + Σ1(e) + choose(e, f, g) + K[i] + W[i];
        int32_t temp2 = Σ0(a) + majority(a, b, c);
        h = g;
        g = f;
        f = e;
        e = d + temp1;
        d = c;
        c = b;
        b = a;
        a = temp1 + temp2;
    }

    // Update hash values.
    H[0] += a;
    H[1] += b;
    H[2] += c;
    H[3] += d;
    H[4] += e;
    H[5] += f;
    H[6] += g;
    H[7] += h;
}

// Compute a SHA-256 hash.
string sha256(const string &message)
{
    vector<uint8_t> paddedMessage = padMessage(message);

    // Process blocks of 512 bits (8 bits/byte * 64 bytes/block = 512 bits/block).
    for (size_t i = 0; i < paddedMessage.size(); i += 64)
    {
        // Passes the address of the i-th byte in the vector.
        processBlock(&paddedMessage[i]);
    }

    stringstream hash;
    for (uint32_t h : H)
    {
        hash << setw(8) << setfill('0') << hex << h;
    }
    return hash.str();
}

string readFile(const string &filepath)
{
    ifstream file(filepath, ios::in | ios::binary);

    if (!file)
    {
        cerr << "Error opening file " << filepath << endl;
        return "";
    }

    string content((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    return content;
}

#define newline "\n"

void usage()
{
    cerr << "Usage:" << newline
         << "sha256.exe <message>" << newline
         << "sha256.exe -m <message>" << newline
         << "sha256.exe -f <filepath>" << endl;
}

int main(int argc, char *argv[])
{
    if (argc == 1)
    {
        usage();
        return 1;
    }

    if (argc == 2)
    {
        string input = argv[1];
        cout << sha256(input) << endl;
        return 0;
    }

    if (argc == 3)
    {
        if (string(argv[1]) == "-m")
        {
            string input = argv[2];
            cout << sha256(input) << endl;
            return 0;
        }
        else if (string(argv[1]) == "-f")
        {
            string filepath = argv[2];
            string fileContent = readFile(filepath);
            cout << sha256(fileContent) << endl;
            return 0;
        }
        else
        {
            usage();
            return 1;
        }
    }

    cerr << "Expected up to two arguments and got three." << endl;
    return 1;
}
