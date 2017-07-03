#include "stdafx.h"
#include "HpBar.h"


CHpBar::CHpBar()
{
}


CHpBar::~CHpBar()
{
	m_pUIRenderCont->ClearData();
}

CHpBar * CHpBar::Create(XMVECTOR xyPos, XMVECTOR xySize)
{
	CHpBar*		pObject = new CHpBar();
	if (FAILED(pObject->Initialize()))
	{
		MSG_BOX("CClickButton Create Faild");
		delete pObject;
		pObject = nullptr;
		return pObject;
	}
	pObject->SetInfo(xyPos, xySize);

	return pObject;
}

void CHpBar::SetInfo(XMVECTOR xyPos, XMVECTOR xySize)
{
	m_pUIRenderCont = RENDERER->GetUIRenderer();
	m_pMesh = RESOURCEMGR->GetMesh("UI");
	m_pCBuffer = CBuffer::CreateConstantBuffer(1, sizeof(tUImatVP), 11, BIND_VS, NULL);	//직교뷰*투영변환행렬 / 알파

	XMStoreFloat2(&m_f2XYPos, xyPos);
	XMStoreFloat2(&m_f2XYSize, xySize);
}

HRESULT CHpBar::Initialize()
{
	XMStoreFloat4x4(&m_f4x4View, XMMatrixIdentity());
	XMStoreFloat4x4(&m_f4x4Proj, XMMatrixOrthographicLH(WINSIZEX, WINSIZEY, 0.f, 1.f));

	m_pTexture = RESOURCEMGR->GetTexture("Bar_Fill");
	m_pBarTexture = RESOURCEMGR->GetTexture("Bar");

	return S_OK;
}

int CHpBar::Update(float fTimeElapsed)
{
	if (m_pUIRenderCont) m_pUIRenderCont->SetRenderContainer(3.0f, this);
	return 0;
}

void CHpBar::Render()
{
	//m_pBarTexture->UpdateShaderState();
	m_pCBuffer->UpdateShaderState();
	m_pBarTexture->SetShaderState();
	SetParameter();
	m_pMesh->Render(1);
	m_pBarTexture->CleanShaderState();

	/////////////////////////////////////////////////////////
	m_pTexture->SetShaderState();

	m_f4x4View._11 = m_f2XYSize.x * 0.95f;
	m_f4x4View._22 = m_f2XYSize.y;
	m_f4x4View._33 = 1.f;

	m_f4x4View._41 = m_f2XYPos.x - WINSIZEX * 0.5f;
	m_f4x4View._42 = -m_f2XYPos.y + WINSIZEY * 0.5f;

	tUImatVP* pdata = (tUImatVP*)m_pCBuffer->Map();

	XMStoreFloat4x4(&pdata->m_xmf4x4VP, XMMatrixTranspose(XMMatrixMultiply(XMLoadFloat4x4(&m_f4x4View), XMLoadFloat4x4(&m_f4x4Proj))));

	m_pCBuffer->Unmap();
	m_pCBuffer->SetShaderState();
	m_pMesh->Render(1);

	m_pTexture->CleanShaderState();

	//해제
	m_pCBuffer->CleanShaderState();
}

void CHpBar::Release(){
	if (m_pCBuffer) {
		m_pCBuffer->End();
		delete m_pCBuffer;
	}
	m_pCBuffer = nullptr;
}

void CHpBar::SetParameter()
{
	m_f4x4View._11 = m_f2XYSize.x;
	m_f4x4View._22 = m_f2XYSize.y;
	m_f4x4View._33 = 1.f;

	m_f4x4View._41 = m_f2XYPos.x - WINSIZEX * 0.5f;
	m_f4x4View._42 = -m_f2XYPos.y + WINSIZEY * 0.5f;

	tUImatVP* pdata = (tUImatVP*)m_pCBuffer->Map();

	XMStoreFloat4x4(&pdata->m_xmf4x4VP, XMMatrixTranspose(XMMatrixMultiply(XMLoadFloat4x4(&m_f4x4View), XMLoadFloat4x4(&m_f4x4Proj))));

	m_pCBuffer->Unmap();
	m_pCBuffer->SetShaderState();
}
