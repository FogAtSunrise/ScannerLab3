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
        
   //    root->getLexTypeToVar(lexemes[this->pointer].first);

       // root->SemSetTypeVar(cur, root->getLexTypeToVar(lexemes[this->pointer].first));

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
           // int type = root->getLexTypeToVar(lexemes[this->pointer].first);
            this->pointer++;
            dataAnalysis(itsConst, root->getLexTypeToVar(lexemes[this->pointer].first));


        }
        else
            showError("Error! expected: type", lex);
    }

    /*
     * ��������� ����������
     */

    else if (lex.first == tInt || lex.first == tBool || lex.first == tShort || lex.first == tLong) {
        //int type = root->getLexTypeToVar(lexemes[this->pointer].first);
        this->pointer++;
        dataAnalysis(itsVar, root->getLexTypeToVar(lexemes[this->pointer].first));
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

    TypeVar type = root->SemGetTypeF(lex);

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
    if (type < expressionAnalysis().type)
    {//cout << type << " " << root->FromConstToType(expressionAnalysis()) << endl;
        root->PrintError("��� ��������� �� ������������� ���� ������������� �������� ", ret);
        
    }
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

            
            //root->SemSetTypeVar(var, root->getLexTypeToVar(lexemes[this->pointer - 1].first));
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
      //  int type = root->getLexTypeToVar(lexemes[this->pointer].first);
        this->pointer++;
        dataAnalysis(itsVar, root->getLexTypeToVar(lexemes[this->pointer].first));
    }
    else if (lex.first == tConst)
    {

        lex = getNextLexeme();
        if (lex.first == tInt || lex.first == tBool || lex.first == tShort || lex.first == tLong) {
           // int type = root->getLexTypeToVar(lexemes[this->pointer].first);
            this->pointer++;
            dataAnalysis(itsConst, root->getLexTypeToVar(lexemes[this->pointer].first));
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
        int type = expressionAnalysis().type;
       root->SemParamFunc(type);////////////////////////////////////////////////////////////////////////
        lex = getCurrentLexeme();

        while (lex.first == tZpt)
        {
            pointer++;
            type = expressionAnalysis().type;
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

    TypeVar type = root->FromConstToType(expressionAnalysis().type);
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


DataTypeAndValue  Analisys::perevod(TypeVar t, DataTypeAndValue perem)
{
    if (t == TTInt && perem.type != TTInt)
    {
        perem.data.Data_int = perem.data.Data_bool;
    }
    else 
        if (t == TTBool && perem.type != TTBool)
        {
            perem.data.Data_bool = perem.data.Data_int;
        }
    return perem;
}


DataTypeAndValue  Analisys::expressionAnalysis() {////////////////////////////////////////////////////////////////////////////
    DataTypeAndValue type = logI();
    Lexem lex = getCurrentLexeme();

    while (lex.first == tOr) {
        this->pointer++;
        DataTypeAndValue typeDop = logI();
        TypeVar t = root->semResType(type.type, typeDop.type);
       type =  perevod(t, type);
       typeDop = perevod(t, typeDop);

        if (t == TTBool )
        {
            type.data.Data_bool = type.data.Data_bool || typeDop.data.Data_bool;
        }
        else  if (t == TTInt)
        {
            type.data.Data_bool = type.data.Data_int || typeDop.data.Data_int;
        }
        type.type = TTBool;
        lex = getCurrentLexeme();
    }
    return type;
}

DataTypeAndValue Analisys::logI() {////////////////////////////////////////////////////////////////////////////
    DataTypeAndValue type = eqFunc1();
    Lexem lex = getCurrentLexeme();

    while (lex.first == tAnd) {
        this->pointer++;

        DataTypeAndValue typeDop = eqFunc1();
        TypeVar t = root->semResType(type.type, typeDop.type);
        type = perevod(t, type);
        typeDop = perevod(t, typeDop);

        if (t == TTBool)
        {
            type.data.Data_bool = type.data.Data_bool && typeDop.data.Data_bool;
        
        }
        else  if (t == TTInt)
        {
            type.data.Data_bool = type.data.Data_int && typeDop.data.Data_int;
        }
        type.type = TTBool;
        lex = getCurrentLexeme();
    }
    return type;
}

DataTypeAndValue Analisys::eqFunc1() {////////////////////////////////////////////////////////////////////////////
    DataTypeAndValue type = eqFunc2();
    Lexem lex = getCurrentLexeme();

    while (lex.first == tEq || lex.first == tNeq) {

        this->pointer++;

        DataTypeAndValue typeDop = eqFunc2();
        TypeVar t = root->semResType(type.type, typeDop.type);
        type = perevod(t, type);
        typeDop = perevod(t, typeDop);

        if (t == TTBool)
        {
            type.data.Data_bool = type.data.Data_bool == typeDop.data.Data_bool;
        }
 
        else  if (t == TTInt)
        {
            type.data.Data_bool = type.data.Data_int == typeDop.data.Data_int;
        }
        type.type = TTBool;
        if (lex.first == tNeq)
            type.data.Data_bool = !type.data.Data_bool;
        lex = getCurrentLexeme();
    }
    return type;
}

DataTypeAndValue Analisys::eqFunc2() {////////////////////////////////////////////////////////////////////////////
    DataTypeAndValue type = eqFunc3();
    Lexem lex = getCurrentLexeme();

    while (lex.first == tMore || lex.first == tLe) {
        this->pointer++;

        DataTypeAndValue typeDop = eqFunc3();
        TypeVar t = root->semResType(type.type, typeDop.type);
        type = perevod(t, type);
        typeDop = perevod(t, typeDop);

        if (t == TTBool)
        {
            type.data.Data_bool = type.data.Data_bool <= typeDop.data.Data_bool;
        }

        else  if (t == TTInt)
        {
            type.data.Data_bool = type.data.Data_int <= typeDop.data.Data_int;
        }
        type.type = TTBool;
        if (lex.first == tMore)
            type.data.Data_bool = !type.data.Data_bool;
        lex = getCurrentLexeme();
    }
    return type;
}

DataTypeAndValue Analisys::eqFunc3() {////////////////////////////////////////////////////////////////////////////
    DataTypeAndValue type = add();
    Lexem lex = getCurrentLexeme();

    while (lex.first == tLess ||
        lex.first == tMe) {
        this->pointer++;
        DataTypeAndValue typeDop = add();
        TypeVar t = root->semResType(type.type, typeDop.type);
        type = perevod(t, type);
        typeDop = perevod(t, typeDop);
        if (t == TTBool)
        {
            type.data.Data_bool = type.data.Data_bool >= typeDop.data.Data_bool;
        }

        else  if (t == TTInt)
        {
            type.data.Data_bool = type.data.Data_int >= typeDop.data.Data_int;
        }
        type.type = TTBool;
        if (lex.first == tLess)
            type.data.Data_bool = !type.data.Data_bool;
        lex = getCurrentLexeme();
    }
    return type;
}


DataTypeAndValue Analisys::add() {
    DataTypeAndValue type = multiplier();
    Lexem lex = getCurrentLexeme();
    while (lex.first == tPlus || lex.first == tMinus) {
        this->pointer++;
        DataTypeAndValue typeDop = multiplier();
        TypeVar t = root->semResType(type.type, typeDop.type);
        type = perevod(t, type);
        typeDop = perevod(t, typeDop);
        if(lex.first == tPlus) {
            if (t == TTBool)
            {
                type.data.Data_int = type.data.Data_bool + typeDop.data.Data_bool;
            }

            else  if (t == TTInt)
            {
                type.data.Data_int = type.data.Data_int +typeDop.data.Data_int;
            }

        }

        else  if (lex.first == tMinus) {
            if (t == TTBool)
            {
                type.data.Data_int = type.data.Data_bool - typeDop.data.Data_bool;
            }

            else  if (t == TTInt)
            {
                type.data.Data_int = type.data.Data_int - typeDop.data.Data_int;
            }
        }


        type.type = TTInt;
        lex = getCurrentLexeme();
    }
    return type;
}

DataTypeAndValue Analisys::multiplier() {
    DataTypeAndValue type = logNe();
    Lexem lex = getCurrentLexeme();
    while (lex.first == tMult || lex.first == tDiv) {
        this->pointer++;
        DataTypeAndValue typeDop = logNe();
        TypeVar t = root->semResType(type.type, typeDop.type);
        type = perevod(t, type);
        typeDop = perevod(t, typeDop);
        if (lex.first == tMult) {
            if (t == TTBool)
            {
                type.data.Data_int = type.data.Data_bool * typeDop.data.Data_bool;

            }
            else  if (t == TTInt)
            {
                type.data.Data_int = type.data.Data_int * typeDop.data.Data_int;
            }

        }

        else  if (lex.first == tDiv) {
            if (t == TTBool)
            {
                type.data.Data_int = type.data.Data_bool / typeDop.data.Data_bool;
            }

            else  if (t == TTInt)
            {
                type.data.Data_int = type.data.Data_int / typeDop.data.Data_int;
            }
        }


        type.type = TTInt;
        lex = getCurrentLexeme();
    }
    return type;
}


DataTypeAndValue Analisys::logNe() {
    DataTypeAndValue type = { 0, TTypeDef };
    Lexem lex = getCurrentLexeme();
    if (lex.first == tNot) {

        
        this->pointer++;
        type = elementaryExpressionAnalysis();

        if (type.type == TTBool)
            type.data.Data_bool = !type.data.Data_bool;
        else
            type.data.Data_bool = !type.data.Data_int;


            type.type = TTBool;
    }
    else
   type = elementaryExpressionAnalysis();
    
    return type;
}


DataTypeAndValue Analisys::elementaryExpressionAnalysis() {

    DataTypeAndValue type = {0, TTypeDef };
  

    Lexem lex = getCurrentLexeme();
    //���������
    if (lex.first == constInt || lex.first == constHex || lex.first == tTrue || lex.first == tFalse) {

        
        
        type.type = root->FromConstToType(lex.first);
        if(type.type==TTBool)
        type.data.Data_bool = (lex.first == tTrue)? true:false;
        else
            if (type.type == TTInt)
                type.data.Data_int = stoi(lex.second);

        this->pointer++;

    }
    //�������������
    else if (lex.first == tId) {


        //SEMANTIC>>>>>>>>>>>>>>>>>>>


        lex = getNextLexeme();
        //����� �������
        if (lex.first == tLs) {
            //typeObject = root->SemGetTypeF(lexemes[pointer - 1]);
            lex = getNextLexeme();
            listParamFunc();
            lex = getNextLexeme();
        }//
        else
        {

          //  typeObject = root->SemGetTypeV(lexemes[pointer - 1]);

            type = root->GetValueIden(lexemes[pointer - 1]);

        }
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
void Analisys::dataAnalysis(TypeObject obj, TypeVar type) {
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
void Analisys::variableAnalysis(TypeObject obj, TypeVar type) {
    Lexem lex = getCurrentLexeme();

    //������� ��������������
    if (lex.first != tId) { showError("Error, expected: Id", lex); }



    //SEMANTIC>>>>>>>>>>>>>>>>>>>

    //��������� ���������� � ������
    SemTree* cur = root->prologue(lex, obj, Data_Value{0}, lexemes[this->pointer - 1]);
    //root->SemSetTypeVar(cur, type);

    lex = getNextLexeme();

    //���� ���� ������������

    while (lex.first == tSave) {

        Lexem lex1 = lexemes[this->pointer - 1];
        this->pointer++;
        /*root->isAssignable(root->FromConstToType(expressionAnalysis().type), lexemes[this->pointer - 2]);
        //expressionAnalysis();
        lex = getCurrentLexeme();
        */

        DataTypeAndValue typeDop = expressionAnalysis();
      //  root->isAssignable(typeDop.type, lexemes[this->pointer - 3]);
        root->SetValueIden(lex1, typeDop);

        lex = getCurrentLexeme();


    }
}


//������������

void Analisys::evalAnalysis() {
    Lexem lex = getCurrentLexeme();
    lex = getNextLexeme();

    // Except '='
    if (lex.first != tSave) showError("Error, expected: '='", lex);

    Lexem lex1 = this->lexemes[pointer - 1];
    this->pointer++;

    //SEMANTIC>>>>>>>>>>>>>>>>>>>
    DataTypeAndValue type1 = expressionAnalysis();
   // root->isAssignable(type1.type, this->lexemes[pointer - 3]);
    root->SetValueIden(lex1, type1);
    //expressionAnalysis();
}


bool Analisys::checkExistData(std::vector<std::string> data, const std::string& newElement) {
    for (auto& i : data) {
        if (i == newElement) return true;
    }
    return false;
}