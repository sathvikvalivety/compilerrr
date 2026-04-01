#include "compiler.h"

ASTNode* parse_or();
ASTNode* parse_and();
ASTNode* parse_relational();
ASTNode* parse_additive();
ASTNode* parse_multiplicative();
ASTNode* parse_primary();

ASTNode* create_node(ASTNodeType type) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    node->node_type = type;
    node->left = NULL;
    node->right = NULL;
    node->expr_type = TYPE_UNKNOWN;
    return node;
}

void match(TokenType expected) {
    if (tokens[token_idx].type == expected) {
        token_idx++;
    } else {
        printf("Syntax Error: Expected token %d but found %d\n", expected, tokens[token_idx].type);
        exit(1);
    }
}

ASTNode* parse() {
    token_idx = 0;
    ASTNode* root = parse_or();
    if (tokens[token_idx].type != TOK_EOF) {
        printf("Syntax Error: Extra tokens at end of input.\n");
        exit(1);
    }
    printf("AST constructed successfully.\n");
    return root;
}

ASTNode* parse_or() {
    ASTNode* left = parse_and();
    while (tokens[token_idx].type == TOK_OR) {
        ASTNode* node = create_node(AST_BINOP); 
        node->op = TOK_OR;
        token_idx++;
        node->left = left;
        node->right = parse_and();
        printf("Parsing rule: expr -> expr || term\n");
        left = node;
    }
    return left;
}

ASTNode* parse_and() {
    ASTNode* left = parse_relational();
    while (tokens[token_idx].type == TOK_AND) {
        ASTNode* node = create_node(AST_BINOP);
        node->op = TOK_AND;
        token_idx++;
        node->left = left;
        node->right = parse_relational();
        printf("Parsing rule: expr -> expr && term\n");
        left = node;
    }
    return left;
}

ASTNode* parse_relational() {
    ASTNode* left = parse_additive();
    
    TokenType type = tokens[token_idx].type;
    if (type == TOK_GT || type == TOK_LT || type == TOK_GE || 
        type == TOK_LE || type == TOK_EQ) {
        
        ASTNode* node = create_node(AST_RELOP);
        node->op = type;
        token_idx++;
        
        node->left = left;
        node->right = parse_additive();
        char op_str[3] = "";
        if (node->op == TOK_GT) strcpy(op_str, ">");
        else if (node->op == TOK_LT) strcpy(op_str, "<");
        else if (node->op == TOK_GE) strcpy(op_str, ">=");
        else if (node->op == TOK_LE) strcpy(op_str, "<=");
        else if (node->op == TOK_EQ) strcpy(op_str, "==");
        printf("Parsing rule: expr -> expr %s term\n", op_str);
        return node;
    }
    return left;
}

ASTNode* parse_additive() {
    ASTNode* left = parse_multiplicative();
    
    while (tokens[token_idx].type == TOK_PLUS || tokens[token_idx].type == TOK_MINUS) {
        ASTNode* node = create_node(AST_BINOP);
        node->op = tokens[token_idx].type;
        token_idx++;
        
        node->left = left;
        node->right = parse_multiplicative();
        char op_char = (node->op == TOK_PLUS) ? '+' : '-';
        printf("Parsing rule: term -> term %c factor\n", op_char);
        left = node;
    }
    return left;
}

ASTNode* parse_multiplicative() {
    ASTNode* left = parse_primary();
    
    while (tokens[token_idx].type == TOK_MUL || tokens[token_idx].type == TOK_DIV) {
        ASTNode* node = create_node(AST_BINOP);
        node->op = tokens[token_idx].type;
        token_idx++;
        
        node->left = left;
        node->right = parse_primary();
        char op_char = (node->op == TOK_MUL) ? '*' : '/';
        printf("Parsing rule: factor -> factor %c primary\n", op_char);
        left = node;
    }
    return left;
}

ASTNode* parse_primary() {
    Token t = tokens[token_idx];
    if (t.type == TOK_ID) {
        ASTNode* node = create_node(AST_ID);
        strcpy(node->name, t.lexeme);
        token_idx++;
        printf("Parsing rule: primary -> ID (%s)\n", t.lexeme);
        return node;
    } else if (t.type == TOK_NUM) {
        ASTNode* node = create_node(AST_NUM);
        node->value = t.value;
        token_idx++;
        printf("Parsing rule: primary -> NUM (%d)\n", t.value);
        return node;
    } else if (t.type == TOK_LPAREN) {
        token_idx++;
        ASTNode* node = parse_or();
        match(TOK_RPAREN);
        printf("Parsing rule: primary -> ( expr )\n");
        return node;
    } else {
        printf("Syntax Error: Unexpected token %d in parsing primary.\n", t.type);
        exit(1);
    }
    return NULL;
}

void print_ast(ASTNode* root, int space) {
    if (!root) return;
    space += 5;
    print_ast(root->right, space);
    printf("\n");
    for (int i = 5; i < space; i++) printf(" ");
    
    if (root->node_type == AST_NUM) printf("%d\n", root->value);
    else if (root->node_type == AST_ID) printf("%s\n", root->name);
    else if (root->node_type == AST_BINOP || root->node_type == AST_RELOP) {
        if (root->op == TOK_PLUS) printf("+\n");
        else if (root->op == TOK_MINUS) printf("-\n");
        else if (root->op == TOK_MUL) printf("*\n");
        else if (root->op == TOK_DIV) printf("/\n");
        else if (root->op == TOK_GT) printf(">\n");
        else if (root->op == TOK_LT) printf("<\n");
        else if (root->op == TOK_GE) printf(">=\n");
        else if (root->op == TOK_LE) printf("<=\n");
        else if (root->op == TOK_EQ) printf("==\n");
        else if (root->op == TOK_AND) printf("&&\n");
        else if (root->op == TOK_OR) printf("||\n");
    }
    
    print_ast(root->left, space);
}
