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
     * ��������� ������
     */
    void analysis();

    int getLine();
private:
    /**
     * ��������� �� ������� �������
     */
    int pointer{};

    /*
     * ���� - ����������� �� ��� ������� main
     */
    bool mainExist{};

    /*
     * ��������� �������
     */
    std::vector<std::string> functions;


    /*
     * �������� ������ �� �������������*/
    bool checkExistData(std::vector<std::string> data, const std::string& newElement);

    /**
     * ������ ���� ������
     */
    std::vector<Lexem> lexemes;

    /**
     * ������ ���� ������
     */
    std::vector<int> numb_str;
    /*
     * ��������� ������
     */
    Lexem getCurrentLexeme();

    Lexem getNextLexeme();

    /*
     * ������ ������ ������
     */
    void showError(const std::string& message, const Lexem& lex);

    /*
     * ������� ��� ��������������� �������
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
