#include <iostream>
#include "../include/find_embedding.hpp"

class MyCppInteractions : public find_embedding::LocalInteraction {
  public:
    bool _canceled = false;
    void cancel() { _canceled = true; }

  private:
    virtual void displayOutputImpl(const std::string& mess) const { std::cout << mess << std::endl; }
    virtual bool cancelledImpl() const { return _canceled; }
};

int main() {
    graph::input_graph triangle(3, {0, 1, 2}, {1, 2, 0});
    graph::input_graph square(4, {0, 1, 2, 3}, {1, 2, 3, 0});
    find_embedding::optional_parameters params;
    params.localInteractionPtr.reset(new MyCppInteractions());

    std::vector<std::vector<int>> chains;

    if (find_embedding::findEmbedding(triangle, square, params, chains)) {
        for (auto chain : chains) {
            for (auto var : chain) std::cout << var << " ";
            std::cout << std::endl;
        }
    } else {
        std::cout << "Couldn't find embedding." << std::endl;
    }

    return 0;
}