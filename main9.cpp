#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <variant>
#include <fstream>
#include <cstring>
#include <cmath>
#include <memory> // Incluir a biblioteca para utilizar smart pointers

using std::cout;
using std::endl;
using std::string;
using std::vector;
using std::stringstream;
using std::shared_ptr; // Usar shared_ptr em vez de ponteiros brutos
using std::make_shared; // Função para criar shared_ptr
using std::dynamic_pointer_cast; // Função para realizar cast seguro de shared_ptr


// unary operators, namely unary plus (+) and unary minus (-) operators.

enum class TokenType
{
    TK_REAL   =0,
    TK_INTEGER ,
    TK_PLUS,
    TK_MINUS,
    TK_MUL,
    TK_DIV,
    TK_POWER,
    TK_LPAREN,
    TK_RPAREN,
    TK_ASSIGN,
    TK_SEMI,
    TK_DOT,
    TK_BEGIN,
    TK_END,
    TK_ID,
    TK_ERROR,
    TK_EOF
};

string tokeToString(TokenType type)
{
    switch (type)
    {
   case TokenType::TK_INTEGER:
        return "INT";
    case TokenType::TK_REAL:
        return "REAL";
    case TokenType::TK_PLUS:
        return "+";
    case TokenType::TK_MINUS:
        return "-";
    case TokenType::TK_MUL:
        return "*";
    case TokenType::TK_DIV:
        return "/";
    case TokenType::TK_POWER:
        return "^";
    case TokenType::TK_LPAREN:
        return "(";
    case TokenType::TK_RPAREN:
        return ")";
    case TokenType::TK_ASSIGN:
        return ":=";
    case TokenType::TK_SEMI:
        return ";";
    case TokenType::TK_DOT:
        return ".";
    case TokenType::TK_BEGIN:
        return "begin";
    case TokenType::TK_END:
        return "end";     
    case TokenType::TK_ERROR:
        return "ERROR";
    case TokenType::TK_EOF:
        return "EOF";
    default:
        return "UNKNOWN";
    }
}




struct Token
{
    TokenType type;
    int       character;
    std::variant<double, int, string> value;
    virtual string toString()
    { 
        stringstream ss; 
        ss << tokeToString(type) ; 

        return ss.str(); 
    }


    Token() = default;
    virtual ~Token() = default;

    template <typename T>
    bool isType()
    {
        return std::holds_alternative<T>(value);
    }

    template <typename T>
    T asType()
    {
        return std::get<T>(value);
    }

    template <typename T>
    auto Value()
    {
            return std::get<T>(value);
    }
    
    
};



class Parser;

class Lexer
{
public:
    Lexer() 
    {
        keywords["begin"]="begin";
        keywords["end"] ="end";
        keywords["for"] ="for";
        keywords["while"] ="while";
        keywords["and"] ="and";
        keywords["or"] ="or";
        keywords["not"] ="not";
        
        
        
    }

    ~Lexer()
    {
        tokens.clear();
        delete[] code; 
    }

    bool Run(const std::string& Text)
    {
        size = Text.size();
        if (size <= 0)
        {
            isPanic = true;
            return false;
        }
        code = new char[size + 1];
        code[size] = '\0';
        std::strcpy(code, Text.c_str());

        pos = 0;
        current = code[pos];

        return true;
    }

    bool Load(const std::string& file_name)
    {
        std::ifstream file(file_name);

        if (!file.is_open())
        {
            isPanic = true;
            std::cout << "Erro ao abrir o arquivo.\n";
            return false;
        }

        file.seekg(0, std::ios::end);
        size = file.tellg();
        file.seekg(0, std::ios::beg);
        code = new char[size + 1];
        file.read(code, size);
        code[size] = '\0';
        pos = 0;
        current = code[pos];
        file.close();
        return true;
    }

    void advance()
    {
        pos++;
        cursor++;
        if (pos > size - 1)
            current = '\0';
        else
            current = code[pos];
    }

    void skip_whitespace()
    {
        while (std::isspace(current) && current != '\0')
            advance();
    }

    double number()
    {
        std::string result = "";
        bool isReal = false;
        while (current != '\0' && isDigit(current) )
        {
            if (current == '.')
            {
                isReal = true;
            }
            result += current;
            advance();
        }
        if (isReal)
            return std::stod(result);
        else
            return (double)std::stoi(result);
    }
    shared_ptr<Token> get_next_token()
    {
        while (current != '\0')
        {
            if (current == '\n')
            {
                line++;
                cursor = 1;
            }
  
            if (std::isspace(current) )
            {
                skip_whitespace();
                continue;
            }
   
            if ( isDigit(current) && current != '.')
            {
                return new_token_number();
            }else
    
            if (std::isalpha(current) && !isEnd)
            {
                return new_token_id();
            }else
            if (current==':' && peek()=='=')
            {
                  advance();
                  advance(); 
                 return new_token(TokenType::TK_ASSIGN, (':'+'='));
            }else
            if (current == '+')
            {
                advance();
                return new_token(TokenType::TK_PLUS, '+');
            }else
            if (current == '-')
            {
                advance();
                return new_token(TokenType::TK_MINUS, '-');
            }else
            if (current == '*')
            {
                advance();
                return new_token(TokenType::TK_MUL, '*');
            }else
            if (current == '/')
            {
                advance();
                return new_token(TokenType::TK_DIV, '/');
            }else
            if (current == '^')
            {
                advance();
                return new_token(TokenType::TK_POWER, '^');
            }else
            if (current == '(')
            {
                advance();
                return new_token(TokenType::TK_LPAREN, '(');
            }else
            if (current == ')')
            {
                advance();
                return new_token(TokenType::TK_RPAREN, ')');
            }else
            if (current == ';')
            { 
                advance();
                return new_token(TokenType::TK_SEMI, ';');
            }else
            if (current == '.')
            {

                advance();
                isEnd = true;
                return new_token(TokenType::TK_DOT, '.');
            }else
            {   
                isPanic = true;
                cout<<"unkow char  (" <<current<<") \n";
                char last = current;
                advance();
                return new_token(TokenType::TK_ERROR, last);   
            }
        }

        return new_token(TokenType::TK_EOF, 0);
    }
    int get_line() { return line; }
    int get_cursor() { return cursor; }

    bool Compile()
    {
        while (current != '\0')
        {
            shared_ptr<Token> t = get_next_token();
            if (t->type == TokenType::TK_ERROR)
            {
                isPanic = true;
                return false;
            }
        }
        return true;
    }
private:
    bool isDigit(char c)
    {
        return std::isdigit(c) || c == '.';
    }
    char peek ()
    {
        peek_pos = pos + 1;
        if (peek_pos > size-1)
            return '\0';
        else
            return code[peek_pos];
    }
    char last ()
    {
        int peek_pos = pos - 1;
        if (peek_pos<=0)
            peek_pos=0;

        return code[peek_pos];
    }
    shared_ptr<Token> new_token(TokenType type, int value)
    {
        shared_ptr<Token> t = make_shared<Token>();
        t->type = type;
        t->character = value;
        tokens.push_back(t);
        return t;
    }
    shared_ptr<Token> new_token_id()
    {
        std::string result = "";
        while (current != '\0' && std::isalpha(current))
        {
            result += std::tolower(current);
            advance();
        } 
          
            
        shared_ptr<Token> t = make_shared<Token>();
        auto iter =keywords.find(result);
        if (iter != keywords.end()) 
        {
           // cout<<"KEY  "<<result<<endl;
            if(result.compare("begin")==0)
                t->type = TokenType::TK_BEGIN;
            else if(result.compare("end")==0)
                t->type = TokenType::TK_END;
                
        }else
        {
            // cout<<"VARIABLE:  "<<result<<endl;
            t->type = TokenType::TK_ID;
        }
        t->value = result;
        tokens.push_back(t);
        return t;
    }
    shared_ptr<Token> new_token(TokenType type, int c, double value)
    {
        shared_ptr<Token> t = make_shared<Token>();
        t->type = type;
        t->character = c;
        t->value = value;
        tokens.push_back(t);
        return t;
    }

    shared_ptr<Token> new_token_number()
    {
        std::string result = "";
        bool isReal = false;
        int point=0;
        while (current != '\0' && isDigit(current))
        {
     
            if (current == '.')
            {
                point++;
                if (point>1)
                {
                    cout<<" error: invalid suffix '"<<std::to_string(point)<<"' on floating constant\n";
                    return new_token(TokenType::TK_ERROR, current);   
                }
                isReal = true;
                
            }
            result += current;
            advance();
        } 
      //  cout<<"Numero:"<<result<<endl;
        shared_ptr<Token> t = make_shared<Token>();

        if (isReal)
        {
            double value  = std::stod(result);
            t->type = TokenType::TK_REAL;
            t->value = value;
        }
        else
        {
            int value = std::stoi(result);
            t->type = TokenType::TK_INTEGER;
            t->value = value;
        }


        
        
        tokens.push_back(t);
        return t;
    }


private:
    friend class Parser;
    char* code{ nullptr };
    int size{ 0 };
    int line{ 1 };
    int cursor{ 1 };

    bool isPanic{ false };
    bool isEnd{false};
    int pos{ 0 };
    int peek_pos{0};
    char current{ '\0' };
    vector<shared_ptr<Token>> tokens;
    std::unordered_map<string, string> keywords;
};

class AST
{
public:
    virtual ~AST() = default;
    virtual string toString() =0;
};

class BinOp : public AST
{
public:
    shared_ptr<AST> left;
    shared_ptr<Token> op;
    shared_ptr<AST> right;
    BinOp(shared_ptr<AST> left, shared_ptr<Token> op, shared_ptr<AST> right)
        : left(left), op(op), right(right)
    {
    }

    virtual ~BinOp() = default;
    virtual string toString() { stringstream ss; ss << "(" << op->toString() << ",  " << left->toString() << " , " << right->toString()<<")"; return ss.str(); }
};

class UnaryOp: public AST
{   
public:
    shared_ptr<Token> op;
    shared_ptr<AST> expr;
    UnaryOp(shared_ptr<Token> op, shared_ptr<AST> expr)
        : op(op), expr(expr)
    {
    }

    virtual ~UnaryOp() = default;
    virtual string toString() { stringstream ss; ss << "(" << op->toString() << ",  " << expr->toString() << ")"; return ss.str(); }
};

class Number : public AST
{
public:
    shared_ptr<Token> token;
    double value;

    Number(shared_ptr<Token> token)
        : token(token)
    {
        if (token->type == TokenType::TK_INTEGER)
            value = token->Value<int>();
        else
            value = token->Value<double>();
    }

  
    virtual string toString() { stringstream ss; ss << "INT:" << value; return ss.str(); }
};

class Var: public AST
{

    public:
    shared_ptr<Token> token;
    string value;
    Var(){}
 
    virtual string toString() { stringstream ss; ss << "VAR(" << value  << ")"; return ss.str(); }
};



class Assign : public AST
{
public:
    shared_ptr<Var> left;
    shared_ptr<Token> op;
    shared_ptr<AST> right;
    Assign(shared_ptr<Var> left, shared_ptr<Token> op, shared_ptr<AST> right)
        : left(left), op(op), right(right)
    {
        
    }

    virtual string toString() { stringstream ss; ss << "(" << op->toString() << ",  " << left->toString() << " , " << right->toString()<<")"; return ss.str(); }
};

class Compound : public AST
{
public:
    vector<shared_ptr<AST>> children;
    Compound() 
    {
    }

    virtual string toString() { stringstream ss; ss << "(Compond)"; return ss.str(); }
};

class NoOp : public AST
{
public:
 
    virtual string toString() { stringstream ss; ss << "( NoOp)"; return ss.str(); }
};

class Parser
{
public:
    Parser(Lexer* lexer)
        : lexer(lexer)
    {
        tokens = lexer->tokens;
        advance();
    }

    ~Parser()
    {
    }

    void eat(TokenType token_type)
    {
        TokenType type =current_token->type;
        if (current_token->type == token_type)
            advance();
        else
            throw std::invalid_argument("Parser Error: Expected '"+tokeToString(token_type)+"' Get '"+tokeToString(type)+"'");

    }

    bool match(TokenType token_type)
    {
        TokenType type =current_token->type;
        if (type == token_type)
        {
            advance();
            return true;
        }
        return false;

    }

    shared_ptr<AST> factor()
    {
        shared_ptr<Token> token = current_token;
        if (token->type == TokenType::TK_PLUS)
        {
            eat(TokenType::TK_PLUS);
            shared_ptr<UnaryOp> n = make_shared<UnaryOp>(token,factor());
            return n;
        }else
        if (token->type == TokenType::TK_MINUS)
        {
            eat(TokenType::TK_MINUS);
            shared_ptr<UnaryOp> n = make_shared<UnaryOp>(token,factor());
            return n;

        }else
        if (token->type == TokenType::TK_REAL)
        {
            eat(TokenType::TK_REAL);
            shared_ptr<Number> n = make_shared<Number>(token);
            return n;
        }else   if (token->type == TokenType::TK_INTEGER)
        {
            eat(TokenType::TK_INTEGER);
            shared_ptr<Number> n = make_shared<Number>(token);
            return n;
        }
        else if (token->type == TokenType::TK_LPAREN)
        {
            eat(TokenType::TK_LPAREN);
            shared_ptr<AST> result = expr();
            eat(TokenType::TK_RPAREN);
            return result;
         }
        else
        {
            shared_ptr<AST> n = variable();
            return n;
        }

 
        return nullptr;
    }

    shared_ptr<AST> term()
    {
        shared_ptr<AST> node = factor();

        while (current_token->type == TokenType::TK_MUL || current_token->type == TokenType::TK_DIV || current_token->type == TokenType::TK_POWER)
        {
            shared_ptr<Token> token = current_token;
            if (token->type == TokenType::TK_MUL)
            {
                eat(TokenType::TK_MUL);
                shared_ptr<BinOp> bin_op = make_shared<BinOp>(node, token, factor());
                node = bin_op;
            }
            else if (token->type == TokenType::TK_DIV)
            {
                eat(TokenType::TK_DIV);
                shared_ptr<BinOp> bin_op = make_shared<BinOp>(node, token, factor());
                node = bin_op;
            }
            else if (token->type == TokenType::TK_POWER)
            {
                eat(TokenType::TK_POWER);
                shared_ptr<BinOp> bin_op = make_shared<BinOp>(node, token, factor());
                node = bin_op;
            }
        }

        return node;
    }

    shared_ptr<AST> expr()
    {
        shared_ptr<AST> node = term();

        while (current_token->type == TokenType::TK_PLUS || current_token->type == TokenType::TK_MINUS)
        {
            shared_ptr<Token> token = current_token;
            if (token->type == TokenType::TK_PLUS)
            {
                eat(TokenType::TK_PLUS);
                shared_ptr<BinOp> bin_op = make_shared<BinOp>(node, token, term());
                node = bin_op;
            }
            else if (token->type == TokenType::TK_MINUS)
            {
                eat(TokenType::TK_MINUS);
                shared_ptr<BinOp> bin_op = make_shared<BinOp>(node, token, term());
                node = bin_op;
            } else 
            {
                cout << " Error: expr Invalid syntax" << endl;
            }
        }

        return node;
    }

    shared_ptr<AST> parse()
    {
        //shared_ptr<AST> node = expr();
        shared_ptr<AST> node = program();
        if (current_token->type == TokenType::TK_ERROR && current_token->type != TokenType::TK_EOF)
            return nullptr;

        return node;
    }



    shared_ptr<AST> program()
    {
        shared_ptr<AST> node = compound_statement();
        if (!match(TokenType::TK_DOT))
        {
            throw std::invalid_argument("Error : programe end with '.'  ");

        }
        return node;
    } 

    vector<shared_ptr<AST>> statement_list()
    {
        
        shared_ptr<AST> node =  statement();
        vector<shared_ptr<AST>> results;
        results.push_back(node);
        while(current_token->type==TokenType::TK_SEMI)
        {
            eat(TokenType::TK_SEMI);
            results.push_back(statement());
        }

        if (current_token->type==TokenType::TK_ID )
        {
            throw std::invalid_argument("Error : statement_list ");

        }


        return results;
    }

    shared_ptr<AST> compound_statement()
    {
        eat(TokenType::TK_BEGIN);
        vector<shared_ptr<AST>> nodes= statement_list();
        eat(TokenType::TK_END);

        shared_ptr<Compound> root =  make_shared<Compound>();
        for (auto node : nodes)
             root->children.push_back(node);

        return root;
    }

    

    shared_ptr<AST> statement()
    {
        shared_ptr<AST> node=nullptr;
        if (current_token->type==TokenType::TK_BEGIN)
        {

          node = compound_statement();
        }else
        if (current_token->type==TokenType::TK_ID  )
        { 
            node = assignment_statement();
        } else
        {
           // cout<<"empy "<<tokeToString(current_token->type)<<endl;
            node = empty();
        }
        return node;
    }
    shared_ptr<AST> assignment_statement()
    {
        
        shared_ptr<Var> left =variable();
        shared_ptr<Token> token = current_token;
        eat(TokenType::TK_ASSIGN);
        shared_ptr<AST> right = expr();
        shared_ptr<Assign> node  =  make_shared<Assign>(left,token,right);
        return node;
    }

    shared_ptr<Var> variable()
    {
    
        shared_ptr<Var> node =  make_shared<Var>();
        node->token = current_token;
        node->value = current_token->Value<string>();
        eat(TokenType::TK_ID);
        return node;
    }
    shared_ptr<AST> empty()
    {
        shared_ptr<NoOp> node =  make_shared<NoOp>();
        return node;
    }

     shared_ptr<Token> advance()
    {
        idx++;
        if (idx < (int)tokens.size())
        {
            current_token = tokens.at(idx);
            return current_token;
        }
        else
            return nullptr;
    
    }


    int Lineno() { return lexer->get_line(); }
private:
    Lexer* lexer;
    shared_ptr<Token> current_token;
    int idx{-1};
    vector<shared_ptr<Token>> tokens;
   
};

class Interpreter
{
public:
    Interpreter()=default;

    double visit(shared_ptr<AST> node)
    {
        
        if (auto bin_op = dynamic_pointer_cast<BinOp>(node))
        {
          //  cout<<node.get()->toString();
            return visit_BinOp(bin_op);
        }
        else if (auto num = dynamic_pointer_cast<Number>(node))
        {
          //  cout<<node.get()->toString();
            return visit_number(num);
        }
         else if (auto num = dynamic_pointer_cast<UnaryOp>(node))
        {
          //  cout<<node.get()->toString();
            return visit_UnaryOp(num);
        }
        else if (auto num = dynamic_pointer_cast<Compound>(node))
        {
          //  cout<<node.get()->toString();
             return visit_Compound(num);
        }
         else if (auto num = dynamic_pointer_cast<Assign>(node))
        {
          //  cout<<node.get()->toString();
             return visit_Assign(num);
        }
         else if (auto num = dynamic_pointer_cast<NoOp>(node))
        {
          //  cout<<node.get()->toString();
             visit_NoOp(num);
        }
         else if (auto num = dynamic_pointer_cast<Var>(node))
        {
          //  cout<<node.get()->toString();
             visit_Var(num);
             return 0;
        }
        else 
        {
             
            cout<<"Unknow Node in Visit "<<node.get()->toString();
        }
            return 0;
    }

    double visit_UnaryOp(shared_ptr<UnaryOp> node)
    {
        if (node.get()->op->type==TokenType::TK_PLUS)
        {
            return +visit(node.get()->expr);
        } else
        if (node.get()->op->type==TokenType::TK_MINUS)
        {
            return -visit(node.get()->expr);
        } else
        {
            cout<<"unkown visit_UnaryOp\n";
            return -1;
        }

    }

    double visit_BinOp(shared_ptr<BinOp> node)
    {
        if (node->op->type == TokenType::TK_PLUS)
            return visit(node->left) + visit(node->right);
        else if (node->op->type == TokenType::TK_MINUS)
            return visit(node->left) - visit(node->right);
        else if (node->op->type == TokenType::TK_MUL)
            return visit(node->left) * visit(node->right);
        else if (node->op->type == TokenType::TK_DIV)
            return visit(node->left) / visit(node->right);
        else if (node->op->type == TokenType::TK_POWER)
            return pow(visit(node->left), visit(node->right));
        return 0;
    }

    double visit_Compound(shared_ptr<Compound> node)
    {
            double result=0;
            for (auto &cmp : node->children)
            {
                result+=visit(cmp);
            }
            return result;
    } 
     
    double visit_number(shared_ptr<Number> node)
    {
        return node->value;
    }
    
    double visit_Assign(shared_ptr<Assign> node)
    {
        
        string var_name = node->left->value;
        GLOBAL_SCOPE[var_name] = visit(node->right);
        return GLOBAL_SCOPE[var_name];
      // cout<<"assigne  :"<<var_name<<"\n";
    }
     double visit_Var(shared_ptr<Var> node)
    {
         (void)node;
      
         string var_name = node->value;
         double result = GLOBAL_SCOPE[var_name];
         return result;
       // cout<<"Variable: "<<var_name<< " Result: "<<result<<endl;

         
        // GLOBAL_SCOPE[var_name] = node;
    } 

     void visit_NoOp(shared_ptr<NoOp> node)
    {
        (void)node;
        //faz nada
    }

    double interpret(shared_ptr<AST> tree)
    {
        if (tree == nullptr)
        {
            return 0;
        }
        return visit(tree);
    } 
    void Trace()
    { 
        for (auto &value: GLOBAL_SCOPE)
        {
            cout<<"Var ("<<value.first<< ") Value: ("<< value.second<<")"<<endl;
        }
    }
private:
  
    std::unordered_map<string,double> GLOBAL_SCOPE;

};

int main()
{
    
    Lexer lexer;
    
    //lexer.Run("3 ^ 2 ^ ");
     lexer.Load("/nfs/homes/lrosa-do/code/project/int42/main.v");
    
    try 
    {
         if (lexer.Compile())
        {

               Parser parser(&lexer);
            
              shared_ptr<AST> tree = parser.parse();
            if (tree != nullptr)
            {                    
               Interpreter interpreter;
               interpreter.interpret(tree);
               interpreter.Trace();
            } else
            {
                cout << "Error: (main) tree is null" << endl;
            }
         
        } else 
        {
            cout <<"Error in parser \n";
        }  
          
       
    }
    catch(const std::exception& e)
    {
        std::cerr <<"Exeption; "<< e.what() << '\n';
        return 0;
    }
    

    
    

    return 0;
}
