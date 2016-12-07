#include "globals.h"
#include "util.h"
int main(int argc, char *argv[]) {
  if(argc == 2 ){
    if(source = fopen(argv[1],"r"))  yyin = source;
    else  perror(argv[0]);
  }else{
    yyin = stdin;
  }
  int token;
  while ((token=yylex())!=ENDFILE) {
    if(token == ERROR){
      printf("Error found at line %d: %s\n", lineno, yytext);
    }else{
      printf("\'%s\' : \'%s\'\n", yytext, names[token]);
    }
  }
  return(0);
}
