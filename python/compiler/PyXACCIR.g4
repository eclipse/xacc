/* 
 * Copyright (c) 2017, UT-Battelle
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of the xacc nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Originally devloped by:
 * Travis Humble, Oak Ridge National Laboratory, March 2017
 *
 * Updated by:
 * H. Charles Zhao, Oak Ridge National Laboratory, July 2018
 *
 * An ANTLR4 specification for Version 2.0 of the IBM Open QASM Grammar 
 * See https://github.com/IBM/qiskit-openqasm for the written spec
 *
*/

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
