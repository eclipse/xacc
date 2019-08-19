#!/bin/bash
set - o errexit
java -jar antlr-4.7.2-complete.jar -Dlanguage=Cpp -listener -o generated/ -package pyxasm pyxasm.g4
