#include "SemTree.hpp"

#define max(a,b) a<b? b : a

SemTree* SemTree::Cur = nullptr; //������� ����

SemTree::SemTree(SemTree* l, SemTree* r, SemTree* u, Node* d)
// ����������� ���� � ��������� ������� � �������
{
	n = new Node();
	Up = u; Left = l; Right = r; 
	memcpy(n, d, sizeof(Node)); 
}

SemTree::SemTree(void)
// �����������  ���� � ������� ������� � �������
{
	n = new Node();
	Up = NULL; 
	Left = NULL; 
	Right = NULL;
	n->id.first = 0;
	n->id.second = ".....";

}

void SemTree::SetLeft(Node* Data)
// ������� ������ ������� �� ������� ������� 
{
	SemTree* a = new SemTree(NULL, NULL, this, Data); 
	Left = a; // ������� � ����� ��������
}

void SemTree::SetRight(Node* Data)
// ������� ������� ������� �� ������� ������� 
{
	SemTree* a = new SemTree(NULL, NULL, this, Data); 
	Right = a; // ������� � ����� ��������
}


SemTree* SemTree::FindUp(SemTree* From, Lexem id)
// ����� ������ �� �������� ������� �� ����� ����� 
{
	SemTree* i = From; // ������� ������� ������
	while ((i != NULL) &&(id.second != i->n->id.second))
		i = i->Up; // ����������� ������ �� ������
	return i;
}



void SemTree::Print()
// ���������� ��������� ������ ������
{
	std::cout << "\n������� � ������� " << n->id.second<< "----->" << std::endl;
	
	if (Left != NULL) std::cout << " ����� ������ " << Left->n->id.second << std::endl;
	if (Right != NULL) std::cout << " ������ ������ " << Right->n->id.second << std::endl;
	
	if (Right != NULL) Right->Print();
	if (Left != NULL) Left->Print();

}

//�������� ����� ������ ����
Node* SemTree::getData()
{
	return this->n;
}



TypeVar SemTree::FromConstToType(int type)
{
	switch (type)
	{
	case tFalse:
		return TBool;
	case tTrue:
		return TBool;
	case constInt:
		return TInt;
	case constHex:
		return TInt;
	default:
		break;
	}
}

//��������, ����� �� ��������� ������� ��������� ���
void SemTree::isAssignable(int typeVar, Lexem lex)
{
	SemTree* v = Cur->SemGetVar(lex);
		if (v->getVarType() < typeVar)
			PrintError("���������� ��������� ��������� ��������� ���������� ��� ������\t ", lex);

}

//�������� ��� switch
void SemTree::semConsInSwich(int typeVar, Lexem lex)
{
		if (FromConstToType(lex.first) > typeVar)
			PrintError("���������� �������� �� ��������� ��������� ��������\t ", lex);
}



SemTree* SemTree::OneLevelFind(SemTree* From, Lexem id)
// ����� �������� id ����� �� ������ �� ������� ������� �� ����� ������ ����������� 
{
	SemTree* i = From; // ������� ������� ������
	while ((i != NULL)
		&& (i->Up)
		&& (i->Up->Right != i)
		)
	{
		if (id.second == i->n->id.second)
			return i; // ����������� �������������
		i = i->Up; // ����������� ������ 
	}
	return NULL;
}

SemTree::~SemTree()
// ���������� 
{
	if (Left != nullptr)
		delete Left;
	if (Right != nullptr)
		delete Right;
	delete n;
}


//����� ������
void SemTree::PrintError(std::string error, Lexem lex)
{
	printf("SemError: ");
	std::cout<<error<<"  "<<" string: "<<lex.numb<<"  lex: "<<lex.second<<std::endl;
	exit(1);
}


// ���������� ������� ���� ������
void SemTree::SemFinishFunc(SemTree* a)

{
	Cur = a;
}

// �������� �������� �������� ���� 
SemTree* SemTree::GetCur(void)

{
	return Cur;
}

stack<SemTree*> SemTree::newBlock;
SemTree* SemTree::prologue(Lexem a, TypeObject t, Data_Value mean)
{
	if (a.first == tFls) //���� �������� �������� ������
	{
		newBlock.push(Cur);
		Node b;
		b.id.second = "";
		b.typeObject = EMPTY; // ������ �������
		Cur->SetRight(&b); // ������� ������ �������
		Cur = Cur->Right;
		return Cur;
	}
	else if (a.first == tFps) //���� �������� �������� ������
	{
		if(newBlock.empty())
		SemTree* Cur1 = newBlock.pop();
		Node b;
		b.id.second = "";
		b.typeObject = EMPTY; // ������ �������
		Cur->SetRight(&b); // ������� ������ �������
		Cur = Cur->Right;
		return Cur;
	}
		//tFps

}

SemTree* SemTree::epilogue() {}

SemTree* SemTree::SemAdd(Lexem a, TypeObject t)
// ��������� �������������� � ������� � ����� t
{
	if (ControlIdent(Cur, a))
	{if(t == itsFunct)
		
		PrintError("������� � ����� ������ ��� ������� ", a);
	else if (t == itsVar)
		PrintError("���������� � ����� ������ ��� ������� ", a);
	else if (t == itsFunct)
		PrintError("��������� � ����� ������ ��� ������� ", a);
	}

	Node b;
	if (t == itsFunct)
	{
		b.id.second = a.second;
		b.typeObject = t;
		b.param = 0; // ���������� ���������� �������
		Cur->SetLeft(&b); // ������� ������� - �������
		Cur = Cur->Left;
		return Cur;
	}
	else
	{
		b.id.second = a.second; 
		b.typeObject = t;
		Cur->SetLeft(&b); // ������� ������� - ���������� ��� ���������
		Cur = Cur->Left;
		return Cur;
	}
}


//�������� ����
SemTree* SemTree::SemAddBlock()

{
	SemTree* semTreeVert;
	Node b;
		semTreeVert = Cur; // ��� ����� �������� ����� ������ �� �������
		b.id.second = "";
		b.typeObject = EMPTY; // ������ �������
		Cur->SetRight(&b); // ������� ������ �������
		Cur = Cur->Right;
		return semTreeVert;
}

// ���������� ��� t ��� ���������� ��� ������� �� ������ Addr
void SemTree::SemSetTypeVar(SemTree* Addr, int t)
{
	Addr->n->typeVar = t;

}

//��������� ����� ���������� �������
void SemTree::increase(SemTree* Addr)
{
	Addr->n->param++;
}


// ��������� ��������� ����� ���������� ���������� � �����������
void SemTree::SemControlCountParam(SemTree* Addr, int num)

{
	if (num != Addr->n->param)
		PrintError("����� ����������� ���������� �� ��������� � ������ ���������� ", Addr->n->id);
}


// ����� � ������� ���������� � ������ a
// � ������� ������ 
SemTree* SemTree::SemGetVar(Lexem a)

{
	SemTree* addr = FindUp(Cur, a);
	if (addr == NULL)
		PrintError("�������������� ����������", a);
	else
		if (addr->n->typeObject == itsFunct)
			PrintError("�������������� ����������", a);
		
	return addr;
}


// �������� �������������� �� ��������� �������� ������ �����.
int SemTree::ControlIdent(SemTree* Addr, Lexem a)

{
	if (OneLevelFind(Addr, a) == NULL) return 0;
	return 1;
}


int SemTree::SemGetTypeV(Lexem a)
// �������� �������������� � �� ��������� �������� ������ �����.
{
	SemTree* dop = OneLevelFind(Cur, a);
	if (dop == NULL) PrintError("�������������� �������������", a);;
	return dop->n->typeVar;
}

int SemTree::SemGetTypeF(Lexem a)
// �������� �������������� �� �������������, ���������� ��� 
{
	SemTree* dop = FindUp(Cur, a);
	if (dop == NULL) PrintError("�������������� �������������", a);;
	return dop->n->typeVar;
}

// �������� �������������� �� �������������, ���������� �����
SemTree* SemTree::SemGetFunc(Lexem a)

{
	SemTree* dop2 =	FindUp(Cur, a);
	if (dop2 == NULL) PrintError("�������������� �������������", a);
	else Cur = dop2->Right;
	return dop2;
}


// �������� ���������� �������
int SemTree::SemParamFunc(int type)

{
	if (Cur->Left == NULL)
	{
		std::cout << "������ ���������� �������(������, ��� �������)" << std::endl;
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
			std::cout << "�������������� ����� ���������� � ����������� ���������� ������� " << std::endl;
			exit (0);
		}
	}

}
//��������� ��� � ������� ���
TypeVar SemTree::getLexTypeToVar(int type)
{
	switch (type)
	{
	case tBool:
		return TBool;
		break;
	case tInt:
		return TInt;
		break;
	default:
		return TypeDef;
		break;
	}
}
//�������� ��� ����
int SemTree::getVarType() {
	return n->typeVar;
}


//���������� ��������� �������������� �������� ����� ����������
int SemTree::semResType(int typeA, int typeB)
{
	if (typeA == typeB)
		return typeA;

	if (typeA < TypeDef && typeB < TypeDef)
	{
		return max(typeA, typeB);
	}
	else
	{
		return TypeDef;
	}
}
