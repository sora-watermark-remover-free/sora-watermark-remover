#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>
#include "SoraWatermarkRemover.h"

static void printHelp(const char* prog) {
    std::cout << "Usage: " << prog << " -i input -o output [options]\n"
              << "Options:\n"
              << "  -i, --input <file>        Input image (jpg/png/...)\n"
              << "  -o, --output <file>       Output image path\n"
              << "  --mask <file>             Optional user mask (white=remove, black=keep)\n"
              << "  --inpaint-radius <int>    Inpainting radius (default 3)\n"
              << "  --alpha-threshold <int>   Alpha threshold for RGBA images (default 250)\n"
              << "  --white-threshold <int>   Threshold for near-white detection (default 240)\n"
              << "  -h, --help                Show this help\n";
}

int main(int argc, char** argv) {
    if (argc < 2) {
        printHelp(argv[0]);
        return 1;
    }

    std::string inputPath, outputPath, maskPath;
    SoraWatermarkRemover::Params params;

    for (int i = 1; i < argc; ++i) {
        std::string a = argv[i];
        if (a == "-i" || a == "--input") {
            if (i + 1 < argc) inputPath = argv[++i];
        } else if (a == "-o" || a == "--output") {
            if (i + 1 < argc) outputPath = argv[++i];
        } else if (a == "--mask") {
            if (i + 1 < argc) maskPath = argv[++i];
        } else if (a == "--inpaint-radius") {
            if (i + 1 < argc) params.inpaintRadius = std::stoi(argv[++i]);
        } else if (a == "--alpha-threshold") {
            if (i + 1 < argc) params.alphaThreshold = std::stoi(argv[++i]);
        } else if (a == "--white-threshold") {
            if (i + 1 < argc) params.whiteThreshold = std::stoi(argv[++i]);
        } else if (a == "-h" || a == "--help") {
            printHelp(argv[0]);
            return 0;
        } else {
            std::cerr << "Unknown arg: " << a << "\n";
            printHelp(argv[0]);
            return 1;
        }
    }

    if (inputPath.empty() || outputPath.empty()) {
        std::cerr << "Input and output required.\n";
        printHelp(argv[0]);
        return 1;
    }

    cv::Mat src = cv::imread(inputPath, cv::IMREAD_UNCHANGED);
    if (src.empty()) {
        std::cerr << "Failed to read input: " << inputPath << "\n";
        return 1;
    }

    SoraWatermarkRemover remover(params);

    cv::Mat userMask;
    if (!maskPath.empty()) {
        userMask = cv::imread(maskPath, cv::IMREAD_UNCHANGED);
        if (userMask.empty()) {
            std::cerr << "Failed to read mask: " << maskPath << "\n";
            return 1;
        }
    }

    cv::Mat out;
    bool ok = remover.remove(src, out, userMask);
    if (!ok) {
        std::cerr << "Removal failed.\n";
        return 1;
    }

    if (!cv::imwrite(outputPath, out)) {
        std::cerr << "Failed to write output: " << outputPath << "\n";
        return 1;
    }

    std::cout << "Saved: " << outputPath << "\n";
    return 0;
}
