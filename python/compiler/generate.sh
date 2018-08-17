#!/bin/bash
set -o errexit

LOCATION=antlr-4.7.1-complete.jar
java -jar $LOCATION -Dlanguage=Cpp -listener -o generated/ -package pyxacc PyXACCIR.g4
