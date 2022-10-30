#include <iostream>
#include <fstream>
#include <string>
#include "../include/CompilerGlobals.h"

// External functions and variables
extern FILE *yyin;
extern int yyparse(void);
extern bool initializeGlobals(const std::string& input_file);
extern void cleanupGlobals();

/**
 * Print the compiler banner
 */
void printBanner() {
    std::cout << "=========================================================" << std::endl;
    std::cout << "  " << COMPILER_NAME << " v" << COMPILER_VERSION << std::endl;
    std::cout << "  Intermediate Code Generator" << std::endl;
    std::cout << "=========================================================" << std::endl;
}

/**
 * Main entry point for the compiler
 *
 * @param argc Number of command line arguments
 * @param argv Array of command line arguments
 * @return 0 if compilation was successful, non-zero otherwise
 */
int main(int argc, char *argv[]) {
    // Check if input file is provided
    if (argc < 2) {
        std::cerr << "Error: No input file specified" << std::endl;
        std::cerr << "Usage: " << argv[0] << " <input_file.c>" << std::endl;
        return 1;
    }

    // Print compiler banner
    printBanner();
    
    // Initialize globals and open files
    if (!initializeGlobals(argv[1])) {
        return 1;
    }
    
    // Open input file
    FILE *input_file = fopen(argv[1], "r");
    if (input_file == nullptr) {
        std::cerr << "Error: Could not open input file " << argv[1] << std::endl;
        cleanupGlobals();
        return 1;
    }
    
    // Set input for flex
    yyin = input_file;
    
    std::cout << "Compiling " << argv[1] << "..." << std::endl;
    
    // Parse input file
    int parse_result = yyparse();
    
    // Close input file
    fclose(input_file);
    
    // Clean up resources
    cleanupGlobals();
    
    // Check if parsing was successful
    if (parse_result == 0 && error_count == 0) {
        std::cout << "Compilation successful. Assembly code generated in optimized.asm" << std::endl;
        return 0;
    } else {
        std::cout << "Compilation failed with " << error_count << " errors." << std::endl;
        std::cout << "Check log.txt for details." << std::endl;
        return 1;
    }
} 