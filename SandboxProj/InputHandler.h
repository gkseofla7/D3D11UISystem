#pragma once
#include "Command.h"
#include "MoveFrontCommand.h"
#include "Actor.h"
namespace hlab {
    using namespace std;
class InputHandler {
  public:
    shared_ptr<Command> HandleInput(HWND hwnd, UINT msg, WPARAM wParam,
                                    LPARAM lParam,
                                    shared_ptr<Actor> selectedActor) { 
        m_selectedActor = selectedActor;
        int prevSize = m_Commands.size();
       switch (msg) { 
       
       case WM_KEYDOWN:
           if (wParam =='O') {
               if (selectedActor) {
                   Vector3 curPos =
                       selectedActor->m_actorConstsCPU.world.Translation();

                   shared_ptr<MoveFrontCommand> newCommand =
                       make_shared<MoveFrontCommand>(
                           selectedActor, curPos.x + 0.2f, curPos.y, curPos.z);
                   AddCommand(newCommand);
               }

           }
       }
       if (m_Commands.size() != prevSize) {

           return m_Commands[commandIndex];
       }
       return shared_ptr<Command>();
    }

    shared_ptr<Command> HandleRedoInput(HWND hwnd, UINT msg, WPARAM wParam,
        LPARAM lParam) {
       if (commandIndex >= m_Commands.size()) {
           return shared_ptr<Command>();
       }
       switch (msg) {
       case WM_KEYDOWN:
           if (wParam == 'L') {  
               return m_Commands[commandIndex++];
           }
       }
       return shared_ptr<Command>();
    }

    shared_ptr<Command> HandleUndoInput(HWND hwnd, UINT msg, WPARAM wParam,
                                        LPARAM lParam) {
       if (commandIndex <= 0) {
           return shared_ptr<Command>();
       }
       switch (msg) {
       case WM_KEYDOWN:
           if (wParam == 'K') {
               return m_Commands[--commandIndex];
           }
       }
       return shared_ptr<Command>();
    }


private:
    void AddCommand(shared_ptr<Command> command) { 
        if (commandIndex!= -1) {
            //현재 가르키는 command 이후 애들 모두 지움
           m_Commands.erase(m_Commands.begin() + commandIndex + 1,
                            m_Commands.end());
        }
        m_Commands.push_back(command);
        commandIndex = m_Commands.size()-1;
    }

  private:
    shared_ptr<Actor> m_selectedActor;
    vector<shared_ptr<Command>> m_Commands;
    int commandIndex = -1;

};

} // namespace hlab
