#include "Encoder.h"
#include <cstring>

Encoder::Encoder(const Codebook& codebook) 
    : codebook_(codebook), escapeCount_(0), symbolCount_(0) {}

void Encoder::writeBits(std::vector<uint8_t>& bitstream, uint32_t value, int bits, int& bitPos) {
    for (int i = bits - 1; i >= 0; --i) {
        int byteIdx = bitPos / 8;
        int bitIdx = 7 - (bitPos % 8);

        if (byteIdx >= static_cast<int>(bitstream.size())) {
            bitstream.push_back(0);
        }

        if ((value >> i) & 1) {
            bitstream[byteIdx] |= static_cast<uint8_t>(1 << bitIdx);
        }
        bitPos++;
    }
}

void Encoder::encodeFrame(const uint8_t* rawFrame, std::vector<uint8_t>& bitstream) {
    bitstream.clear();
    escapeCount_ = 0;
    symbolCount_ = 0;
    int bitPos = 0;

    const uint32_t* pixels = reinterpret_cast<const uint32_t*>(rawFrame);
    for (size_t i = 0; i < 3840 * 2160; ++i) {
        uint32_t bgra = pixels[i];
        uint16_t codeword;
        uint8_t bitLen;

        if (codebook_.encodePixel(bgra, codeword, bitLen)) {
            // Encoded as symbol
            writeBits(bitstream, codeword, bitLen, bitPos);
            symbolCount_++;
        } else {
            // Escape code + raw 32-bit pixel
            writeBits(bitstream, codebook_.getEscapeCode(), 10, bitPos);
            writeBits(bitstream, bgra, 32, bitPos);
            escapeCount_++;
        }
    }
}
