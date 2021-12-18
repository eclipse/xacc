# *******************************************************************************
# Copyright (c) 2019-2021 UT-Battelle, LLC.
# All rights reserved. This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v1.0
# and Eclipse Distribution License v.10 which accompany this distribution.
# The Eclipse Public License is available at http://www.eclipse.org/legal/epl-v10.html
# and the Eclipse Distribution License is available at
# https://eclipse.org/org/documents/edl-v10.php
#
# Contributors:
#   Thien Nguyen - initial API and implementation
#   Daniel Strano - adaption from Quantum++ to Qrack
#   Austin Adams - adaption for GTRI testbed
# *******************************************************************************/

add_xacc_test(IonTrapOneQubitPass)
target_link_libraries(IonTrapOneQubitPassTester xacc-iontrap)

add_xacc_test(IonTrapTwoQubitPass)
target_link_libraries(IonTrapTwoQubitPassTester xacc-iontrap)
