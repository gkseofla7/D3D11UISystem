#pragma once
#include "Command.h"
namespace hlab {
class MoveFrontCommand {
public:
    MoveFrontCommand(shared_ptr<Actor> actor, float x, float y, float z)
      : m_actor(actor), m_x(x), m_y(y), m_z(z) {

  }
    virtual void excutue(shared_ptr<Actor> actor) {
        Vector3 curPos = actor->m_actorConstsCPU.world.Translation();
        m_px = curPos.x;
        m_py = curPos.y;
        m_pz = curPos.z;
        //m_x = m_px + 0.2f;
        //m_y = m_py;
        //m_z = m_pz;
        Matrix translationMatrix = Matrix::CreateTranslation(Vector3(0.2f, 0.0f, 0.0f));
        m_actor->UpdateWorldRow(m_actor->m_worldMatrix * translationMatrix);
  }
    virtual void undo() {
        Matrix translationMatrix =
            Matrix::CreateTranslation(Vector3(m_px - m_x, m_py - m_y, m_py- m_z));
        m_actor->UpdateWorldRow(m_actor->m_worldMatrix * translationMatrix);
    }





private:
  shared_ptr<Actor> m_actor;
  float m_x;
  float m_y;
  float m_z;
  float m_px;
  float m_py;
  float m_pz;
};

} // namespace hlab
