#ifndef ASM_UTILS_H
#define ASM_UTILS_H

#include "CompilerGlobals.h"
#include "SymbolTable/SymbolInfo.h"
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <stack>

// Forward declarations
class SymbolInfo;

// Global assembly-related variables
extern std::vector<std::string> asmVarList;
extern std::vector<std::pair<std::string, int>> asmArrList;
extern std::ofstream code, optimized;
extern int label_count;

/**
 * Manages temporary variables for code generation
 */
class VarManager {
private:
    int size = 0;
    std::stack<std::string> free;

public:
    /**
     * Gets a temporary variable for use in code generation
     * 
     * @return A string representing the name of a temporary variable
     */
    std::string getTempVar() {
        std::string tempVar;
        if (free.empty()) {
            tempVar = "temp" + std::to_string(size);
            size++;
            asmVarList.push_back(tempVar);
        } else {
            tempVar = free.top();
            free.pop();
        }
        return tempVar;
    }

    /**
     * Returns a temporary variable to the pool
     * 
     * @param tempVar The name of the temporary variable to free
     */
    void freeTempVar(std::string tempVar) {
        if (tempVar.substr(0, 4) == "temp") {
            free.push(tempVar);
        }
    }

    /**
     * Gets the current size of the variable pool
     * 
     * @return The size of the variable pool
     */
    int getSize() { 
        return size; 
    }
};

/**
 * Outputs the generated code from a symbol to the code file
 * 
 * @param sym Pointer to the symbol containing the code
 */
inline void printCode(SymbolInfo *sym) { 
    code << sym->getCode() << std::endl; 
}

/**
 * Generates a new unique label for assembly code
 * 
 * @return A string representing a unique label
 */
inline std::string newLabel() { 
    return "L" + std::to_string(label_count++); 
}

/**
 * Adds the data segment to the assembly output
 */
inline void addDataSegment() {
    code << ".MODEL MEDIUM \n.STACK 100H \n.DATA" << std::endl << std::endl;

    asmVarList.push_back("return_loc");
    for (std::string s : asmVarList) {
        code << s << " DW ?" << std::endl;
    }

    for (auto p : asmArrList) {
        code << p.first << " DW " << p.second << " DUP (?)" << std::endl;
    }
}

/**
 * Starts the code segment in the assembly output
 */
inline void startCodeSegment() { 
    code << std::endl << ".CODE" << std::endl; 
}

/**
 * Ends the code segment in the assembly output
 */
inline void endCodeSegment() { 
    code << std::endl << "END MAIN\n"; 
}

/**
 * Generates assembly code for moving data from one memory location to another
 * 
 * @param lhs Pointer to the destination symbol
 * @param rhs Pointer to the source symbol
 * @return The generated assembly code
 */
inline std::string memToMem(SymbolInfo *lhs, SymbolInfo *rhs) {
    std::string asmCode = "MOV AX, ";
    asmCode += rhs->getAsmVar() + "\n";
    asmCode += "MOV " + lhs->getAsmVar() + ", AX \n";
    return asmCode;
}

/**
 * Generates assembly code for moving a constant value to a memory location
 * 
 * @param lhs Pointer to the destination symbol
 * @param constVal Pointer to the constant value symbol
 * @return The generated assembly code
 */
inline std::string constToMem(SymbolInfo *lhs, SymbolInfo *constVal) {
    std::string asmCode = "MOV AX, ";
    asmCode += constVal->getName() + "\n";
    asmCode += "MOV " + lhs->getAsmVar() + ", AX \n";
    return asmCode;
}

/**
 * Adds the print function to the assembly output
 */
inline void addPrintFunc() {
    std::ifstream srcFile("./AsmLibraries/outdec.h");
    std::string line;
    while (std::getline(srcFile, line, '.'))
        code << line << '\n';
    srcFile.close();
}

/**
 * Splits a string by a delimiter
 * 
 * @param str The string to split
 * @param delim The delimiter to split by
 * @return A vector of the split string parts
 */
inline std::vector<std::string> split(const std::string &str, const std::string &delim) {
    std::vector<std::string> tokens;
    size_t prev = 0, pos = 0;
    do {
        pos = str.find(delim, prev);
        if (pos == std::string::npos)
            pos = str.length();
        std::string token = str.substr(prev, pos - prev);
        if (!token.empty())
            tokens.push_back(token);
        prev = pos + delim.length();
    } while (pos < str.length() && prev < str.length());
    return tokens;
}

/**
 * Performs optimization on MOV instructions
 * 
 * @param inst The instruction to optimize
 * @return True if the instruction should be removed, false otherwise
 */
inline bool optimize_mov(std::string inst);

/**
 * Performs optimization on arithmetic instructions
 * 
 * @param inst The instruction to optimize
 * @return True if the instruction should be removed, false otherwise
 */
inline bool optimize_arithmetic(std::string inst);

/**
 * Performs peephole optimization on the generated assembly code
 */
inline void optimize();

#endif // ASM_UTILS_H 