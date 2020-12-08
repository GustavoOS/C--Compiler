# C- Compiler
C- Compiler developed with Flex and Bison


##Language Specification

###Lexical Convention:
reserved words:

else  if  int  return  void  while

special symbols:

<  <= << >  >= >> % | ^ ==  !=  = - + *  /   ;  ,  (  )  [  ]  {  }  

Identifiers:
ID = May optionally start with _ @ or : followed by a letter followed by any number of repetitions of (letter or num or _)

Numbers:
NUM = At least one numeric digit

Comments entre /*  */ e //

###Some restrictions

Functions may have void or int types
If a declared function doesn't receive any parameters, it should have the void words.
Example:
"int function(void){
    //Here goes your code
}"
Variables can only accept int (which is an integer) type, but can also be a int vector
Variables must be declared first



