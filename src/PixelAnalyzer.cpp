#include "PixelAnalyzer.h"
#include <fstream>
#include <algorithm>
#include <iostream>

void PixelAnalyzer::analyzeFrames(const std::vector<std::string>& frameFiles) {
    frequencyMap_.clear();
    totalPixels_ = 0;

    for (const auto& file : frameFiles) {
        std::ifstream input(file, std::ios::binary);
        if (!input) {
            std::cerr << "Warning: Cannot open " << file << "\n";
            continue;
        }

        std::vector<uint8_t> frame(3840 * 2160 * 4);
        input.read(reinterpret_cast<char*>(frame.data()), static_cast<std::streamsize>(frame.size()));
        input.close();

        // Read pixels as 32-bit BGRA values
        uint32_t* pixels = reinterpret_cast<uint32_t*>(frame.data());
        for (size_t i = 0; i < 3840 * 2160; ++i) {
            frequencyMap_[pixels[i]]++;
            totalPixels_++;
        }
    }
}

std::vector<PixelFreq> PixelAnalyzer::getFrequencyList() const {
    std::vector<PixelFreq> list;
    for (const auto& pair : frequencyMap_) {
        list.push_back({pair.first, pair.second});
    }
    // Sort by frequency descending
    std::sort(list.begin(), list.end(), [](const PixelFreq& a, const PixelFreq& b) {
        return a.frequency > b.frequency;
    });
    return list;
}
