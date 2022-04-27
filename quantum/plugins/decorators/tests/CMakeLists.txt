# *******************************************************************************
# Copyright (c) 2019 UT-Battelle, LLC.
# All rights reserved. This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v1.0
# and Eclipse Distribution License v.10 which accompany this distribution.
# The Eclipse Public License is available at http://www.eclipse.org/legal/epl-v10.html
# and the Eclipse Distribution License is available at
# https://eclipse.org/org/documents/edl-v10.php
#
# Contributors:
#   Alexander J. McCaskey - initial API and implementation
# *******************************************************************************/
#target_link_libraries(SymVerificationDecoratorTester xacc-vqe-decorators)
#add_xacc_test(VQERestartDecorator)
#target_link_libraries(VQERestartDecoratorTester xacc-vqe-decorators)
#add_xacc_test(PurificationDecorator)
#target_link_libraries(PurificationDecoratorTester xacc-vqe-decorators)

add_xacc_test(RDMPurificationDecorator)
target_link_libraries(RDMPurificationDecoratorTester xacc xacc-fermion)

add_xacc_test(ROErrorDecorator)
target_link_libraries(ROErrorDecoratorTester xacc xacc-pauli)


add_xacc_test(RichExtrapDecorator)
target_link_libraries(RichExtrapDecoratorTester xacc xacc-pauli xacc-quantum-gate)


add_xacc_test(ImprovedSamplingDecorator)
target_link_libraries(ImprovedSamplingDecoratorTester xacc)

add_xacc_test(AssignmentErrorKernelDecorator)
target_link_libraries(AssignmentErrorKernelDecoratorTester xacc xacc-quantum-gate)