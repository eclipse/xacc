from abc import abstractmethod, ABC


class AnsatzGenerator(ABC):
    """
        Abstract class to be inherited by iPOPO bundles to generate quantum circuit ansatz for quantum computation

        Subclasses must inherit and implement the generate() method to return an XACC Intermediate Representation (IR) Function instance which expresses
        a composition of instructions to be executed
    """
    @abstractmethod
    def generate(self, inputParams):
        pass
