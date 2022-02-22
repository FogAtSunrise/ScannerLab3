#pragma once
#include "ScannerClass.hpp"
#include "ListOfConstants.hpp"
#include <string.h>
#include <iostream>
#include <string>
#include <vector>
#include <stack>  
using namespace std;

#define EMPTY -1 // признак пустой ссылки



enum TypeObject {//тип объекта
	itsVar = 1, // простая переменная
	itsConst, // константа
	itsFunct // функция
};

enum TypeVar { //тип элементарных данных
	TypeDef = 0,
	TBool,
	TInt
};

union Data_Value { // значение одного элемента данных
	int Data_int;
	bool Data_bool;
};

struct Node // вершина семантического дерева
{
	Lexem id; // идентификатор переменной
	int typeObject; // тип объекта
	int typeVar; // возвращаемый тип
	Data_Value data ;// ссылка на значение
	int param = 0; // количество параметров

};

class SemTree
{
protected:
	Node* n; // данные таблицы
	SemTree* Up, * Left, * Right;

public:
	static SemTree* Cur;
static stack<SemTree*> newBlock;
	SemTree(SemTree* l, SemTree* r, SemTree* u, Node* Data);
	SemTree();
	~SemTree();
	void SetLeft(Node* Data);
	void SetRight(Node* Data);
	SemTree* FindUp(SemTree* From, Lexem id);
	SemTree* OneLevelFind(SemTree* From, Lexem id);
	
	void Print();

	void PrintError(std::string error, Lexem lex);
	
	

	
	SemTree* SemGetFunc(Lexem a);
	SemTree* prologue(Lexem a, TypeObject t, Data_Value mean, Lexem a1);//выделение памяти
	SemTree* epilogue();
	static TypeVar getLexTypeToVar(int type); //узнать тип переменной
	static TypeVar FromConstToType(int type); //узнать тип константы
	static int semResType(int typeA, int typeB); // получить тип приведенной переменной
	void isAssignable(int typeVar, Lexem lex); // проверить, можно ли присвоить
	Node* getData();
	int SemParamFunc(int type);//проверка соответствия типов параметров
	void increase(SemTree* Addr);//увеличить число параметров функции
	void semConsInSwich(int typeVar, Lexem lex);
	void SemFinishFunc(SemTree* a);// установить текущий узел дерева
	SemTree* GetCur(void);// получить значение текущего узла дерева
	SemTree* SemAdd(Lexem a, TypeObject t, Lexem a1);// занесение идентификатора a в таблицу с типом t
	void SemSetTypeVar(SemTree* Addr, int t);// установить тип простой t переменной 
	int SemGetTypeV(Lexem a);//проверяет,  описана ли переменная и если да, возвращает ее тип
	int SemGetTypeF(Lexem a);//проверяет,  описана ли функция и если да, возвращает ее тип
	SemTree* SemAddBlock();//создать блок
	void SemControlCountParam(SemTree* Addr, int n);// проверить равенство числа формальных параметров 
	SemTree* SemGetVar(Lexem a);// найти в таблице переменную с именем a
	int ControlIdent(SemTree* Addr, Lexem a);//проверка дубляжей
	int getVarType();// получить тип объекта переменной
	



};