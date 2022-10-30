#include "../include/CompilerGlobals.h"

// Global file streams
std::ofstream log;
std::ofstream code;
std::ofstream optimized;

// Error tracking
int error_count = 0;

// Global variable for assembly code generation
std::vector<std::string> asmVarList;
std::vector<std::pair<std::string, int>> asmArrList;

/**
 * Initializes global variables and opens necessary files
 * 
 * @param input_file The input C file
 * @return True if initialization was successful, false otherwise
 */
bool initializeGlobals(const std::string& input_file) {
    // Open log file
    log.open("log.txt");
    if (!log.is_open()) {
        std::cerr << "Error: Could not open log file" << std::endl;
        return false;
    }
    
    // Open code file
    code.open("code.asm");
    if (!code.is_open()) {
        std::cerr << "Error: Could not open code.asm file" << std::endl;
        log.close();
        return false;
    }
    
    // Open optimized code file
    optimized.open("optimized.asm");
    if (!optimized.is_open()) {
        std::cerr << "Error: Could not open optimized.asm file" << std::endl;
        log.close();
        code.close();
        return false;
    }
    
    // Reset tracking variables
    error_count = 0;
    asmVarList.clear();
    asmArrList.clear();
    
    return true;
}

/**
 * Cleans up resources when compiler is done
 */
void cleanupGlobals() {
    // Close all open files
    if (log.is_open()) log.close();
    if (code.is_open()) code.close();
    if (optimized.is_open()) optimized.close();
} 