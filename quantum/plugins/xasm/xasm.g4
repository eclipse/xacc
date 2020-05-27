
grammar xasm;

/* This part of the grammar is particular to XACC */
/**********************************************************************/
xaccsrc
   : xacckernel
   | xacclambda
   ;

    xacckernel : '__qpu__' 'void' kernelname =
                    id '(' typedparam (',' typedparam) * ')' '{' mainprog? '}';

    xacclambda : '['('&'|'=')?']' '(' typedparam (',' typedparam) * ')' '{' mainprog? '}';

    typedparam : type ('&' | '*')? variable_param_name;
    variable_param_name : id;

    type : 'auto' | 'int' | id ('<' id (',' id)? '>')? | id'::'id ('<' id (',' id)? '>')? | 'std::shared_ptr<' type '>';

/***********************************************************************/


/* The main program */
mainprog
   : program
   ;

/* The actual program statements */
program
   : (line)+
   ;

line
   : statement+
   | comment
   ;

/* A program statement */
statement
   : instruction ';'
   | composite_generator ';'
   | forstmt
   | ifstmt
   | 'return' ';'
   ;

/* A program comment */
comment
   : COMMENT
   ;

forstmt
   :
   'for' '('
        ('int'|'auto') varname=id '=' start=INT ';'
        id comparator=('<' | '>' | '<=' | '>=') end=INT ';'
        id inc_or_dec=('++'|'--')  ')'
        '{' forScope=program '}'
   ;

ifstmt
   :
   'if' '(' buffer_name=id '[' idx=INT ']' ')' '{' program '}'
   ;

instruction
   : inst_name=id '(' buffer_list=bufferList (',' param_list=paramList)? ')'
   ;

bufferList
   : bufferIndex (',' bufferIndex)?
   ;

paramList
   : parameter (',' parameter)*
   ;

parameter
   : exp
   ;

composite_generator
   : composite_name=id '(' buffer_name=id
                        (',' composite_params=paramList)?
                        (',' composite_options=optionsMap)? ')'
   ;

bufferIndex
   : buffer_name=id ('[' idx=exp ']')
   ;

optionsMap
   : '{' optionsType (',' optionsType)* '}'
   ;

optionsType
   : '{' key=string ',' value=exp '}'
   ;

/* A list of expressions */
explist
   : exp ( ',' exp )*
   ;

/* An expression */
exp
   : id
   | exp '+' exp
   | exp '-' exp
   | exp '*' exp
   | exp '/' exp
   | '-'exp
   | exp '^' exp
   | '(' exp ')'
   | unaryop '(' exp ')'
   | string
   | real
   | INT
   | 'pi'
   | var_name=id '[' idx=(INT|ID) ']'
   ;

/* unary operations */
unaryop
   : 'sin'
   | 'cos'
   | 'tan'
   | 'exp'
   | 'ln'
   | 'sqrt'
   ;

/* variable identity */
id
   : ID
   ;

/* real numbers */
real
   : REAL
   ;

/* strings are enclosed in quotes */
string
   : STRING
   ;

/* Tokens for the grammer */

/* Comment */
COMMENT
   : '//' ~ [\r\n]* EOL
   ;

/* id, ego, and super-ego */
ID
   : [a-z][A-Za-z0-9_]*
   | [A-Z][A-Za-z0-9_]*
   | [A-Z][A-Za-z]*
   ;

/* Keep it real...numbers */
REAL
   : INT? ( '.' (INT) )
   ;

/* Non-negative integers */
INT
   : ('0'..'9')+
   ;

/* Strings include numbers and slashes */
STRING
   : '"' ~ ["]* '"'
   ;

/* Whitespaces, we skip'em */
WS
   : [ \t\r\n] -> skip
   ;

/* This is the end of the line, boys */
EOL
: '\r'? '\n'
;
