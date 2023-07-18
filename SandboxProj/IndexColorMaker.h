#pragma once
#include <directxtk/SimpleMath.h>

namespace hlab {

using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;


class IndexColorMaker {

public:
	static IndexColorMaker& getInstance() {
		static IndexColorMaker colorMaker;
		return colorMaker;
	}
	Vector3 GetNewIndexColor() { 
		m_curColor.x += 1.f;
		m_curColor.y += 1.f;
		m_curColor.z += 1.f;
		return m_curColor;
	}


private:
	Vector3 m_curColor = Vector3();

};


}