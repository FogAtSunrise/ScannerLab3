#ifndef SRC_ANALISYS_H
#define SRC_ANALISYS_H


#include <vector>
#include "ScannerClass.hpp"
#include "SemTree.hpp"
#include "InterCod.hpp"



class Analisys {
protected:
    SemTree* root;
public:
    InterCod* interCode;
    explicit Analisys(std::string scanner);
    Analisys()
    {
    }
    /**
     * Запустить анализ
     */
    void analysis();

    int getLine();
private:
    /**
     * Указатель на текущию лексему
     */
    int pointer{};

    /*
     * Флаг - встречалась ли уже функция main
     */
    bool mainExist{};

    /*
     * Созданные функции
     */
    std::vector<std::string> functions;


    /*
     * Проверка данных на существование*/
    bool checkExistData(std::vector<std::string> data, const std::string& newElement);

    /**
     * Список всех лексем
     */
    std::vector<Lexem> lexemes;

    /**
     * Список всех лексем
     */
    std::vector<int> numb_str;
    /*
     * Получение лексем
     */
    Lexem getCurrentLexeme();

    Lexem getNextLexeme();

    /*
     * Фунция вывода ошибки
     */
    void showError(const std::string& message, const Lexem& lex);

    /*
     * Функции для синтаксического анализа
     */
  
    DataTypeAndValue logI();
    
    DataTypeAndValue logNe();
    void oneParam(SemTree* cur);
    DataTypeAndValue eqFunc1();
    DataTypeAndValue CallFunction();
    DataTypeAndValue eqFunc2();
    DataTypeAndValue eqFunc3();
    bool flagInterpret = true;  //флаг интерпретации
    DataTypeAndValue  perevod(TypeVar t, DataTypeAndValue perem);

    DataTypeAndValue add();

    DataTypeAndValue multiplier();

    void checkLexeme();

    void printInterCode();

    void functionAnalysis(SemTree* cur);

    void constantAnalysis();

    void evalAnalysis();

    void operatorAndDescriptionsAnalysis();

    void operatorAnalysis();

    void dataAnalysis(TypeObject obj, TypeVar type);
    int listParamFunc(SemTree* func);

    void namedConstantAnalise();

    void variableAnalysis(TypeObject obj, TypeVar type);

    DataTypeAndValue elementaryExpressionAnalysis();

    void cycleAnalysis();

    bool operatorCase();

    DataTypeAndValue expressionAnalysis();

public: void setSemTree(SemTree* root);
      

    //  int getValue(DataTypeAndValue);
};

#endif //SRC_SYNTAXANALYSER_H
