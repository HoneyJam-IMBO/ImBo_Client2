#include "stdafx.h"
#include "RenderContainer.h"



//---------------------------dxobject---------------------------------
bool CRenderContainer::Begin() {

	return true;
}
bool CRenderContainer::End() {
	//m_lpObjects.clear();
	m_lpObjects.clear();
	m_vpTexture.clear();
	m_vpBuffer.clear();
	m_vpMesh.clear();

	delete[] m_ppBufferData;

	return true;
}
//---------------------------dxobject---------------------------------

//--------------------------container---------------------------------
void CRenderContainer::UpdateShaderState(shared_ptr<CCamera> pCamera) {

	//----------------------------update instance buffer--------------------------
	if (m_vpBuffer.empty()) return;
	int nInstance = 0;
	int nBuffer = 0;
	//map
	size_t vecBufferSize = m_vpBuffer.size();
	for (size_t i = 0; i < vecBufferSize; ++i)
	{
		m_ppBufferData[nBuffer++] = m_vpBuffer[i]->Map();
	}

	bool bIsDebug = INPUTMGR->GetDebugMode();
	size_t vecSize = m_lpObjects.size();
	for (size_t i = 0; i < vecSize; ++i)
	{
		if (true == bIsDebug)
		{
			DEBUGER->RegistToDebugRenderContainer(m_lpObjects[i]);
			m_lpObjects[i]->RegistToDebuger();
		}
		m_lpObjects[i]->SetBufferInfo(m_ppBufferData, nInstance, pCamera);
		nInstance++;
	}
	//unmap
	for (size_t i = 0; i < vecBufferSize; ++i)
	{
		m_vpBuffer[i]->Unmap();
	}
	/*for (auto p : m_vpBuffer) {
		p->Unmap();
	}*/
	//----------------------------update instance buffer--------------------------


	//if (m_pAnimater) m_pAnimater->Update(TIMEMGR->GetTimeElapsed());

	m_pShader->UpdateShaderState();
	size_t vecTexSize = m_vpTexture.size();
	for (size_t i = 0; i < vecTexSize; ++i)
	{
		m_vpTexture[i]->UpdateShaderState();
	}
	size_t vecMatSize = m_vpMaterial.size();
	for (size_t i = 0; i < vecMatSize; ++i)
	{
		m_vpMaterial[i]->UpdateShaderState();
	}
	for (size_t i = 0; i < vecBufferSize; ++i)
	{
		m_vpBuffer[i]->UpdateShaderState();
	}
	/*for (auto p : m_vpBuffer) {
		p->UpdateShaderState();
	}*/

	//if (m_pGlobalBuffer) m_pGlobalBuffer->UpdateShaderState();

}
void CRenderContainer::SetShaderState() {
	m_pShader->SetShaderState();
	size_t vecSize = m_vpTexture.size();
	for (size_t i = 0; i < vecSize; ++i)
	{
		m_vpTexture[i]->SetShaderState();
	}

	vecSize = m_vpVolatileTexture.size();
	for (size_t i = 0; i < vecSize; ++i)
	{
		m_vpVolatileTexture[i]->SetShaderState();
	}

	vecSize = m_vpMaterial.size();
	for (size_t i = 0; i < vecSize; ++i)
	{
		m_vpMaterial[i]->SetShaderState();
	}

	vecSize = m_vpBuffer.size();
	for (size_t i = 0; i < vecSize; ++i)
	{
		m_vpBuffer[i]->SetShaderState();
	}

	vecSize = m_vpVolatileBuffer.size();
	for (size_t i = 0; i < vecSize; ++i)
	{
		m_vpVolatileBuffer[i]->SetShaderState();
	}
	if (m_pAnimater)m_pAnimater->SetShaderState();

}

void CRenderContainer::ClearVolatileResources() {
	m_vpVolatileTexture.clear();
	m_vpVolatileBuffer.clear();
}

void CRenderContainer::RenderExcute() {
	size_t vecSize = m_vpMesh.size();
	for (size_t i = 0; i < vecSize; ++i)
	{
		m_vpMesh[i]->Render(m_lpObjects.size());
	}
}
void CRenderContainer::RenderExcuteWithOutObject() {
	size_t vecSize = m_vpMesh.size();
	for (size_t i = 0; i < vecSize; ++i)
	{
		m_vpMesh[i]->Render(1);
	}
}
void CRenderContainer::CleanShaderState() {
	m_pShader->CleanShaderState();

	size_t vecSize = m_vpTexture.size();
	for (size_t i = 0; i < vecSize; ++i){
		m_vpTexture[i]->CleanShaderState();
	}
	vecSize = m_vpVolatileTexture.size();
	for (size_t i = 0; i < vecSize; ++i) {
		m_vpVolatileTexture[i]->CleanShaderState();
	}
	vecSize = m_vpMaterial.size();
	for (size_t i = 0; i < vecSize; ++i) {
		m_vpMaterial[i]->CleanShaderState();
	}
	vecSize = m_vpBuffer.size();
	for (size_t i = 0; i < vecSize; ++i) {
		m_vpBuffer[i]->CleanShaderState();
	}
	vecSize = m_vpVolatileBuffer.size();
	for (size_t i = 0; i < vecSize; ++i) {
		m_vpVolatileBuffer[i]->CleanShaderState();
	}

	if (m_pAnimater)m_pAnimater->CleanShaderState();
}


//--------------------------container �Һ� �Լ�---------------------------------
void CRenderContainer::Render(shared_ptr<CCamera> pCamera) {
	if (m_lpObjects.empty()) return;

	UpdateShaderState(pCamera);
	SetShaderState();
	RenderExcute();			//Render;

	m_pShader->CleanShaderState();
	if (m_pAnimater)m_pAnimater->CleanShaderState();
}

void CRenderContainer::RenderWithOutObject(shared_ptr<CCamera> pCamera) {
	//shader State Update/ Instancing Buffet Update
	UpdateShaderState(pCamera);
	SetShaderState();
	RenderExcuteWithOutObject();
	//CleanShaderState();
	m_pShader->CleanShaderState();
	if (m_pAnimater)m_pAnimater->CleanShaderState();
}

void CRenderContainer::ClearMesh() {
	if (m_vpMesh.empty())return;
	m_vpMesh.clear();
}

void CRenderContainer::ClearBuffer() {
	if (m_vpBuffer.empty())return;
	m_vpBuffer.size();
	m_nBuffer = 0;
}
void CRenderContainer::ClearAnimater() {
	m_pAnimater = nullptr;
	//animater�� �������� ����.. ����ϴ°��� ���� ������ �༮�� �Ӿƴϰŵ�..
}

void CRenderContainer::AddMesh(shared_ptr<CMesh> pMesh) {
	if (!pMesh) return;
	//m_vpMesh.e(pMesh);
	m_vpMesh.push_back(pMesh);
}
void CRenderContainer::SetMesh(shared_ptr<CMesh> pMesh) {
	if (!pMesh) return;
	//m_vpMesh.Add(pMesh);
	m_vpMesh.push_back(pMesh);
}
void CRenderContainer::SetShader(shared_ptr<CRenderShader> pShader) {
	if (!pShader)return;

	m_pShader = pShader;
}
void CRenderContainer::AddTexture(shared_ptr<CTexture> pTexture) {
	if (!pTexture)return;
	//m_vpTexture.Add(pTexture);
	m_vpTexture.emplace_back(pTexture);
}

void CRenderContainer::AddVolatileTexture(shared_ptr<CTexture> pTexture) {
	if (!pTexture)return;
	//m_vpVolatileTexture.Add(pTexture);
	m_vpVolatileTexture.emplace_back(pTexture);
}

//-----------------------------------------buffer-----------------------------------------
void CRenderContainer::AddBuffer(shared_ptr<CBuffer> pBuffer) {
	if (!pBuffer) return;


	//void**�� �ʱ�ȭ �ϱ� ���� �ʿ��ѳ�
	m_nBuffer++;
	//m_vpBuffer.Add(pBuffer);
	m_vpBuffer.emplace_back(pBuffer);

	if (m_ppBufferData) delete[] m_ppBufferData;
	m_ppBufferData = new void*[m_vpBuffer.size()];
}
void CRenderContainer::AddVolatileBuffer(shared_ptr<CBuffer> pBuffer) {
	if (!pBuffer) return;
	//m_vpVolatileBuffer.Add(pBuffer);
	m_vpVolatileBuffer.emplace_back(pBuffer);
}
void CRenderContainer::AddInstanceBuffer(shared_ptr<CBuffer> pBuffer) {
	if (!pBuffer) return;

	//void**�� �ʱ�ȭ �ϱ� ���� �ʿ��ѳ�
	m_nBuffer++;
	//m_vpBuffer.Add(pBuffer);
	m_vpBuffer.emplace_back(pBuffer);

	if (m_ppBufferData) delete[] m_ppBufferData;
	m_ppBufferData = new void*[m_vpBuffer.size()];

	ID3D11Buffer* ppBuffers[1] = { pBuffer->GetBuffer() };
	UINT ppStrides[1] = { pBuffer->GetBufferStride() };
	UINT ppOffset[1] = { pBuffer->GetOffset() };


	size_t vecSize = m_vpMesh.size();
	for (size_t i = 0; i < vecSize; ++i)
	{
		m_vpMesh[i]->AddInstancingBuffer(pBuffer.get());
	}
	//for (auto p : m_vpMesh) {
	//	//p->AssembleToVertexBuffer(1, ppBuffers, ppStrides, ppOffset);
	//	p->AddInstancingBuffer(pBuffer.get());
	//}

}
//-----------------------------------------buffer-----------------------------------------

void CRenderContainer::AddMaterial(shared_ptr<CMaterial> pMaterial) {
	if (!pMaterial)return;

	m_nMaterial++;
	//m_vpMaterial.Add(pMaterial);
	m_vpMaterial.emplace_back(pMaterial);
}

void CRenderContainer::SetObejcts(int n, CGameObject** ppObjects) {
	if (!ppObjects) return;

	for (int i = 0; i < n; ++i) {
		//m_lpObjects.Add(ppObjects[i]);
		m_lpObjects.emplace_back(ppObjects[i]);
	}
}

void CRenderContainer::AddObject(CGameObject* pObject) {
	if (!pObject) return;

	//m_lpObjects.Add(pObject);
	m_lpObjects.emplace_back(pObject);
}
//void CRenderContainer::RemoveObject(CGameObject* pObject) {
//	if (!pObject) return;
//	//if (0 == m_lpObjects.size()) return;
//
//	// ���ͷ� �� �ٲ�
//	/*m_lpObjects.remove_if([&pObject](CGameObject* pO) {
//		return pObject == pO;
//	});*/
//}
//--------------------------container �Һ� �Լ�---------------------------------

CRenderContainer::CRenderContainer() : DXObject("rendercontainer") {
}
CRenderContainer::~CRenderContainer() {
}