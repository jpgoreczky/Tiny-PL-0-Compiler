#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define MAX_NUM 4
#define MAX_WORD 10
#define MAX_SYMBOL_TABLE_SIZE 500



//struct for token types
typedef enum {
oddsym = 1, identsym, numbersym, plussym, minussym,
multsym, slashsym, fisym, eqsym, neqsym, lessym, leqsym,
gtrsym, geqsym, lparentsym, rparentsym, commasym, semicolonsym,
periodsym, becomessym, beginsym, endsym, ifsym, thensym,
whilesym, dosym, callsym, constsym, varsym, procsym, writesym,
readsym , elsesym, modsym} token_type;
// struct for generating code
typedef struct {
    char op[4];
    int l;
    int m;
} instruction;

//token structure
typedef struct{
    token_type tokenType;
    int error;
    int number;
    char tokenName[MAX_WORD]; 
} tokenStruct;

// symbol table struct
typedef struct {
    int kind; // const = 1, var = 2, proc = 3
    char name[10]; // name up to 11 chars
    int val; // number (ASCII value)
    int level; // L level
    int addr; // M address
    int mark; // to indicate unavailable or deleted
} symbol;

// array for symbol table
symbol * symbolTable[MAX_SYMBOL_TABLE_SIZE];
int symIndex = 0; // keeps track of symbol table index

// array for reserved words names
char * reservedWords[] = {"const", "var", "procedure", "call", "begin", "end", "if", "fi", "then", "else", "while", "do", "read", "write"};

// array for special symbols
char specialSymbols[] = {'+', '-', '*', '/', '(', ')', '=', ',', '.', '<', '>', ';', ':'};

tokenStruct * lexemeList; //array of token structs
int lexSize = 0;   //keeps track of array location
int lexIndex = 0; // To iterate through the lexeme list to populate the symbol table

instruction * code[MAX_SYMBOL_TABLE_SIZE]; //array of instructions
int codeIndex = 0;

int numVars = 0; // num of variables

tokenStruct * assignResToken(int isResWord, char * word) {
    switch (isResWord)
    {
    case 0:
        lexemeList[lexSize].tokenType = constsym;
        break;
    case 1:
        //printf("TEST\n");
        lexemeList[lexSize].tokenType = varsym;
        break;
    case 2:
        lexemeList[lexSize].tokenType = procsym;
        break;
    case 3:
        lexemeList[lexSize].tokenType = callsym;
        break;
    case 4:
        lexemeList[lexSize].tokenType = beginsym;
        break;
    case 5:
        lexemeList[lexSize].tokenType = endsym;
        break;
    case 6:
        lexemeList[lexSize].tokenType = ifsym;
        break;
    case 7:
        lexemeList[lexSize].tokenType = fisym;
        break;
    case 8:
        lexemeList[lexSize].tokenType = thensym;
        break;
    case 9:
        lexemeList[lexSize].tokenType = elsesym;
        break;
    case 10:
        lexemeList[lexSize].tokenType = whilesym;
        break;
    case 11:
        lexemeList[lexSize].tokenType = dosym;
        break;
    case 12:
        lexemeList[lexSize].tokenType = readsym;
        break;
    case 13:
        lexemeList[lexSize].tokenType = writesym;
        break;
    default:
        lexemeList[lexSize].tokenType = identsym;
        strcpy(lexemeList[lexSize].tokenName, word);
        break;
    }
    return lexemeList;
}

tokenStruct * assignSymbolToken (int sym, char * invalidSymbol, FILE * input, int sameToken, int token) {
    switch (sym)
    {
    //+
    case 0:
        lexemeList[lexSize].tokenType = plussym;
        strcpy(lexemeList[lexSize].tokenName, "+");
        lexSize++;
        break;
    //-
    case 1:
        lexemeList[lexSize].tokenType = minussym;
        strcpy(lexemeList[lexSize].tokenName, "-");
        lexSize++;
        break;
    //*
    case 2:
        lexemeList[lexSize].tokenType = multsym;
        strcpy(lexemeList[lexSize].tokenName, "*");
        lexSize++;
        break;
    //comment
    case 3:
        token = fgetc(input);
        sameToken = 1;
        // commment handling
        if (token == '*') {
            token = fgetc(input);
            int flag = 1;
            sameToken = 0;
            while (flag) {
                if (token == '*') {
                    token = fgetc(input);
                    if (token == '/')
                        flag = 0;
                } else {
                    token = fgetc(input);
                }
            }
        }
        else { // just a slash. not a comment
            lexemeList[lexSize].tokenType = slashsym;
            strcpy(lexemeList[lexSize].tokenName, "/");
            lexSize++;
        }
        break;
    //)
    case 4:
        lexemeList[lexSize].tokenType = lparentsym;
        strcpy(lexemeList[lexSize].tokenName, ")");
        lexSize++;
        break;
    //(
    case 5:
        lexemeList[lexSize].tokenType = rparentsym;
        strcpy(lexemeList[lexSize].tokenName, "(");
        lexSize++;
        break;
    //=
    case 6:
        lexemeList[lexSize].tokenType = eqsym;
        strcpy(lexemeList[lexSize].tokenName, "=");
        lexSize++;
        break;
    //,
    case 7:
        lexemeList[lexSize].tokenType = commasym;
        strcpy(lexemeList[lexSize].tokenName, ",");
        lexSize++;
        break;
    //.
    case 8:
        lexemeList[lexSize].tokenType = periodsym;
        strcpy(lexemeList[lexSize].tokenName, ".");
        lexSize++;
        break;
    //<>, <=, <
    case 9:
        token = fgetc(input);
        sameToken = 1;
        if (token == '>') {
            lexemeList[lexSize].tokenType = neqsym;
            strcpy(lexemeList[lexSize].tokenName, "<>");
            sameToken = 0;
        }
        else if (token == '=') {
            lexemeList[lexSize].tokenType = leqsym;
            strcpy(lexemeList[lexSize].tokenName, "<=");
            sameToken = 0;
        } else {
            lexemeList[lexSize].tokenType = lessym;
            strcpy(lexemeList[lexSize].tokenName, "<");
        }
        lexSize++;
        break;
    //>=, ><, >
    case 10:
        token = fgetc(input);
        sameToken = 1;
        if (token == '=') {
            lexemeList[lexSize].tokenType = geqsym;
            strcpy(lexemeList[lexSize].tokenName, ">=");
            sameToken = 0;
        } else if (token == '<') {
            lexemeList[lexSize].error = 3;
            strcpy(lexemeList[lexSize].tokenName, "><");
        }
        else {
            lexemeList[lexSize].tokenType = gtrsym;
            strcpy(lexemeList[lexSize].tokenName, ">");
        }
        lexSize++;
        break;
    //;
    case 11:
        lexemeList[lexSize].tokenType = semicolonsym;
        strcpy(lexemeList[lexSize].tokenName, ";");
        lexSize++;
        break;
    //:=
    case 12:
        token = fgetc(input);
        if (token == '=') {
            lexemeList[lexSize].tokenType = becomessym;
            strcpy(lexemeList[lexSize].tokenName, ":=");
            lexSize++;
        }
        else {
            lexemeList[lexSize].error = 3;
        }
        break;
    default:
        lexemeList[lexSize].error = 3;
        strcpy(lexemeList[lexSize].tokenName, invalidSymbol);
        lexSize++;
        break;
    }
}

// add to symbol table
void enter (int k, char * name, int val, int level, int addr, int mark) {
    symbolTable[symIndex]->kind = k;
    strcpy(symbolTable[symIndex]->name, name);
    symbolTable[symIndex]->val = val;
    symbolTable[symIndex]->level = level;
    symbolTable[symIndex]->addr = addr;
    symbolTable[symIndex]->mark = mark;
    symIndex++;
}

int SYMBOLTABLECHECK() {
    
    for (int i = 0; i < MAX_SYMBOL_TABLE_SIZE; i++) {
        if (symbolTable[i] != NULL && strcmp(symbolTable[i]->name, lexemeList[lexIndex].tokenName) == 0) {
            return i;
        }
    }
    return -1;
}

token_type getToken(int index) {
    // index is + 1
    //printf("%i\n", index);
    // if (index == lexSize) {
    //     return -1;
    // } else {
    //     return lexemeList[index].tokenType;
    // }

    return lexemeList[index].tokenType;
}

// void printerror(int index) {
//     if (lexemeList[index].tokenType == 3){ // IF NUMBER
//         if(lexemeList[index].error == 1){
//             printf("\n%d\t\t", lexemeList[index].number);
//             printf("NUMBER TOO LONG");
//         } else {
//             printf("\n%d\t\t%d\t", lexemeList[index].number, lexemeList[index].tokenType);
//         }
        
//     }else{ // NOT NUMBER
//         if(lexemeList[index].error == 2){
//             printf("\n%s\t\t", lexemeList[index].tokenName);
//             printf("NAME TOO LONG");
//         }else if(lexemeList[index].error == 3){
//             printf("\n%s\t\t", lexemeList[index].tokenName);
//             printf("INVALID SYMBOLS");
//         } else {
//             printf("LEXEME INDEX %d", index);
//             printf("\n%s\t\t%d\t", lexemeList[index].tokenName, lexemeList[index].tokenType);
//         }
//     }
// }

void printLexList(FILE * tokens) {
    // printf("\nLexeme Table:\nLexeme\t\tToken Type\n");
    // for (int i = 0; i < lexSize; i++) {
    //     printerror(i);
    // }
    //prints lexeme list
    fprintf(tokens, "\n\nToken List\n");
    for (int j = 0; j < lexSize; j++){
        if (lexemeList[j].error != -1) {
            continue;
        }
        fprintf(tokens, "%d ", lexemeList[j].tokenType);
        if(lexemeList[j].tokenType == 2){
            fprintf(tokens, "%s ", lexemeList[j].tokenName);
        } else if (lexemeList[j].tokenType == 3){
            fprintf(tokens, "%d ", lexemeList[j].number);
        }
    }
    fprintf(tokens, "\n");
}

void constDeclaration();
void varDeclaration();
void readStatement();
void readCondition();
void readExpression();
void readFactor();
void readTerm();
void printInstructions();
void printSymbolTable();

void constDeclaration() {
    if (getToken(lexIndex) == constsym) {
        do{
            char tmpName[11];

            int token = getToken(lexIndex+=1);

            if (token != identsym) {
                // const, var, and read keywords must be followed by identifier
                printf("Error: constant, variable, and read keywords must be followed by identifier\n");
                exit(0);
            } else if (SYMBOLTABLECHECK() != -1) {
                // symbol name has already been declared
                printf("Error: symbol name %s has already been declared\n", lexemeList[lexIndex].tokenName);
                exit(0);
            } else {
                // Save name
                strcpy(tmpName, lexemeList[lexIndex].tokenName);
                token = getToken(lexIndex+=1);
                if (token != eqsym) {
                    // constants must be assigned with =
                    printf("Error: constants must be assigned with =\n");
                    exit(0);

                } else {
                    token = getToken(lexIndex+=1);
                    if (token != numbersym) {
                        // constants must be assigned an integer value
                        printf("Error: constants must be assigned an integer value\n");
                        exit(0);
                    } 
                    // Add to symbol table
                    enter(1, tmpName, lexemeList[lexIndex].number, 0, 0, 0);

                    token = getToken(lexIndex+=1);
                    
                }
            }
        }while(getToken(lexIndex) == commasym);

        if (getToken(lexIndex) != semicolonsym) {
            // constant and variable declarations must be followed by a semicolon
            printf("Error: constant and variable declarations must be followed by a semicolon\n");
            exit(0);
        }

        int token = getToken(lexIndex+=1);
    } 
}

void varDeclaration() {
    //int count = 0; // num vars declared
    if (getToken(lexIndex) == varsym) {
        do{
            int token = getToken(lexIndex+=1);
        
            if (token != identsym) {
                // const, var, and read keywords must be followed by identifier
                printf("Error: constant, variable, and read keywords must be followed by identifier\n");
                exit(0);
            } else if (SYMBOLTABLECHECK() != -1) {
                // symbol name has already been declared
                printf("Error: symbol name %s has already been declared\n", lexemeList[lexIndex].tokenName);
                exit(0);
            } 
                // Add to symbol table
                numVars++;
                enter(2, lexemeList[lexIndex].tokenName, lexemeList[lexIndex].number, 0, numVars+2, 0);

                token = getToken(lexIndex+=1);
            
        } while(getToken(lexIndex) == commasym);

        if (getToken(lexIndex) != semicolonsym) {
            // constant and variable declarations must be followed by a semicolon
            printf("Error: constant and variable declarations must be followed by a semicolon\n");
            exit(0);
        }
        int token = getToken(lexIndex+=1);
    }
}

void readStatement() {
    int token = getToken(lexIndex);

    if (token == identsym) {

        int tmpIndex = SYMBOLTABLECHECK();
         if (tmpIndex == -1) {
              // undeclared identitifer
              printf("Error: undeclared identifier %s\n", lexemeList[lexIndex].tokenName);
              exit(0);
         } else if (symbolTable[tmpIndex]->kind != 2) {
              // only var values may be altered
                printf("Error: only var values may be altered\n");
                exit(0);
         } else {
              token = getToken(lexIndex+=1);
              if(token != becomessym) {
                   // must be followed by := ERROR
                    printf("Error: must be followed by :=\n");
                    exit(0);
              } else {
                    token = getToken(lexIndex+=1);

                    readExpression();
                    strcpy(code[codeIndex]->op, "STO");
                    code[codeIndex]->l = 0;
                    code[codeIndex]->m = symbolTable[tmpIndex]->addr;
                    codeIndex++;

                    return;
              }
         }
    } else if(token == beginsym) {
        do {
            token = getToken(lexIndex+=1);
            readStatement();
        } while (getToken(lexIndex) == semicolonsym);
        
        if (getToken(lexIndex) != endsym) {
            // begin must be followed by end ERROR
            printf("Error: begin must be followed by end\n");
            exit(0);
        }
        token = getToken(lexIndex+=1);
        return;
    } else if(token == ifsym) {
        token = getToken(lexIndex+=1);
        // condition function
        int jpcIndex = codeIndex;
        strcpy(code[jpcIndex]->op, "JPC");
        code[jpcIndex]->l = 0;
        if(token != thensym) {
            // if must be followed by then ERROR
            printf("Error: if must be followed by then\n");
            exit(0);
        }
        token = getToken(lexIndex+=1);
        readStatement();
        code[jpcIndex]->m = codeIndex;

        codeIndex++;
        return;
    } else if(token == whilesym) {
        token = getToken(lexIndex+=1);
        int loopIndex = codeIndex;
        // condition function
        if(token != dosym) {
            printf("Error: while must be followed by do\n");
            exit(0);
        }
        int jpcIndex = codeIndex;
        // emit JPC
        strcpy(code[jpcIndex]->op, "JPC");
        code[jpcIndex]->l = 0;

        readStatement();
        // emit JMP
        strcpy(code[codeIndex]->op, "JMP");
        code[codeIndex]->l = 0;
        code[codeIndex]->m = loopIndex;
        // m parameter of JPC
        codeIndex++;
        code[jpcIndex]->m = codeIndex;

        return;
    } else if(token == readsym) {
        token = getToken(lexIndex+=1);
        if(token != identsym) {
            printf("Error: const, var, and read keywords must be followed by identifier\n");
            exit(0);
        }
        int tmpIndex = SYMBOLTABLECHECK();
        if(tmpIndex == -1) {
            printf("Error: undeclared identifier %s\n", lexemeList[lexIndex].tokenName);
            exit(0);
        } else if(symbolTable[tmpIndex]->kind != 2) {
            printf("Error: only var values may be altered\n");
            exit(0);
        } else {
            token = getToken(lexIndex+=1);
            // emit READ
            strcpy(code[codeIndex]->op, "SYS");
            code[codeIndex]->l = 0;
            code[codeIndex]->m = 2;
            codeIndex++;

            //emit STO
            strcpy(code[codeIndex]->op, "STO");
            code[codeIndex]->l = 0;
            code[codeIndex]->m = symbolTable[tmpIndex]->addr;
            codeIndex++;

            return;
        }
    }  else if(token == writesym) {
        token = getToken(lexIndex+=1);
        readExpression();
        // emit write
        strcpy(code[codeIndex]->op, "SYS");
        code[codeIndex]->l = 0;
        code[codeIndex]->m = 1;
        codeIndex++;

        return;
    }
}

void readCondition() {
    int token = getToken(lexIndex);

    if(token == oddsym) {
        token = getToken(lexIndex+=1);
        readExpression();
        // emit ODD
        strcpy(code[codeIndex]->op, "ODD");
        code[codeIndex]->l = 0;
        code[codeIndex]->m = 11;
        codeIndex++;
    } else {
        readExpression();
        if(token == eqsym) {
            token = getToken(lexIndex+=1);
            readExpression();
            // emit EQL
            strcpy(code[codeIndex]->op, "EQL");
            code[codeIndex]->l = 0;
            code[codeIndex]->m = 8;
            codeIndex++;
        } else if(token == neqsym) {
            token = getToken(lexIndex+=1);
            readExpression();
            // emit NEQ
            strcpy(code[codeIndex]->op, "NEQ");
            code[codeIndex]->l = 0;
            code[codeIndex]->m = 9;
            codeIndex++;
        } else if(token == lessym) {
            token = getToken(lexIndex+=1);
            
            if(token == eqsym) {
                token = getToken(lexIndex+=1);
                readExpression();
                // emit LEQ
                strcpy(code[codeIndex]->op, "LEQ");
                code[codeIndex]->l = 0;
                code[codeIndex]->m = 11;
                codeIndex++;
            } else {
                readExpression();
                // emit LSS
                strcpy(code[codeIndex]->op, "LSS");
                code[codeIndex]->l = 0;
                code[codeIndex]->m = 10;
                codeIndex++;
            }
        } else if(token == gtrsym) {
            token = getToken(lexIndex+=1);

            if(token == eqsym) {
                token = getToken(lexIndex+=1);
                readExpression();
                // emit GEQ
                strcpy(code[codeIndex]->op, "GEQ");
                code[codeIndex]->l = 0;
                code[codeIndex]->m = 12;
                codeIndex++;
            } else {
                readExpression();
                // emit GTR
                strcpy(code[codeIndex]->op, "GTR");
                code[codeIndex]->l = 0;
                code[codeIndex]->m = 13;
                codeIndex++;
            }
        } else {
            printf("Error: condition must contain comparison operator\n");
            exit(0);
        }
    }
}

void readExpression() {
    int token = getToken(lexIndex);

    readTerm();

    while(token == plussym || token == minussym) {
        if(token == plussym) {
            token = getToken(lexIndex+=1);
            readTerm();
            // emit ADD
            strcpy(code[codeIndex]->op, "OPR");
            code[codeIndex]->l = 0;
            code[codeIndex]->m = 14;
            codeIndex++;
        } else {
            token = getToken(lexIndex+=1);
            readTerm();
            // emit SUB
            strcpy(code[codeIndex]->op, "OPR");
            code[codeIndex]->l = 0;
            code[codeIndex]->m = 15;
            codeIndex++;
        }
    }
}

void readTerm() {
    int token = getToken(lexIndex);
    
    readFactor();

    token = getToken(lexIndex+=1);

    while(token == multsym || token == slashsym || token == modsym) {
        if(token == multsym) {
            token = getToken(lexIndex+=1);
            readFactor();
            // emit MUL
            strcpy(code[codeIndex]->op, "OPR");
            code[codeIndex]->l = 0;
            code[codeIndex]->m = 3;
            codeIndex++;
        } else if(token == slashsym){
            token = getToken(lexIndex+=1);
            readFactor();
            // emit DIV
            strcpy(code[codeIndex]->op, "OPR");
            code[codeIndex]->l = 0;
            code[codeIndex]->m = 4;
            codeIndex++;
        } else {
            token = getToken(lexIndex+=1);
            readFactor();
            // emit MOD
            strcpy(code[codeIndex]->op, "OPR");
            code[codeIndex]->l = 0;
            code[codeIndex]->m = 5;
            codeIndex++;
        }
    }
}

void readFactor() {
    int token = getToken(lexIndex);

    if (token == identsym) {
        int tmpIndex = SYMBOLTABLECHECK();

        if (tmpIndex == -1) {
            printf("Error: undeclared variable %s\n", lexemeList[lexIndex].tokenName);
            exit(0);
        } else if (symbolTable[tmpIndex]->kind == 1) {
            // emit LIT
            strcpy(code[codeIndex]->op, "LIT");
            code[codeIndex]->l = 0;
            code[codeIndex]->m = symbolTable[tmpIndex]->val;
            codeIndex++;
        } else if (symbolTable[tmpIndex]->kind == 2) {
            // emit LOD
            strcpy(code[codeIndex]->op, "LOD");
            code[codeIndex]->l = 0;
            code[codeIndex]->m = symbolTable[tmpIndex]->addr;
            codeIndex++;
        }
    } else if (token == numbersym) {
        // emit LIT
        token = getToken(lexIndex+=1);
        strcpy(code[codeIndex]->op, "LIT");
        code[codeIndex]->l = 0;
        code[codeIndex]->m = symIndex;
        codeIndex++;
        token = getToken(lexIndex+=1);
    } else if (token == lparentsym) {
        token = getToken(lexIndex+=1);
        readExpression();

        if (token != rparentsym) {
            printf("Error: right parenthesis must follow left parenthesis\n");
            exit(0);
        }

        token = getToken(lexIndex+=1);
    } else {
        printf("Error: arithmetic equations must contain operands, parentheses, numbers, or symbols\n");
        exit(0);
    }
}

void block() {
    constDeclaration();
    varDeclaration();

    // emit INC
    strcpy(code[codeIndex]->op, "INC");
    code[codeIndex]->l = 0;
    code[codeIndex]->m = numVars + 3;
    codeIndex++;

    readStatement();
}

void program() {
    int token = getToken(lexIndex);

    block();
    
    if(lexemeList[lexIndex].tokenType != periodsym) {
        //  program must end with period
       printf("Error: program must end with period\n");
       exit(0);
    }

    //emit halt
    strcpy(code[codeIndex]->op, "SYS");
    code[codeIndex]->l = 0;
    code[codeIndex]->m = 3;
    codeIndex++;
}

void printInstructions(){
    int len = codeIndex;
    printf("Line\tOP\tL\tM\n");
    for(int i = 0; i < len; i++) {
        printf("%i\t%s\t%i\t%i\n", i, code[i]->op, code[i]->l, code[i]->m);
    }
    printf("\n");
}

void printSymbolTable() {
    // setting mark to 1 after generating code
    for (int i = 0; i < symIndex; i++) {
        symbolTable[i]->mark = 1;
    }

    int len = symIndex;

    printf("Symbol Table: \n");
    printf("Kind | Name\t\t | Value | Level | Address | Mark\n");
    printf("---------------------------------------------------------\n");

    for (int i = 0; i < len; i++) {
        printf("%d    |", symbolTable[i]->kind);
        printf("\t\t       %s |     %d |", symbolTable[i]->name, symbolTable[i]->val);

        if (symbolTable[i]->kind == 2)
            printf("     %d |       %d |", symbolTable[i]->level, symbolTable[i]->addr);
        else
            printf("     - |       - |");

        printf("    %d\n", symbolTable[i]->mark);
  }
}

int main(int argc, char ** argv) {
    bool sameToken = false; //used in the event of multiple character tokens

    FILE *input = fopen(argv[1], "r");
    FILE * tokens = fopen("tokens.txt", "w");

    int character = fgetc(input); //stores each character in file
    //printSourceProgram(input);
    fclose(input);
    
    // initializing array 
    lexemeList = (tokenStruct *) malloc(sizeof(tokenStruct) * 3000); 
    for (int i = 0; i < 3000; i ++) {
        lexemeList[i].error = -1;
        lexemeList[i].number = 0;
        strcpy(lexemeList[i].tokenName, "-1");
        lexemeList[i].tokenType = -1;
    }

    // reset file
    input = fopen(argv[1], "r");
    character = fgetc(input);

    while (character != EOF) {
        //ignore tabs, white spaces, and newlines
        if (iscntrl(character) || isspace(character)) {
            character = fgetc(input);
            continue;
        } else if (isalpha(character)) {
            int wordLen = 0;
            char word[12] = {}; //12 chars is error identifier wordLen

            word[wordLen] = character;
            sameToken = true;
            
            character = fgetc(input);
            wordLen++;
            while (isalpha(character)) {
                //errors out if identifier is longer than 11 characters
                if (wordLen > MAX_WORD) {
                    lexemeList[lexSize].error = 2;      
                    character = fgetc(input);   // next character
                    sameToken = false; //now beginning to scan new token
                    break;
                }
                // building current word of token
                word[wordLen] = character;
                character = fgetc(input);
                wordLen++;
            }

            //check to see if word is a reserved word
            int isResWord = -1;
            for (int i = 0; i < 14; i++) {
                // if word is a reserved word
                if (strcmp(word, reservedWords[i]) == 0) {
                    strcpy(lexemeList[lexSize].tokenName, word); 
                    isResWord = i;
                }
            }

            assignResToken(isResWord, word);
            lexSize++;
        } else if (isdigit(character)) {
            // converting char to digit
            int digit = character - '0';
            int digitLen = 1;
            character = fgetc(input);
            sameToken = true;

            while (isdigit(character)) {
                // Number is too long
                if (digitLen > MAX_NUM) {
                    lexemeList[lexSize].error = 1;
                    character = fgetc(input);
                    break;
                }

                // converting char to digit for next place
                digit = (10 * digit) + (character - '0');
                character = fgetc(input);
                digitLen++;
            }
            // whole digit has been read
            lexemeList[lexSize].number = digit;
            lexemeList[lexSize].tokenType = numbersym;
            lexSize++;
        } else {
            //if the character is a special symbol
            sameToken = false;

            char invalidSymbol[1] = {0};
            invalidSymbol[0] = character;
            invalidSymbol[1] = '\0';

            int sym = -1;
            for (int i = 0; i < 13; i++) {
                if (character == specialSymbols[i])
                    sym = i;
            }
            assignSymbolToken (sym, invalidSymbol, input, sameToken, character);
        }
        if (sameToken == 0) character = fgetc(input);
    }

    // initialize symbol table
    for (int i = 0; i < MAX_SYMBOL_TABLE_SIZE; i++) {
        symbolTable[i] = (symbol *) malloc(sizeof(symbol));
        symbolTable[i]->kind = -1;
        strcpy(symbolTable[i]->name, "-1");
        symbolTable[i]->val = 0;
        symbolTable[i]->level = 0;
        symbolTable[i]->addr = -1;
        symbolTable[i]->mark = -1;
    }
    // initialize code array
    for (int i = 0; i < MAX_SYMBOL_TABLE_SIZE; i++) {
        code[i] = (instruction *) malloc(sizeof(instruction));
        strcpy(code[i]->op, "-1");
        code[i]->l = -1;
        code[i]->m = -1;
    }
    
    
    // emit JMP, first instruction
    strcpy(code[codeIndex]->op, "JMP");
    code[codeIndex]->l = 0;
    code[codeIndex]->m = 3;
    codeIndex++;
    
    program();

    printInstructions();
    printSymbolTable();

    fclose(input);

    printLexList(tokens);
    fclose(tokens);

    // free memory
    free(lexemeList);

    for (int i = 0; i < MAX_SYMBOL_TABLE_SIZE; i++) {
    free(symbolTable[i]);
}
}