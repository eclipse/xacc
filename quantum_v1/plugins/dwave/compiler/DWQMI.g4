/* 
 * Copyright (c) 2018, UT-Battelle
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
 * Alex McCaskey, Oak Ridge National Laboratory, July 2018
 *
 * An ANTLR4 specification for Version 1.0 of the XACC D-Wave QMI Language
 *
 */

grammar DWQMI;


/* This part of the grammar is particular to XACC */
/*************************************************************************************/
xaccsrc         : xacckernel* ;

xacckernel      : '__qpu__' kernelname=id '(' 'AcceleratorBuffer' acceleratorbuffer=id (',' typedparam)* ')' '{' mainprog '}' ;

kernelcall      : kernelname=id '(' id? (',' id)* ')' ;

typedparam      : TYPE id ;

TYPE            : 'int' | 'double' | 'float' ;
/*************************************************************************************/


/* The main program */
mainprog
   : program
   ;

/* The actual program statements */
program
   : (line)+
   ;

line
   : statement+ EOL
   | comment
   | EOL
   ;

/* A program statement */
statement
   : annealdecl ';'
   | inst ';'
   | kernelcall ';'
   ;

/* A program comment */
comment
   : COMMENT 
   ;

inst 
   : INT INT (id | real | INT)
   ;
   
annealdecl
   : anneal ta tp tq direction?
   ;

/* A gate name */
ta
   : id
   | real 
   | INT
   ;

tp
   : id
   | real 
   | INT
   ;
tq
   : id
   | real 
   | INT
   ;

anneal
   : ANNEAL
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

direction
   : forward 
   | reverse
   ;

/* Tokens for the grammer */

/* Comment */
COMMENT
   : '//' ~ [\r\n]* EOL
   ;

/* gate opcode */
ANNEAL
   : 'anneal'
   ;

forward
   : 'forward'
   ;

reverse
   : 'reverse'
   ;

ID 
   : [A-Za-z][A-Za-z0-9_]*
   ;

/* Keep it real...numbers */
REAL
   : (MINUS)? [0-9]* '.' [0-9]+
   ;
MINUS
   : '-'
   ;

/* Non-negative integers */
INT
   : (MINUS)? ('0'..'9')+ 
   ;

/* Strings include numbers and slashes */
STRING
   : '"' ~ ["]* '"'
   ;

/* This is the end of the line, boys */
EOL
: '\r'? '\n'
;

/* Whitespaces, we skip'em */
WS
   : [ \t\r\n] -> skip
   ;
