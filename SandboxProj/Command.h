#pragma once
#include "Actor.h"
namespace hlab {
class Command {
  public:
    virtual ~Command() {}
    virtual void excutue(shared_ptr<Actor> actor) = 0;
    virtual void undo(){}
    virtual void redo(){}

};

} // namespace hlab
