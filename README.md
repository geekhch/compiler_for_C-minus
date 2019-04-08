# Complier
编译原理课程设计

`g++ run.cpp LexPaser.cpp -o a.exe`

## 词法分析器
### V 1.0
1. 完成词法分析的基本功能,将规定C-代码解析为tokens
2. 调用接口：`printTokenList()`、`nextToken()`
3. 实现算法：正则表达式 -> NFA -> 对应逻辑代码 (规定C-语法过于简单，后期迭代考虑实现更复杂语法解析)
