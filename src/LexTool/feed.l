%{
    #define KEY 1 //不能从零开始
    #define INT 2
    #define ID 3
    #define OP 4
    int yylval;
%}

delim      [" "\n\t]
ws         {delim}+
digit      [0-9]
num        {digit}+
letter     [A-Za-z]
key        (if)|(else)|(int)|(return)|(void)|(while)
id         {letter}+

%%
{ws}        {}
{num}       {yylval = 99; return(INT);}
{key}       {yylval = 6;return(KEY);}
{id}        {yylval = 100; return(ID);}
"<"         {yylval = 11; return(OP);}
">"         {yylval = 12; return(OP);}
"="         {yylval = 13;return(OP);}
"<="        {yylval = 14;return(OP);}
">="        {yylval = 15;return(OP);}
"!="        {yylval = 16;return(OP);}
"=="        {yylval = 17;return(OP);}
"+"         {yylval = 18;return(OP);}
"-"         {yylval = 19;return(OP);}
"*"         {yylval = 20;return(OP);}
"/"         {yylval = 21;return(OP);}
"!"         {yylval = 22;return(OP);}
";"         {yylval = 23;return(OP);}
","         {yylval = 24;return(OP);}
"("         {yylval = 25;return(OP);}
")"         {yylval = 26;return(OP);}
"["         {yylval = 27;return(OP);}
"]"         {yylval = 28;return(OP);}
"{"         {yylval = 29;return(OP);}   
"}"         {yylval = 30;return(OP);}

%%
int yywrap (){
  return 1;
}

int main(int argc,char * argv[])
{
  yyin = fopen(argv[1],"r");
  int c=0,i = 0;
  char ss[5][5] = {"","KEY","INT","ID","OP"};
  while(c=yylex()){
      printf("%d %s %s\n",++i,ss[c],yytext);
  }
  fclose(yyin);
  return 0;
}