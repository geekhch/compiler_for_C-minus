#ifndef SYNTAX_PARSER_REC_H
#define SYNTAX_PARSER_REC_H
#include <iostream>
#include <vector>
#include <string>
#include <iterator>
#include <fstream>
#include "../lexical/LexParser.hpp"
#include "../utils.hpp"
using namespace std;

class SynParser
{
public:
    SynParser(LexParser &lex_) : lex(lex_)
    {
        root = new SynNode{"program"};
        curNode = root;
        declaration_list(root);
        exception_line = 0;
        step = 1;
    }
    ~SynParser()
    {
        printTree(root, 0);
    }

private:
    SynNode *root;
    SynNode *curNode;
    LexParser &lex;
    int exception_line;
    int step;
    bool match(string terminal, SynNode *parent); //匹配终结符
    void freeTree(SynNode *);
    void freeChildren(SynNode *);
    void printTree(SynNode *root, int indent);

    //非终结符匹配
    void declaration_list(SynNode *parent);
    void declaration(SynNode *parent);
    void var_declaration(SynNode *parent);
    void type_specifier(SynNode *parent);
    void fun_declaration(SynNode *parent);
    void params(SynNode *parent);
    void param_list(SynNode *parent);
    void param(SynNode *parent);
    void compound_stmt(SynNode *parent);
    void local_declarations(SynNode *parent);
    void statement_list(SynNode *parent);
    void statement(SynNode *parent);
    void expression_stmt(SynNode *parent);
    void selection_stmt(SynNode *parent);
    void iteration_stmt(SynNode *parent);
    void return_stmt(SynNode *parent);
    void expression(SynNode *parent);
    void var(SynNode *parent);
    void simple_expression(SynNode *parent);
    void relop(SynNode *parent);
    void additive_expression(SynNode *parent);
    void addop(SynNode *parent);
    void term(SynNode *parent);
    void mulop(SynNode *parent);
    void factor(SynNode *parent);
    void call(SynNode *parent);
    void args(SynNode *parent);
    void arg_list(SynNode *parent);
};

#endif

void SynParser::declaration_list(SynNode *parent)
{
    SynNode *cur = new SynNode{"declaration_list"};
    try
    {
        declaration(cur);
    }
    catch (runtime_error e)
    {
        freeTree(cur);
        throw e;
    }

    while (true)
    {
        uint32_t mk = lex.getMarker();
        try
        {
            declaration(cur);
            cout << "=====end========" << endl;
        }
        catch (runtime_error e)
        {
            if (lex.isEnd())
            {
                //没有更多申明
                break;
            }
            else
            {
                throw e;
            }
        }
    }
    parent->children.push_back(cur);
}

void SynParser::declaration(SynNode *parent)
{
    uint32_t mk = lex.getMarker(); //标记当前词法扫描器状态
    SynNode *cur = new SynNode{"declaration"};
    try
    {   //是否为变量声明
        var_declaration(cur);
    }
    catch (runtime_error e1)
    {   //不是变量声明则回到词法扫描器之前状态
        cur->children.clear();
        lex.toMarker(mk);
        try
        {//是否为函数声明
            fun_declaration(cur);
        }
        catch (runtime_error e2)
        {//语法错误
            delete cur;
            cout <<"near line "<<exception_line<<": "<< e1.what() << " or " << e2.what() << endl;
            throw runtime_error("illegel declaration");
            // throw e;
        }
    }
    //找到申明则将当前结点插入语法树
    parent->children.push_back(cur);
}

void SynParser::var_declaration(SynNode *parent)
{
    SynNode *cur = new SynNode{"var_declaration"};
    try
    {
        type_specifier(cur);
    }
    catch (runtime_error e)
    {
        freeTree(cur);
        throw e;
    }

    if (!match("ID", cur))
    {
        freeTree(cur);
        throw runtime_error("expected an ID");
    }

    uint32_t mk = lex.getMarker();
    if (!match(";", cur))
    {
        lex.toMarker(mk);
        if (!(match("[", cur) && match("NUM", cur) && match("]", cur) && match(";", cur)))
        {
            //匹配失败回退
            lex.toMarker(mk);
            freeTree(cur);
            throw runtime_error("expected declaration format type id[NUM]");
        }
    }
    parent->children.push_back(cur);
}

void SynParser::type_specifier(SynNode *parent)
{
    SynNode *cur = new SynNode{"type_specifier"};
    uint32_t mk = lex.getMarker();
    if (!match("int", cur))
    {
        lex.toMarker(mk);
        if (!match("void", cur))
        { //匹配失败
            freeTree(cur);
            throw runtime_error("expected type_specifier: int or void");
        }
    }
    //匹配成功
    parent->children.push_back(cur);
}

void SynParser::fun_declaration(SynNode *parent)
{
    SynNode *cur = new SynNode{"fun_declaration"};
    try
    {
        type_specifier(cur);
    }
    catch (runtime_error e)
    {
        freeTree(cur);
        throw e;
    }
    if (!(match("ID", cur) && match("(", cur)))
    {
        freeTree(cur);
        throw runtime_error("expected an ID");
    }

    try
    {
        params(cur);
    }
    catch (runtime_error e)
    {
        freeTree(cur);
        throw e;
    }

    if (!match(")", cur))
    {
        freeTree(cur);
        cout << "expected a ')'" <<endl;
        throw runtime_error("expected a ')'");
    }
    try
    {
        compound_stmt(cur);
    }
    catch (runtime_error e)
    {
        freeTree(cur);
        throw e;
    }
    parent->children.push_back(cur);
}

void SynParser::params(SynNode *parent)
{
    SynNode *cur = new SynNode{"params"};
    uint32_t mk = lex.getMarker();
    if (match("void", cur))
    {
        parent->children.push_back(cur);
        return;
    }
    else
    {
        lex.toMarker(mk);
    }
    try
    {
        param_list(cur);
    }
    catch (runtime_error e)
    {
        freeTree(cur);
        throw e;
    }
    parent->children.push_back(cur);
}

void SynParser::param_list(SynNode *parent)
{
    SynNode *cur = new SynNode{"param_list"};
    try
    {
        param(cur);
    }
    catch (runtime_error e)
    {
        freeTree(cur);
        throw e;
    }

    while (true)
    {
        uint32_t mk = lex.getMarker();
        if (!match(",", cur))
        {
            lex.toMarker(mk);
            break;
        }
        try
        {
            param(cur);
        }
        catch (runtime_error e)
        {
            lex.toMarker(mk);
            cur->children.pop_back();
            break;
        }
    }
    parent->children.push_back(cur);
}

void SynParser::param(SynNode *parent)
{
    SynNode *cur = new SynNode{"param"};
    try
    {
        type_specifier(cur);
    }
    catch (runtime_error e)
    {
        freeTree(cur);
        throw e;
    }
    if (!match("ID", cur))
    {
        freeTree(cur);
        throw runtime_error("expected an ID in param");
    }

    //尝试匹配下标符
    uint32_t mk = lex.getMarker();
    int num_child = cur->children.size();
    if (!(match("[", cur) && match("]", cur)))
    {
        lex.toMarker(mk);
        while (num_child < (cur->children.size()))
        {
            cur->children.pop_back();
        }
    }

    parent->children.push_back(cur);
}

void SynParser::compound_stmt(SynNode *parent)
{
    SynNode *cur = new SynNode{"compound_stmt"};
    if (!match("{", cur))
    {
        freeTree(cur);
        throw runtime_error("expected a '{'");
    }

    try
    {
        local_declarations(cur);
        statement_list(cur);
    }
    catch (runtime_error e)
    {
        freeTree(cur);
        throw e;
    }
    if (!match("}", cur))
    {
        freeTree(cur);
        throw runtime_error("expected a '}'");
    }
    parent->children.push_back(cur);
}
void SynParser::local_declarations(SynNode *parent)
{
    SynNode *cur = new SynNode{"local_declarations"};
    while (true)
    {
        uint32_t mk = lex.getMarker();
        try
        {
            var_declaration(cur);
        }
        catch (runtime_error e)
        {
            lex.toMarker(mk);
            break;
        }
    }
    if (!cur->children.empty())
        parent->children.push_back(cur);
}
void SynParser::statement_list(SynNode *parent)
{
    SynNode *cur = new SynNode{"statement_list"};
    while (true)
    {
        uint32_t mk = lex.getMarker();
        try
        {
            statement(cur);
        }
        catch (runtime_error e)
        {
            lex.toMarker(mk);
            break;
        }
    }
    if (!cur->children.empty())
        parent->children.push_back(cur);
}
void SynParser::statement(SynNode *parent)
{
    SynNode *cur = new SynNode{"statement"};
    uint32_t mk = lex.getMarker();
    try
    {
        expression_stmt(cur);
    }
    catch (runtime_error e1)
    {   
        cur->children.clear();
        lex.toMarker(mk);
        try
        {
            compound_stmt(cur);
        }
        catch (runtime_error e2)
        {
            cur->children.clear();
            lex.toMarker(mk);
            try
            {
                selection_stmt(cur);
            }
            catch (runtime_error e3)
            {
                cur->children.clear();
                lex.toMarker(mk);
                try
                {
                    iteration_stmt(cur);
                }
                catch (runtime_error e4)
                {
                    cur->children.clear();
                    lex.toMarker(mk);
                    try
                    {
                        return_stmt(cur);
                    }
                    catch (runtime_error e5)
                    {
                        cur->children.clear();
                        lex.toMarker(mk);
                        throw runtime_error("illegel statment");
                    }
                }
            }
        }
    }

    parent->children.push_back(cur);
}
void SynParser::expression_stmt(SynNode *parent)
{
    SynNode *cur = new SynNode{"expression_stmt"};
    uint32_t mk = lex.getMarker();

    if (!match(";", cur))
    {
        lex.toMarker(mk);
        try
        {
            expression(cur);
        }
        catch (runtime_error e)
        {
            freeTree(cur);
            throw e;
        }
        if (!match(";", cur))
        {
            freeTree(cur);
            throw runtime_error("expected a ';'");
        }
    }
    parent->children.push_back(cur);
}
void SynParser::selection_stmt(SynNode *parent)
{
    SynNode *cur = new SynNode{"selection_stmt"};
    if (!(match("if", cur) && match("(", cur)))
    {
        freeTree(cur);
        throw runtime_error("illegel selection_stmt, expected 'if('");
    }
    try
    {
        expression(cur);
    }
    catch (runtime_error e)
    {
        freeTree(cur);
        throw e;
    }
    if (!match(")", cur))
    {
        freeTree(cur);
        throw runtime_error("illegel selection_stmt, expected ')'");
    }
    try
    {
        statement(cur);
    }
    catch (runtime_error e)
    {
        freeTree(cur);
        throw e;
    }
    uint32_t mk = lex.getMarker();
    if (match("else", cur))
    {
        try
        {
            statement(cur);
        }
        catch (runtime_error e)
        {
            freeTree(cur);
            throw e;
        }
    }
    else
    {
        lex.toMarker(mk);
    }
    parent->children.push_back(cur);
}
void SynParser::iteration_stmt(SynNode *parent)
{
    SynNode *cur = new SynNode{"iteration_stmt"};
    if (!(match("while", cur) && match("(", cur)))
    {
        freeTree(cur);
        throw runtime_error("illegel iteration_stmt, expected 'while('");
    }
    try
    {
        expression(cur);
    }
    catch (runtime_error e)
    {
        freeTree(cur);
        throw e;
    }
    if (!match(")", cur))
    {
        freeTree(cur);
        throw runtime_error("illegel selection_stmt, expected ')'");
    }
    try
    {
        statement(cur);
    }
    catch (runtime_error e)
    {
        freeTree(cur);
        throw e;
    }
    parent->children.push_back(cur);
}
void SynParser::return_stmt(SynNode *parent)
{
    SynNode *cur = new SynNode{"return_stmt"};
    if (!match("return", cur))
    {
        freeTree(cur);
        throw runtime_error("illegel return_stmt, expected 'return'");
    }
    uint32_t mk = lex.getMarker();
    if (!match(";", cur))
    {
        lex.toMarker(mk);
        try
        {
            expression(cur);
        }
        catch (runtime_error e)
        {
            freeTree(cur);
            throw e;
        }
        if (!match(";", cur))
        {
            freeTree(cur);
            throw runtime_error("expected ';'");
        }
    }

    parent->children.push_back(cur);
}
void SynParser::expression(SynNode *parent)
{
    SynNode *cur = new SynNode{"expression"};
    uint32_t mk = lex.getMarker();
    try
    {
        var(cur);
        if (!match("=", cur))
        {
            freeTree(cur);
            throw runtime_error("expected '='");
        }
        expression(cur);
    }
    catch (runtime_error e)
    {
        cur->children.clear();
        lex.toMarker(mk);
        simple_expression(cur);
    }
    parent->children.push_back(cur);
}
void SynParser::var(SynNode *parent)
{
    SynNode *cur = new SynNode{"var"};
    if (!match("ID", cur))
    {
        freeTree(cur);
        throw runtime_error("illegel return_stmt, expected 'return'");
    }

    uint32_t mk = lex.getMarker();
    if (match("[", cur))
    {
        try
        {
            expression(cur);
        }
        catch (runtime_error e)
        {
            lex.toMarker(mk);
            while (cur->children.size() > 1)
            {
                cur->children.pop_back();
            }
        }
        if (!match("]", cur))
        {
            lex.toMarker(mk);
            while (cur->children.size() > 1)
            {
                cur->children.pop_back();
            }
        }
    }
    else
    {
        lex.toMarker(mk);
    }
    parent->children.push_back(cur);
}
void SynParser::simple_expression(SynNode *parent)
{
    SynNode *cur = new SynNode{"simple_expression"};
    try
    {
        additive_expression(cur);
    }
    catch (runtime_error e)
    {
        freeTree(cur);
        throw e;
    }

    //选择条件，尝试匹配，失败则回退结点以及token列表
    uint32_t mk = lex.getMarker();
    int num_child = cur->children.size();
    try
    {
        relop(cur);
        additive_expression(cur);
    }
    catch (runtime_error e)
    {
        lex.toMarker(mk);
        while (cur->children.size() > num_child)
            cur->children.pop_back();
    }

    parent->children.push_back(cur);
}
void SynParser::relop(SynNode *parent)
{
    SynNode *cur = new SynNode{"relop"};
    uint32_t mk = lex.getMarker();

    if (!match("<=", cur))
    {
        lex.toMarker(mk);
        if (!match(">=", cur))
        {
            lex.toMarker(mk);
            if (!match("==", cur))
            {
                lex.toMarker(mk);
                if (!match("!=", cur))
                {
                    lex.toMarker(mk);
                    if (!match(">", cur))
                    {
                        lex.toMarker(mk);
                        if (!match("<", cur))
                        {
                            throw runtime_error("expected a relop");
                        }
                    }
                }
            }
        }
    }

    parent->children.push_back(cur);
}
void SynParser::additive_expression(SynNode *parent)
{
    SynNode *cur = new SynNode{"additive_expression"};
    uint32_t mk = lex.getMarker();
    try
    {
        term(cur);
    }
    catch (runtime_error e)
    {
        freeTree(cur);
        throw e;
    }

    while (true)
    {
        uint32_t mk = lex.getMarker();
        int num_child = cur->children.size();
        try
        {
            addop(cur);
            term(cur);
        }
        catch (runtime_error e)
        {
            lex.toMarker(mk);
            while (num_child < cur->children.size())
            {
                cur->children.pop_back();
            }
            break;
        }
    }
    parent->children.push_back(cur);
}
void SynParser::addop(SynNode *parent)
{
    SynNode *cur = new SynNode{"addop"};
    uint32_t mk = lex.getMarker();
    if (!match("+", cur))
    {
        lex.toMarker(mk);
        if (!match("-", cur))
        {
            throw runtime_error("expected a '+' or '-'");
        }
    }
    parent->children.push_back(cur);
}
void SynParser::term(SynNode *parent)
{
    SynNode *cur = new SynNode{"term"};
    try
    {
        factor(cur);
    }
    catch (runtime_error e)
    {
        freeTree(cur);
        throw e;
    }

    while (true)
    {
        uint32_t mk = lex.getMarker();
        int num_child = cur->children.size();
        try
        {
            mulop(cur);
            factor(cur);
        }
        catch (runtime_error e)
        {
            lex.toMarker(mk);
            while (num_child < cur->children.size())
            {
                cur->children.pop_back();
            }
            break;
        }
    }
    parent->children.push_back(cur);
}
void SynParser::mulop(SynNode *parent)
{
    SynNode *cur = new SynNode{"mulop"};
    uint32_t mk = lex.getMarker();
    if (!match("*", cur))
    {
        lex.toMarker(mk);
        if (!match("/", cur))
        {
            freeTree(cur);
            throw runtime_error("expected a '*' or '/'");
        }
    }
    parent->children.push_back(cur);
}
void SynParser::factor(SynNode *parent)
{
    SynNode *cur = new SynNode{"factor"};
    uint32_t mk = lex.getMarker();
    if (!match("NUM", cur))
    {
        lex.toMarker(mk);
        try
        {
            call(cur);
        }
        catch (runtime_error e1)
        {
            cur->children.clear();
            lex.toMarker(mk);
            try
            {
                var(cur);
            }
            catch (runtime_error e2)
            {
                cur->children.clear();
                lex.toMarker(mk);
                if (!match("(", cur))
                {
                    freeTree(cur);
                    throw runtime_error("illegel language, expected '('");
                }
                else
                {
                    try
                    {
                        expression(cur);
                    }
                    catch (runtime_error e3)
                    {
                        freeTree(cur);
                        throw e3;
                    }
                    if (!match("(", cur))
                    {
                        freeTree(cur);
                        throw runtime_error("illegel language, expected ')'");
                    }
                }
            }
        }
    }
    parent->children.push_back(cur);
}
void SynParser::call(SynNode *parent)
{
    SynNode *cur = new SynNode{"call"};
    if (!(match("ID", cur) && match("(", cur)))
    {
        freeTree(cur);
        throw runtime_error("illegel call");
    }
    try
    {
        args(cur);
    }
    catch (runtime_error e)
    {
        freeTree(cur);
        throw e;
    }
    if (!match(")", cur))
    {
        freeTree(cur);
        throw runtime_error("illegel call, expected ')'");
    }

    parent->children.push_back(cur);
}
void SynParser::args(SynNode *parent)
{
    SynNode *cur = new SynNode{"args"};
    uint32_t mk = lex.getMarker();
    try
    {
        arg_list(cur);
        parent->children.push_back(cur);
    }
    catch (runtime_error e)
    { //有empty,不抛出新异常
        lex.toMarker(mk);
        freeTree(cur);
    }
}
void SynParser::arg_list(SynNode *parent)
{
    SynNode *cur = new SynNode{"arg_list"};
    try
    {
        expression(cur);
    }
    catch (runtime_error e)
    {
        freeTree(cur);
        throw e;
    }

    while (true)
    {
        uint32_t mk = lex.getMarker();
        if (!match(",", cur))
        {
            lex.toMarker(mk);
            break;
        }
        try
        {
            expression(cur);
        }
        catch (runtime_error e)
        {
            lex.toMarker(mk);
            cur->children.pop_back();
            break;
        }
    }
    parent->children.push_back(cur);
}

void SynParser::freeTree(SynNode *root)
{
    if (root->children.empty())
        delete root;
    else
    {
        for (auto node : root->children)
        {
            freeTree(node);
        }
    }
}

bool SynParser::match(string terminal, SynNode *parent)
{
    try
    {
        Token tk = lex.nextToken();
        // cout << "step"<<step++<<": "<< tk.strfToken()<<endl;
        if (terminal == "ID" && tk.type == ID)
        {
            parent->children.push_back(new SynNode{tk.s_value, ID});
            return true;
        }
        else if (terminal == "NUM" && tk.type == INT)
        {
            parent->children.push_back(new SynNode{to_string(tk.i_value), INT});
            return true;
        }
        else if (terminal == tk.s_value)
        {
            parent->children.push_back(new SynNode{tk.s_value, tk.type});
            return true;
        }
        exception_line = tk.line;
        return false;
    }
    catch (runtime_error e)
    {
        //没有新的终结符
        throw e;
    }
}

void SynParser::freeChildren(SynNode *parent)
{
    while (!parent->children.empty())
    {
        auto cld = parent->children.back();
        parent->children.pop_back();
        freeTree(cld);
    }
}

void SynParser::printTree(SynNode *root, int indent = 0)
{
    for (int i = 0; i < indent; ++i) //缩进量
        cout << ' ';
    if(root->type!=-1)
        cout << TYPE_NAME[root->type] << ": ";
    cout << root->word << endl;
    for (auto cld : root->children)
    {
        printTree(cld, indent + 2);//递归实现孩子结点缩进量增加
    }
}