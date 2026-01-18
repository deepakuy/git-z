#include "Hash.h"
#include <cstring>
#include <cstdint>
#include <sstream>
#include <iomanip>
#include <vector>

namespace gitz
{
    namespace
    {
        // SHA-1 constants
        constexpr uint32_t K[] = {
            0x5A827999, 0x6ED9EBA1, 0x8F1BBCDC, 0xCA62C1D6
        };

        // Left rotate operation
        inline uint32_t leftRotate(uint32_t value, int count)
        {
            return (value << count) | (value >> (32 - count));
        }

        // SHA-1 functions
        inline uint32_t f(int round, uint32_t x, uint32_t y, uint32_t z)
        {
            if (round < 20) {
                return (x & y) | (~x & z);
            } else if (round < 40) {
                return x ^ y ^ z;
            } else if (round < 60) {
                return (x & y) | (x & z) | (y & z);
            } else {
                return x ^ y ^ z;
            }
        }

        inline uint32_t getK(int round)
        {
            if (round < 20) return 0x5A827999;
            if (round < 40) return 0x6ED9EBA1;
            if (round < 60) return 0x8F1BBCDC;
            return 0xCA62C1D6;
        }

        // Convert big-endian bytes to uint32_t
        inline uint32_t bytesToUint32(const uint8_t* bytes)
        {
            return (static_cast<uint32_t>(bytes[0]) << 24) |
                   (static_cast<uint32_t>(bytes[1]) << 16) |
                   (static_cast<uint32_t>(bytes[2]) << 8) |
                   (static_cast<uint32_t>(bytes[3]));
        }

        // Convert uint32_t to big-endian bytes
        inline void uint32ToBytes(uint32_t value, uint8_t* bytes)
        {
            bytes[0] = static_cast<uint8_t>(value >> 24);
            bytes[1] = static_cast<uint8_t>(value >> 16);
            bytes[2] = static_cast<uint8_t>(value >> 8);
            bytes[3] = static_cast<uint8_t>(value);
        }
    }

    std::string Hash::sha1(const std::string& input)
    {
        // Initialize hash values
        uint32_t h0 = 0x67452301;
        uint32_t h1 = 0xEFCDAB89;
        uint32_t h2 = 0x98BADCFE;
        uint32_t h3 = 0x10325476;
        uint32_t h4 = 0xC3D2E1F0;

        // Pre-processing: padding
        uint64_t msgLen = input.length();
        uint64_t msgLenBits = msgLen * 8;

        // Create padded message
        std::vector<uint8_t> message;
        message.insert(message.end(), input.begin(), input.end());
        message.push_back(0x80);  // Append '1' bit (as 0x80 in byte)

        // Pad with zeros until message length ≡ 56 (mod 64)
        while ((message.size() % 64) != 56) {
            message.push_back(0x00);
        }

        // Append original message length as 64-bit big-endian
        uint8_t lenBytes[8];
        for (int i = 0; i < 8; ++i) {
            lenBytes[7 - i] = static_cast<uint8_t>(msgLenBits >> (i * 8));
        }
        message.insert(message.end(), lenBytes, lenBytes + 8);

        // Process message in 512-bit (64-byte) blocks
        for (size_t blockStart = 0; blockStart < message.size(); blockStart += 64) {
            uint32_t w[80];

            // Parse block into 16 32-bit words (big-endian)
            for (int i = 0; i < 16; ++i) {
                w[i] = bytesToUint32(&message[blockStart + i * 4]);
            }

            // Extend 16 32-bit words into 80 32-bit words
            for (int i = 16; i < 80; ++i) {
                w[i] = leftRotate(w[i - 3] ^ w[i - 8] ^ w[i - 14] ^ w[i - 16], 1);
            }

            // Initialize working variables
            uint32_t a = h0;
            uint32_t b = h1;
            uint32_t c = h2;
            uint32_t d = h3;
            uint32_t e = h4;

            // Main loop: 80 rounds
            for (int i = 0; i < 80; ++i) {
                uint32_t temp = leftRotate(a, 5) + f(i, b, c, d) + e + w[i] + getK(i);
                e = d;
                d = c;
                c = leftRotate(b, 30);
                b = a;
                a = temp;
            }

            // Add this block's hash to result so far
            h0 += a;
            h1 += b;
            h2 += c;
            h3 += d;
            h4 += e;
        }

        // Convert final hash to hexadecimal string
        std::ostringstream oss;
        uint32_t hash[5] = {h0, h1, h2, h3, h4};
        for (int i = 0; i < 5; ++i) {
            oss << std::hex << std::setfill('0') << std::setw(8) << hash[i];
        }

        return oss.str();
    }
}
