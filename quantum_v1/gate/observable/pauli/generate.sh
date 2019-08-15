#!/bin/bash
set - o errexit
java -jar antlr-4.7.2-complete.jar -Dlanguage=Cpp -listener -o generated/ -package pauli PauliOperator.g4
