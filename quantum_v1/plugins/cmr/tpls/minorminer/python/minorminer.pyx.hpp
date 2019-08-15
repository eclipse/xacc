#pragma once
#include "../include/util.hpp"

namespace {

class LocalInteractionPython : public find_embedding::LocalInteraction {
  public:
    virtual ~LocalInteractionPython() {}

  private:
    virtual void displayOutputImpl(const std::string& msg) const { PySys_WriteStdout("%s", msg.c_str()); }

    virtual bool cancelledImpl() const {
        if (PyErr_CheckSignals()) {
            PyErr_Clear();
            return true;
        }

        return false;
    }
};
}
