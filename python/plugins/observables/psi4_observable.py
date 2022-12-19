import xacc
from pelix.ipopo.decorators import ComponentFactory, Property, Requires, Provides, \
    Validate, Invalidate, Instantiate

@ComponentFactory("fc_observable_factory")
@Provides("observable")
@Property("_observable", "observable", "psi4")
@Property("_name", "name", "psi4")
@Instantiate("psi4_observable_instance")
class Psi4Observable(xacc.Observable):

    def __init__(self):
        xacc.Observable.__init__(self)
        self.observable = None
        self.asPauli = None

    def toString(self):
        return self.observable.toString()

    def observe(self, program):
        return self.observable.observe(program)

    def postProcess(self, buffer, post_process_task, extra_data):
        return self.observable.postProcess(buffer, post_process_task, extra_data)
        
    def nBits(self):
        return self.observable.nBits()

    def name(self):
        return 'psi4'

    def __iter__(self):
        return self.asPauli.__iter__()

    def fromOptions(self, inputParams):
        import numpy as np
        import psi4
        psi4.core.be_quiet()
        g = inputParams['geometry']
        basis = inputParams['basis']
        moleculeGeom = psi4.geometry(g)
        options = {'basis':basis,
                          'scf_type':'pk',
                          'mp2_type':'conv',
                          'e_convergence': 1e-8,
                          'd_convergence': 1e-8}
        if moleculeGeom.multiplicity() > 1:
            options['reference'] = 'rohf'
        psi4.set_options(options)
        scf_e, scf_wfn = psi4.energy('scf', return_wfn=True)
        E_nucl = moleculeGeom.nuclear_repulsion_energy()

        # Get MO coefficients from SCF wavefunction
        # ==> ERIs <==
        # Create instance of MintsHelper class:
        mints = psi4.core.MintsHelper(scf_wfn.basisset())

        # Get orbital coefficients:
        Ca = np.asarray(scf_wfn.Ca())
        Cb = np.asarray(scf_wfn.Cb())
        C = np.block([
            [Ca,np.zeros_like(Cb)],
            [np.zeros_like(Ca),Cb]
        ])

        # Get the two electron integrals using MintsHelper
        Ints = np.asarray(mints.ao_eri())
        def spin_block_tei(I):
            """
             Function that spin blocks two-electron integrals
             Using np.kron, we project I into the space of the 2x2 identity, tranpose the result
             and project into the space of the 2x2 identity again. This doubles the size of each axis.
             The result is our two electron integral tensor in the spin orbital form.
            """
            identity = np.eye(2)
            I = np.kron(identity, I)
            return np.kron(identity, I.T)

        # Spin-block the two electron integral array
        I_spinblock = spin_block_tei(Ints)

        # Converts chemist's notation to physicist's notation, and antisymmetrize
        # (pq | rs) ---> <pr | qs>
        # Physicist's notation
        tmp = I_spinblock.transpose(0, 2, 1, 3)

        # Antisymmetrize:
        # <pr||qs> = <pr | qs> - <pr | sq>
        gmo = tmp - tmp.transpose(0, 1, 3, 2)
        gmo = np.einsum('pqrs, sS -> pqrS', gmo, C)
        gmo = np.einsum('pqrS, rR -> pqRS', gmo, C)
        gmo = np.einsum('pqRS, qQ -> pQRS', gmo, C)
        gmo = np.einsum('pQRS, pP -> PQRS', gmo, C)

        # -------- 1-body term:
        #   Ca*
        # Build core Hamiltonian
        T = np.asarray(mints.ao_kinetic())
        V = np.asarray(mints.ao_potential())
        H_core_ao = T + V

        # ---- this version breaks is we permuted  SCF eigvecs
        # Hamiltonian_1body = np.block([
        #            [  H_core_mo_alpha             ,           np.zeros_like(H_core_mo_alpha)],
        #            [np.zeros_like(H_core_mo_beta) ,  H_core_mo_beta      ]])
        #
        # --- th is version is  safer than above (H_1b is permutted correctly if eigvecs are permutted)
        Hamiltonian_1body_ao = np.block([[H_core_ao, np.zeros_like(H_core_ao)],
                                         [np.zeros_like(H_core_ao), H_core_ao]])
        Hamiltonian_1body = np.einsum('ij, jk, kl -> il', C.T, Hamiltonian_1body_ao, C)
        Hamiltonian_2body = gmo

        if 'frozen-spin-orbitals' in inputParams and 'active-spin-orbitals' in inputParams:
            MSO_frozen_list = inputParams['frozen-spin-orbitals']
            MSO_active_list = inputParams['active-spin-orbitals']
            n_frozen = len(MSO_frozen_list)
            n_active = len(MSO_active_list)
        else:
            MSO_frozen_list = []
            MSO_active_list = range(Hamiltonian_1body.shape[0])
            n_frozen = 0
            n_active = len(MSO_active_list)

        # ----- 0-body frozen-core:
        Hamiltonian_fc_0body = E_nucl
        for a in range(n_frozen):

            ia = MSO_frozen_list[a]
            Hamiltonian_fc_0body += Hamiltonian_1body[ia, ia]

            for b in range(a):

                ib = MSO_frozen_list[b]
                Hamiltonian_fc_0body += gmo[ia, ib, ia, ib]

        f_str = str(Hamiltonian_fc_0body)

        pos_or_neg = lambda x : ' + ' if x > 0. else ' - '

        # --- 1-body frozen-core:
        Hamiltonian_fc_1body = np.zeros((n_active, n_active))
        Hamiltonian_fc_1body_tmp = np.zeros((n_active, n_active))
        for p in range(n_active):

            ip = MSO_active_list[p]

            for q in range(n_active):

                iq = MSO_active_list[q]
                Hamiltonian_fc_1body[p, q] = Hamiltonian_1body[ip, iq]
                #Hamiltonian_fc_1body_tmp[p,q] =  Hamiltonian_1body[ip,iq]

                for a in range(n_frozen):

                    ia = MSO_frozen_list[a]
                    Hamiltonian_fc_1body[p, q] += gmo[ia, ip, ia, iq]
                if abs(Hamiltonian_fc_1body[p,q]) > 1e-12:
                    f_str += pos_or_neg(Hamiltonian_fc_1body[p,q]) + str(abs(Hamiltonian_fc_1body[p,q])) + ' ' + str(p) + '^ ' + str(q)


        # ------- 2-body frozen-core:

        Hamiltonian_fc_2body = np.zeros((n_active, n_active, n_active, n_active))
        for p in range(n_active):

            ip = MSO_active_list[p]

            for q in range(n_active):

                iq = MSO_active_list[q]

                for r in range(n_active):

                    ir = MSO_active_list[r]

                    for ss in range(n_active):

                        iss = MSO_active_list[ss]
                        #Hamiltonian_fc_2body[p,q,r,ss]= 0.25* gmo[ip,iq,ir,iss]
                        Hamiltonian_fc_2body[p, q, r,ss] = gmo[ip, iq, ir, iss]
                        #Hamiltonian_fc_2body[p,q,r,ss]= 0.25* gmo[ip,iq,iss,ir]


        # checking whether to reduce Hamiltonian
        reduce_hamiltonian = False
        if 'reduce-hamiltonian' in inputParams and inputParams['reduce-hamiltonian']:
            reduce_hamiltonian = True

        if reduce_hamiltonian:

            if Hamiltonian_fc_1body.shape[0] != 4:
                raise NotImplementedError("Reduction only implemented for Hamiltonians with 4 spin orbitals / 2 spatial orbitals.")

            # Gather pieces of the Hamiltonian
            h11 = Hamiltonian_fc_1body[0,0]
            h22 = Hamiltonian_fc_1body[1,1]
            J11 = Hamiltonian_fc_2body[0,2,0,2]
            J12 = Hamiltonian_fc_2body[1,2,1,2]
            J22 = Hamiltonian_fc_2body[1,3,1,3]
            K12 = Hamiltonian_fc_2body[0,2,1,3]
            e1 = h11 + J11
            e2 = h22 + 2 * J12 - K12

            # The reduced Hamiltonian has the form
            # H = g0 x I + g1 x z1 + g2 x z2 + g3 x z1z2 + g4 x x1x2
            # We drop the y1y2 term because the Hamiltonian is real
            # We now solve a 4 x 4 linear system
            # <00|H|00> = g0 + g1 + g2 + g3 = HF = 2*e1 - J11 + E(R)
            # <10|H|10> = g0 - g1 + g2 - g3 = e1 + e2 - J11 - J12 + K12 + E(R)
            # <01|H|01> = g0 + g1 - g2 - g3 = e1 + e2 - J11 - J12 + K12 + E(R)
            # <11|H|11> = g0 - g1 - g2 + g3 = 2*e2 - 4*J12 + J22 + 2*K12 + E(R)
            # <10|H|01> = K12 = g4

            from numpy import linalg
            M = np.array([[1, 1, 1, 1], [1, -1, 1, -1], [1, 1, -1, -1], [1, -1, -1, 1]])
            Minv = linalg.inv(M)

            b = np.zeros(4)
            b[0] = 2 * e1 - J11 + E_nucl
            b[1] = e1 + e2 - J11 - J12 + K12 + E_nucl
            b[2] = b[1]
            b[3] = 2 * e2 - 4 * J12 + J22 + 2 * K12 + E_nucl

            reduced_h_coeffs = Minv @ b
            reduced_h_coeffs = np.append(reduced_h_coeffs, K12)

            pauli_terms = ["Z0", "Z1", "Z0Z1", "X0X1"]

            f_str = str(reduced_h_coeffs[0])
            for a, b in zip(reduced_h_coeffs[1:], pauli_terms):
                f_str += pos_or_neg(a) + str(abs(a)) + " " + b

            self.observable = xacc.getObservable('pauli', f_str)

        else:

            Hamiltonian_fc_2body_tmp = 0.25 * Hamiltonian_fc_2body.transpose(0, 1, 3, 2)
            for p in range(n_active):
                ip = MSO_active_list[p]
                for q in range(n_active):
                    iq = MSO_active_list[q]
                    for r in range(n_active):
                        ir = MSO_active_list[r]
                        for ss in range(n_active):
                            if abs(Hamiltonian_fc_2body_tmp[p,q,r,ss]) > 1e-12:
                                f_str += pos_or_neg(Hamiltonian_fc_2body_tmp[p,q,r,ss]) + str(abs(Hamiltonian_fc_2body_tmp[p,q,r,ss])) + ' ' + str(p) + '^ ' + str(q) + '^ ' + str(r) + ' ' + str(ss)
            self.observable = xacc.getObservable('fermion', f_str)
            self.asPauli = xacc.transformToPauli('jw', self.observable)