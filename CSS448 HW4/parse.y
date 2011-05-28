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
	} cur;
}

%start  CompilationUnit
%token  yand yarray yassign ybegin ycaret ycase ycolon ycomma yconst ydispose 
        ydiv ydivide ydo  ydot ydotdot ydownto yelse yend yequal yfalse
        yfor yfunction ygreater ygreaterequal yident  yif yin yleftbracket
        yleftparen yless ylessequal yminus ymod ymultiply ynew ynil ynot 
        ynotequal ynumber yof  yor yplus yprocedure yprogram yread yreadln  
        yrecord yrepeat yrightbracket yrightparen  ysemicolon yset ystring
        ythen  yto ytrue ytype  yuntil  yvar ywhile ywrite ywriteln ystring
        yunknown
        
%type <cur> DesignatorList Designator DesignatorStuff theDesignatorStuff
		SimpleExpression Expression TermExpr Term Factor

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
					  StatementSequence  yend { cout << "}"; }
                   ;
Block              :  Declarations  ybegin 
					  StatementSequence  yend
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
TypeDef            :  yident { printf(value.c_str()); printf(" "); symbolName.assign(value);
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
                   |  UnaryOperator ConstFactor             /* you finish it */
                   |  ystring { newSymbol = getString(); } 
                   ;
ConstFactor        :  yident { printf(value.c_str()); printf(" ");
					  Symbol *tVal = table.lookUp(value);
					  if(tVal == NULL) {
						  tVal = sit.getSymbol(value);
						  if(tVal == NULL) {
							  cout << "***ERROR: Constant type " << value << " does not exist" << endl;
						  }
						  else {
							  newSymbol = tVal;
						  }
					  }
					  else {
						  newSymbol = tVal;
					  }
					  }
                   |  ynumber { newSymbol = getNumber(); }
                   |  ytrue { newSymbol = getTrue(); }
                   |  yfalse { newSymbol = getFalse(); }
                   |  ynil { newSymbol = getNil(); }
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
                   |  StatementSequence  ysemicolon  Statement
                   ;
Statement          :  Assignment
                   |  ProcedureCall
                   |  IfStatement
                   |  CaseStatement
                   |  WhileStatement
                   |  RepeatStatement
                   |  ForStatement
                   |  IOStatement
                   |  MemoryStatement
                   |  ybegin StatementSequence yend
                   |  /*** empty ***/
                   ;
Assignment         :  Designator yassign { cout << " = "; }
					  Expression {
						 Symbol *temp1 = static_cast<Symbol*>($1.sym);
						 Symbol *temp2 = static_cast<Symbol*>($4.sym);
					     if(temp1->name != temp2->name) {
							cout << "***ERROR: type mismatch" << endl;
					     }
					     cout << ";" << endl;
					  }/* maybe allow subprocedure call here? */
                   ;
ProcedureCall      :  yident { printf(value.c_str()); printf(" "); }
                   |  yident { printf(value.c_str()); printf(" "); } ActualParameters
                   ;
IfStatement        :  yif  Expression  ythen  Statement  ElsePart
                   ;
ElsePart           :  /*** empty ***/
                   |  yelse  Statement  
                   ;
CaseStatement      :  ycase  Expression  yof  CaseList  yend
                   ;
CaseList           :  Case
                   |  CaseList  ysemicolon  Case  
                   ;
Case               :  CaseLabelList  ycolon  Statement
                   ;
CaseLabelList      :  ConstExpression  
                   |  CaseLabelList  ycomma  ConstExpression   
                   ;
WhileStatement     :  ywhile  Expression  ydo  Statement  
                   ;
RepeatStatement    :  yrepeat  StatementSequence  yuntil  Expression
                   ;
ForStatement       :  yfor  yident { printf(value.c_str()); printf(" "); } yassign  Expression  WhichWay  Expression
                            ydo  Statement
                   ;
WhichWay           :  yto  |  ydownto
                   ;
IOStatement        :  yread  yleftparen  DesignatorList  yrightparen
                   |  yreadln  
                   |  yreadln  yleftparen DesignatorList  yrightparen 
                   |  ywrite  yleftparen  ExpList  yrightparen
                   |  ywriteln  
                   |  ywriteln  yleftparen  ExpList  yrightparen 
                   ;

/***************************  Designator Stuff  ******************************/

DesignatorList     :  Designator  
                   |  DesignatorList  ycomma  Designator 
                   ;
Designator         :  yident { cout << value.c_str(); 
					     Variable *var = dynamic_cast<Variable*>(table.lookUp(value.c_str()));
					     if(var != NULL) {
							currentBase = var;
					        if(dynamic_cast<ArrayType*>(var->type) != NULL) {
					           symbolName.assign(value.c_str());
					        }
					     }
					     else {
					        cout << "***ERROR: " << value << " is not a variable" << endl;
					     }
					  } DesignatorStuff {
					     if(!strcmp($3.type, "array")) {
					        Variable *var = dynamic_cast<Variable*>(table.lookUp(symbolName));
					        if(dynamic_cast<ArrayType*>(var->type)->numDim != $3.num) {
					           cout << "***ERROR: Incorrect number of dimensions" << endl;
					        }
					        $$.type = "var";
					        $$.sym = var->type;
					        symbolName.assign("");
					     }
					     else if(!strcmp($3.type, "element")) {
					        Symbol *temp = static_cast<Symbol*>($3.sym);
					        Variable *var = dynamic_cast<Variable*>(temp);
					        $$.type = "var";
							$$.sym = var->type;
					     }
					     else if(!strcmp($3.type, "empty")) {
					        $$.sym = table.lookUp(symbolName);
					     }
					     else {
					        cout << $3.type;
					        cout << "***ERROR: invalid RHS" << endl;
					     }
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
                      ExpList yrightbracket { cout << "]"; $$.type = "arrayindex"; }
                   |  ycaret { cout << "->"; $$.type = "pointer"; }
                   ;
ActualParameters   :  yleftparen  ExpList  yrightparen
                   ;
ExpList            :  Expression   
                   |  ExpList  ycomma  Expression       
                   ;
MemoryStatement    :  ynew  yleftparen  yident { 
					  Variable *var = dynamic_cast<Variable*>(table.lookUp(value));
					  if(var != NULL) {
					     PointerType *type = dynamic_cast<PointerType*>(var->type);
					     if(type != NULL) {
					        for(int i = 0; i < gLevel; i++) {
							   cout << "\t";
							}
					        cout << var->name << " = new " << getFinalTypeName(type->typeTo) << ";" << endl;
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
							   cout << "\t";
							}
					        cout << "delete " << var->name << ";" << endl;
					     }
					  } 
					  }
					  yrightparen
                   ;

/***************************  Expression Stuff  ******************************/

Expression         :  SimpleExpression  { 
                         $$.type = $1.type;
                         $$.sym = $1.sym;
                      } 
                   |  SimpleExpression  Relation  SimpleExpression 
                   ;
SimpleExpression   :  TermExpr { 
                         $$.type = $1.type;
                         $$.sym = $1.sym;
                      }
                   |  UnaryOperator  TermExpr
                   ;
TermExpr           :  Term  { 
                         $$.type = $1.type;
                         $$.sym = $1.sym;
                      } 
                   |  TermExpr  AddOperator  Term
                   ;
Term               :  Factor  { 
                         $$.type = $1.type;
                         $$.sym = $1.sym;
                      } 
                   |  Term  MultOperator  Factor
                   ;
Factor             :  ynumber
                   |  ytrue
                   |  yfalse
                   |  ynil
                   |  ystring
                   |  Designator { 
                         $$.type = $1.type;
                         $$.sym = $1.sym;
                      }
                   |  yleftparen  Expression  yrightparen
                   |  ynot Factor
                   |  Setvalue
                   |  FunctionCall
                   ;
/*  Functions with no parameters have no parens, but you don't need         */
/*  to handle that in FunctionCall because it is handled by Designator.     */
/*  A FunctionCall has at least one parameter in parens, more are           */
/*  separated with commas.                                                  */
FunctionCall       :  yident { printf(value.c_str()); printf(" "); } ActualParameters
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
ProcedureDecl      :  ProcedureHeading  { addScope(); } ysemicolon  Block { 
					  table.exitScope();
					  for(int i = 0; i < table.getScopeLevel(); i++) {
					     cout << "\t";
					  }
					  gLevel--;
					  cout << "}" << endl; }
                   ;
FunctionDecl       :  FunctionHeading  ycolon  yident { addScope(true); } 
					  ysemicolon  Block { table.exitScope(); gLevel--; }
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

UnaryOperator      :  yplus | yminus
                   ;
MultOperator       :  ymultiply | ydivide | ydiv | ymod | yand 
                   ;
AddOperator        :  yplus | yminus | yor
                   ;
Relation           :  yequal  | ynotequal | yless | ygreater 
                   |  ylessequal | ygreaterequal | yin
                   ;

%%

/* program section */

void yyerror(char *s) {
   cout << s << endl;
}

extern int yylex();

