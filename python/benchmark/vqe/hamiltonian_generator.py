from abc import abstractmethod, ABC


class HamiltonianGenerator(ABC):
    """
        Abstract class to be inherited by iPOPO bundles to generate a Hamiltonian for quantum computation

        Subclasses must inherit and implement the generate() method to return an XACC-VQE PauliOperator
    """
    @abstractmethod
    def generate(self, data):
        pass
