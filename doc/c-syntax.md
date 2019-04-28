### C-语法
# 终结符：
||||||||||||||||
|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|
|ID|int|void|(|)|,|;|
```c
program → declaration-list
declaration-list → declaration-list declaration|declaration
declaration → var-declaration|fun-declaration
var-declaration → type-specifier ID;|type-specifier ID[NUM];
type-specifier → int|void
fun-declaration → type-specifier ID(params)compound-stmt
params → param-list|void
param-list → param-list,param|param
param → type-specifier ID|type-specifier ID[]
compound-stmt → {local-declarations statement-list}
local-declarations → local-declarations var-declaration|empty
statement-list → statement-list statement|empty
statement → expression-stmt|compound-stmt|selection-stmt|iteration-stmt|return-stmt
expression-stmt → expression;|;
selection-stmt → if(expression) statement | if(expression) statement else statement
iteration-stmt → while(expression) statement
return-stmt → return;|return expression;
expression → var=expression|simple-expression
var → ID|ID[expression]
simple-expression → additive-expression relop additive-expression | additive-expression
relop → <=|<|>|>=|==|!=
additive-expression → additive-expression addop term|term
addop → +|-
term → term mulop factor|factor
mulop → *|/
factor → (expression)|var|call|NUM
call → ID(args)
args → arg-list|empty
arg-list → arg-list, expression|expression
```

### 拆解后
```
program  →  declaration-list
declaration-list  →  declaration-list declaration
declaration-list  → declaration
declaration  →  var-declaration
declaration  → fun-declaration
var-declaration  →  type-specifier ID;
var-declaration  → type-specifier ID[NUM];
type-specifier  →  int
type-specifier  → void
fun-declaration  →  type-specifier ID(params)
fun-declaration  → compound-stmt
params  →  param-list
params  → void
param-list  →  param-list,param
param-list  → param
param  →  type-specifier ID
param  → type-specifier ID[]
compound-stmt  →  {local-declarations statement-list}
local-declarations  →  local-declarations var-declaration
local-declarations  → empty
statement-list  →  statement-list statement
statement-list  → empty
statement  →  expression-stmt
statement  → compound-stmt
statement  → selection-stmt
statement  → iteration-stmt
statement  → return-stmt
expression-stmt  →  expression;
expression-stmt  → ;
selection-stmt  →  if(expression) statement 
selection-stmt  →  if(expression) statement else statement
iteration-stmt  →  while(expression) statement
return-stmt  →  return;
return-stmt  → return expression;
expression  →  var=expression
expression  → simple-expression
var  →  ID
var  → ID[expression]
simple-expression  →  additive-expression relop additive-expression 
simple-expression  →  additive-expression
relop  →  <=
relop  → <
relop  → >
relop  → >=
relop  → ==
relop  → !=
additive-expression  →  additive-expression addop term
additive-expression  → term
addop  →  +
addop  → -
term  →  term mulop factor
term  → factor
mulop  →  *
mulop  → /
factor  →  (expression)
factor  → var
factor  → call
factor  → NUM
call  →  ID(args)
args  →  arg-list
args  → empty
arg-list  →  arg-list, expression
arg-list  → expression
```
