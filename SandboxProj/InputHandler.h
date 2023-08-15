#pragma once
#include "Command.h"
#include "MoveFrontCommand.h"
#include "Actor.h"
namespace hlab {
    using namespace std;
class InputHandler {
  public:
    shared_ptr<Command> handleInput(HWND hwnd, UINT msg, WPARAM wParam,
                                    LPARAM lParam) { 
       switch (msg) { 
       
       case WM_KEYDOWN:
           if (wParam =='O') {
               if (m_selectedActor) {
                   Vector3 curPos =
                       m_selectedActor->m_actorConstsCPU.world.Translation();
                   return make_shared<MoveFrontCommand>(
                       m_selectedActor, curPos.x + 0.2f, curPos.y, curPos.z);
               }

           }
       }
    }

    shared_ptr<Command> m_buttonFront;
    shared_ptr<Actor> m_selectedActor;
};

} // namespace hlab
