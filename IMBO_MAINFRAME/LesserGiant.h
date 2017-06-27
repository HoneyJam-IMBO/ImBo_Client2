#pragma once
#include "GameObject.h"

class CLesserGiant :
	public CGameObject
{


public:
	virtual void Animate(float fTimeElapsed);
	virtual void RegistToContainer();

public:
	virtual void PhisicsLogic(CAtlMap<utag, CAtlArray<CGameObject*>*>* pUtagObjectAtlMap, float fDeltaTime);


public:
	CLesserGiant(string name, tag t = tag::TAG_DEFAULT);
	~CLesserGiant();
};

