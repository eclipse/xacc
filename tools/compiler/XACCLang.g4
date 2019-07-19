
grammar XACCLang;

/* This part of the grammar is particular to XACC */
/**********************************************************************/
xaccsrc
   : xacckernel*
   ;

    xacckernel : '__qpu__' kernelname =
                    id '(' 'AcceleratorBuffer' acceleratorbuffer =
                        id(',' typedparam) * ')' '{' mainprog '}';

    typedparam : type param;

    type : 'int' | 'double' | 'float';

    kernelcall : kernelname = id '(' param ? (',' param) * ')';

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
   : uop ';'
   | 'return' ';'
   ;

/* A program comment */
comment
   : COMMENT
   ;

/* A list of parameters */
paramlist
   : param (',' paramlist)?
   ;

/* A parameter */
param
   : id
   | '*' id
   ;

/* A unitary operation */
uop ';'
   : gatename=gate  '(' ( explist )? ')'
   | allbitsOp
   ;


allbitsOp
   : gatename=gate '(' '...' ')'
   | gatename=gate '(' INT ',...,' INT ')'
   ;

gate
   : id
   ;

/* A list of expressions */
explist
   : exp ( ',' exp )*
   ;

/* An expression */
exp
   : real
   | INT
   | 'pi'
   | id
   | exp '+' exp
   | exp '-' exp
   | exp '*' exp
   | exp '/' exp
   | '-'exp
   | exp '^' exp
   | '(' exp ')'
   | unaryop '(' exp ')'
   | key '=' exp
   | '\'[' ( ','? coupler )* ']\''
   ;

key
   : id
   ;

coupler
   : '[' INT ',' INT ']'
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
   : '#' ~ [\r\n]* EOL
   ;

/* id, ego, and super-ego */
ID
   : [a-z][A-Za-z0-9_]*
   | [A-Z][A-Za-z0-9_]*
   | [A-Z][A-Za-z]*
   ;

/* Keep it real...numbers */
REAL
   : INT ( '.' (INT) )
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
