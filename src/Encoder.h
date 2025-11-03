#ifndef ENCODER_H
#define ENCODER_H

#include "Codebook.h"
#include <vector>
#include <cstdint>

class Encoder {
public:
    explicit Encoder(const Codebook& codebook);

    // Encode frame to bitstream
    void encodeFrame(const uint8_t* rawFrame, std::vector<uint8_t>& bitstream);

    // Get statistics
    size_t getEscapeCount() const { return escapeCount_; }
    size_t getSymbolCount() const { return symbolCount_; }

private:
    const Codebook& codebook_;
    size_t escapeCount_;
    size_t symbolCount_;

    void writeBits(std::vector<uint8_t>& bitstream, uint32_t value, int bits, int& bitPos);
};

#endif // ENCODER_H
