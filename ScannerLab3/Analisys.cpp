#include <iostream>
#include <string>
#include "Analisys.hpp"



//считываю код с файла и пробегаю сканером
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

//получить следующую лексему
Lexem Analisys::getNextLexeme() {
    return this->lexemes[++pointer];
}

//получить текущую лексему
Lexem Analisys::getCurrentLexeme() {
    return this->lexemes[pointer];
}

//вывод ошибки
void Analisys::showError(const std::string& message, const Lexem& Lexeme) {

    //   std::cout << message + "\nError string " + std::to_string(numb_str[pointer-1]) << std::endl;
    std::cout << message + "\nError string " + std::to_string(Lexeme.numb) << std::endl;
    std::exit(0);
}

int  Analisys::getLine()
{
    return numb_str[pointer];
}
//начинаю анализ
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
    *Проверяем функцию (начало схемы "Функция")
    */
    if ((lex.first == tInt || lex.first == tBool || lex.first == tShort || lex.first == tLong)
        && (this->lexemes[this->pointer + 1].first == tId || this->lexemes[this->pointer + 1].first == tMain)
        && (this->lexemes[this->pointer + 2].first == tLs)) {


        //SEMANTIC>>>>>>>>>>>>>>>>>>>

        //добавляем функцию в дерево
        SemTree* cur = root->prologue(lexemes[this->pointer + 1], itsFunct, Data_Value{ 1 }, lexemes[this->pointer]);
        
        lex = getNextLexeme();

        // Id
        if (lex.first != tId && lex.first != tMain) showError("Error, expected: Id", lex);
        bool IsMain = (lex.first == tMain ? true : false);

        

        lex = getNextLexeme();
        // expect '('
        if (lex.first != tLs) showError("Error, expected: '('", lex);


        //блок "список параметров"
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

        
       root->flagInterpret = IsMain ? true : false;
      
          
      
        
       bool FlagReturnMeet = root->flagReturn;
       root->flagReturn = false;
       cur->n->adress = pointer;
       functionAnalysis(cur);
       root->flagReturn = FlagReturnMeet;
    
       root->flagInterpret = true;

        root->SemFinishFunc(cur);
  root->epilogue();
    }


    //Схема "Данные" - первое разветвление на константу и переменную
    /*
     * Проверяем именованные константы
     */

    else if (lex.first == tConst) {
        //добавляем функцию в дерево
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
     * Проверяем переменную
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
 * схема "функция"
 */
void Analisys::functionAnalysis(SemTree* cur) {

    SemTree* treeFunc = root->CurFunc;
    root->CurFunc = cur;

   
   // bool FlagReturnMeet = false;

    
    Lexem lex = getCurrentLexeme();

    //схема "составной оператор", но подстроенная под тело функции, т.е. следим за return
        // expect '{'
    if (lex.first != tFls) showError("Error, expected: '{'", lex);
    lex = getNextLexeme();


    // пока не 'return'
    while (lex.first != tFps && !root->flagReturn) {

        
        // expect '}'
        if ((lex.first>4&& lex.first < 9)&&(lexemes[pointer+1].first == tId)&&(lexemes[pointer + 2].first)==tLs) showError("Error! '}'", lex);
        operatorAndDescriptionsAnalysis();
        lex = getNextLexeme();

    }

    if (!root->flagReturn) showError("Error! Except 'return'", lex);
    else if(lex.first != tFps) //если встретили ретурн, но не в конце, прогоняем функцию до конца
        // пока не 'return'
        while (lex.first != tFps ) {

                // expect '}'
            if ((lex.first > 4 && lex.first < 9) && (lexemes[pointer + 1].first == tId) && (lexemes[pointer + 2].first) == tLs) showError("Error! '}'", lex);
            lex = getNextLexeme();

        }
      
    root->CurFunc = treeFunc;

}

//вспомогательная подпрограммка, часть схемы "Список параметров"
void Analisys::oneParam(SemTree* cur)
{
    Lexem lex = getCurrentLexeme();
    if (lex.first == tInt || lex.first == tBool || lex.first == tShort || lex.first == tLong) {
        lex = getNextLexeme();
        if (lex.first == tId) {

            //SEMANTIC>>>>>>>>>>>>>>>>>>>

                //добавляем параметр функции в дерево
            SemTree* var = root->prologue(lex, itsVar, Data_Value{ 1 }, lexemes[this->pointer - 1]);

            
            //root->SemSetTypeVar(var, root->getLexTypeToVar(lexemes[this->pointer - 1].first));
            //Увеличить число параметров функции
            root->increase(cur);

            lex = getNextLexeme();
        }
        else showError("Error, expected parameter name", lex);
    }

    else if (lex.first != tPs) { showError("Error, expected parameter type", lex); }
}

// схема "Оператор и описания"
void Analisys::operatorAndDescriptionsAnalysis() {

    //"данные"
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

    //"оператор"
    else operatorAnalysis();

}


int Analisys::listParamFunc(SemTree* func)
{

    SemTree* treeFunc = root->CurFunc;
    
root->CurFunc = func->Right;
    SemTree* treeFunc1 = root->Cur;
    root->Cur = func;
    
    Lexem lex;
    //список фактических параметров
    int count = 0;

    if (lexemes[pointer].first != tPs)
    {
        count = 1;
    
       root->SemParamFunc(expressionAnalysis());////////////////////////////////////////////////////////////////////////
        lex = getCurrentLexeme();

        while (lex.first == tZpt)
        {
            pointer++;
           
            root->SemParamFunc(expressionAnalysis());//////////////////////////////////////////////////////////////////////
            count++;
            lex = getCurrentLexeme();
        }
        if (lex.first != tPs) showError("Error, expected: ')'!", lex);

    }
    root->SemControlCountParam(func, count);
    

    root->CurFunc = treeFunc;
    root->Cur = treeFunc1;
    return count;
}
//схема "оператор"
void Analisys::operatorAnalysis() {

    Lexem lex = getCurrentLexeme();

    //присваивание
    if (lex.first == tId && this->lexemes[pointer + 1].first == tSave) {
        evalAnalysis();
    }

    //схема "вызов функции"
    else if (lex.first == tId && this->lexemes[pointer + 1].first == tLs) {
        CallFunction();

      
        lex = getCurrentLexeme();

        //    Except ';'
        if (lex.first != tTzpt) showError("Error, expected: ';'", lex);
    }

    //свитч
    else if (lex.first == tSwi) {
        this->pointer++;
        cycleAnalysis();
    }

    //схема "составной оператор"
    else if (lex.first == tFls) {

        //SEMANTIC>>>>>>>>>>>>>>>>>>>

        //добавим блок
        SemTree* cur = root->prologue(Lexem{29, "block", lex.numb}, itsVar, Data_Value{ 1 }, lexemes[this->pointer - 1]);
       
        lex = getNextLexeme();
        // пока не '}'
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
        DataTypeAndValue res = expressionAnalysis();
        if (root->GetTypeCur(root->CurFunc) < res.type)
        {
            root->PrintError("Тип выражения не соответствует типу возвращаемого функцией ", lex);
        }
        else
        {
           
            root->SetValueFunc(root->getId(root->CurFunc), res);
            //проверяем, что после return есть точка с запятой
            lex = getCurrentLexeme();
            // expect ';'
            if (lex.first != tTzpt) showError("Error! ';'", lex);
            root->flagReturn = true;

        }
    }

    // Except ';'
   // else if (lex.first != tTzpt) showError("Error, expected: ';'", lex);
}


//схема "switch"
void Analisys::cycleAnalysis() {

    bool localFlag = flagInterpret; //запоминаю текущее состояние флага интерпритации
    bool flagBreak = false; //флаг break, false - если break еще не вызывался, true если break уже был
   
    DataTypeAndValue meanSwitch;//значение в скобках
    
    Lexem lex = getCurrentLexeme();

    // Except '('
    if (lex.first != tLs) showError("Error, expected: '('", lex);
    lex = getNextLexeme();

    meanSwitch = expressionAnalysis();
    TypeVar type = root->FromConstToType(meanSwitch.type);
    lex = getCurrentLexeme();

    // Except ')'
    if (lex.first != tPs) showError("Error, expected: ')'", lex);
    lex = getNextLexeme();
    flagInterpret = false;
    // Except '{'
    if (lex.first != tFls) showError("Error, expected: '{'", lex);
    lex = getNextLexeme();


    while (lex.first != tFps) {

        if (lex.first == tDef ){
            lex = getNextLexeme();
            if (!flagBreak) //default 
                flagBreak = operatorCase();
            
           
        }
        else  if (lex.first == tCase ) //case
        {
            lex = getNextLexeme();

            if (!flagBreak)
            { 
                DataTypeAndValue  meanCase = expressionAnalysis();

                if (!flagInterpret)
                {
                    
                    //проверка выражения в кейсе

                   
                    DataTypeAndValue  meanDop = meanSwitch;
                    TypeVar needType = root->semResType(meanSwitch.type, meanCase.type);

                    if (meanCase.type != needType)
                        meanCase = perevod(needType, meanCase);

                    if (meanSwitch.type != needType)
                        meanDop = perevod(needType, meanDop);

                    if (needType == TTBool)
                    {
                        if (meanDop.data.Data_bool == meanCase.data.Data_bool)
                        {
                            flagInterpret = true;
                            flagBreak = operatorCase();
                        }
                    }

                    else  if (needType == TTInt)
                    {
                        if (meanDop.data.Data_int == meanCase.data.Data_int)
                        {
                            flagInterpret = true;
                            flagBreak = operatorCase();
                        }
                    }
                }
                  
                else  flagBreak = operatorCase();

            }
        }
        else         {
            while (lex.first != tCase && lex.first != tDef && lex.first != tFps)
            {
                lex = getNextLexeme();
            }
        }
      
        lex = getCurrentLexeme();

        if(root->flagReturn)
            while (lex.first != tFps) { lex = getNextLexeme(); }
    }


    flagInterpret = localFlag; //восстанавливаю состояние флага интерпретации
}



//тело case
bool Analisys::operatorCase() {

    Lexem lex = getCurrentLexeme();
    if (lex.first != tDtoch) showError("Error, expected: ':'", lex);
   // SemTree* var = root->prologue(Lexem{29, (lexemes[this->pointer - 1].first==tDef)? "default":"Case", lex.numb}, itsVar, Data_Value{ 1 }, lexemes[this->pointer - 1]);
    lex = getNextLexeme();
    //"составной оператор"
    while (lex.first != tBreak && lex.first != tCase && lex.first != tDef && lex.first != tFps) {
        operatorAndDescriptionsAnalysis();
        lex = getNextLexeme();
        //if (lex.first == tFps || lex.first == tDef || lex.first == tCase) showError("Error, expected: 'break'", lex);
    }  
    
   // root->epilogue();

    if (lex.first == tBreak)
    {
    lex = getNextLexeme();
    if (lex.first != tTzpt) showError("Error, expected: ';'", lex);

    while (lex.first != tCase && lex.first != tDef && lex.first != tFps) 
    {
        lex = getNextLexeme();
    }
    flagInterpret = false;
    return true;
    }
    
  

    return false;;
}


DataTypeAndValue  Analisys::perevod(TypeVar t, DataTypeAndValue perem)
{
    if (t == TTInt && perem.type != TTInt)
    {
        perem.data.Data_int = perem.data.Data_bool;
        perem.type = TTInt;
        
        
        interCode->addTriad(tPreob, perem.oper, Oper{ false, 0, Lexem{1, "int", 1} });//**********************************************************************************************
        perem.oper = Oper{ true, interCode->getK() - 1, Lexem{1, "", 1} };
    }
    else 
        if (t == TTBool && perem.type != TTBool)
        {
            perem.data.Data_bool = perem.data.Data_int;
            perem.type = TTBool;
            interCode->addTriad(tPreob, perem.oper, Oper{ false, 0, Lexem{1, "bool", 1} });//**********************************************************************************************
            perem.oper = Oper{ true, interCode->getK() - 1, Lexem{1, "", 1} };
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
        interCode->addTriad(tOr, type.oper, typeDop.oper);//*****************************************************************************************************    
        type.oper = Oper{ true, interCode->getK() - 1, Lexem{1, "", 1} };//***********************************************************************************************************


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

        interCode->addTriad(tAnd, type.oper, typeDop.oper);//*****************************************************************************************************    
        type.oper = Oper{ true, interCode->getK() - 1, Lexem{1, "", 1} };//***********************************************************************************************************


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
        {  type.data.Data_bool = !type.data.Data_bool;

        interCode->addTriad(tNeq, type.oper, typeDop.oper);//*****************************************************************************************************    
        }
        else
            interCode->addTriad(tEq, type.oper, typeDop.oper);


        type.oper = Oper{ true, interCode->getK() - 1, Lexem{1, "", 1} };//***********************************************************************************************************

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
        {type.data.Data_bool = !type.data.Data_bool;
        interCode->addTriad(tMore, type.oper, typeDop.oper);//*****************************************************************************************************    
        }
        else
            interCode->addTriad(tLe, type.oper, typeDop.oper);


        type.oper = Oper{ true, interCode->getK() - 1, Lexem{1, "", 1} };//***********************************************************************************************************

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
        {
            type.data.Data_bool = !type.data.Data_bool;
            interCode->addTriad(tLess, type.oper, typeDop.oper);//*****************************************************************************************************    
        }
        else
            interCode->addTriad(tMe, type.oper, typeDop.oper);


        type.oper = Oper{ true, interCode->getK() - 1, Lexem{1, "", 1} };//***********************************************************************************************************

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
            interCode->addTriad(tPlus, type.oper, typeDop.oper);//*****************************************************************************************************         

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
            interCode->addTriad(tMinus, type.oper, typeDop.oper);//*****************************************************************************************************         

        }


        type.type = TTInt;

      
        type.oper = Oper{ true, interCode->getK() - 1, Lexem{1, "", 1} };//***********************************************************************************************************

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
            interCode->addTriad(tMult, type.oper, typeDop.oper);//*****************************************************************************************************  
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
            interCode->addTriad(tDiv, type.oper, typeDop.oper);//*****************************************************************************************************  
        }


        type.type = TTInt;

       
        type.oper = Oper{ true, interCode->getK() - 1, Lexem{1, "", 1} };//***********************************************************************************************************

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
        {
            type.data.Data_bool = !type.data.Data_int;
           
        }

            type.type = TTBool;

            interCode->addTriad(tNot, type.oper, Oper{ false, 0, Lexem{1, "", 1} });//*****************************************************************************************************         
            type.oper = Oper{ true, interCode->getK() - 1, Lexem{1, "", 1} };//***********************************************************************************************************
      
    }
    else
   type = elementaryExpressionAnalysis();
    
    return type;
}

DataTypeAndValue Analisys::CallFunction()
{ 
    Lexem lex = getCurrentLexeme();
    if (root->flagInterpret)
    {
        SemTree* func = root->CreateCopy(root->SemGetFuncId(lexemes[pointer]));
        this->pointer += 2;

        listParamFunc(func);

        int adr = pointer + 1;
        pointer = func->n->adress;
     //   root->Print();

        bool FlagReturnMeet = root->flagReturn;
        root->flagReturn = false;
        SemTree* treeFunc1 = root->Cur;
        root->Cur = func->Right;
        for(int i= 0; i<func->n->param; i++)
        root->Cur = root->Cur->Left;
        functionAnalysis(func);
        root->Cur = treeFunc1;
        root->flagReturn = FlagReturnMeet;
        pointer = adr;
        DataTypeAndValue result{ func->n->data, func->n->typeVar };
        root->DelCopy(func);
        return result;
    }
    else while(lex.first != tTzpt)
        lex = getNextLexeme();
   
}

DataTypeAndValue Analisys::elementaryExpressionAnalysis() {

    DataTypeAndValue type = {0, TTypeDef };
  

    Lexem lex = getCurrentLexeme();
    //константа
    if (lex.first == constInt || lex.first == constHex || lex.first == tTrue || lex.first == tFalse) {

        
        
        type.type = root->FromConstToType(lex.first);
        if(type.type==TTBool)
        type.data.Data_bool = (lex.first == tTrue)? true:false;
        else
            if (type.type == TTInt)
                type.data.Data_int = stoi(lex.second);

        type.oper = Oper{false, 0, lex};
        this->pointer++;

    }
    //идентификатор
    else if (lex.first == tId) {


        //SEMANTIC>>>>>>>>>>>>>>>>>>>


       
        //вызов функции
        if (lexemes[pointer + 1].first == tLs) {
           type= CallFunction();
            //typeObject = root->SemGetTypeF(lexemes[pointer - 1]);
          
        }//
        else
        {
            lex = getNextLexeme();
          //  typeObject = root->SemGetTypeV(lexemes[pointer - 1]);
          
            type = root->GetValueIden(lexemes[pointer - 1]);
            type.oper = Oper{ false, 0, lexemes[pointer - 1] };//*****************************************
        
           
        }
    }
    //(выражение)
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


//Схема "Список"
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

//часть схемы "список" - идентификатора или идентификатор с присваиванием
void Analisys::variableAnalysis(TypeObject obj, TypeVar type) {
    Lexem lex = getCurrentLexeme();

    //наличие идентификатора
    if (lex.first != tId) { showError("Error, expected: Id", lex); }



    //SEMANTIC>>>>>>>>>>>>>>>>>>>

    //добавляем переменную в дерево
    SemTree* cur = root->prologue(lex, obj, Data_Value{0}, lexemes[this->pointer - 1]);
    //root->SemSetTypeVar(cur, type);

    lex = getNextLexeme();

    //если есть присваивание

    while (lex.first == tSave) {

        Lexem lex1 = lexemes[this->pointer - 1];
        this->pointer++;
        /*root->isAssignable(root->FromConstToType(expressionAnalysis().type), lexemes[this->pointer - 2]);
        //expressionAnalysis();
        lex = getCurrentLexeme();
        */

        DataTypeAndValue typeDop = expressionAnalysis();

        typeDop=perevod(root->getLexTypeToVar(lexemes[this->pointer - 4].first), typeDop);

        interCode->addTriad(tSave, Oper{ false, 0, lex1 }, typeDop.oper);//*************************************************************************************************8


        
        root->SetValueIden(lex1, typeDop);

        lex = getCurrentLexeme();


    }
}


//присваивание

void Analisys::evalAnalysis() {
    Lexem lex = getCurrentLexeme();
    Lexem iden = lex;

    lex = getNextLexeme();

    // Except '='
    if (lex.first != tSave) showError("Error, expected: '='", lex);

   // Lexem lex1 = this->lexemes[pointer - 1];
    this->pointer++;

    //SEMANTIC>>>>>>>>>>>>>>>>>>>
    DataTypeAndValue type1 = expressionAnalysis();



   // type1 = perevod(root->SemGetTypeF(iden), type1);

    interCode->addTriad(tSave, Oper{false, 0, iden }, type1.oper);//*************************************************************************************************8
   // root->isAssignable(type1.type, this->lexemes[pointer - 3]);
   // root->SetValueIden(lex1, type1);
    root->SetValueIden(iden, type1);
    //expressionAnalysis();
}


bool Analisys::checkExistData(std::vector<std::string> data, const std::string& newElement) {
    for (auto& i : data) {
        if (i == newElement) return true;
    }
    return false;
}