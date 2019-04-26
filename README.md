# Complier
编译原理课程设计

`g++ run.cpp LexPaser.cpp -o a.exe`

## 词法分析器
### V 1.0
1. 完成词法分析的基本功能,将规定C-代码解析为tokens
2. 调用接口：`printTokenList()`、`nextToken()`
3. 实现算法：正则表达式 -> NFA -> 对应逻辑代码 (规定C-语法过于简单，后期迭代考虑实现更复杂语法解析)

```c
program → declaration-list
declaration-list → declaration-list declaration|declaration
declaration → var-declaration|fun-declaration
var-declaration → type-specifier ID;|type-specifier ID[NUM];
type-specifier → int|void
fun-declaration → type-specifier ID(params)|compound-stmt
params → params-list|void
param-list → param-list,param|param
param → type-specifier ID|type-specifier ID[]
compound-stmt → {local-declarations statement-list}
local-declarations → local-declarations var-declaration|empty
statement-list → statement-list statement|empty
statement → expression-stmt|compound-stmt|selection-stmt|
iteration-stmt|return-stmt
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