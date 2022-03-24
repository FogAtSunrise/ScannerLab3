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
  
    DataTypeAndValue logI();
    
    DataTypeAndValue logNe();
    void oneParam(SemTree* cur);
    DataTypeAndValue eqFunc1();
    DataTypeAndValue CallFunction();
    DataTypeAndValue eqFunc2();
    DataTypeAndValue eqFunc3();
    bool flagInterpret = true;  //���� �������������
    DataTypeAndValue  perevod(TypeVar t, DataTypeAndValue perem);

    DataTypeAndValue add();

    DataTypeAndValue multiplier();

    void checkLexeme();

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
