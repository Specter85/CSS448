#include "symboltable.h"
extern SymbolTable table;
extern int yyparse();

int main() {
   int result = yyparse();
   //table.printST();
   return result;
}
