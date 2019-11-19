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
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
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

grammar OQASM2;

/* This part of the grammar is particular to XACC */
/**********************************************************************/
xaccsrc : xacckernel ;

xacckernel : '__qpu__' 'void' kernelname =
                 id '(' 'qbit' acceleratorbuffer =
                     id(',' typedparam) * ')' '{' mainprog '}';

typedparam : type param;

type : 'int' | 'double' | 'float';

kernelcall : kernelname = id '(' param ? (',' param) * ')';
/***********************************************************************/

/* The main program */
mainprog : comment * | OPENQASM real ';' | program ? ;

/* The actual program statements */
program : (line) + ;

line : statement + | comment | include + | OPENQASM real ';' ;

/* A program statement */
statement : regdecl ';' | gatedecl | opaque ';' | qop ';' | conditional ';' |
            kernelcall ';';

/* A program comment */
comment : COMMENT;

/* An include statemnent */
include : 'include' filename ';';

filename : string;

/* Register declarations, called 'decl' in OQASM spec */
regdecl : qreg qregister | creg cregister;

/* A register and its size */
qregister : id '[' registersize ']' | id;

cregister : id '[' registersize ']' | id;

registersize : INT;

/* A gate declaration */
gatedecl : gate gatename gatearglist gatebody ? ;

/* A gate name */
gatename : id | id '(' paramlist ? ')';

/* A coma-separated list of gate arguments */
gatearglist : gatearg(',' gatearg) *;

/* A gate argument */
/* FIXME: Should this be qregister or anything? Spec is unclear */
gatearg : id '[' INT ']' | id;

/* The body of the gate declaration */
gatebody : '{' gateprog '}';

/* The program defining the gate */
gateprog : gateline *;

/* statements in the gateprog must be semicolon terminated */
gateline : uop ';';

/* A list of parameters */
paramlist : param(',' paramlist) ? ;

/* A parameter */
param : id;

/* An opaque gate declaration */
opaque : OPAQUE opaquename opaquearglist;

/* An opaque gate name */
opaquename : id | id '(' paramlist ? ')';

/* A coma-separated list of opaqueargs */
opaquearglist : opaquearg(',' opaquearglist) ? ;

/* A argument to opaque function */
opaquearg : id '[' INT ']';

/* A quantum operation */
/* FIXME: Spec calls for 'reset' keyword but this clashes with antlr
 * parser.reset() we use 'reeset' instead..ha-ha, I know, right?
 */
qop : uop | reeset | measure | barrier;

/* A unitary operation */
uop : 'U' '(' explist ? ')' gatearg #U | 'CX' gatearg ',' gatearg #CX |
                            gatename('(' explist ')')
      ? gatearglist #UserDefGate;

/* A classical conditional expression */
conditional : 'if' '(' id '==' INT ')' action;

/* An action in a conditional branch is a quantum operation */
action : qop;

/* A list of expressions */
explist : exp(',' exp) *;

/* An expression */
exp :  real | INT | 'pi' | id | exp '+' exp | exp '-' exp | exp '*' exp |
      exp '/' exp | '-' exp | exp '^' exp | '(' exp ')' | unaryop '(' exp ')';

/* unary operations */
unaryop : 'sin' | 'cos' | 'tan' | 'exp' | 'ln' | 'sqrt';

/* A quantum register declaration */
qreg : QREG;

/* A classical register declaration */
creg : CREG;

/* A gate declaration */
gate : GATE;

/* A measure declaration */
measure : MEASURE(qbit = gatearg | qregister) '->'(cbit = gatearg | cregister);

/* A reset declaration */
reeset : RESET(gatearg | qregister | cregister);

/* A barrier declaration */
barrier : BARRIER gatearglist;

/* variable identity */
id : ID;

/* real numbers */
real : REAL;
/*
SCIENTIFIC_NUMBER
   : NUMBER (E SIGN? UNSIGNED_INTEGER)?
   ;
*/
/* strings are enclosed in quotes */
string : STRING;

/* Tokens for the grammer */

/* Comment */
COMMENT : '//' ~[\r\n] * EOL;

/* quantum register init opcode */
QREG : 'qreg';

/* classical register init opcode */
CREG : 'creg';

/* gate opcode */
GATE : 'gate';

/* measurement opcode */
MEASURE : 'measure';

/* reset opcode */
RESET : 'reset';

/* barrier opcode */
BARRIER : 'barrier';

/* Grammer header */
OPENQASM : 'OpenQASM' | 'OPENQASM';

/* Opaque gate label */
OPAQUE : 'opaque';

/* id, ego, and super-ego */
ID
   : [a-z][A-Za-z0-9_]*
   | [A-Z][A-Za-z0-9_]*
   | [A-Z][A-Za-z]*
   ;
/* Keep it real...numbers */
REAL
   : INT ( '.' (INT)? )
   ;

/* Non-negative integers */
INT : ('0'..'9') + ;

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
