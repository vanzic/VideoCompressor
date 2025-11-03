#ifndef PIXEL_ANALYZER_H
#define PIXEL_ANALYZER_H

#include <cstdint>
#include <map>
#include <vector>
#include <string>

struct PixelFreq {
    uint32_t bgra;
    uint64_t frequency;
};

class PixelAnalyzer {
public:
    // Analyze frames and build frequency map
    void analyzeFrames(const std::vector<std::string>& frameFiles);
    
    // Get sorted pixel frequency list
    std::vector<PixelFreq> getFrequencyList() const;
    
    // Get total pixels analyzed
    uint64_t getTotalPixels() const { return totalPixels_; }

private:
    std::map<uint32_t, uint64_t> frequencyMap_;
    uint64_t totalPixels_;
};

#endif // PIXEL_ANALYZER_H
