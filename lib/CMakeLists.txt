add_library(itmoscript STATIC
    ast/nodes.cpp
    interpreter/interpreter.cpp
    lexer/lexer.cpp
    parser/parser.cpp
    symbol_table/SymbolTable.cpp
    tokens/tokens.cpp
)