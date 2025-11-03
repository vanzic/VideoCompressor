#include "Codebook.h"
#include <algorithm>
#include <iostream>

void Codebook::buildFromFrequencies(const std::vector<std::pair<uint32_t, uint64_t>>& freqList) {
    entries_.clear();
    pixelToCode_.clear();

    const size_t TIER1_MAX = 256;
    const size_t TIER2_MAX = TIER1_MAX + 32768;

    for (size_t i = 0; i < freqList.size(); ++i) {
        uint32_t bgra = freqList[i].first;
        uint16_t codeword;
        uint8_t bitLen;
        uint8_t tier;

        // Assign tier and codeword length
        if (i < TIER1_MAX) {
            tier = 1;
            bitLen = 8;
            codeword = static_cast<uint16_t>(i);
        } else if (i < TIER2_MAX) {
            tier = 2;
            bitLen = 16;
            codeword = static_cast<uint16_t>(i);
        } else {
            tier = 3;
            bitLen = 0;  // Escape code + raw 32-bit
            codeword = ESCAPE_CODE;
            // For tier 3, we'll use escape + raw, so we stop adding entries
            break;
        }

        CodebookEntry entry{bgra, codeword, bitLen, tier};
        entries_.push_back(entry);
        pixelToCode_[bgra] = entry;
    }

    std::cout << "Codebook built: " << entries_.size() << " entries (Tier1: " 
              << TIER1_MAX << ", Tier2: " << (TIER2_MAX - TIER1_MAX) << ")\n";
}

bool Codebook::encodePixel(uint32_t bgra, uint16_t& codeword, uint8_t& bitLen) const {
    auto it = pixelToCode_.find(bgra);
    if (it != pixelToCode_.end()) {
        codeword = it->second.codeword;
        bitLen = it->second.codewordLen;
        return true;
    }
    // Not in codebook, use escape code
    codeword = ESCAPE_CODE;
    bitLen = 10;
    return false;
}

uint32_t Codebook::decodeCodeword(uint16_t codeword, uint8_t bitLen) const {
    // Lookup in entries
    for (const auto& entry : entries_) {
        if (entry.codeword == codeword && entry.codewordLen == bitLen) {
            return entry.bgra;
        }
    }
    return 0;  // Fallback
}
