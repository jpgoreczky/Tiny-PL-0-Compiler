#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define MAX_NUM 4
#define MAX_WORD 10

//struct for token types
typedef enum {
skipsym = 1, identsym, numbersym, plussym, minussym,
multsym, slashsym, fisym, eqsym, neqsym, lessym, leqsym,
gtrsym, geqsym, lparentsym, rparentsym, commasym, semicolonsym,
periodsym, becomessym, beginsym, endsym, ifsym, thensym,
whilesym, dosym, callsym, constsym, varsym, procsym, writesym,
readsym , elsesym} token_type;

//token structure
typedef struct{
    token_type tokenType;
    int error;
    int number;
    char tokenName[MAX_WORD]; 
} tokenStruct;

//array for reserved words names
char *reservedWords[] = {"const", "var", "procedure", "call", "begin", "end", "if", "fi", "then", "else", "while", "do", "read", "write"};

//array for special symbols
char specialSymbols[] = {'+', '-', '*', '/', '(', ')', '=', ',', '.', '<', '>', ';', ':'};

tokenStruct * lexemeList; //array of token structs
int lexSize = 0;   //keeps track of array location

void printSourceProgram(FILE * fp) {
    printf("Source Program: \n");
    char input;

    fseek(fp, 0, SEEK_SET);
    while((input = fgetc(fp)) != EOF) {
        putchar(input);
    }
}

void printerror(int index) {
    if (lexemeList[index].tokenType == 3){ // IF NUMBER
        if(lexemeList[index].error == 1){
            printf("\n%d\t\t", lexemeList[index].number);
            printf("NUMBER TOO LONG");
        } else {
            printf("\n%d\t\t%d\t", lexemeList[index].number, lexemeList[index].tokenType);
        }
        
    }else{ // NOT NUMBER
        if(lexemeList[index].error == 2){
            printf("\n%s\t\t", lexemeList[index].tokenName);
            printf("NAME TOO LONG");
        }else if(lexemeList[index].error == 3){
            printf("\n%s\t\t", lexemeList[index].tokenName);
            printf("INVALID SYMBOLS");
        } else {
            printf("\n%s\t\t%d\t", lexemeList[index].tokenName, lexemeList[index].tokenType);
        }
    }
}

void printLexList() {
    printf("\nLexeme Table:\nLexeme\t\tToken Type\n");
    for (int i = 0; i < lexSize; i++) {
        printerror(i);
    }
    //prints lexeme list
    printf("\n\nToken List\n");
    for (int j = 0; j < lexSize; j++){
        if (lexemeList[j].error != -1) {
            continue;
        }
        printf("%d ", lexemeList[j].tokenType);
        if(lexemeList[j].tokenType == 2){
            printf("%s ", lexemeList[j].tokenName);
        } else if (lexemeList[j].tokenType == 3){
            printf("%d ", lexemeList[j].number);
        }
    }
    printf("\n");
}

tokenStruct * assignResToken(int isResWord, char * word) {
    switch (isResWord)
    {
    case 0:
        lexemeList[lexSize].tokenType = constsym;
        break;
    case 1:
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

int main(int argc, char ** argv) {
    bool sameToken = false; //used in the event of multiple character tokens

    FILE *input = fopen(argv[1], "r");
    int token = fgetc(input); //stores each character in file
    printSourceProgram(input);
    fclose(input);
    
    // initializing array 
    lexemeList = (tokenStruct *) malloc(sizeof(tokenStruct) * 3000); 
    for (int i = 0; i < 3000; i ++) {
        lexemeList[i].error = -1;
        lexemeList[i].number = -1;
        strcpy(lexemeList[i].tokenName, "-1");
        lexemeList[i].tokenType = -1;
    }

    // reset file
    input = fopen(argv[1], "r");
    token = fgetc(input);

    while (token != EOF) {
        //ignore tabs, white spaces, and newlines
        if (iscntrl(token) || isspace(token)) {
            token = fgetc(input);
            continue;
        } else if (isalpha(token)) {
            int wordLen = 0;
            char word[12] = {}; //12 chars is error identifier wordLen

            word[wordLen] = token;
            sameToken = true;
            
            token = fgetc(input);
            wordLen++;
            while (isalpha(token)) {
                //errors out if identifier is longer than 11 characters
                if (wordLen > MAX_WORD) {
                    lexemeList[lexSize].error = 2;      
                    token = fgetc(input);   // next character
                    sameToken = false; //now beginning to scan new token
                    break;
                }
                // building current word of token
                word[wordLen] = token;
                token = fgetc(input);
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
        } else if (isdigit(token)) {
            // converting char to digit
            int digit = token - '0';
            int digitLen = 1;
            token = fgetc(input);
            sameToken = true;

            while (isdigit(token)) {
                // Number is too long
                if (digitLen > MAX_NUM) {
                    lexemeList[lexSize].error = 1;
                    token = fgetc(input);
                    break;
                }

                // converting char to digit for next place
                digit = (10 * digit) + (token - '0');
                token = fgetc(input);
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
            invalidSymbol[0] = token;
            invalidSymbol[1] = '\0';

            int sym = -1;
            for (int i = 0; i < 13; i++) {
                if (token == specialSymbols[i])
                    sym = i;
            }
            assignSymbolToken (sym, invalidSymbol, input, sameToken, token);
        }
        if (sameToken == 0) token = fgetc(input);
    }
    printLexList();

   free(lexemeList);
}