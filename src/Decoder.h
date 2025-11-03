#ifndef DECODER_H
#define DECODER_H

#include "Codebook.h"
#include <vector>
#include <cstdint>

class Decoder {
public:
    explicit Decoder(const Codebook& codebook);

    // Decode bitstream back to raw frame
    void decodeFrame(const std::vector<uint8_t>& bitstream, uint8_t* rawFrame);

private:
    const Codebook& codebook_;

    uint32_t readBits(const std::vector<uint8_t>& bitstream, int& bitPos, int bits);
};

#endif // DECODER_H
