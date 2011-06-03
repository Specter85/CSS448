%{

/* declarations section */
#include <iostream>
#include <stack>
#include <list>
#include "yylex.h"
#include "parsefuncs.h"
#include "codefuncs.h"
#include "pointertype.h"
#include "recordtype.h"
#include "symbol.h"
#include "simpletype.h"
#include "variable.h"
#include "arraytype.h"
#include "symboltable.h"
#include "variable.h"
#include "typeredef.h"
using namespace std; 

int gLevel = 0;

SymbolTable table;
string symbolName;
string typeName;
string scopeName;
string recordName;
string forControl;

int test = 0;

Symbol *newSymbol = NULL;
Symbol *currentBase = NULL;

stack<string> identStack;
stack<ArrayType*> arrayStack;
list<Variable*> variableStack;

// Stack of unfinished pointers.
list<umfPointer> pointerStack;

// List for holding onto types before they are output.
list<Type*> typeList;

Symbol *setStart = NULL;
Symbol *setEnd = NULL;

Type *currentType = NULL;

SimpleType pString("pString");
SimpleType nil("nil");
SimpleType pParam("pParam");

string lSeperator;

struct tNode {
   Symbol *sym;
   tNode *next;
};

%}

/* definition section */

%union {
	int ival;
	double fval;
	char *str;
	struct {
	   char *type;
	   const char *str;
	   int num;
	   void *sym;
	   void *list;
	} cur;
}

%start  CompilationUnit
%token  <cur> yand yarray yassign ybegin ycaret ycase ycolon ycomma yconst ydispose 
        ydiv ydivide ydo  ydot ydotdot ydownto yelse yend yequal yfalse
        yfor yfunction ygreater ygreaterequal yident  yif yin yleftbracket
        yleftparen yless ylessequal yminus ymod ymultiply ynew ynil ynot 
        ynotequal ynumber yof  yor yplus yprocedure yprogram yread yreadln  
        yrecord yrepeat yrightbracket yrightparen  ysemicolon yset ystring
        ythen  yto ytrue ytype  yuntil  yvar ywhile ywrite ywriteln ystring
        yunknown
        
%type <cur> DesignatorList Designator DesignatorStuff theDesignatorStuff
		SimpleExpression Expression TermExpr Term Factor Setvalue
		FunctionCall MultOperator UnaryOperator AddOperator Relation
		ExpList ConstFactor ConstExpression ActualParameters
		
%type <ival> WhichWay

%%
/* rules section */

/**************************  Pascal program **********************************/

CompilationUnit    :  ProgramModule        
                   ;
ProgramModule      :  { outputHeader(); }
					  yprogram yident { scopeName.assign(value); } ProgramParameters
					  { addScope(); }
					  ysemicolon 
					  ProgBlock 
					  ydot 
                   ;
ProgramParameters  :  yleftparen  IdentList { addProgramVars(); } yrightparen
                   ;
IdentList          :  yident { identStack.push(value); }
                   |  IdentList ycomma yident { identStack.push(value); }
                   ;

/**************************  Declarations section ***************************/

ProgBlock          :  Declarations  ybegin { cout << "int main() {" << endl; }
					  StatementSequence /*{
					     cout << endl;
					     for(int i = 0; i < gLevel; i++) {
						    cout << "   ";
						 }
					  }*/ yend { cout << endl << "}"; }
                   ;
Block              :  Declarations  ybegin 
					  StatementSequence /*{
					     cout << endl;
					     for(int i = 0; i < gLevel; i++) {
						    cout << "   ";
						 }
					  }*/ yend
                   ;
Declarations       :  ConstantDefBlock              /* you do this one */
                      TypeDefBlock
                      VariableDeclBlock             /* you finish this one */
                      SubprogDeclList  
                   ;
ConstantDefBlock   :  /*** empty ***/
                   |  yconst ConstantDefList   /* you finish it */
                   ;
ConstantDefList    :  ConstantDef ysemicolon
                   |  ConstantDefList ConstantDef ysemicolon
                   ;
TypeDefBlock       :  /*** empty ***/
                   |  ytype  TypeDefList { processUmfPointers(); 
                      outputTypes(table.getScopeLevel()); }
                   ;
TypeDefList        :  TypeDef  ysemicolon
                   |  TypeDefList TypeDef ysemicolon  
                   ;
VariableDeclBlock  :  /*** empty ***/
                   |  yvar VariableDeclList           /* you finish it */
                   ;
VariableDeclList   :  VariableDecl ysemicolon
                   |  VariableDeclList VariableDecl ysemicolon                                /* you finish it */
                   ;  
ConstantDef        :  yident { symbolName.assign(value); } yequal  ConstExpression
				      { newSymbol->name = symbolName; 
				      table.insert(newSymbol);
				      outputConst(newSymbol, table.getScopeLevel());
				      newSymbol = NULL; }
                   ;
TypeDef            :  yident { symbolName.assign(value);
					  typeName.assign(value); } yequal  Type 
					  {
					  if(currentType->name != "") {
					     TypeRedef *rTemp = new TypeRedef;
					     rTemp->typeTo = currentType;
					     currentType = rTemp;
					  }
					  else {
					     currentType->name.assign(typeName);
					  }
					  
					  if(table.lookUpCS(typeName) == NULL) {
					     table.insert(currentType);
					     typeList.push_back(currentType);
					  }
					  else {
					     cout << "***ERROR: " << symbolName << " already in symbol table" << endl;
					     PointerType *temp = dynamic_cast<PointerType*>(currentType);
					     if(temp != NULL) {
						    if(!pointerStack.empty() && pointerStack.front().point == temp) {
					           pointerStack.pop_front();
					        }
					     }
					     delete currentType;
					  }
					  currentType = NULL; typeName.assign(""); }
                   ;
VariableDecl       :  IdentList  ycolon  Type { addVars(); }
                   ;

/***************************  Const/Type Stuff  ******************************/

ConstExpression    :  ConstFactor
                   |  UnaryOperator ConstFactor
                   |  ystring { newSymbol = getString();
                         $$.type = "string";
                         $$.str = strdup(value.c_str());
                         $$.sym = NULL;
                      } 
                   ;
ConstFactor        :  yident { /*printf(value.c_str()); printf(" ");*/
					  Symbol *tVal = table.lookUp(value);
					  if(tVal == NULL) {
						  tVal = sit.getSymbol(value);
						  if(tVal == NULL) {
							  cout << "***ERROR: Constant type " << value << " does not exist" << endl;
							  $$.type = "bad";
							  $$.sym = new Symbol;
						  }
						  else {
							  newSymbol = tVal;
							  Constant<int> *iTemp = dynamic_cast<Constant<int>*>(tVal);
							  if(iTemp != NULL) {
								$$.type = "integer";
								$$.str = iTemp->name.c_str();
								$$.sym = iTemp->type;
							  }
							
							  Constant<double> *dTemp = dynamic_cast<Constant<double>*>(tVal);
							  if(dTemp != NULL) {
								$$.type = "real";
								$$.str = dTemp->name.c_str();
								$$.sym = dTemp->type;
							  }
							
							  Constant<bool> *bTemp = dynamic_cast<Constant<bool>*>(tVal);
							  if(bTemp != NULL) {
								$$.type = "boolean";
								$$.str = bTemp->name.c_str();
								$$.sym = bTemp->type;
							  }
							
							  Constant<string> *sTemp = dynamic_cast<Constant<string>*>(tVal);
							  if(sTemp != NULL) {
								$$.type = "string";
								$$.str = sTemp->name.c_str();
								$$.sym = sTemp->type;
							  }
							
							  Constant<void*> *nTemp = dynamic_cast<Constant<void*>*>(tVal);
							  if(nTemp != NULL) {
								$$.type = "pointer";
								$$.str = nTemp->name.c_str();
								$$.sym = NULL;
							  }
							
							  if(tVal == NULL) {
							    $$.type = "bad";
							    $$.str = "bad";
							    $$.sym = NULL;
							  }
						  }
					  }
					  else {
						  newSymbol = tVal;
					  }
					  }
                   |  ynumber { newSymbol = getNumber();
                         Constant<int> *iTemp = dynamic_cast<Constant<int>*>(newSymbol);
                         if(iTemp != NULL) {
							$$.type = "integer";
							$$.str = NULL;
							$$.sym = iTemp;
					     }
					     
					     Constant<double> *dTemp = dynamic_cast<Constant<double>*>(newSymbol);
						 if(dTemp != NULL) {
							$$.type = "real";
							$$.str = NULL;
							$$.sym = dTemp;
					     }
                      }
                   |  ytrue { newSymbol = getTrue();
                         Constant<bool> *bTemp = dynamic_cast<Constant<bool>*>(newSymbol);
					     if(bTemp != NULL) {
							$$.type = "boolean";
							$$.str = NULL;
							$$.sym = bTemp->type;
					     }
                      }
                   |  yfalse { newSymbol = getFalse();
                         Constant<bool> *bTemp = dynamic_cast<Constant<bool>*>(newSymbol);
					     if(bTemp != NULL) {
							$$.type = "boolean";
							$$.str = NULL;
							$$.sym = bTemp->type;
					     }
                      }
                   |  ynil { newSymbol = getNil();
                         Constant<void*> *nTemp = dynamic_cast<Constant<void*>*>(newSymbol);
					     if(nTemp != NULL) {
							$$.type = "pointer";
							$$.str = NULL;
							$$.sym = NULL;
						 }
                      }
                   ;
Type               :  yident { symbolName.assign(value);
					  Type *temp = dynamic_cast<Type*>(table.lookUp(symbolName));
					  if(temp != NULL) {
					     currentType = temp;
					  }
					  else {
					     temp = dynamic_cast<Type*>(sit.getSymbol(symbolName));
					     if(temp != NULL) {
					        currentType = temp;
					     }
					     else {
					        cout << "***ERROR: type " << symbolName << " does not exist" << endl;
					        currentType = NULL;
					     }
					  } 
					  }
                   |  ArrayType
                   |  PointerType
                   |  RecordType
                   |  SetType
                   ;
ArrayType          :  yarray { ArrayType *temp = new ArrayType; temp->numDim = 0;
					  arrayStack.push(temp); }
					  yleftbracket Subrange {
					  ArrayType *temp = arrayStack.top(); ArrayType::DimRange dim;
					  dim.start = setStart->clone(); dim.end = setEnd->clone(); 
					  temp->dimRanges.push_back(dim);
					  temp->numDim += 1;
					  if(!setStart->name.compare("")) {
							delete setStart;
					  }
					  if(!setEnd->name.compare("")) {
							delete setEnd;
					  }
					  }
					  SubrangeList 
                      yrightbracket  yof Type { ArrayType *temp = arrayStack.top();
                      temp->type = currentType; arrayStack.pop();
                      currentType = temp;
                      }
                   ;
SubrangeList       :  /*** empty ***/
                   |  SubrangeList ycomma Subrange {
					  ArrayType *temp = arrayStack.top(); ArrayType::DimRange dim;
					  dim.start = setStart->clone(); dim.end = setEnd->clone(); 
					  temp->dimRanges.push_back(dim);
					  temp->numDim += 1;
					  if(!setStart->name.compare("")) {
							delete setStart;
					  }
					  if(!setEnd->name.compare("")) {
							delete setEnd;
					  }
					  }
                   ;
Subrange           :  ConstFactor { setStart = newSymbol;
					  newSymbol = NULL; } ydotdot ConstFactor { setEnd = newSymbol;
					  newSymbol = NULL; }
                   |  ystring { setStart = getString(); } ydotdot  ystring
                      { setEnd = getString(); }
                   ;
RecordType         :  yrecord {recordName.assign(symbolName); }
					  FieldListSequence  yend { addRecord(); }
                   ;
SetType            :  yset  yof  Subrange { addSetType(); currentType->display(0); }
                   ;
PointerType        :  ycaret yident {  
					  makePointerType(); }
                   ;
FieldListSequence  :  FieldList  
                   |  FieldListSequence  ysemicolon  FieldList
                   ;
FieldList          :  IdentList  ycolon  Type { addRecordVars(); }
                   ;

/***************************  Statements  ************************************/

StatementSequence  :  Statement
                   |  StatementSequence  
                      ysemicolon  /*{
					     cout << endl;
					     for(int i = 0; i < gLevel; i++) {
						    cout << "   ";
						 }
					  }*/
                      Statement
                   ;
Statement          :  Assignment { cout << ";"; }
                   |  ProcedureCall { cout << ";"; }
                   |  IfStatement
                   |  CaseStatement
                   |  WhileStatement
                   |  RepeatStatement
                   |  ForStatement
                   |  IOStatement { cout << ";"; }
                   |  MemoryStatement { cout << ";"; }
                   |  ybegin StatementSequence yend
                   |  /*** empty ***/
                   ;
Assignment         :  Designator yassign { cout << " = "; }
					  Expression {
						 Symbol *temp1 = static_cast<Symbol*>($1.sym);
						 if((temp1->name == "real" || temp1->name == "integer") &&
						    isNumber($4.type, $4.sym)) {
						 }
						 else if((temp1->name == "boolean") && isBool($4.type, $4.sym)) {
						 }
						 else if(cmpPtrs(isPointer($1.type, $1.sym), isPointer($4.type, $4.sym))
						 && isPointer($1.type, $1.sym).type != "NULL") {
					     }
					     else if($4.sym != NULL) {
					        Symbol *temp2 = static_cast<Symbol*>($4.sym);
					        if(temp1->name != temp2->name) {
							   cout << "***ERROR: type mismatch" << endl;
							}
					     }
					     else {
					        cout << "***ERROR: type mismatch" << endl;
					     }

					  }/* maybe allow subprocedure call here? */
                   ;
ProcedureCall      :  yident { 
                         ProcFunc *pTemp = dynamic_cast<ProcFunc*>(table.lookUp(value));
                         if(pTemp != NULL) {
                            cout << value << "()"; 
                         }
                         else {
                            cout << "***ERROR: " << value
                            << " is not a subprocedure" << endl;
                         }
                      }
                   |  yident { 
                         ProcFunc *pTemp = dynamic_cast<ProcFunc*>(table.lookUp(value));
                         if(pTemp != NULL) {
                            cout << value << "("; 
                         }
                         else {
                            cout << "***ERROR: " << value
                            << " is not a subprocedure" << endl;
                         }
                         
                         $1.type = strdup(value.c_str());
                      } 
                      ActualParameters {
                         ProcFunc *pTemp = dynamic_cast<ProcFunc*>(table.lookUp($1.type));
                         free($1.type);
                         
                         tNode *tTemp = static_cast<tNode*>($3.list);
                         
                         for(int i = pTemp->params.size() - 1; i >= 0; i--, tTemp = tTemp->next) {
                            if(pTemp->params[i]->type->name != tTemp->sym->name) {
                               cout << "***ERROR: invalid parameter";
                            }
                         }
                         
                         cout << ")";
                      }
                   ;
IfStatement        :  yif { cout << "if("; }
					  Expression {
					     if(strcmp($3.type, "bool")) {
					        cout << "***ERROR: If statments can only be controlled by "
					        << "bool values" << endl;
					     }
					     cout << ") ";
					  } 
					  ythen { cout << " {"; } Statement { cout << endl << "}"; } ElsePart
                   ;
ElsePart           :  /*** empty ***/
                   |  yelse { cout << "else "; } IfStatement
                   |  yelse { cout << "else {";} ElseStatement { cout << endl << "}"; }
                   ;
ElseStatement      :  Assignment { cout << ";"; }
                   |  ProcedureCall { cout << ";"; }
                   |  CaseStatement
                   |  WhileStatement
                   |  RepeatStatement
                   |  ForStatement
                   |  IOStatement { cout << ";"; }
                   |  MemoryStatement { cout << ";"; }
                   |  ybegin StatementSequence yend
                   |  /*** empty ***/
                   ;
CaseStatement      :  ycase { cout << "switch("; }  
					  Expression {
					     if(strcmp($3.type, "number") || strcmp($3.type, "char")) {
							 if($3.str != NULL && strcmp($3.str, "integer")) {
								cout << "***ERROR: switched cannot be contolled by reals" << endl;
							 }
						 }
						 else {
							 cout << "***ERROR: switches can only be controlled by chars and ints" << endl;
						 }
					     cout << ") ";
					  } 
					  yof { cout << " {" << endl; } CaseList  yend { cout << endl << "}"; }
                   ;
CaseList           :  Case
                   |  CaseList  ysemicolon  Case  
                   ;
Case               :  CaseLabelList  ycolon { cout << ": " << endl; } 
					  Statement { cout << endl << "break;" << endl; }
                   ;
CaseLabelList      :  ConstExpression {
						 cout << "case ";
					     if(strcmp($1.type, "integer") && strcmp($1.type, "string")) {
					        cout << "***ERROR: Switch statement, case constant can only be an "
					        << "int or char value" << endl;
					     }
					     else if(!strcmp($1.type, "string") && $1.str[1] != '\0' && $1.sym == NULL) {
					        cout << "***ERROR: Switch statement, case constant cannot be a string" << endl;
					     }
					     else {
					        if(!strcmp($1.type, "string") && $1.sym == NULL) {
					           cout << "'" << $1.str << "'";
					        }
					        
					        if(!strcmp($1.type, "string") && $1.sym != NULL) {
					           Constant<string> *sTemp = static_cast<Constant<string>*>($1.sym);
					           if(sTemp->value[1] != NULL) {
					              cout << "***ERROR: Switch statement, case constant cannot be a string" << endl;
					           }
					           else {
					              cout << sTemp->name;
					           }
					        }
					        
					        if(!strcmp($1.type, "integer")) {
					           Constant<int> *iTemp = static_cast<Constant<int>*>($1.sym);
					           if($1.str == NULL) {
					              cout << iTemp->value;
					           }
					           else {
					              cout << iTemp->name;
					           }
					        }
					     }
					  } 
                   |  CaseLabelList  ycomma { cout << ":" << endl; } ConstExpression {
						 cout << "case ";
					     if(strcmp($4.type, "integer") && strcmp($4.type, "string")) {
					        cout << "***ERROR: Switch statement, case constant can only be an "
					        << "int or char value" << endl;
					     }
					     else if(!strcmp($4.type, "string") && $4.str[1] != '\0' && $4.sym == NULL) {
					        cout << "***ERROR: Switch statement, case constant cannot be a string" << endl;
					     }
					     else {
					        if(!strcmp($4.type, "string") && $4.sym == NULL) {
					           cout << "'" << $4.str << "'";
					        }
					        
					        if(!strcmp($4.type, "string") && $4.sym != NULL) {
					           Constant<string> *sTemp = static_cast<Constant<string>*>($4.sym);
					           if(sTemp->value[1] != NULL) {
					              cout << "***ERROR: Switch statement, case constant cannot be a string" << endl;
					           }
					           else {
					              cout << sTemp->name;
					           }
					        }
					        
					        if(!strcmp($4.type, "integer")) {
					           Constant<int> *iTemp = static_cast<Constant<int>*>($4.sym);
					           if($4.str == NULL) {
					              cout << iTemp->value;
					           }
					           else {
					              cout << iTemp->name;
					           }
					        }
					     }
					     //cout << ":" << endl;
					  }   
                   ;
WhileStatement     :  ywhile { cout << "while("; } 
					  Expression {
					     if(strcmp($3.type, "bool")) {
					        cout << "***ERROR: while statments can only be controled by "
					        << "bool values" << endl;
					     }
					     cout << ") {";
					  }  
					  ydo  
					  Statement { cout << endl << "}"; }
                   ;
RepeatStatement    :  yrepeat { cout << "do {"; } 
					  StatementSequence 
					  yuntil { cout << endl << "} while(!("; }
					  Expression {
					     if(strcmp($6.type, "bool")) {
					        cout << "***ERROR: while statments can only be controled by "
					        << "bool values" << endl;
					     }
					     cout << ")); ";
					  }  
                   ;
ForStatement       :  yfor  yident { forControl.assign(value);
					     cout << "for(" << forControl;
					  } 
					  yassign { cout << " = "; }
					  Expression {
					     Variable *temp = dynamic_cast<Variable*>(table.lookUp(forControl));
					     if(temp == NULL) {
					        cout << "***ERROR: " <<
					        symbolName << " does not exist";
					     }
					     else {
							 if(temp->type->name != "integer" || strcmp($6.str, "integer")) {
								cout << "***ERROR: for statments can only be controled by ints";
							 }
						 }
						 
						 cout << ";";
					  } 
					  WhichWay {
					     if($8 == 1) {
					        cout << " " << forControl << " <= ";
					     }
					     else {
					        cout << " " << forControl << " >= ";
					     }
					  }
					  Expression {
					     if(!strcmp($10.str, "integer")) {
					        if($8 == 1) {
					           cout << "; " << forControl << "++) {";
					        }
					        else {
					           cout << "; " << forControl << "--) {";
					        }
					     }
					     else {
					        cout << "***ERROR: for statments can only be controled by ints";
					     }
					  }
                      ydo 
                      Statement { cout << endl << "}"; }
                   ;
WhichWay           :  yto { $$ = 1; } |  ydownto { $$ = 0; }
                   ;
IOStatement        :  yread { lSeperator = " >> "; cout << "cin >> "; } 
					  yleftparen  DesignatorList  yrightparen
                   |  yreadln {
                         char buf[400];
                         cin.getline(buf, 399);
                      }
                   |  yreadln { lSeperator = " >> "; cout << "cin >> "; }
                      yleftparen DesignatorList  yrightparen {
                         char buf[400];
                         cin.getline(buf, 399);
                      }
                   |  ywrite { lSeperator = " << "; cout << "cout << "; } 
                      yleftparen  ExpList  yrightparen
                   |  ywriteln { cout << "cout << endl"; }
                   |  ywriteln { lSeperator = " << "; cout << "cout << "; } 
                      yleftparen  ExpList  yrightparen {
					     cout << " << endl";
                      }
                   ;

/***************************  Designator Stuff  ******************************/

DesignatorList     :  Designator  
                   |  DesignatorList  ycomma  Designator 
                   ;
Designator         :  yident { 
						 Symbol *temp = table.lookUp(value);
					     Variable *var = dynamic_cast<Variable*>(temp);
					     ProcFunc *pTemp = dynamic_cast<ProcFunc*>(table.lookUpCS(value));
					     if(var != NULL) {
							currentBase = var;
							cout << value.c_str(); 
					     }
					     else if(pTemp != NULL) {
							currentBase = pTemp;
							cout << "ret_" << value.c_str();
						 }
					     else if(temp != NULL && temp->isConst) {
							currentBase = temp;
							cout << value.c_str(); 
					     }
					     else {
							Symbol *temp = new Symbol;
							currentBase = temp;
					        cout << "***ERROR: " << value << " is not a variable" << endl;
					     }
					     $1.type = strdup(value.c_str());
					  } DesignatorStuff {
					     if(!strcmp($3.type, "array")) {
					        Variable *var;
					        if($3.sym == NULL) {
					           var = dynamic_cast<Variable*>(table.lookUp($1.type));
					        }
					        else {
					           Symbol *temp = static_cast<Symbol*>($3.sym);
					           var = dynamic_cast<Variable*>(temp);
					        }
					        ArrayType *arr = dynamic_cast<ArrayType*>(var->type);
					        if(arr != NULL) {
								if(arr->numDim != $3.num) {
								   cout << "***ERROR: Incorrect number of dimensions" << endl;
								}
								$$.type = "var";
								$$.str = arr->type->name.c_str();
								$$.sym = arr->type;
								symbolName.assign("");
							}
					     }
					     else if(!strcmp($3.type, "element")) {
					        Symbol *temp = static_cast<Symbol*>($3.sym);
					        Variable *var = dynamic_cast<Variable*>(temp);
					        $$.type = "var";
					        $$.str = var->type->name.c_str();
							$$.sym = var->type;
					     }
					     else if(!strcmp($3.type, "empty")) {
					        Symbol *temp = table.lookUp($1.type);
					        Variable *vTemp = dynamic_cast<Variable*>(temp);
					        if(vTemp != NULL) {
								$$.type = "var";
								$$.str = vTemp->type->name.c_str();
								$$.sym = vTemp->type;
							}
							
							ProcFunc *proTemp = dynamic_cast<ProcFunc*>(table.lookUpCS($1.type));
							if(proTemp != NULL) {
							   $$.type = "func";
							   $$.str = proTemp->type->name.c_str();
							   $$.sym = proTemp->type;
							}
							
							ProcFunc *fTemp = dynamic_cast<ProcFunc*>(temp);
							if(fTemp != NULL) {
							   $$.type = "func";
							   $$.str = fTemp->type->name.c_str();
							   $$.sym = fTemp->type;
							}
							
							Constant<int> *iTemp = dynamic_cast<Constant<int>*>(temp);
							if(iTemp != NULL) {
								$$.type = "const";
								$$.str = iTemp->type->name.c_str();
								$$.sym = iTemp->type;
							}
							
							Constant<double> *dTemp = dynamic_cast<Constant<double>*>(temp);
							if(dTemp != NULL) {
								$$.type = "const";
								$$.str = dTemp->type->name.c_str();
								$$.sym = dTemp->type;
							}
							
							Constant<bool> *bTemp = dynamic_cast<Constant<bool>*>(temp);
							if(bTemp != NULL) {
								$$.type = "const";
								$$.str = bTemp->type->name.c_str();
								$$.sym = bTemp->type;
							}
							
							Constant<string> *sTemp = dynamic_cast<Constant<string>*>(temp);
							if(sTemp != NULL) {
								$$.type = "const";
								$$.str = sTemp->type->name.c_str();
								$$.sym = sTemp->type;
							}
							
							Constant<void*> *nTemp = dynamic_cast<Constant<void*>*>(temp);
							if(nTemp != NULL) {
								$$.type = "pointer";
								$$.str = nTemp->type->name.c_str();
								$$.sym = NULL;
							}
							
							if(temp == NULL) {
							   $$.type = "bad";
							   $$.str = "bad";
							   $$.sym = NULL;
							}
					     }
					     else {
					        cout << $3.type;
					        cout << "***ERROR: invalid RHS" << endl;
					     }
					     free($1.type);
					  }
                   ;
DesignatorStuff    :  /*** empty ***/ { 
					  $$.type = "empty";
					  $$.sym = currentBase;
					  }
                   |  DesignatorStuff theDesignatorStuff {
					     if(!strcmp($2.type, "arrayindex")) {
							if($1.type != NULL) {
								if(!strcmp($1.type, "array")) {
								   $$.type = "array"; $$.num = $$.num + 1;
								}
								else if(!strcmp($1.type, "empty")) {
								   $$.type = "array"; $$.num = 1;
								}
								else if(!strcmp($1.type, "element")) {
								   $$.type = "array"; $$.num = 1;
								   $$.sym = $1.sym;
								}
							}
					     }
					     else if(!strcmp($2.type, "element")) {
					        Symbol *temp = static_cast<Symbol*>($1.sym);
					        RecordType *rTemp = 
					        dynamic_cast<RecordType*>(dynamic_cast<Variable*>(temp)->type);
					        PointerType *pTemp =
					        dynamic_cast<PointerType*>(dynamic_cast<Variable*>(temp)->type);
					        
					        // Handle the case when temp is a record
					        if(rTemp != NULL) {
					           Variable *vTemp = rTemp->getMember($2.str);
					           if(vTemp == NULL) {
					              cout << "***ERROR: " << $2.str
					              << "is not a member of " << rTemp->name;
					           }
					           else {
					              cout << "." << vTemp->name;
					              $$.type = "element";
					              $$.sym = vTemp;
					           }
					        }
					        // Handle the case when temp is a pointer.
					        else if(pTemp != NULL) {
					           rTemp = dynamic_cast<RecordType*>(pTemp->typeTo);
					           Variable *vTemp = rTemp->getMember($2.str);
					           if(vTemp == NULL) {
					              cout << "***ERROR: " << $2.str
					              << "is not a member of " << rTemp->name;
					           }
					           else {
					              cout << $2.str;
					              $$.type = "element";
					              $$.sym = vTemp;
					           }
					        }
					        else {
					           cout << "***ERROR: LHS is not a record or a pointer" << endl;
					        }
					     }
					     else if(!strcmp($2.type, "pointer")) {
					        $$.type = "pointer";
					        $$.sym = $1.sym;
					     }
                      }
                   ;
theDesignatorStuff :  ydot yident { 
					  $$.type = "element"; $$.str = value.c_str(); }
                   |  yleftbracket { cout << "["; } 
                      ExpList yrightbracket {
                      if(!strcmp($3.type, "string") && $3.type[1] != '\0') {
                         cout << "***ERROR: arrays cannot be indexed bys strings" << endl;
                      }
                      else if(strcmp($3.type, "number") || strcmp($3.type, "char")) {
                         if($3.str != NULL && strcmp($3.str, "integer")) {
						    cout << "***ERROR: arrays cannot be indexed by reals" << endl;
						 }
                      }
                      else {
                         cout << "***ERROR: arrays can only be indexed by chars and ints" << endl;
                      }
                      cout << "]"; 
                      $$.type = "arrayindex"; }
                   |  ycaret { cout << "->"; $$.type = "pointer"; }
                   ;
ActualParameters   :  yleftparen { lSeperator = ", "; } ExpList  yrightparen { $$.list = $3.list; }
                   ;
ExpList            :  Expression { 
					     tNode *temp = new tNode;
					     temp->next = NULL;
					     temp->sym = static_cast<Symbol*>($1.sym);
					     
					     $$.type = $1.type;
					     $$.sym = $1.sym;
					     $$.list = temp;
					  }
                   |  ExpList  ycomma { cout << lSeperator; } Expression {
                         tNode *temp = new tNode;
					     temp->sym = static_cast<Symbol*>($4.sym);
					     tNode *pTemp = static_cast<tNode*>($1.list);
					     temp->next = pTemp;
					     
                         $$.type = $1.type;
                         $$.sym = $1.sym;
                         $$.list = temp;
                      }     
                   ;
MemoryStatement    :  ynew  yleftparen  yident { 
					  Variable *var = dynamic_cast<Variable*>(table.lookUp(value));
					  if(var != NULL) {
					     PointerType *type = dynamic_cast<PointerType*>(var->type);
					     if(type != NULL) {
					        for(int i = 0; i < gLevel; i++) {
							   cout << "   ";
							}
					        cout << var->name << " = new " << getFinalTypeName(type->typeTo);
					     }
					  } 
					  }
					  yrightparen  
                   |  ydispose yleftparen  yident  { 
					  Variable *var = dynamic_cast<Variable*>(table.lookUp(value));
					  if(var != NULL) {
					     PointerType *type = dynamic_cast<PointerType*>(var->type);
					     if(type != NULL) {
					        for(int i = 0; i < gLevel; i++) {
							   cout << "   ";
							}
					        cout << "delete " << var->name;
					     }
					  } 
					  }
					  yrightparen
                   ;

/***************************  Expression Stuff  ******************************/

Expression         :  SimpleExpression  { 
                         $$.type = $1.type;
                         $$.sym = $1.sym;
                         $$.str = $1.str;
                      } 
                   |  SimpleExpression  Relation  SimpleExpression {
                         if(!(isNumber($1.type, $1.sym) && isNumber($3.type, $3.sym)) &&
                         !cmpPtrs(isPointer($1.type, $1.sym), isPointer($3.type, $3.sym))) {
							cout << "***ERROR: relations can contain only numbers or pointers" << endl;
                         }
                         $$.type = "bool";
                         $$.str = "bool";
                         $$.sym = sit.getSymbol("boolean");
                      }
                   ;
SimpleExpression   :  TermExpr { 
                         $$.type = $1.type;
                         $$.sym = $1.sym;
                         $$.str = $1.str;
                      }
                   |  UnaryOperator  TermExpr {
                         if(!isNumber($2.type, $2.sym)) {
                            cout << 
                            "***ERROR: unary operators can only be applied to numbers" 
                            << endl;
                         }
                         $$.type = $2.type;
                         $$.sym = $2.sym;
                         $$.str = $2.str;
                      }
                   ;
TermExpr           :  Term  { 
                         $$.type = $1.type;
                         $$.sym = $1.sym;
                         $$.str = $1.str;
                      } 
                   |  TermExpr  AddOperator  Term {
                         if((isNumber($1.type, $1.sym) && isNumber($3.type, $3.sym))) {
                            if((!strcmp($2.type, "bool")) && (!isBool($1.type, $1.sym)
                            || !isBool($3.type, $3.sym))) {
                               cout <<
                               "***ERROR: or can only be used with bools" << endl;
                            }
                         }
                         else {
                            cout <<
                            "***ERROR: only numbers can be added" << endl;
                         }
                         $$.type = $1.type;
                         $$.sym = $1.sym;
                         $$.str = $1.str;
                      }
                   ;
Term               :  Factor  { 
                         $$.type = $1.type;
                         $$.sym = $1.sym;
                         $$.str = $1.str;
                      } 
                   |  Term  MultOperator  Factor {
						 Symbol *temp = static_cast<Symbol*>($3.sym);
                         if((isNumber($1.type, $1.sym) && isNumber($3.type, $3.sym))) {
                            if((!strcmp($2.type, "int")) && (!(string($1.str) == "integer") ||
                            !(string($3.str) == "integer"))) {
                               cout <<
                               "***ERROR: div and mod can only be applied to ints"
                               << endl;
                            }
                            
                            if((!strcmp($2.type, "bool")) && (!isBool($1.type, $1.sym)
                            || !isBool($3.type, $3.sym))) {
                               cout <<
                               "***ERROR: and can only be used with bools" << endl;
                            }
                         }
                         else {
                            cout <<
                            "***ERROR: only numbers can be added" << endl;
                         }
                         $$.type = $1.type;
                         $$.sym = $1.sym;
                         $$.str = $1.str;
                      }
                   ;
Factor             :  ynumber {
					     Symbol *temp = getNumber();
					     temp->displayVal();
					     Constant<int> *iTemp = dynamic_cast<Constant<int>*>(temp);
					     if(iTemp != NULL) {
					        $$.str = iTemp->type->name.c_str();
					     }
					     Constant<double> *dTemp = dynamic_cast<Constant<double>*>(temp);
					     if(dTemp != NULL) {
					        $$.str = dTemp->type->name.c_str();
					     }
					     $$.type = "number";
					     $$.sym = temp;
					  }
                   |  ytrue {
                         Symbol *temp = getTrue();
                         cout << "true";
                         $$.type = "bool";
                         $$.sym = temp;
                      }
                   |  yfalse {
                         Symbol *temp = getFalse();
                         cout << "false";
                         $$.type = "bool";
                         $$.sym = temp;
                      }
                   |  ynil {
                         Symbol *temp = getNil();
                         cout << "NULL";
                         $$.type = "pointer";
                         $$.sym = temp;
                      }
                   |  ystring {
                         Symbol *temp = getString();
                         cout << "\"" << value << "\"";
                         $$.type = "string";
                         $$.sym = temp;
                      }
                   |  Designator { 
                         $$.type = $1.type;
                         $$.sym = $1.sym;
                         $$.str = $1.str;
                      }
                   |  yleftparen { cout << "("; } Expression  yrightparen {
						 cout << ")";
                         $$.type = $3.type;
                         $$.sym = $3.sym;
                         $$.str = $3.str;
                      }
                   |  ynot { cout << "!"; } Factor {
                         $$.type = $3.type;
                         $$.sym = $3.sym;
                         $$.str = $3.str;
                      }
                   |  Setvalue {
                         $$.type = $1.type;
                         $$.sym = $1.sym;
                         $$.str = $1.str;
                      }
                   |  FunctionCall {
                         $$.type = $1.type;
                         $$.sym = $1.sym;
                         $$.str = $1.str;
                      }
                   ;
/*  Functions with no parameters have no parens, but you don't need         */
/*  to handle that in FunctionCall because it is handled by Designator.     */
/*  A FunctionCall has at least one parameter in parens, more are           */
/*  separated with commas.                                                  */
FunctionCall       :  yident { 
                         ProcFunc *pTemp = dynamic_cast<ProcFunc*>(table.lookUp(value));
                         $1.type = strdup(value.c_str());
                         if(pTemp != NULL) {
                            cout << value << "("; 
                            //$1.type = "var";
                            $1.str = pTemp->type->name.c_str();
                            $1.sym = pTemp->type;
                         }
                         else {
                            cout << "***ERROR: " << value
                            << " is not a function" << endl;
                            //$1.type = "bad";
                            $1.str = "bad";
                            $1.sym = new Symbol;
                         }
                      }
                      ActualParameters {
                         ProcFunc *pTemp = dynamic_cast<ProcFunc*>(table.lookUp($1.type));
                         //free($1.type);
                         
                         tNode *tTemp = static_cast<tNode*>($3.list);
                         
                         for(int i = pTemp->params.size() - 1; i >= 0; i--, tTemp = tTemp->next) {
                            if(tTemp == NULL) {
                               cout << "***ERROR: invalid number of params";
                               break;
                            }
                         
                            if(pTemp->params[i]->type->name != tTemp->sym->name) {
                               cout << "***ERROR: invalid parameter";
                            }
                         }
                         
                         $$.str = $1.str;
                         $$.sym = $1.sym;
                         
                         if(!strcmp($1.str, "bad")) {
							$$.type = "bad";
						 }
						 else {
						    $$.type = "var";
						 }
                         
                         cout << ")";
                      }
					  /* you finish it, Expressions are valid parameters */
                   ;
Setvalue           :  yleftbracket ElementList  yrightbracket
                   |  yleftbracket yrightbracket
                   ;
ElementList        :  Element  
                   |  ElementList  ycomma  Element
                   ;
Element            :  ConstExpression  
                   |  ConstExpression  ydotdot  ConstExpression 
                   ;

/***************************  Subprogram Stuff  ******************************/

SubprogDeclList    :  /*** empty ***/
                   |  SubprogDeclList ProcedureDecl ysemicolon  
                   |  SubprogDeclList FunctionDecl ysemicolon
                   ;
ProcedureDecl      :  ProcedureHeading  { addScope(); } ysemicolon 
					  Block { 
					  table.exitScope();
					  for(int i = 0; i < table.getScopeLevel(); i++) {
					     cout << "   ";
					  }
					  gLevel--;
					  cout << "}" << endl; }
                   ;
FunctionDecl       :  FunctionHeading  ycolon  yident { addScope(true); } 
					  ysemicolon  Block { 
					     for(int i = 0; i < gLevel; i++) {
					        cout << "   ";
					     }
					     Symbol *temp = table.lookUpCS(table.getScopeName());
					     cout << "return ret_" << temp->name << ";" << endl;
					     cout << "}";
					     table.exitScope(); gLevel--; 
					  }
                   ;
ProcedureHeading   :  yprocedure  yident { scopeName.assign(value); } 
                   |  yprocedure  yident { scopeName.assign(value); 
                      } FormalParameters
                   ;
FunctionHeading    :  yfunction  yident { scopeName.assign(value); } 
                   |  yfunction  yident { scopeName.assign(value);
                      } FormalParameters
                   ;
FormalParameters   :  yleftparen FormalParamList yrightparen 
                   ;
FormalParamList    :  OneFormalParam 
                   |  FormalParamList ysemicolon OneFormalParam
                   ;
OneFormalParam     :  yvar  IdentList  ycolon  yident { addFuncVars(true); }
                   |  IdentList  ycolon  yident { addFuncVars(false); }
                   ;

/***************************  More Operators  ********************************/

UnaryOperator      :  yplus { cout << "+"; } | yminus { cout << "-"; }
                   ;
MultOperator       :  ymultiply { cout << " * "; $$.type = "number"; }
				   |  ydivide { cout << " / "; $$.type = "number"; }
				   |  ydiv { cout << " / "; $$.type = "int"; }
				   |  ymod { cout << " % "; $$.type = "int"; }
				   |  yand { cout << " && "; $$.type = "bool"; }
                   ;
AddOperator        :  yplus { cout << " + "; $$.type = "number"; }
				   |  yminus { cout << " - "; $$.type = "number"; }
				   |  yor { cout << " || "; $$.type = "bool"; }
                   ;
Relation           :  yequal { cout << " == "; $$.type = "number"; } 
				   |  ynotequal { cout << " != "; $$.type = "number"; }
				   |  yless { cout << " < "; $$.type = "number"; }
				   |  ygreater { cout << " > "; $$.type = "number"; }
                   |  ylessequal { cout << " <= "; $$.type = "number"; }
                   |  ygreaterequal { cout << " >= "; $$.type = "number"; }
                   |  yin
                   ;

%%

/* program section */

void yyerror(char *s) {
   cout << s << endl;
}

extern int yylex();

