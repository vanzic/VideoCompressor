#include "Decoder.h"

Decoder::Decoder(const Codebook& codebook) : codebook_(codebook) {}

uint32_t Decoder::readBits(const std::vector<uint8_t>& bitstream, int& bitPos, int bits) {
    uint32_t value = 0;
    for (int i = 0; i < bits; ++i) {
        int byteIdx = bitPos / 8;
        int bitIdx = 7 - (bitPos % 8);

        if (byteIdx < static_cast<int>(bitstream.size())) {
            if ((bitstream[byteIdx] >> bitIdx) & 1) {
                value |= (1U << (bits - 1 - i));
            }
        }
        bitPos++;
    }
    return value;
}

void Decoder::decodeFrame(const std::vector<uint8_t>& bitstream, uint8_t* rawFrame) {
    int bitPos = 0;
    uint32_t* pixels = reinterpret_cast<uint32_t*>(rawFrame);

    for (size_t i = 0; i < 3840 * 2160; ++i) {
        uint16_t codeword = static_cast<uint16_t>(readBits(bitstream, bitPos, 10));
        if (codeword == codebook_.getEscapeCode()) {
            // Read raw 32-bit pixel
            pixels[i] = readBits(bitstream, bitPos, 32);
        } else {
            // Decode from codebook
            pixels[i] = codebook_.decodeCodeword(codeword, 10);
        }
    }
}
