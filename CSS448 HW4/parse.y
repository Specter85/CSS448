%{

/* declarations section */
#include <iostream>
#include <stack>
#include <list>
#include "yylex.h"
#include "parsefuncs.h"
#include "codefuncs.h"
#include "pointertype.h"
#include "symbol.h"
#include "simpletype.h"
#include "variable.h"
#include "arraytype.h"
#include "symboltable.h"
using namespace std; 

SymbolTable table;
string symbolName;
string typeName;
string scopeName;
string recordName;

Symbol *newSymbol = NULL;

stack<string> identStack;
stack<ArrayType*> arrayStack;
list<Variable*> variableStack;
// Stack of unfinished pointers.
list<umfPointer> pointerStack;

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
	char *sval;
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

%%
/* rules section */

/**************************  Pascal program **********************************/

CompilationUnit    :  ProgramModule        
                   ;
ProgramModule      :  yprogram yident { scopeName.assign(value); } ProgramParameters
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
                   |  ytype  TypeDefList { processUmfPointers(); }
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
					  { currentType->name.assign(typeName); 
					  if(table.lookUpCS(typeName) == NULL) {
					     table.insert(currentType);
					  }
					  else {
					     cout << "ERROR: " << symbolName << " already in symbol table" << endl;
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
							  cout << "ERROR: Constant type " << value << " does not exist" << endl;
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
					        cout << "ERROR: type " << symbolName << " does not exist" << endl;
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
SetType            :  yset  yof  Subrange { addSetType(); }
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
Assignment         :  Designator yassign Expression /* maybe allow subprocedure call here? */
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
Designator         :  yident { printf(value.c_str()); printf(" "); } DesignatorStuff 
                   ;
DesignatorStuff    :  /*** empty ***/
                   |  DesignatorStuff  theDesignatorStuff
                   ;
theDesignatorStuff :  ydot yident { printf(value.c_str()); printf(" "); }
                   |  yleftbracket ExpList yrightbracket 
                   |  ycaret 
                   ;
ActualParameters   :  yleftparen  ExpList  yrightparen
                   ;
ExpList            :  Expression   
                   |  ExpList  ycomma  Expression       
                   ;
MemoryStatement    :  ynew  yleftparen  yident { printf(value.c_str()); printf(" "); } yrightparen  
                   |  ydispose yleftparen  yident { printf(value.c_str()); printf(" "); } yrightparen
                   ;

/***************************  Expression Stuff  ******************************/

Expression         :  SimpleExpression  
                   |  SimpleExpression  Relation  SimpleExpression 
                   ;
SimpleExpression   :  TermExpr
                   |  UnaryOperator  TermExpr
                   ;
TermExpr           :  Term  
                   |  TermExpr  AddOperator  Term
                   ;
Term               :  Factor  
                   |  Term  MultOperator  Factor
                   ;
Factor             :  ynumber
                   |  ytrue
                   |  yfalse
                   |  ynil
                   |  ystring
                   |  Designator
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
ProcedureDecl      :  ProcedureHeading  { addScope(); } ysemicolon  Block { table.exitScope(); }
                   ;
FunctionDecl       :  FunctionHeading  ycolon  yident { printf(value.c_str()); printf(" ");
					  addScope(true); } ysemicolon  Block { table.exitScope(); }
                   ;
ProcedureHeading   :  yprocedure  yident { printf(value.c_str()); printf(" "); scopeName.assign(value); } 
                   |  yprocedure  yident { printf(value.c_str()); printf(" "); scopeName.assign(value); 
                      } FormalParameters
                   ;
FunctionHeading    :  yfunction  yident { printf(value.c_str()); printf(" "); scopeName.assign(value); } 
                   |  yfunction  yident { printf(value.c_str()); printf(" "); scopeName.assign(value);
                      } FormalParameters
                   ;
FormalParameters   :  yleftparen FormalParamList yrightparen 
                   ;
FormalParamList    :  OneFormalParam 
                   |  FormalParamList ysemicolon OneFormalParam
                   ;
OneFormalParam     :  yvar  IdentList  ycolon  yident { printf(value.c_str()); printf(" "); addFuncVars(true); }
                   |  IdentList  ycolon  yident { printf(value.c_str()); printf(" "); addFuncVars(false); }
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

