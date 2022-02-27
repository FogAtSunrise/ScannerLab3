#include "SemTree.hpp"

#define max(a,b) a<b? b : a

SemTree* SemTree::Cur = NULL; //текущий узел

SemTree::SemTree(SemTree* l, SemTree* r, SemTree* u, Node* d)
// конструктор узла с заданными связями и данными
{
	n = new Node();
	Up = u; Left = l; Right = r; 
	memcpy(n, d, sizeof(Node)); 
}

SemTree::SemTree(void)
// конструктор  узла с пустыми связями и данными
{
	n = new Node();
	Up = NULL; 
	Left = NULL; 
	Right = NULL;
	n->id.first = 0;
	n->id.second = ".....";

}

void SemTree::SetLeft(Node* Data)
// создать левого потомка от текущей вершины 
{
	SemTree* a = new SemTree(NULL, NULL, this, Data); 
	Left = a; // связали с новой вершиной
}

void SemTree::SetRight(Node* Data)
// создать правого потомка от текущей вершины 
{
	SemTree* a = new SemTree(NULL, NULL, this, Data); 
	Right = a; // связали с новой вершиной
}


SemTree* SemTree::FindUp(SemTree* From, Lexem id)
// поиск данных от заданной вершины до корня вверх 
{
	SemTree* i = From; // текущая вершина поиска
	while ((i != NULL) &&(id.second != i->n->id.second))
		i = i->Up; // поднимаемся наверх по связям
	return i;
}



void SemTree::Print()
// отладочная программа печати дерева
{
	
	std::cout << "\n(*)вершина ";
	if (n->id.second == "")
	std::cout << "EMPTY -->" << endl;
	else
	std::cout<<  n->id.second << " [тип: " << NameType(n->typeVar) << ", значение:"<< valueString(this)  <<"]-->" << endl;
//	if(n->typeVar== TypeVar->TypeDef)
	//<< "] ----->" << std::endl;
	
	if (Left != NULL) std::cout << "	--левый потомок " << Left->n->id.second << std::endl;
	if (Right != NULL) std::cout << "	--правый потомок " << ((Right->n->id.second == "") ? "EMPTY" : Right->n->id.second) << std::endl;
	
	if (Right != NULL) Right->Print();
	if (Left != NULL) Left->Print();

}

string SemTree::valueString(SemTree* Addr)
{
	if (Addr->n->init == false)
		return "..";
	else
	if (Addr->n->typeVar == TTBool)
		return (Addr->n->data.Data_bool==true)? "true":"false";
	else
		if (Addr->n->typeVar == TTInt)
			return std::to_string(Addr->n->data.Data_int);
		else		return " ";
}
//получить адрес данных узла
Node* SemTree::getData()
{
	return this->n;
}



TypeVar SemTree::FromConstToType(int type)
{
	
	switch (type)
	{
	case tFalse:
		return TTBool;
	case tTrue:
		return TTBool;
	case constInt:
		return TTInt;
	case constHex:
		return TTInt;
	default:
		break;
	}
}

//проверка, можно ли присвоить лексеме указанный тип
void SemTree::isAssignable(TypeVar typeVar, Lexem lex)
{
	SemTree* v = Cur->SemGetVar(lex);
		if (v->n->typeVar < typeVar)
			PrintError("Невозможно присвоить результат выражения переменной без потерь\t ", lex);

}

//проверка для switch
void SemTree::semConsInSwich(TypeVar typeVar, Lexem lex)
{
		if (FromConstToType(lex.first) > typeVar)
			PrintError("Невозможно получить из выражения указанное значение\t ", lex);
}



SemTree* SemTree::OneLevelFind(SemTree* From, Lexem id)
// Поиск элемента id вверх по дереву от текущей вершины на одном уровне вложенности 
{
	SemTree* i = From; // текущая вершина поиска
	while ((i != NULL)
		&& (i->Up)
		&& (i->Up->Right != i)
		)
	{
		if (id.second == i->n->id.second)
			return i; // совпадающий идентификатор
		i = i->Up; // поднимаемся наверх 
	}
	return NULL;
}

SemTree::~SemTree()
// деструктор 
{
	if (Left != NULL)
		delete Left;
	if (Right != NULL)
		delete Right;
	delete n;
}


//Вывод ошибки
void SemTree::PrintError(std::string error, Lexem lex)
{
	printf("SemError: ");
	std::cout<<error<<"  "<<" string: "<<lex.numb<<"  lex: "<<lex.second<<std::endl;
	exit(1);
}


// установить текущий узел дерева
void SemTree::SemFinishFunc(SemTree* a)

{
	Cur = a;
}

// получить значение текущего узла 
SemTree* SemTree::GetCur(void)

{
	return Cur;
}

stack<SemTree*> SemTree::newBlock;
SemTree* SemTree::prologue(Lexem a, TypeObject t, Data_Value mean, Lexem a1)
{

	SemTree* dop;
	if (a.first == tFls) //если получена открытая скобка
	{
		newBlock.push(Cur);
		Node b;
		b.id.second = "";
		b.typeObject = EMPTY; // пустая вершина
		Cur->SetRight(&b); // сделали пустую вершину
		Cur = Cur->Right;
		cout << "\nВыделена память под блок " << a.second <<endl;
		dop= newBlock.top();
	}
	else 
	{
		SemTree* n = SemAdd( a, t, a1);///////////////////////////////////
	
		dop= n;
		//n.data.
	}
//	cout << "-----------------START TREE-----------------------" << endl;
	//Print();
	//cout << "-----------------FINISH TREE-----------------------" << endl << endl;
	
	return dop;
}

SemTree* SemTree::epilogue() {
	if (!newBlock.empty())
	{

		Cur = newBlock.top();
		newBlock.pop();
		//закомментированный код позволяет оставлять параметры 
		/*SemTree* dop = Cur->Right;
		for (int i = 0; i < Cur->n->param; i++)
			dop = dop->Left; 
		dop->Left= nullptr;*/
		cout << "СОБЫТИЕ: Удален блок-----------------\n" << endl;
		Cur->Right = nullptr;
	}
	//cout << "-----------------START TREE-----------------------" << endl;
	//Print();
	//cout << "-----------------FINISH TREE-----------------------" << endl << endl;
	
	return Cur; }

SemTree* SemTree::SemAdd(Lexem a, TypeObject t, Lexem a1)
// занесение идентификатора в таблицу с типом t
{
	if (ControlIdent(Cur, a))
	{if(t == itsFunct)
		
		PrintError("Функция с таким именем уже описана ", a);
	else if (t == itsVar)
		PrintError("Переменная с таким именем уже описана ", a);
	else if (t == itsFunct)
		PrintError("Константа с таким именем уже описана ", a);
	}

	Node b;
	if (t == itsFunct)
	{
		
		b.id.second = a.second;
		b.typeObject = t;
		b.typeVar = getLexTypeToVar(a1.first); 
		b.param = 0; // количество параметров функции
		Cur->SetLeft(&b); // сделали вершину - функцию
		Cur = Cur->Left;
		newBlock.push(Cur);
		b.id.second = "";
		b.typeObject = EMPTY; // пустая вершина
		Cur->SetRight(&b); // сделали пустую вершину
		Cur = Cur->Right;
		cout << "\nСОБЫТИЕ: Добавлен идентификатор функции: " << a.second<<" Тип: "<< a1.second <<"----------------"<< endl << endl;
		return newBlock.top();
		
	}
	else
	{
		b.id.second = a.second; 
		b.typeObject = t;
		b.typeVar = getLexTypeToVar(a1.first); 
		Cur->SetLeft(&b); // сделали вершину - переменной или константы
		Cur = Cur->Left;
		cout << "\nСОБЫТИЕ: Добавлен идентификатор: " << a.second << " Тип: " << a1.second << endl << endl;
		return Cur;
	}
}


//добавить блок
SemTree* SemTree::SemAddBlock()

{
	SemTree* semTreeVert;
	Node b;
		semTreeVert = Cur; // это точка возврата после выхода из функции
		b.id.second = "";
		b.typeObject = EMPTY; // пустая вершина
		Cur->SetRight(&b); // сделали пустую вершину
		Cur = Cur->Right;
		return semTreeVert;
}

string SemTree::NameType(TypeVar type)
{
	switch (type)
	{  
	case TTBool:
		return "bool";
	case TTInt:
		return "int";
	default:
		return "void";
		break;
	}
}

// установить тип t для переменной или функции по адресу Addr
void SemTree::SemSetTypeVar(SemTree* Addr, TypeVar t)
{
	Addr->n->typeVar = t;

}

//Уведичить число параметров функции
void SemTree::increase(SemTree* Addr)
{
	Addr->n->param++;
}


// проверить равенство числа формальных параметров и фактических
void SemTree::SemControlCountParam(SemTree* Addr, int num)

{
	if (num != Addr->n->param)
		PrintError("Число фактических параметров не совпадает с числом формальных ", Addr->n->id);
}

//присвоить идентификатору новое значение
void SemTree::SetValueIden(Lexem a, DataTypeAndValue val)
{
	SemTree* addr = SemGetVar(a);
	if (addr != NULL  )
	{
		addr->n->data = val.data;
		addr->n->init = true;
	}
	if(val.type > addr->n->typeVar)
		PrintError("ТИПЫ НЕ СООТВЕТСТВУЮТ", a);

	cout << "СОБЫТИЕ: присвоено новое значение переменной " << a.second << " [тип: " << NameType(addr->n->typeVar) << ", значение:"<< valueString(addr) << "]" << endl;



	//cout << "-----------------START TREE-----------------------" << endl;
	//Print();
	//cout << "-----------------FINISH TREE-----------------------" << endl << endl;
}
//получить значение указанного идентификатора
DataTypeAndValue SemTree::GetValueIden(Lexem a)
{
	SemTree* addr = SemGetVar(a);
	if (addr == NULL)
		PrintError("Несуществующая переменная", a);
	else
		if (addr->n->init==true)
			return DataTypeAndValue{ addr->n->data, addr->n->typeVar };
		else PrintError("Не инициализированная переменная", a);

}

// найти в таблице переменную с именем a
// и вернуть ссылку 
SemTree* SemTree::SemGetVar(Lexem a)

{
	SemTree* addr = FindUp(Cur, a);
	if (addr == NULL)
		PrintError("Несуществующая переменная", a);
	else
		if (addr->n->typeObject == itsFunct)
			PrintError("Несуществующая переменная", a);
		
	return addr;
}


// Проверка идентификатора на повторное описание внутри блока.
int SemTree::ControlIdent(SemTree* Addr, Lexem a)

{
	if (OneLevelFind(Addr, a) == NULL) return 0;
	return 1;
}


TypeVar SemTree::SemGetTypeV(Lexem a)
// Проверка идентификатора а на повторное описание внутри блока.
{
	//SemTree* dop = OneLevelFind(Cur, a);
	SemTree* dop = FindUp(Cur, a);
	if (dop == NULL) PrintError("Несуществующий идентификатор", a);;
	return dop->n->typeVar;
}

TypeVar SemTree::SemGetTypeF(Lexem a)
// Проверка идентификатора на существование, возвращает тип 
{
	SemTree* dop = FindUp(Cur, a);
	if (dop == NULL) PrintError("Несуществующий идентификатор", a);;
	return dop->n->typeVar;
}

// Проверка идентификатора на существование, возвращает адрес
SemTree* SemTree::SemGetFunc(Lexem a)

{
	SemTree* dop2 =	FindUp(Cur, a);
	if (dop2 == NULL) PrintError("Несуществующий идентификатор", a);
	else Cur = dop2->Right;
	return dop2;
}


// Проверка параметров функции
int SemTree::SemParamFunc(int type)

{

/*	if (Cur->Left == NULL)
	{
		std::cout << "Ошибка параметров функции(меньше, чем введено)" << std::endl;
		exit(0);
}
	else
	{
		Cur = Cur->Left;
		if (type <= Cur->n->typeVar)
			//semResType(int typeA, int typeB)
			return type;
		else
		{
			printf("SemError: ");
			std::cout << "Несоответствие типов формальных и фактических параметров функции " << std::endl;
			exit (0);
		}
		
	}
*/
	return 1;
}
//перевести тип в рабочий вид
TypeVar SemTree::getLexTypeToVar(int type)
{
	switch (type)
	{
	case tBool:
		return TTBool;
		break;
	case tInt:
		return TTInt;
		break;
	default:
		return TTypeDef;
		break;
	}
}
//получить тем узла
TypeVar SemTree::getVarType() {
	return n->typeVar;
}


//возвращает результат арифметической операции между значениями
TypeVar SemTree::semResType(TypeVar typeA, TypeVar typeB)
{
	if (typeA == typeB)
		return typeA;

	if (typeA  >TTypeDef && typeB > TTypeDef)
	{
		return max(typeA, typeB);
	}
	else
	{
		return TTypeDef;
	}
}
