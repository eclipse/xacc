from abc import abstractmethod, ABC


class HamiltonianGenerator(ABC):

    @abstractmethod
    def generate(self, data):
        pass
