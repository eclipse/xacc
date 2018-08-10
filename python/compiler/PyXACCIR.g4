/*******************************************************************************
 * Copyright (c) 2018 UT-Battelle, LLC.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompanies this
 * distribution. The Eclipse Public License is available at
 * http://www.eclipse.org/legal/epl-v10.html and the Eclipse Distribution License
 * is available at https://eclipse.org/org/documents/edl-v10.php
 *
 * Contributors:
 *   Alexander J. McCaskey - initial API and implementation
 *******************************************************************************/
grammar PyXACCIR;

/* This part of the grammar is particular to XACC */
/**********************************************************************/
xaccsrc
   : xacckernel*
   ;

xacckernel
   : 'def' kernelname=id '(' ( ','? param )* ')' ':' mainprog 
   ;

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
   : uop 
   | 'return'
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
   ;

/* A quantum operation */
/* FIXME: Spec calls for 'reset' keyword but this clashes with antlr parser.reset() 
 * we use 'reeset' instead..ha-ha, I know, right?
*/

/* A unitary operation */
uop
   : gatename=gate '(' ( explist ) ')'
   ;

gate 
   : 'X'
   | 'Rx'
   | 'Rz'
   | 'Ry'
   | 'Y'
   | 'Z'
   | 'CNOT'
   | 'Cz'
   | 'H'
   | 'Swap'
   | 'Identity'
   | 'CPhase'
   | 'Measure'
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
   ;

/* Keep it real...numbers */
REAL
   : INT ( '.' (INT)? )
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
