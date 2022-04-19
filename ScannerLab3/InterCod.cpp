#include <iostream>
#include <string>
#include "InterCod.hpp"

using namespace std;


InterCod::InterCod()
{
    k = 0;

}

void InterCod::printInterCode()
{
    if (tr.empty())
        cout << "No Code" << endl;
    else
        for (Triad n : tr)
        {
            cout << n.triad << ") " << forPrint[n.operationID] << " " << getOp(n, 0) << " " << getOp(n, 1) << endl;
        }

}


string InterCod::getOp(Triad n, int i)
{
    Oper operand = (i == 0) ? n.operand1 : n.operand2;

    if (operand.isLink) return "(" + std::to_string(operand.numb) + ")";
    else return operand.lex.second;
}

void InterCod::addTriad(int oper, Oper a, Oper b)
{

    tr.push_back(Triad{ k++, oper, a, b });
}