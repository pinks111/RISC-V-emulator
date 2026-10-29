#include <iostream>
#include <fstream>
#include <vector>
#include <optional>
#include <cstdint>
#include <iomanip>

#include "cpu.hpp"

constexpr uint64_t max_steps = 100'000'000;

std::optional<std::vector<uint8_t>> readFile(const std::string& filename) {
    std::ifstream file (filename, std::ios::binary);
    if (!file) {
        return std::nullopt;
    }
    return std::vector<uint8_t>(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <binary_file>" << std::endl;
        return 1;
    }

    auto program = readFile(argv[1]);
    if (!program) {
        std::cerr << "Failed to read file: " << argv[1] << std::endl;
        return 1;
    }

    CPU cpu;
    if (!cpu.memory().load_bytes(*program)) {
        std::cerr << "Failed to load program into memory." << std::endl;
        return 1;
    }

    uint64_t steps = 0;
    while (!cpu.is_halted()) {
        if (steps++ >= max_steps) {
            std::cerr << "Maximum execution steps reached." << std::endl;
            return 1;
        }
        cpu.step();
    }

    if (cpu.exited()) {
        return cpu.exit_code();
    }

    std::cerr << "error: execution stopped at pc=0x" << std::hex << cpu.pc();
    if (auto word = cpu.memory().read32(cpu.pc())) {
        std::cerr << " (instruction: 0x" << std::setw(8) << std::setfill('0') << *word << ")";
    }
    std::cerr << std::endl;
    return 1;
}