#include "stdafx.h"
#include "LesserGiant.h"


CLesserGiant::CLesserGiant(string name, tag t)
	: CGameObject(name, t)
{
}

CLesserGiant::~CLesserGiant()
{
}

void CLesserGiant::Animate(float fTimeElapsed)
{
	if (m_pAnimater) m_pAnimater->Update(TIMEMGR->GetTimeElapsed() * 0.2f);
	CGameObject::Animate(fTimeElapsed);
}

void CLesserGiant::RegistToContainer()
{
	CGameObject::RegistToContainer();
}

void CLesserGiant::PhisicsLogic(CAtlMap<utag, CAtlArray<CGameObject*>*>* pUtagObjectAtlMap, float fDeltaTime)
{
	CAtlArray<CGameObject*>* lpCollsion;
	pUtagObjectAtlMap->Lookup(utag::UTAG_ARROW, lpCollsion);
	// move player to mapmesh
	size_t iSize = lpCollsion->GetCount();
	for (size_t i = 0; i < iSize; ++i)
	{
		if (false == (*lpCollsion)[i]->GetActive()) continue;
		if (true == IsCollision((*lpCollsion)[i]))
		{
			//XMStoreFloat3(&m_xmf3Position, XMLoadFloat3(&m_xmf3Position) + ((XMVector3Normalize(m_xmvShift) * -m_fSpeed) * fDeltaTime));
			//SetPosition(XMLoadFloat3(&m_xmf3Position));
			(*lpCollsion)[i]->DisappearSkill();
			break;
		}
	}
}

