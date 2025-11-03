#include "PixelAnalyzer.h"
#include "Codebook.h"
#include "Encoder.h"
#include "Decoder.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <chrono>
#include <sstream>
#include <cstring>

int main() {
    // Step 1: Analyze pixel frequencies from first 10 frames
    std::cout << "========== Step 1: Analyzing Pixel Frequencies ==========\n";
    PixelAnalyzer analyzer;
    std::vector<std::string> frameFiles;
    for (int i = 1; i <= 10; ++i) {
        std::stringstream ss;
        ss << "test_data/frame_" << std::setw(4) << std::setfill('0') << i << ".raw";
        frameFiles.push_back(ss.str());
    }
    analyzer.analyzeFrames(frameFiles);
    auto freqList = analyzer.getFrequencyList();
    std::cout << "Total unique pixel values: " << freqList.size() << "\n";
    std::cout << "Total pixels analyzed: " << analyzer.getTotalPixels() << "\n\n";

    // Print top 10 pixels
    std::cout << "Top 10 pixel values by frequency:\n";
    for (size_t i = 0; i < 10 && i < freqList.size(); ++i) {
        double percentage = (100.0 * freqList[i].frequency) / analyzer.getTotalPixels();
        std::cout << "  " << i + 1 << ". 0x" << std::hex << std::setw(8) << std::setfill('0') 
                  << freqList[i].bgra << std::dec << " : " << freqList[i].frequency 
                  << " (" << std::fixed << std::setprecision(2) << percentage << "%)\n";
    }
    std::cout << "\n";

    // Step 2: Build tiered codebook
    std::cout << "========== Step 2: Building Tiered Codebook ==========\n";
    Codebook codebook;
    std::vector<std::pair<uint32_t, uint64_t>> freqPairs;
    for (const auto& pf : freqList) {
        freqPairs.push_back({pf.bgra, pf.frequency});
    }
    codebook.buildFromFrequencies(freqPairs);
    std::cout << "Codebook entries: " << codebook.getEntries().size() << "\n\n";

    // Step 3: Encode and decode frames, measure compression
    std::cout << "========== Step 3: Encoding Frames ==========\n";
    Encoder encoder(codebook);
    Decoder decoder(codebook);

    double totalOriginalSize = 0;
    double totalEncodedSize = 0;
    size_t totalSymbols = 0;
    size_t totalEscapes = 0;

    for (int i = 1; i <= 258; ++i) {
        std::stringstream ss;
        ss << "test_data/frame_" << std::setw(4) << std::setfill('0') << i << ".raw";
        std::string filename = ss.str();

        std::ifstream input(filename, std::ios::binary);
        if (!input) {
            std::cerr << "Cannot open " << filename << "\n";
            continue;
        }

        std::vector<uint8_t> rawFrame(3840 * 2160 * 4);
        input.read(reinterpret_cast<char*>(rawFrame.data()), static_cast<std::streamsize>(rawFrame.size()));
        input.close();

        std::vector<uint8_t> encoded;
        auto t1 = std::chrono::high_resolution_clock::now();
        encoder.encodeFrame(rawFrame.data(), encoded);
        auto t2 = std::chrono::high_resolution_clock::now();
        auto encTime = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();

        double ratio = static_cast<double>(rawFrame.size()) / encoded.size();
        totalOriginalSize += rawFrame.size();
        totalEncodedSize += encoded.size();
        totalSymbols += encoder.getSymbolCount();
        totalEscapes += encoder.getEscapeCount();

        std::cout << "Frame " << std::setw(3) << i
                  << ": orig=" << rawFrame.size()
                  << ", enc=" << encoded.size()
                  << ", ratio=" << std::fixed << std::setprecision(3) << ratio
                  << ", sym=" << encoder.getSymbolCount()
                  << ", esc=" << encoder.getEscapeCount()
                  << ", ms=" << encTime << "\n";

        // Optional: test decoding (uncomment to verify losslessness)
        /*
        std::vector<uint8_t> decoded(3840 * 2160 * 4);
        decoder.decodeFrame(encoded, decoded.data());
        if (std::memcmp(rawFrame.data(), decoded.data(), rawFrame.size()) == 0) {
            std::cout << "  [OK] Decode verified lossless\n";
        } else {
            std::cout << "  [ERROR] Decode mismatch!\n";
        }
        */
    }

    std::cout << "\n========== Final Statistics ==========\n";
    std::cout << "Total original size: " << totalOriginalSize << " bytes\n";
    std::cout << "Total encoded size: " << totalEncodedSize << " bytes\n";
    std::cout << "Overall compression ratio: " << std::fixed << std::setprecision(3) 
              << (totalOriginalSize / totalEncodedSize) << "\n";
    std::cout << "Total symbols encoded: " << totalSymbols << "\n";
    std::cout << "Total escapes used: " << totalEscapes << "\n";
    std::cout << "Symbol hit rate: " << std::fixed << std::setprecision(2)
              << (100.0 * totalSymbols / (totalSymbols + totalEscapes)) << "%\n";

    return 0;
}
