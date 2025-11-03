#ifndef CODEBOOK_H
#define CODEBOOK_H

#include <cstdint>
#include <map>
#include <vector>
#include <string>

struct CodebookEntry {
    uint32_t bgra;
    uint16_t codeword;
    uint8_t codewordLen;  // in bits
    uint8_t tier;         // 1, 2, or 3
};

class Codebook {
public:
    // Build codebook from frequency list
    void buildFromFrequencies(const std::vector<std::pair<uint32_t, uint64_t>>& freqList);

    // Encode a pixel to (codeword, bitLen) or escape code
    bool encodePixel(uint32_t bgra, uint16_t& codeword, uint8_t& bitLen) const;

    // Decode codeword back to BGRA
    uint32_t decodeCodeword(uint16_t codeword, uint8_t bitLen) const;

    // Get codebook entries
    const std::vector<CodebookEntry>& getEntries() const { return entries_; }

    // Tier 3 escape code
    uint16_t getEscapeCode() const { return ESCAPE_CODE; }
    uint8_t getEscapeCodeLen() const { return 10; }

private:
    std::vector<CodebookEntry> entries_;
    std::map<uint32_t, CodebookEntry> pixelToCode_;

    static constexpr uint16_t ESCAPE_CODE = 0x3FF;  // 10 bits all 1s for escape
};

#endif // CODEBOOK_H
