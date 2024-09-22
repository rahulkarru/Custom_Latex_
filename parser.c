#include <stdio.h>
#include <string.h>

void processFraction(char *input, int *i, FILE *outputFile);
void processLatexSymbol(char *input, int *i, FILE *outputFile);
void handleSqrt(char *input, int *i, int depth, FILE *outputFile);
void processMatrix(char *input, int *i, FILE *outputFile);
void processIntegral(char *input, int *i, FILE *outputFile);
void processDifferentiation(char *input, int *i, FILE *outputFile);
void processSummation(char *input, int *i, FILE *outputFile);
void processProduct(char *input, int *i, FILE *outputFile);
void convertLatex(char *input, FILE *outputFile);
void displayNotFoundError(char *input, int *i, FILE *outputFile) {
    fprintf(outputFile, "Error: Unrecognized LaTeX command near '%s'\n", &input[*i]);
}


int checkBraceBalance(const char *input, FILE *outputFile) {
    int openBraces = 0, closeBraces = 0;
    for (int i = 0; input[i] != '\0'; i++) {
        if (input[i] == '{') openBraces++;
        if (input[i] == '}') closeBraces++;
    }
    if (openBraces != closeBraces) {
        fprintf(outputFile, "Error: Mismatched braces. Open: %d, Close: %d\n", openBraces, closeBraces);
        return 0;  // Mismatched braces
    }
    return 1;  // Braces match
}

void handleSqrt(char *input, int *i, int depth, FILE *outputFile) {
    fprintf(outputFile, "√");
    for (int j = 0; j < depth; j++) {
        fprintf(outputFile, "'");
    }
    *i += 6; 
    if (input[*i] == '{') *i += 1; 
    while (input[*i] != '}' && input[*i] != '\0') {
        if (strncmp(&input[*i], "\\sqrt{", 6) == 0) {
            handleSqrt(input, i, depth + 1, outputFile);  
        } else if (strncmp(&input[*i], "\\frac{", 6) == 0) {
            *i += 6;
            processFraction(input, i, outputFile); 
        } else {
            fprintf(outputFile, "%c", input[*i]);
            *i += 1;
        }
    }
    if (input[*i] == '}') {
        *i += 1;
    } else {
        fprintf(outputFile, "Error: Missing closing brace for \\sqrt\n");
    }
}

void processFraction(char *input, int *i, FILE *outputFile) {
    fprintf(outputFile, "(");
    if (input[*i] == '{') *i += 1;
    while (input[*i] != '}' && input[*i] != '\0') {
        if (strncmp(&input[*i], "\\frac{", 6) == 0) {
            *i += 6;
            processFraction(input, i, outputFile); 
        } else if (strncmp(&input[*i], "\\sqrt{", 6) == 0) {
            handleSqrt(input, i, 1, outputFile);
        } else {
            fprintf(outputFile, "%c", input[*i]);
            *i += 1;
        }
    }
    if (input[*i] == '}') {
        *i += 1;
    } else {
        fprintf(outputFile, "Error: Missing closing brace for \\frac numerator\n");
    }

    fprintf(outputFile, "/");
    if (input[*i] == '{') *i += 1;
    while (input[*i] != '}' && input[*i] != '\0') {
        if (strncmp(&input[*i], "\\frac{", 6) == 0) {
            *i += 6;
            processFraction(input, i, outputFile);
        } else if (strncmp(&input[*i], "\\sqrt{", 6) == 0) {
            handleSqrt(input, i, 1, outputFile);
        } else {
            fprintf(outputFile, "%c", input[*i]);
            *i += 1;
        }
    }
    if (input[*i] == '}') *i += 1;
    else {
        fprintf(outputFile, "Error: Missing closing brace for \\frac denominator\n");
    }

    fprintf(outputFile, ")");
}

void processMatrix(char *input, int *i, FILE *outputFile) {
    fprintf(outputFile, "[");
    *i += 14;  
    while (strncmp(&input[*i], "\\end{matrix}", 12) != 0 && input[*i] != '\0') {
        if (strncmp(&input[*i], "\\frac{", 6) == 0) {
            *i += 6;
            processFraction(input, i, outputFile);
        } else if (strncmp(&input[*i], "\\sqrt{", 6) == 0) {
            handleSqrt(input, i, 1, outputFile);
        } else if (input[*i] == '&') {
            fprintf(outputFile, ", ");
            *i += 1;
        } else if (input[*i] == '\\' && input[*i+1] == '\\') {
            fprintf(outputFile, "; ");
            *i += 2;
        } else {
            fprintf(outputFile, "%c", input[*i]);
            *i += 1;
        }
    }
    if (strncmp(&input[*i], "\\end{matrix}", 12) == 0) {
        fprintf(outputFile, "]");
        *i += 12; 
    } else {
        fprintf(outputFile, "Error: Missing \\end{matrix}\n");
    }
}

void processIntegral(char *input, int *i, FILE *outputFile) {
    fprintf(outputFile, "∫");
    if (input[*i] == '_') {
        *i += 1;  // Skip '_'
        if (input[*i] == '{') *i += 1;  // Skip '{'
        fprintf(outputFile, "_(");
        while (input[*i] != '}' && input[*i] != '\0') {
            fprintf(outputFile, "%c", input[*i]);
            *i += 1;
        }
        if (input[*i] == '}') *i += 1;  // Skip '}'
        fprintf(outputFile, ")");
    }
    if (input[*i] == '^') {
        *i += 1;  // Skip '^'
        if (input[*i] == '{') *i += 1;  // Skip '{'
        fprintf(outputFile, "^(");
        while (input[*i] != '}' && input[*i] != '\0') {
            fprintf(outputFile, "%c", input[*i]);
            *i += 1;
        }
        if (input[*i] == '}') *i += 1;  // Skip '}'
        fprintf(outputFile, ")");
    }
    fprintf(outputFile, " ");
}

void processDifferentiation(char *input, int *i, FILE *outputFile) {
    fprintf(outputFile, "d/dx ");
    *i += 9;  // Skip '\frac{d}{dx}'
    while (input[*i] != '\0') {
        fprintf(outputFile, "%c", input[*i]);
        *i += 1;
    }
}

void processSummation(char *input, int *i, FILE *outputFile) {
    fprintf(outputFile, "Σ");
    if (input[*i] == '_') {
        *i += 1;  // Skip '_'
        if (input[*i] == '{') *i += 1;  // Skip '{'
        fprintf(outputFile, "_(");
        while (input[*i] != '}' && input[*i] != '\0') {
            fprintf(outputFile, "%c", input[*i]);
            *i += 1;
        }
        if (input[*i] == '}') *i += 1;  // Skip '}'
        fprintf(outputFile, ")");
    }
    if (input[*i] == '^') {
        *i += 1;  // Skip '^'
        if (input[*i] == '{') *i += 1;  // Skip '{'
        fprintf(outputFile, "^(");
        while (input[*i] != '}' && input[*i] != '\0') {
            fprintf(outputFile, "%c", input[*i]);
            *i += 1;
        }
        if (input[*i] == '}') *i += 1;  // Skip '}'
        fprintf(outputFile, ")");
    }
    fprintf(outputFile, " ");
}

void processProduct(char *input, int *i, FILE *outputFile) {
    fprintf(outputFile, "∏");
    if (input[*i] == '_') {
        *i += 1;  // Skip '_'
        if (input[*i] == '{') *i += 1;  // Skip '{'
        fprintf(outputFile, "_(");
        while (input[*i] != '}' && input[*i] != '\0') {
            fprintf(outputFile, "%c", input[*i]);
            *i += 1;
        }
        if (input[*i] == '}') *i += 1;  // Skip '}'
        fprintf(outputFile, ")");
    }
    if (input[*i] == '^') {
        *i += 1;  // Skip '^'
        if (input[*i] == '{') *i += 1;  // Skip '{'
        fprintf(outputFile, "^(");
        while (input[*i] != '}' && input[*i] != '\0') {
            fprintf(outputFile, "%c", input[*i]);
            *i += 1;
        }
        if (input[*i] == '}') *i += 1;  // Skip '}'
        fprintf(outputFile, ")");
    }
    fprintf(outputFile, " ");
}

void processLatexSymbol(char *input, int *i, FILE *outputFile) {
    if (strncmp(&input[*i], "\\sqrt", 5) == 0) {
        handleSqrt(input, i, 1, outputFile);
    } else if (strncmp(&input[*i], "\\frac", 5) == 0) {
        processFraction(input, i, outputFile);
    } else if (strncmp(&input[*i], "\\int", 4) == 0) {
        *i += 4;
        processIntegral(input, i, outputFile);
    } else if (strncmp(&input[*i], "\\frac{d}{dx}", 12) == 0) {
        *i += 12;
        processDifferentiation(input, i, outputFile);
    } else if (strncmp(&input[*i], "\\sum", 4) == 0) {
        *i += 4;
        processSummation(input, i, outputFile);
    } else if (strncmp(&input[*i], "\\prod", 5) == 0) {
        *i += 5;
        processProduct(input, i, outputFile);
    } else if (strncmp(&input[*i], "\\begin{matrix}", 14) == 0) {
        processMatrix(input, i, outputFile);
    }
    else {
        
        displayNotFoundError(input, i, outputFile);
        *i += 1; 
    }
}

void convertLatex(char *input, FILE *outputFile) {
    int i = 0, depth = 0;

    if (!checkBraceBalance(input, outputFile)) {
        return; 
    }

    if (input[0] == '$') i++;
    size_t len = strlen(input);
    if (input[len - 1] == '$') input[len - 1] = '\0';

    while (input[i] != '\0') {
        if (strncmp(&input[i], "\\sqrt{", 6) == 0) {
            depth++;
            handleSqrt(input, &i, depth, outputFile);
        } else if (strncmp(&input[i], "\\frac{", 6) == 0) {
            i += 6;
            processFraction(input, &i, outputFile);
        } else if (strncmp(&input[i], "\\int", 4) == 0) {
            i += 4;
            processIntegral(input, &i, outputFile);
        } else if (strncmp(&input[i], "\\frac{d}{dx}", 12) == 0) {
            i += 12;
            processDifferentiation(input, &i, outputFile);
        } else if (strncmp(&input[i], "\\sum", 4) == 0) {
            i += 4;
            processSummation(input, &i, outputFile);
        } else if (strncmp(&input[i], "\\prod", 5) == 0) {
            i += 5;
            processProduct(input, &i, outputFile);
        } else if (strncmp(&input[i], "\\begin{matrix}", 14) == 0) {
            processMatrix(input, &i, outputFile);
        } else if (input[i] == '\\') {
            processLatexSymbol(input, &i, outputFile);
        } else if (input[i] == '{' || input[i] == '^' || input[i] == '_') {
            i++;
        } else if (input[i] == '}') {
            depth--;
            i++;
        } else {
            fprintf(outputFile, "%c", input[i]);
            i++;
        }
    }
    fprintf(outputFile, "\n");
}
void process_latex_to_output(const char *input, char *output, int max_size) {
   
    memset(output, 0, max_size);

    FILE *outputFile = fmemopen(output, max_size, "w");
    if (!outputFile) {
        fprintf(stderr, "Error opening output buffer.\n");
        return;
    }

    convertLatex((char*)input, outputFile);

    
    fclose(outputFile);
}