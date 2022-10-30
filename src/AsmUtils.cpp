#include "../include/AsmUtils.h"

// Global variables
std::vector<std::string> asmVarList;
std::vector<std::pair<std::string, int>> asmArrList;
int label_count = 0;

// Optimization variables
std::string prevMovLhs = "", prevMovRhs = "";
std::string BXval = "";
std::string currentLabel = "";

bool optimize_mov(std::string inst) {
    inst.erase(std::remove(inst.begin(), inst.end(), ','), inst.end());
    std::vector<std::string> tokens = split(inst, " ");
    if (tokens.size() == 3 && tokens[0] == "MOV") {
        std::string movLhs = tokens[1];
        std::string movRhs = tokens[2];

        if (movLhs == prevMovRhs && movRhs == prevMovLhs) {
            return true;
        } else if (movLhs == "BX") {
            BXval = movRhs;
        } else if (movLhs == movRhs) {
            return true;
        }

        prevMovLhs = movLhs;
        prevMovRhs = movRhs;
    }
    return false;
}

bool optimize_arithmetic(std::string inst) {
    inst.erase(std::remove(inst.begin(), inst.end(), ','), inst.end());
    std::vector<std::string> tokens = split(inst, " ");

    if (tokens.size() == 3 && (tokens[0] == "ADD" || tokens[0] == "SUB") &&
        tokens[2] == "0") {
        return true;
    } else if (tokens.size() == 2 &&
               (tokens[0] == "IMUL" || tokens[0] == "IDIV") && BXval == "1") {
        return true;
    }

    return false;
}

void optimize() {
    std::ifstream srcFile("code.asm");
    std::string line;
    int line_count = 0, line_removed = 0;
    std::vector<std::string> lines;
    
    while (std::getline(srcFile, line, '\n')) {
        lines.push_back(line);
    }

    log << "-------------------------------------" << std::endl;
    log << "Optimizer log: " << std::endl;

    for (std::string s : lines) {
        line_count++;

        if (s.substr(0, 1) == "L") {
            // Clears prevMovLhs and prevMovRhs when entering a new Label
            // so that MOV optimization doesn't happen
            prevMovLhs = "";
            prevMovRhs = "";
        }

        if (s == " ") {
            log << "Removed blank line : " << line_count << std::endl;
            line_removed++;
        } else if (optimize_mov(s)) {
            log << "Optimized redundant MOV operation: " << line_count << std::endl;
            line_removed++;
        } else if (optimize_arithmetic(s)) {
            log << "Optimized redundant arithmetic operation : " << line_count
                << std::endl;
            line_removed++;
        } else {
            optimized << s << std::endl;
        }
    }
    
    log << "Line removed:" << line_removed << std::endl;
    log << "-------------------------------------" << std::endl;
    optimized << "END MAIN" << std::endl;
    srcFile.close();
} 