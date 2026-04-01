#include "compiler.h"

// Prototypes for recursive descent
ASTNode* parse_expr();
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
    ASTNode* root = parse_expr();
    if (tokens[token_idx].type != TOK_EOF) {
        printf("Syntax Error: Extra tokens at end of input.\n");
        exit(1);
    }
    return root;
}

ASTNode* parse_expr() {
    ASTNode* left = parse_additive();
    
    TokenType type = tokens[token_idx].type;
    if (type == TOK_GT || type == TOK_LT || type == TOK_GE || 
        type == TOK_LE || type == TOK_EQ) {
        
        ASTNode* node = create_node(AST_RELOP);
        node->op = type;
        token_idx++; // consume relop
        
        node->left = left;
        node->right = parse_additive();
        return node;
    }
    return left; // No relational operator
}

ASTNode* parse_additive() {
    ASTNode* left = parse_multiplicative();
    
    while (tokens[token_idx].type == TOK_PLUS || tokens[token_idx].type == TOK_MINUS) {
        ASTNode* node = create_node(AST_BINOP);
        node->op = tokens[token_idx].type;
        token_idx++;
        
        node->left = left;
        node->right = parse_multiplicative();
        left = node; // Left-associative
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
        left = node; // Left-associative
    }
    return left;
}

ASTNode* parse_primary() {
    Token t = tokens[token_idx];
    if (t.type == TOK_ID) {
        ASTNode* node = create_node(AST_ID);
        strcpy(node->name, t.lexeme);
        token_idx++;
        return node;
    } else if (t.type == TOK_NUM) {
        ASTNode* node = create_node(AST_NUM);
        node->value = t.value;
        token_idx++;
        return node;
    } else if (t.type == TOK_LPAREN) {
        token_idx++; // '('
        ASTNode* node = parse_expr();
        match(TOK_RPAREN); // ')'
        return node;
    } else {
        printf("Syntax Error: Unexpected token %d in parsing primary.\n", t.type);
        exit(1);
    }
    return NULL;
}
