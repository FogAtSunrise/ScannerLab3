#ifndef MAT3_NEW_FIX_SCANNER_H
#define MAT3_NEW_FIX_SCANNER_H

#include "ListOfConstants.hpp"
#include <string>
#include <map>
#include <string>
#include <vector>

struct Lexem
{
    int first;
    std::string second = "";
    int numb;
};
//typedef std::pair<int, std::string> Lexem;     // ����: [��� �������, �������]
class ScannerClass {

public:
    ScannerClass() = default;
    Lexem scan();
    std::vector<int> number_str;
    bool readDataFromFile(const std::string& fileName);

private:
    //�������� �����
    std::map<std::string, int> keywords = {
            {"const",       tConst}, 
            {"bool",       tBool},
            {"short",       tShort},
            {"long",        tLong},
            {"int",         tInt},
            {"switch",      tSwi},
            {"default",      tDef},
            {"case",        tCase},
            {"break",        tBreak},
            {"return",      tRet},
            {"main",        tMain},
            {"true",        tTrue},
            {"false",        tFalse}
    };


    int pointer;   // ��������� �� ������� � ������ (�������)

    std::string text_Info;  // �������� �����

    int number_Line{}; // ����� ������

    void put_pointer(int pointer); // ���������� ��������� � ��������� �������

    int get_pointer() const;  // ������� ���������
    

    void show_Error(const std::string& message); //������
};


#endif

