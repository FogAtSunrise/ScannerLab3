#pragma once
#ifndef MAT3_InterCod_H
#define MAT3_InterCod_H

#include <iostream>


#include <vector>
#include "ScannerClass.hpp"

struct Oper
{
    bool isLink = false; // является ли ссылкой 
    int numb; //номер триады-ссылки 
    Lexem lex; // непосредственный операнд



};

struct Triad {
public:
    int triad; // номер триады 
    int operationID; // код операции 
    Oper operand1, operand2; //операнды


};





class InterCod {
private:
    std::vector<Triad> tr;

    int  k;   // индекс  первой триады  для занесения 

public:
    void addTriad(int oper, Oper a, Oper b);

    void printInterCode();
    InterCod();
    std::string getOp(Triad n, int i);
    ~InterCod();


    std::map<int, std::string > forPrint = {

        {tRet,      "return"},
        { tLess ,      "<"},
{ tMore  ,     ">"},
{ tLe  ,     "<="},
{ tMe  ,     ">="},
{ tEq  ,     "=="},
{ tNeq ,     "!="},
{ tOr  ,     "||"},
{ tAnd  ,     "&&"},
{ tPlus  ,     "+"},
{ tMinus  ,     "-"},
{ tMult  ,     "*"},
{ tDiv  ,     "/"},
{ tNot  ,     "!"},
{ tSave  ,     "="},
{ tProc  ,     "proc"},
{ tProl  ,     "prolog"},
{ tEpil  ,     "epilog"},
{ tRetF  ,     "ret"},
{ tEndp  ,     "endp"},
{ tIf  ,     "if"},
    { tGoto  ,     "goto"},
{ tNop  ,     "nop"},
{ tPush  ,     "push"},
{ tPop  ,     "pop"},
{ tCall  ,     "call"}

    };
};



#endif