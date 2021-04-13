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
        psi4.set_options({'basis':basis,
                          'scf_type':'pk',
                          'mp2_type':'conv',
                          'e_convergence': 1e-8,
                          'd_convergence': 1e-8})
        scf_e, scf_wfn = psi4.energy('scf', return_wfn=True)
        E_nucl = moleculeGeom.nuclear_repulsion_energy()

        # Get MO coefficients from SCF wavefunction
        # ==> ERIs <==
        # Create instance of MintsHelper class:

        mints = psi4.core.MintsHelper(scf_wfn.basisset())



        nbf = mints.nbf()           # number of basis functions
        nso = 2 * nbf               # number of spin orbitals
        nalpha = scf_wfn.nalpha()   # number of alpha electrons
        nbeta = scf_wfn.nbeta()     # number of beta electrons
        nocc = nalpha + nbeta       # number of occupied orbitals
        nvirt = 2 * nbf - nocc      # number of virtual orbitals
        list_occ_alpha = np.asarray(scf_wfn.occupation_a())
        list_occ_beta = np.asarray(scf_wfn.occupation_b())

        # Get orbital energies
        eps_a = np.asarray(scf_wfn.epsilon_a())
        eps_b = np.asarray(scf_wfn.epsilon_b())
        eps = np.append(eps_a, eps_b)

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
        gao = tmp - tmp.transpose(0, 1, 3, 2)
        gmo = np.einsum('pQRS, pP -> PQRS',
                        np.einsum('pqRS, qQ -> pQRS',
                                  np.einsum('pqrS, rR -> pqRS',
                                            np.einsum('pqrs, sS -> pqrS', gao, C), C), C), C)

        # -------- 0-body term:
        Hamiltonian_0body = E_nucl

        # -------- 1-body term:
        #   Ca*
        # Build core Hamiltonian
        T = np.asarray(mints.ao_kinetic())
        V = np.asarray(mints.ao_potential())
        H_core_ao = T + V



        # -- check  which one more efficient (matmul vs einsum)
        #   H_core_mo = np.matmul(Ca.T,np.matmul(H_core_ao,Ca)))
        H_core_mo = np.einsum('ij, jk, kl -> il', Ca.T, H_core_ao, Ca)
        H_core_mo_alpha = H_core_mo
        H_core_mo_beta = H_core_mo

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

