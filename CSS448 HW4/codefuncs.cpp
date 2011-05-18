// File: codefuncs.cpp

#include "codefuncs.h"
#include <iostream>
#include <string>
#include "pointertype.h"
#include "recordtype.h"
#include "arraytype.h"
using namespace std;

void outputConst(Symbol *rhs, int level) {
	for(int i = 0; i < level; i++) {
		cout << "\t";
	}

	Constant<bool> *bTemp = dynamic_cast<Constant<bool>*>(rhs);
	if(bTemp != NULL) {
		cout << "const bool " << bTemp->name << " = ";
		if(bTemp->value) {
			cout << "true;" << endl;
		}
		else {
			cout << "false;" << endl;
		}
		return;
	}

	Constant<int> *iTemp = dynamic_cast<Constant<int>*>(rhs);
	if(iTemp != NULL) {
		cout << "const int " << iTemp->name << " = " << iTemp->value << ";" << endl;
		return;
	}

	Constant<double> *dTemp = dynamic_cast<Constant<double>*>(rhs);
	if(dTemp != NULL) {
		cout << "const double " << dTemp->name << " = " << dTemp->value << ";" << endl;
		return;
	}

	Constant<string> *sTemp = dynamic_cast<Constant<string>*>(rhs);
	if(sTemp != NULL) {
		cout << "const char " << sTemp->name << "[] = \"" << sTemp->value << "\";" << endl;
		return;
	}
}

void outputVar(Variable *rhs, int level) {
	for(int i = 0; i < level; i++) {
		cout << "\t";
	}

	string type = rhs->type->name;
	if(type == "integer") {
		cout << "int " << rhs->name << ";" << endl;
	}
	else if(type == "real") {
		cout << "double " << rhs->name << ";" << endl;
	}
	else if(type == "boolean") {
		cout << "bool " << rhs->name << ";" << endl;
	}
	else if(type == "") {
		PointerType *pTemp = dynamic_cast<PointerType*>(rhs->type);
		if(pTemp != NULL) {
			cout << pTemp->typeTo->name << " *" << rhs->name << ";" << endl;
		}

		typedef ArrayType::DimRange dRange;

		ArrayType *aTemp = dynamic_cast<ArrayType*>(rhs->type);
		if(aTemp != NULL) {
			cout << aTemp->type->name << " " << rhs->name;
			for(vector<dRange>::iterator i = aTemp->dimRanges.begin();
				i != aTemp->dimRanges.end(); i++) {
					cout << "[";
					i->end->displayVal();
					cout << "]";
			}
			cout << ";" << endl;
		}

		RecordType *rTemp = dynamic_cast<RecordType*>(rhs->type);
		if(rTemp != NULL) {
			cout << "struct {" << endl;
			for(vector<Variable*>::iterator i = rTemp->vars.begin();
				i != rTemp->vars.end(); i++) {
					outputVar(*i, level+1);
			}
			cout << "} " << rhs->name << ";" << endl;
		}
	}
	else {
		cout << type << " " << rhs->name << ";" << endl;
	}
}