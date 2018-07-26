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
#ifndef XACC_IBM_DWQMILISTENER_H
#define XACC_IBM_DWQMILISTENER_H

#include <PyXACCIRBaseListener.h>
#include <IR.hpp>
#include "IRProvider.hpp"

using namespace pyxacc;

namespace xacc {
namespace quantum {

template<class C, class T>
auto contains(const C& v, const T& x)
-> decltype(std::end(v), true)
{
    return std::end(v) != std::find(std::begin(v), std::end(v), x);
}
/**
 * 
 */
class PyXACCListener : public PyXACCIRBaseListener {
	protected:
	        std::shared_ptr<Function> f;
            std::vector<std::string> functionVarNames;
            std::shared_ptr<IRProvider> provider;
	public:
            std::shared_ptr<Function> getKernel();

            PyXACCListener();

            virtual void enterXacckernel(PyXACCIRParser::XacckernelContext * /*ctx*/) override;

            virtual void enterUop(PyXACCIRParser::UopContext * /*ctx*/) override;

        
};
    
}

}

#endif
