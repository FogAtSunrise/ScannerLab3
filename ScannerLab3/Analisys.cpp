#include <iostream>
#include <string>
#include "Analisys.hpp"



//�������� ��� � ����� � �������� ��������
Analisys::Analisys(std::string fileName) {
    setlocale(0, "");
    ScannerClass scanner;
    if (!scanner.readDataFromFile(fileName))
        return;

    Lexem lex;
    do {
        lex = scanner.scan();
        if (lex.first == tError)
            break;
        this->lexemes.push_back(lex);
    } while (lex.first != tEnd);
    this->numb_str = scanner.number_str;
    for (int i = 0; i < this->numb_str.size(); i++)
    {
        this->lexemes[i].numb = this->numb_str[i];
    }
    this->pointer = 0;
}

//�������� ��������� �������
Lexem Analisys::getNextLexeme() {
    return this->lexemes[++pointer];
}

//�������� ������� �������
Lexem Analisys::getCurrentLexeme() {
    return this->lexemes[pointer];
}

//����� ������
void Analisys::showError(const std::string& message, const Lexem& Lexeme) {

    //   std::cout << message + "\nError string " + std::to_string(numb_str[pointer-1]) << std::endl;
    std::cout << message + "\nError string " + std::to_string(Lexeme.numb) << std::endl;
    std::exit(0);
}

int  Analisys::getLine()
{
    return numb_str[pointer];
}
//������� ������
void Analisys::analysis() {
    if (this->lexemes.empty()) {
        std::cout << "Lexems not found\n";
        return;
    }

    while (this->lexemes[pointer].first != tEnd) {
        if (pointer > this->lexemes.size()) showError("Error! not excepted end of file!", this->lexemes[pointer]);
        try {
            checkLexeme();
            this->pointer++;
        }
        catch (const std::out_of_range& e) {
            showError("Error! not excepted end of file!", this->lexemes[pointer]);
        }

    }
}

void Analisys::setSemTree(SemTree* root)
{
    this->root = root;
}

void Analisys::checkLexeme() {
    Lexem lex = getCurrentLexeme();


    /*
    *��������� ������� (������ ����� "�������")
    */
    if ((lex.first == tInt || lex.first == tBool || lex.first == tShort || lex.first == tLong)
        && (this->lexemes[this->pointer + 1].first == tId || this->lexemes[this->pointer + 1].first == tMain)
        && (this->lexemes[this->pointer + 2].first == tLs)) {


        //SEMANTIC>>>>>>>>>>>>>>>>>>>

        //��������� ������� � ������
        SemTree* cur = root->prologue(lexemes[this->pointer + 1], itsFunct, Data_Value{ 1 }, lexemes[this->pointer]);
        
        int type = root->getLexTypeToVar(lexemes[this->pointer].first);
        root->SemSetTypeVar(cur, type);

        this->pointer++;
        functionAnalysis(cur);
        root->SemFinishFunc(cur);
  root->epilogue();
    }


    //����� "������" - ������ ������������ �� ��������� � ����������
    /*
     * ��������� ����������� ���������
     */

    else if (lex.first == tConst) {
        //��������� ������� � ������
        lex = getNextLexeme();
        if (lex.first == tInt || lex.first == tBool || lex.first == tShort || lex.first == tLong) {
            int type = root->getLexTypeToVar(lexemes[this->pointer].first);
            this->pointer++;
            dataAnalysis(itsConst, type);


        }
        else
            showError("Error! expected: type", lex);
    }

    /*
     * ��������� ����������
     */

    else if (lex.first == tInt || lex.first == tBool || lex.first == tShort || lex.first == tLong) {
        int type = root->getLexTypeToVar(lexemes[this->pointer].first);
        this->pointer++;
        dataAnalysis(itsVar, type);
    }
    else {

        showError("Error! expected: type", lex);
    }
}


/*
 * ����� "�������"
 */
void Analisys::functionAnalysis(SemTree* cur) {

    Lexem lex = getCurrentLexeme();

    // Id
    if (lex.first != tId && lex.first != tMain) showError("Error, expected: Id", lex);

    int type = root->SemGetTypeF(lex);

    lex = getNextLexeme();
    // expect '('
    if (lex.first != tLs) showError("Error, expected: '('", lex);


    //���� "������ ����������"
    pointer++;
    oneParam(cur);
    lex = getCurrentLexeme();
    while (lex.first == tZpt)
    {
        pointer++;
        oneParam(cur);
        lex = getCurrentLexeme();
    }

    // expect ')'
    if (lex.first != tPs) showError("Error, expected: ')'", lex);
    lex = getNextLexeme();

    //����� "��������� ��������", �� ������������ ��� ���� �������, �.�. ������ �� return
        // expect '{'
    if (lex.first != tFls) showError("Error, expected: '{'", lex);
    lex = getNextLexeme();


    // ���� �� 'return'
    while (lex.first != tRet) {

        if (lex.first == tFps) showError("Error! Except 'return'", lex);
        operatorAndDescriptionsAnalysis();
        lex = getNextLexeme();

    }
    Lexem ret = lex;
    //����������� ��������� ����� ������
    lex = getNextLexeme();
    if (type < expressionAnalysis())
        root->PrintError("��� ��������� �� ������������� ���� ������������� ��������", ret);

    //���������, ��� ����� return ���� ����� � �������
    lex = getCurrentLexeme();
    // expect ';'
    if (lex.first != tTzpt) showError("Error! ';'", lex);
    lex = getNextLexeme();

    // expect '}'
    if (lex.first != tFps) showError("Error! '}'", lex);

}

//��������������� �������������, ����� ����� "������ ����������"
void Analisys::oneParam(SemTree* cur)
{
    Lexem lex = getCurrentLexeme();
    if (lex.first == tInt || lex.first == tBool || lex.first == tShort || lex.first == tLong) {
        lex = getNextLexeme();
        if (lex.first == tId) {

            //SEMANTIC>>>>>>>>>>>>>>>>>>>

                //��������� �������� ������� � ������
            SemTree* var = root->prologue(lex, itsVar, Data_Value{ 1 }, lexemes[this->pointer - 1]);

            int type = root->getLexTypeToVar(lexemes[this->pointer - 1].first);
            root->SemSetTypeVar(var, type);
            //��������� ����� ���������� �������
            root->increase(cur);

            lex = getNextLexeme();
        }
        else showError("Error, expected parameter name", lex);
    }

    else if (lex.first != tPs) { showError("Error, expected parameter type", lex); }
}

// ����� "�������� � ��������"
void Analisys::operatorAndDescriptionsAnalysis() {

    //"������"
    Lexem lex = getCurrentLexeme();
    if ((lex.first == tInt || lex.first == tShort || lex.first == tBool || lex.first == tLong) && lexemes[this->pointer + 2].first != tLs) {
        int type = root->getLexTypeToVar(lexemes[this->pointer].first);
        this->pointer++;
        dataAnalysis(itsVar, type);
    }
    else if (lex.first == tConst)
    {

        lex = getNextLexeme();
        if (lex.first == tInt || lex.first == tBool || lex.first == tShort || lex.first == tLong) {
            int type = root->getLexTypeToVar(lexemes[this->pointer].first);
            this->pointer++;
            dataAnalysis(itsConst, type);
        }
        else showError("Error! expected: type", lex);
    }

    //"��������"
    else operatorAnalysis();

}


int Analisys::listParamFunc()
{
    Lexem lex;
    //������ ����������� ����������
    int count = 0;
    SemTree* cur = root->GetCur();
    SemTree* func = root->SemGetFunc(lexemes[pointer - 2]);
    if (lexemes[pointer].first != tPs)
    {
        count = 1;
        int type = expressionAnalysis();
       root->SemParamFunc(type);////////////////////////////////////////////////////////////////////////
        lex = getCurrentLexeme();

        while (lex.first == tZpt)
        {
            pointer++;
            type = expressionAnalysis();
            root->SemParamFunc(type);//////////////////////////////////////////////////////////////////////
            count++;
            lex = getCurrentLexeme();
        }
        if (lex.first != tPs) showError("Error, expected: ')'!", lex);

    }
    root->SemControlCountParam(func, count);
    root->SemFinishFunc(cur);


    return count;
}
//����� "��������"
void Analisys::operatorAnalysis() {

    Lexem lex = getCurrentLexeme();

    //������������
    if (lex.first == tId && this->lexemes[pointer + 1].first == tSave) {
        evalAnalysis();
    }

    //����� "����� �������"
    else if (lex.first == tId && this->lexemes[pointer + 1].first == tLs) {


        this->pointer += 2;
        lex = getCurrentLexeme();


        listParamFunc();


        lex = getNextLexeme();

        //    Except ';'
        if (lex.first != tTzpt) showError("Error, expected: ';'", lex);
    }

    //�����
    else if (lex.first == tSwi) {
        this->pointer++;
        cycleAnalysis();
    }

    //����� "��������� ��������"
    else if (lex.first == tFls) {

        //SEMANTIC>>>>>>>>>>>>>>>>>>>

        //������� ����
        SemTree* cur = root->prologue(Lexem{29, "block", lex.numb}, itsVar, Data_Value{ 1 }, lexemes[this->pointer - 1]);
       
        lex = getNextLexeme();
        // ���� �� '}'
        while (lex.first != tFps) {

            if (lex.first == tRet) showError("Error! Except '}'", lex);
            operatorAndDescriptionsAnalysis();
            lex = getNextLexeme();
        }

        root->SemFinishFunc(cur);
       root->epilogue();
    }

    //return
    else if (lex.first == tRet) {
        this->pointer++;
        expressionAnalysis();
    }

    // Except ';'
   // else if (lex.first != tTzpt) showError("Error, expected: ';'", lex);
}


//����� "switch"
void Analisys::cycleAnalysis() {
    Lexem lex = getCurrentLexeme();

    // Except '('
    if (lex.first != tLs) showError("Error, expected: '('", lex);
    lex = getNextLexeme();

    int type = expressionAnalysis();
    lex = getCurrentLexeme();

    // Except ')'
    if (lex.first != tPs) showError("Error, expected: ')'", lex);
    lex = getNextLexeme();

    // Except '{'
    if (lex.first != tFls) showError("Error, expected: '{'", lex);
    lex = getNextLexeme();

    while (lex.first != tFps) {
        if (lex.first == tDef)
        {
            lex = getNextLexeme();
            operatorCase();
        }
        else  if (lex.first == tCase)
        {
            lex = getNextLexeme();

            if (lex.first != constInt && lex.first != constHex && lex.first != tTrue && lex.first != tFalse) showError("Error, expected: 'expression'", lex);


            //SEMANTIC>>>>>>>>>>>>>>>>>>>
            root->semConsInSwich(type, lex);

            lex = getNextLexeme();
            operatorCase();
        }
        else showError("Error, expected: 'Case' or '}'", lex);
        lex = getNextLexeme();
    }

}

//���� case
void Analisys::operatorCase() {

    Lexem lex = getCurrentLexeme();
    if (lex.first != tDtoch) showError("Error, expected: ':'", lex);
    SemTree* var = root->prologue(Lexem{29, "Case", lex.numb}, itsVar, Data_Value{ 1 }, lexemes[this->pointer - 1]);
    lex = getNextLexeme();
    //"��������� ��������"
    while (lex.first != tBreak) {
        operatorAndDescriptionsAnalysis();
        lex = getNextLexeme();
        if (lex.first == tFps || lex.first == tDef || lex.first == tCase) showError("Error, expected: 'break'", lex);
    }

    lex = getNextLexeme();
    if (lex.first != tTzpt) showError("Error, expected: ';'", lex);
    root->epilogue();
}

int Analisys::expressionAnalysis() {////////////////////////////////////////////////////////////////////////////
    int type = logI();
    Lexem lex = getCurrentLexeme();

    while (
        lex.first == tOr) {
        this->pointer++;
        //type = root->semResType(type, logI());
        type = TBool;
        lex = getCurrentLexeme();
    }
    return type;
}

int Analisys::logI() {////////////////////////////////////////////////////////////////////////////
    int type = eqFunc1();
    Lexem lex = getCurrentLexeme();

    while (lex.first == tAnd) {
        this->pointer++;
        // type = root->semResType(type, eqFunc1());
        type = TBool;
        lex = getCurrentLexeme();
    }
    return type;
}

int Analisys::eqFunc1() {////////////////////////////////////////////////////////////////////////////
    int type = eqFunc2();
    Lexem lex = getCurrentLexeme();

    while (lex.first == tEq || lex.first == tNeq) {
        this->pointer++;
        //  type = root->semResType(type, eqFunc2());
        type = TBool;
        lex = getCurrentLexeme();
    }
    return type;
}

int Analisys::eqFunc2() {////////////////////////////////////////////////////////////////////////////
    int type = add();
    Lexem lex = getCurrentLexeme();

    while (
        lex.first == tMore || lex.first == tLess ||
        lex.first == tLe || lex.first == tMe) {

        this->pointer++;
        //type = root->semResType(type, add());
        type = TBool;
        lex = getCurrentLexeme();
    }
    return type;
}



int Analisys::add() {
    int type = multiplier();
    Lexem lex = getCurrentLexeme();
    while (lex.first == tPlus || lex.first == tMinus) {
        lex = getNextLexeme();
        type = root->semResType(type, multiplier());
        lex = getCurrentLexeme();
    }
    return type;
}

int Analisys::multiplier() {
    int type = logNe();
    Lexem lex = getCurrentLexeme();
    while (lex.first == tMult || lex.first == tDiv) {
        lex = getNextLexeme();
        type = root->semResType(type, logNe());
        lex = getCurrentLexeme();
    }
    return type;
}


int Analisys::logNe() {
    int type = elementaryExpressionAnalysis();
    Lexem lex = getCurrentLexeme();
    if (lex.first == tNot) {
        type = TBool;
        lex = getNextLexeme();
    }
    return type;
}


int Analisys::elementaryExpressionAnalysis() {

    int type = TypeDef;

    Lexem lex = getCurrentLexeme();
    //���������
    if (lex.first == constInt || lex.first == constHex || lex.first == tTrue || lex.first == tFalse) {

        //SEMANTIC>>>>>>>>>>>>>>>>>>>
        type = root->FromConstToType(lex.first);
        this->pointer++;

    }
    //�������������
    else if (lex.first == tId) {


        //SEMANTIC>>>>>>>>>>>>>>>>>>>


        lex = getNextLexeme();
        //����� �������
        if (lex.first == tLs) {
            type = root->SemGetTypeF(lexemes[pointer - 1]);
            lex = getNextLexeme();

            listParamFunc();



            lex = getNextLexeme();
        }//
        else  type = root->SemGetTypeV(lexemes[pointer - 1]);
    }
    //(���������)
    else if (lex.first == tLs) {
        pointer++;
        type = expressionAnalysis();


        lex = getCurrentLexeme();
        if (lex.first != tPs) showError("Error, expected: ')'!", lex);
        pointer++;
    }//
    else showError("Error, expected: Value", lex);

    return type;
}


//����� "������"
void Analisys::dataAnalysis(TypeObject obj, int type) {
    Lexem lex = getCurrentLexeme();
    variableAnalysis(obj, type);
    lex = getCurrentLexeme();
    while (lex.first != tTzpt) {
        if (lex.first != tZpt) showError("Error, expected: ';' or ','", lex);
        this->pointer++;
        variableAnalysis(obj, type);
        lex = getCurrentLexeme();
    }
}

//����� ����� "������" - �������������� ��� ������������� � �������������
void Analisys::variableAnalysis(TypeObject obj, int type) {
    Lexem lex = getCurrentLexeme();

    //������� ��������������
    if (lex.first != tId) { showError("Error, expected: Id", lex); }



    //SEMANTIC>>>>>>>>>>>>>>>>>>>

    //��������� ���������� � ������
    SemTree* cur = root->prologue(lex, obj, Data_Value{0}, lexemes[this->pointer - 1]);
    root->SemSetTypeVar(cur, type);

    lex = getNextLexeme();

    //���� ���� ������������
    while (lex.first == tSave) {

        this->pointer++;
        root->isAssignable(expressionAnalysis(), lexemes[this->pointer - 2]);
        //expressionAnalysis();
        lex = getCurrentLexeme();

    }
}


//������������

void Analisys::evalAnalysis() {
    Lexem lex = getCurrentLexeme();
    lex = getNextLexeme();

    // Except '='
    if (lex.first != tSave) showError("Error, expected: '='", lex);

    this->pointer++;

    //SEMANTIC>>>>>>>>>>>>>>>>>>>
    root->isAssignable(expressionAnalysis(), this->lexemes[pointer - 2]);
    //expressionAnalysis();
}


bool Analisys::checkExistData(std::vector<std::string> data, const std::string& newElement) {
    for (auto& i : data) {
        if (i == newElement) return true;
    }
    return false;
}