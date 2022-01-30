#ifndef SRC_ANALISYS_H
#define SRC_ANALISYS_H


#include <vector>
#include "ScannerClass.hpp"
#include "SemTree.hpp"

class Analisys {
protected:
    SemTree* root;
public:
    
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
  
    int logI();
    
    int logNe();
    void oneParam(SemTree* cur);
    int eqFunc1();

    int eqFunc2();

    int add();

    int multiplier();

    void checkLexeme();

    void functionAnalysis(SemTree* cur);

    void constantAnalysis();

    void evalAnalysis();

    void operatorAndDescriptionsAnalysis();

    void operatorAnalysis();

    void dataAnalysis(TypeObject obj, int type);
    int listParamFunc();

    void namedConstantAnalise();

    void variableAnalysis(TypeObject obj, int type);

    int elementaryExpressionAnalysis();

    void cycleAnalysis();

    void operatorCase();

    int expressionAnalysis();

public: void setSemTree(SemTree* root);
};

#endif //SRC_SYNTAXANALYSER_H
