/*******************************************************************************
 * Copyright (c) 2019 UT-Battelle, LLC.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompanies this
 * distribution. The Eclipse Public License is available at
 * http://www.eclipse.org/legal/epl-v10.html and the Eclipse Distribution
 *License is available at https://eclipse.org/org/documents/edl-v10.php
 *
 * Contributors:
 *   Alexander J. McCaskey - initial API and implementation
 *******************************************************************************/
#ifndef XACC_IBM_OQASMERRORLISTENER_HPP
#define XACC_IBM_OQASMERRORLISTENER_HPP

using namespace antlr4;

class OQASMErrorListener : public BaseErrorListener {
public:
  void syntaxError(Recognizer *recognizer, Token *offendingSymbol, size_t line,
                   size_t charPositionInLine, const std::string &msg,
                   std::exception_ptr e) override {
    std::ostringstream output;
    output << "Invalid OpenQASM source: ";
    output << "line " << line << ":" << charPositionInLine << " " << msg;
    xacc::error(output.str());
  }
};

#endif // XACC_IBM_OQASMERRORLISTENER_HPP
