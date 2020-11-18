grammar FermionOperator;

fermionSrc : term ( plusorminus term ) *;

plusorminus : '+' | '-';

term : coeff ? (fermion) *;

fermion : op;

op : INT carat?;

carat : '^';

coeff : complex | real ;

complex : '('(real | INT | scientific)','(real | INT | scientific)')';

real : REAL | scientific;

comment : COMMENT;

COMMENT : '#' ~[\r\n] * EOL;

/* Real number */
REAL : ('-') ? INT ? '.' INT;

/* scientific notation */
scientific : (REAL | INT) ? ('e'|'E') ? INT;

/* Non-negative integer */
INT : ('-') ? ('0'..'9') + ;

/* Strings include numbers and slashes */

/* Whitespaces, we skip'em */
WS : [ \t\r\n]->skip;

/* This is the end of the line, boys */
EOL : '\r' ? '\n';
