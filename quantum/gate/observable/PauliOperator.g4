grammar PauliOperator;

pauliSrc
   : term (plusorminus term)*
   ;

plusorminus
   : '+' | '-'
   ;
term
   : coeff? (pauli)*
   ;


pauli
   : ('X' | 'Y' | 'Z')INT
   | ('I')
   ;

coeff
   : complex
   | real
   ;


complex
   : '(' (real|INT) ',' (real|INT) ')'
   ;

real
   : REAL
   ;

comment
   : COMMENT
   ;

COMMENT
   : '#' ~ [\r\n]* EOL
   ;

/* Real number */
REAL
   : ('-')? INT? '.' INT
   ;

/* Non-negative integer */
INT
   : ('0'..'9')+
   ;

/* Strings include numbers and slashes */


/* Whitespaces, we skip'em */
WS
   : [ \t\r\n] -> skip
   ;

/* This is the end of the line, boys */
EOL
: '\r'? '\n'
;
