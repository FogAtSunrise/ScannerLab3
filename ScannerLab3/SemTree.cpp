#include "SemTree.hpp"

#define max(a,b) a<b? b : a

SemTree* SemTree::Cur = NULL; //������� ����
SemTree* SemTree::CurFunc = NULL; //������� ����


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

SemTree* SemTree::CopyTree(SemTree* node)
{
	if (node == NULL)
		return NULL;
	SemTree* newnode = new SemTree(NULL, NULL, node, node->n);
	newnode->Left = CopyTree(node->Left);
	if(newnode->Left!=NULL)
	newnode->Left->Up = newnode;
	return newnode;
}

SemTree* SemTree::CreateCopy(SemTree* From)
{
	SemTree* a = new SemTree(NULL, NULL, From, new Node{ From->n->adress, From->n->id, From->n->typeObject, From->n->typeVar, From->n->data, From->n->param, From->n->init });
	a->Left = From->Left;
	a->Right = CopyTree(From->Right);
	if(a->Right!=NULL)
	a->Right->Up = a;
	From->Left = a;
	return a;
}

void SemTree::DelCopy(SemTree* From)
{
	From->Up->Left = From->Left;
}

SemTree* SemTree::FindUp(SemTree* From, Lexem id)
// ����� ������ �� �������� ������� �� ����� ����� 
{
	SemTree* i = From; // ������� ������� ������
	while ((i != NULL) &&(id.second != i->n->id.second))
		i = i->Up; // ����������� ������ �� ������
	return i;
}

Lexem SemTree::getId(SemTree* tree)
{
	return tree->n->id;
}

void SemTree::Print()
// ���������� ��������� ������ ������
{
	
	std::cout << "\n(*)������� ";
	if (n->id.second == "")
	std::cout << "EMPTY -->" << endl;
	else
	std::cout<<  n->id.second << " [���: " << NameType(n->typeVar) << ", ��������:"<< valueString(this)  <<"]-->" << endl;
//	if(n->typeVar== TypeVar->TypeDef)
	//<< "] ----->" << std::endl;
	
	if (Left != NULL) std::cout << "	--����� ������� " << Left->n->id.second << std::endl;
	if (Right != NULL) std::cout << "	--������ ������� " << ((Right->n->id.second == "") ? "EMPTY" : Right->n->id.second) << std::endl;
	
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

//��������, ����� �� ��������� ������� ��������� ���
void SemTree::isAssignable(TypeVar typeVar, Lexem lex)
{
	SemTree* v = Cur->SemGetVar(lex);
		if (v->n->typeVar < typeVar)
			PrintError("���������� ��������� ��������� ��������� ���������� ��� ������\t ", lex);

}

//�������� ��� switch
void SemTree::semConsInSwich(TypeVar typeVar, TypeVar typeVar2, Lexem lex)
{
		if (typeVar2 > typeVar)
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
	if (Left != NULL)
		delete Left;
	if (Right != NULL)
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
SemTree* SemTree::prologue(Lexem a, TypeObject t, Data_Value mean, Lexem a1)
{

	SemTree* dop=NULL;
	if (a.first == tFls) //���� �������� �������� ������
	{
		newBlock.push(Cur);
		Node b;
		b.id.second = "";
		b.typeObject = EMPTY; // ������ �������
		Cur->SetRight(&b); // ������� ������ �������
		Cur = Cur->Right;
		cout << "\n������ ���� " << a.second <<endl;
		dop= newBlock.top();
	}
	else 
		if(flagInterpret)
	{
		SemTree* n = SemAdd( a, t, a1);///////////////////////////////////
		dop= n;
		//n.data.
	}

	return dop;
}

SemTree* SemTree::epilogue() {
	if (!newBlock.empty())
	{

		Cur = newBlock.top();
		newBlock.pop();
		//������������������ ��� ��������� ��������� ��������� 
		SemTree* dop = Cur->Right;
		for (int i = 0; i < Cur->n->param; i++)
			dop = dop->Left; 
		dop->Left= nullptr;
		cout << "�������: ����� �����-----------------\n" << endl;
		//Cur->Right = nullptr;
	}
	//cout << "-----------------START TREE-----------------------" << endl;
	//Print();
	//cout << "-----------------FINISH TREE-----------------------" << endl << endl;
	
	return Cur; }

SemTree* SemTree::SemAdd(Lexem a, TypeObject t, Lexem a1)
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
		b.typeVar = getLexTypeToVar(a1.first); 
		b.param = 0; // ���������� ���������� �������
		Cur->SetLeft(&b); // ������� ������� - �������
		Cur = Cur->Left;
		newBlock.push(Cur);
		b.id.second = "";
		b.typeObject = EMPTY; // ������ �������
		Cur->SetRight(&b); // ������� ������ �������
		Cur = Cur->Right;
		cout << "�������: �������� ������������� �������: " << a.second<<" ���: "<< a1.second <<"----------------"<< endl;
		return newBlock.top();
		
	}
	else
	{
		b.id.second = a.second; 
		b.typeObject = t;
		b.typeVar = getLexTypeToVar(a1.first); 
		Cur->SetLeft(&b); // ������� ������� - ���������� ��� ���������
		Cur = Cur->Left;
		cout << "<><><><><><><> �������: �������� �������������: " << a.second << " ���: " << a1.second  << endl ;
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

// ���������� ��� t ��� ���������� ��� ������� �� ������ Addr
void SemTree::SemSetTypeVar(SemTree* Addr, TypeVar t)
{
	Addr->n->typeVar = t;

}

//��������� ����� ���������� �������
void SemTree::increase(SemTree* Addr)
{
	Addr->n->param++;
}


TypeVar SemTree::GetTypeCur(SemTree* Addr)
{
	return Addr->n->typeVar;
}

// ��������� ��������� ����� ���������� ���������� � �����������
void SemTree::SemControlCountParam(SemTree* Addr, int num)

{
	if (num != Addr->n->param)
		PrintError("����� ����������� ���������� �� ��������� � ������ ���������� ", Addr->n->id);
}

//��������� �������������� ����� ��������
void SemTree::SetValueIden(Lexem a, DataTypeAndValue val)
{
	if (flagInterpret) {
	SemTree* addr = SemGetVar(a);
	if (addr != NULL)
	{
		addr->n->data = val.data;
		addr->n->init = true;
	}
	if (val.type > addr->n->typeVar)
		PrintError("���� �� �������������", a);

	Cur = addr;
	cout << "�������: ��������� ����� �������� ���������� " << a.second << " [���: " << NameType(addr->n->typeVar) << ", ��������:" << valueString(addr) << "]" << endl;

}

	//cout << "-----------------START TREE-----------------------" << endl;
	//Print();
	//cout << "-----------------FINISH TREE-----------------------" << endl << endl;
}	

//��������� ������� ����� ��������

void SemTree::SetValueFunc(Lexem a, DataTypeAndValue val)
{
	if (flagInterpret) {
		SemTree* addr = SemGetFuncId(a);
		if (addr != NULL)
		{
			addr->n->data = val.data;
			addr->n->init = true;
		}
		if (val.type > addr->n->typeVar)
			PrintError("���� �� �������������", a);
		Cur = addr;
		cout << "�������: ������� " << a.second << " ������� �������� [���: " << NameType(addr->n->typeVar) << ", ��������:" << valueString(addr) << "]" << endl;

	}
}

// ����� � ������� ���������� � ������ a
// � ������� ������ 
SemTree* SemTree::SemGetFuncId(Lexem a)

{
	SemTree* addr = FindUp(Cur, a);
	if (addr == NULL)
		PrintError("�������������� �������", a);
	else
		if (addr->n->typeObject == itsVar)
			PrintError("�������������� �������", a);

	return addr;
}

//�������� �������� ���������� ��������������
DataTypeAndValue SemTree::GetValueIden(Lexem a)
{
	if (flagInterpret) {
		SemTree* addr = SemGetVar(a);
		if (addr == NULL)
			PrintError("�������������� ����������", a);
		else
			if (addr->n->init == true)
				return DataTypeAndValue{ addr->n->data, addr->n->typeVar };
			else PrintError("�� ������������������ ����������", a);
	}
	else return DataTypeAndValue{ 0, TTypeDef};

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


TypeVar SemTree::SemGetTypeV(Lexem a)
// �������� �������������� � �� ��������� �������� ������ �����.
{
	//SemTree* dop = OneLevelFind(Cur, a);
	SemTree* dop = FindUp(Cur, a);
	if (dop == NULL) PrintError("�������������� �������������", a);;
	return dop->n->typeVar;
}

TypeVar SemTree::SemGetTypeF(Lexem a)
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
int SemTree::SemParamFunc(DataTypeAndValue val)

{
	
	if (CurFunc->Left == NULL)
	{
		std::cout << "������ ���������� �������(������, ��� �������)" << std::endl;
		exit(0);
}
	else
	{
		CurFunc = CurFunc->Left;
		if (val.type <= CurFunc->n->typeVar)
		{
			CurFunc->n->data = val.data;
			CurFunc->n->init = true;
			Cur = CurFunc;
			return val.type;
		}
		else
		{
			printf("SemError: ");
			std::cout << "�������������� ����� ���������� � ����������� ���������� ������� " << std::endl;
			exit (0);
		}
		
	}

	return 1;
}
//��������� ��� � ������� ���
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
//�������� ��� ����
TypeVar SemTree::getVarType() {
	return n->typeVar;
}


//���������� ��������� �������������� �������� ����� ����������
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
