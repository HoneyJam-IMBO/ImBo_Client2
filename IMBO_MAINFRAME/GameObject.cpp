#include "stdafx.h"
#include "GameObject.h"

#include "RenderContainer.h"
#include "RenderContainerSeller.h"
#include "FileBasedMesh.h"
#include "TerrainContainer.h"
#include "Animater.h"
bool CGameObject::Begin() {

	XMStoreFloat4x4(&m_xmf4x4World, XMMatrixIdentity());

	m_pRenderContainer = RCSELLER->GetRenderContainer(m_name);
	if (m_pRenderContainer->GetMesh())//mesh가 있으면
	{//aabb 해당 mesh에서 aabb를 얻어온다.
		m_OriBoundingBox = m_pRenderContainer->GetMesh()->GetAABB();
	}
	else {//없으면 최대 최소 aabb를 얻어온다.
		BoundingBox::CreateFromPoints(m_OriBoundingBox, XMVectorSet(+10.f, +10.f, +10.f, 0.f), XMVectorSet(-10.f, -10.f, -10.f, 0.f));
	}

	if (m_pRenderContainer->GetAnimater()) {
		m_pAnimater = m_pRenderContainer->GetAnimater();
		//m_pAnimater->GetAnimationInfo()->InitCurFrame();
	}

	XMStoreFloat4(&m_xmf4Quaternion, XMQuaternionIdentity());
	return true;
}
bool CGameObject::End() {
	m_vObjectActiveOBBs.clear();
	//-------------------------------component---------------------------
	ClearComponents();
	//-------------------------------component---------------------------
	return true;
}

void CGameObject::Animate(float fTimeElapsed) {
	//if (m_pAnimater) {
	//	//obb animate
	//	m_vObjectActiveOBBs.clear();
	//	for (auto OBB : m_pAnimater->GetActiveOBBs()) {
	//		OBB.Transform(OBB, m_pAnimater->GetMeshOffsetMtx()*GetWorldMtx());
	//		m_vObjectActiveOBBs.push_back(OBB);
	//		DEBUGER->RegistOBB(OBB);
	//	}
	//}

	DEBUGER->RegistCoordinateSys(GetWorldMtx());

	//모든 컴포넌트를 돌면서 Update실행
	for (auto i : m_mapComponents) {
		i.second->Update(fTimeElapsed);
	}
}
void CGameObject::Move(XMVECTOR xmvDir, float fDistance) {

	XMVECTOR pos;
	pos = GetPosition() + XMVector3Normalize(xmvDir)*fDistance;

	//추가 
	if (m_indexNaviMesh != -1) {
		XMFLOAT3 xmf3Pos;
		XMStoreFloat3(&xmf3Pos, pos);
		//미리 가봐서 
		if (CNaviObjectManager::IsIntersection(xmf3Pos.x, xmf3Pos.z, m_indexNaviMesh)) {
			//그 자리가 아직 내 navi mesh안이라면
			xmf3Pos.y = CNaviObjectManager::GetHeight(XMFLOAT2(xmf3Pos.x, xmf3Pos.z), m_indexNaviMesh);
		}
		else {
			//아니면 index를 새로 구하는데 만약 navi mesh위가 아니라면
			int new_index = CNaviObjectManager::GetIndex(xmf3Pos.x, xmf3Pos.z, m_indexNaviMesh);
			if (new_index != -1) {
				//만약 다른 navi mesh위에 내가 위치하게 됬다면.
				//navi mesh index를 갱신하고 내 높이도 갱신
				m_indexNaviMesh = new_index;
				xmf3Pos.y = CNaviObjectManager::GetHeight(XMFLOAT2(xmf3Pos.x, xmf3Pos.z), m_indexNaviMesh);
			}
			else {
				//만약 내가 navi mesh위에 있지 않다면
				//나는 이동하지 않는다.
				pos = GetPosition();
			}
		}
	}



	SetPosition(pos);
}
void CGameObject::Rotate(XMMATRIX xmMtx) {
	XMMATRIX xmmtxRotate = XMMatrixMultiply(xmMtx, XMLoadFloat4x4(&m_xmf4x4World));
	XMStoreFloat4x4(&m_xmf4x4World, xmmtxRotate);
	SetQuaternion(XMQuaternionRotationMatrix(GetWorldMtx()));
}
void CGameObject::Rotate(float x, float y, float z) {
	XMMATRIX xmmtxRotate;
	if (x != 0.0f)
	{
		xmmtxRotate = XMMatrixRotationAxis(GetRight(), (float)XMConvertToRadians(x));
		XMStoreFloat4x4(&m_xmf4x4World, XMMatrixMultiply(xmmtxRotate, XMLoadFloat4x4(&m_xmf4x4World)));
		//SetRotationQuaternion(XMQuaternionRotationAxis(GetRight(), x));
	}
	if (y != 0.0f)
	{
		//플레이어의 로컬 y-축을 기준으로 회전하는 행렬을 생성한다.
		xmmtxRotate = XMMatrixRotationAxis(GetUp(), (float)XMConvertToRadians(y));
		XMStoreFloat4x4(&m_xmf4x4World, XMMatrixMultiply(xmmtxRotate, XMLoadFloat4x4(&m_xmf4x4World)));
		//SetRotationQuaternion(XMQuaternionRotationAxis(GetUp(), y));
	}
	if (z != 0.0f)
	{
		//플레이어의 로컬 z-축을 기준으로 회전하는 행렬을 생성한다.
		xmmtxRotate = XMMatrixRotationAxis(GetLook(), (float)XMConvertToRadians(z));
		XMStoreFloat4x4(&m_xmf4x4World, XMMatrixMultiply(xmmtxRotate, XMLoadFloat4x4(&m_xmf4x4World)));
		//SetRotationQuaternion(XMQuaternionRotationAxis(GetLook(), z));
	}
	
	XMStoreFloat4(&m_xmf4Quaternion, XMQuaternionMultiply(XMQuaternionRotationRollPitchYaw(x, y, z), XMLoadFloat4(&m_xmf4Quaternion)));
	//SetQuaternion(XMQuaternionRotationMatrix(GetWorldMtx()));
}

void CGameObject::RotateWorldAxis(float x, float y, float z) {
	XMMATRIX xmmtxRotate;
	if (x != 0.0f)
	{
		xmmtxRotate = XMMatrixRotationAxis(XMVectorSet(1.f,0.f,0.f,0.f), (float)XMConvertToRadians(x));
		XMStoreFloat4x4(&m_xmf4x4World, XMMatrixMultiply(xmmtxRotate, XMLoadFloat4x4(&m_xmf4x4World)));
		XMStoreFloat4(&m_xmf4Quaternion, XMQuaternionMultiply(XMQuaternionRotationAxis(GetRight(), x), XMLoadFloat4(&m_xmf4Quaternion)));
	}
	if (y != 0.0f)
	{
		//플레이어의 로컬 y-축을 기준으로 회전하는 행렬을 생성한다.
		xmmtxRotate = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), (float)XMConvertToRadians(y));
		XMStoreFloat4x4(&m_xmf4x4World, XMMatrixMultiply(xmmtxRotate, XMLoadFloat4x4(&m_xmf4x4World)));
		XMStoreFloat4(&m_xmf4Quaternion, XMQuaternionMultiply(XMQuaternionRotationAxis(GetUp(), y), XMLoadFloat4(&m_xmf4Quaternion)));
	}
	if (z != 0.0f)
	{
		//플레이어의 로컬 z-축을 기준으로 회전하는 행렬을 생성한다.
		xmmtxRotate = XMMatrixRotationAxis(XMVectorSet(0.f, 0.f, 1.f, 0.f), (float)XMConvertToRadians(z));
		XMStoreFloat4x4(&m_xmf4x4World, XMMatrixMultiply(xmmtxRotate, XMLoadFloat4x4(&m_xmf4x4World)));
		XMStoreFloat4(&m_xmf4Quaternion, XMQuaternionMultiply(XMQuaternionRotationAxis(GetLook(), z), XMLoadFloat4(&m_xmf4Quaternion)));
	}

	m_xmf3Rotate = XMFLOAT3{ m_xmf3Rotate.x + x, m_xmf3Rotate.y + y, m_xmf3Rotate.z + z };
	//SetQuaternion(XMQuaternionRotationMatrix(GetWorldMtx()));
}
void CGameObject::SetPosition(XMVECTOR pos) {
	XMStoreFloat3(&m_xmf3Position, pos);

	m_xmf4x4World._41 = m_xmf3Position.x;
	m_xmf4x4World._42 = m_xmf3Position.y;
	m_xmf4x4World._43 = m_xmf3Position.z;
	if (false == m_bJump &&m_pTerrainContainer) {
		m_xmf4x4World._42 = GetTerrainHeight();
		m_xmf3Position.y = GetTerrainHeight();
	}
}
void CGameObject::SetPositionX(const float pos){
	m_xmf4x4World._41 = pos;
	m_xmf3Position.x = pos;
}
void CGameObject::SetPositionY(const float pos) {
	m_xmf4x4World._42 = pos;
	m_xmf3Position.y = pos;
}
void CGameObject::SetPositionZ(const float pos) {
	m_xmf4x4World._43 = pos;
	m_xmf3Position.z = pos;
}
void CGameObject::SetWorldMtx(XMMATRIX mtxWorld) {
	XMStoreFloat4x4(&m_xmf4x4World, mtxWorld);
	SetQuaternion(XMQuaternionRotationMatrix(mtxWorld));
	SetPosition(XMVectorSet(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43, 1.0f));
	//SetScale(XMVectorSet(1.f, 1.f, 1.f, 1.f));
}

void CGameObject::SetScale(XMVECTOR xmv){
	XMStoreFloat4(&m_xmf4Scale, xmv);
	SetWorldMtx(XMMatrixAffineTransformation(GetScale(), XMQuaternionIdentity(), GetQuaternion(), GetPosition()));
}

void CGameObject::SetRotation(XMMATRIX mtxRotation){
	XMFLOAT4X4 xmf4x4;
	XMStoreFloat4x4(&xmf4x4, mtxRotation);
	m_xmf4x4World._11 = xmf4x4._11, m_xmf4x4World._12 = xmf4x4._12, m_xmf4x4World._13 = xmf4x4._13;
	m_xmf4x4World._21 = xmf4x4._21, m_xmf4x4World._22 = xmf4x4._22, m_xmf4x4World._23 = xmf4x4._23;
	m_xmf4x4World._31 = xmf4x4._31, m_xmf4x4World._32 = xmf4x4._32, m_xmf4x4World._33 = xmf4x4._33;

}

void CGameObject::SetRight(XMVECTOR xmvRight) {
	XMFLOAT3 xmfRight;
	XMStoreFloat3(&xmfRight, xmvRight);
	m_xmf4x4World._11 = xmfRight.x;
	m_xmf4x4World._12 = xmfRight.y;
	m_xmf4x4World._13 = xmfRight.z;
}
void CGameObject::SetUp(XMVECTOR xmvUp) {
	XMFLOAT3 xmfUp;
	XMStoreFloat3(&xmfUp, xmvUp);
	m_xmf4x4World._21 = xmfUp.x;
	m_xmf4x4World._22 = xmfUp.y;
	m_xmf4x4World._23 = xmfUp.z;
}
void CGameObject::SetLook(XMVECTOR xmvLook) {
	XMFLOAT3 xmfLook;
	XMStoreFloat3(&xmfLook, xmvLook);
	m_xmf4x4World._31 = xmfLook.x;
	m_xmf4x4World._32 = xmfLook.y;
	m_xmf4x4World._33 = xmfLook.z;
}
bool CGameObject::IsCollision(CGameObject * pOther){
	//if (false == pOther->GetActive()) return false;
	
	if (m_pAnimater == nullptr && pOther->GetAnimater() == nullptr) {
		return IsCollisionMeshToMesh(pOther);
	}
	else if (m_pAnimater && pOther->GetAnimater()) {
		return IsCollisionAnimToAnim(pOther);
	}
	else if (m_pAnimater) {
		return IsCollisionAnimToMesh(pOther);
	}
	else if (pOther->GetAnimater()) {
		return pOther->IsCollisionAnimToMesh(this);
	}
}
bool CGameObject::IsCollisionMeshToMesh(CGameObject * pOther){
	CMesh* pMesh1 = GetRenderContainer()->GetMesh(0);
	CMesh * pMesh2 = pOther->GetRenderContainer()->GetMesh(0);

	BoundingBox Myaabb = pMesh1->GetAABB();
	BoundingBox Otheraabb = pMesh2->GetAABB();
	Myaabb.Transform(Myaabb, GetWorldMtx());
	Otheraabb.Transform(Otheraabb, pOther->GetWorldMtx());

	bool val = Myaabb.Intersects(Otheraabb);
	if (val) {
		//mesh - mesh
		int MynObb = pMesh1->GetOBBCnt();
		int OthernObb = pMesh2->GetOBBCnt();
		//int nCollisio{ 0 };

		if (MynObb > 0 && OthernObb > 0) {
			val = false;
			size_t nMyObb = pMesh1->GetvOBBObject().size();
			size_t nOtherObb = pMesh2->GetvOBBObject().size();
			for (size_t i = 0; i < nMyObb; ++i) {
				for (size_t j = 0; j < nOtherObb; ++j) {
					BoundingOrientedBox Myobb = pMesh1->GetvOBBObject()[i].GetOBB();
					Myobb.Transform(Myobb, GetWorldMtx());
					DEBUGER->RegistOBB(Myobb, GetUTag());

					BoundingOrientedBox Otherobb = pMesh2->GetvOBBObject()[j].GetOBB();
					Otherobb.Transform(Otherobb, pOther->GetWorldMtx());
					DEBUGER->RegistOBB(Otherobb, pOther->GetUTag());

					if (Myobb.Intersects(Otherobb)) return true;
						//nCollisio++;
				}
			}
			//if (nCollisio > 0)
			//	val = true;
		}
		else if (MynObb > 0) {
			val = false;
			//int nCollisio{ 0 };
			size_t nMyObb = pMesh1->GetvOBBObject().size();
			
			for (size_t i = 0; i < nMyObb; ++i) {

				BoundingOrientedBox Myobb = pMesh1->GetvOBBObject()[i].GetOBB();
				Myobb.Transform(Myobb, GetWorldMtx());
				DEBUGER->RegistOBB(Myobb, GetUTag());
				if (Otheraabb.Intersects(Myobb)) return true;
					//nCollisio++;
			}
			//if (nCollisio > 0)
			//	val = true;
		}
		else if (OthernObb > 0) {
			val = false;
			//int nCollisio{ 0 };
			size_t nOtherObb = pMesh2->GetvOBBObject().size();
			for (size_t j = 0; j < nOtherObb; ++j) {

				BoundingOrientedBox Otherobb = pMesh2->GetvOBBObject()[j].GetOBB();
				Otherobb.Transform(Otherobb, pOther->GetWorldMtx());
				DEBUGER->RegistOBB(Otherobb, pOther->GetUTag());
				if (Myaabb.Intersects(Otherobb)) return true;
					//nCollisio++;
			}
			//if (nCollisio > 0)
			//	val = true;
		}

	}
	return val;
}
bool CGameObject::IsCollisionAnimToAnim(CGameObject * pOther){
	//animation - animation
	CAnimater* pOtherAnimater = pOther->GetAnimater();
	BoundingBox Myaabb = m_pAnimater->GetMainAABB()->GetAABB();
	BoundingBox Otheraabb = pOtherAnimater->GetMainAABB()->GetAABB();

	Myaabb.Transform(Myaabb, m_pAnimater->GetMeshOffsetMtx()*GetWorldMtx());
	Otheraabb.Transform(Otheraabb, pOtherAnimater->GetMeshOffsetMtx() * pOther->GetWorldMtx());

	bool val = Myaabb.Intersects(Otheraabb);
	if (val) {
		CAnimationInfo* pMyAnimationInfo = m_pAnimater->GetCurAnimationInfo();
		CAnimationInfo* pOtherAnimationInfo = pOtherAnimater->GetCurAnimationInfo();

		int MynObb = pMyAnimationInfo->GetvActiveOBBs().size();
		int OthernObb = pOtherAnimationInfo->GetvActiveOBBs().size();
		//int nCollisio{ 0 };

		if (MynObb > 0 && OthernObb > 0) {//animater - animater
			val = false;

			for (auto pMyOBB : pMyAnimationInfo->GetvActiveOBBs()) {
				for (auto pOtherOBB : pOtherAnimationInfo->GetvActiveOBBs()) {
					BoundingOrientedBox Myobb = pMyOBB->GetOBB();
					XMMATRIX xmmtxOffset = m_pAnimater->GetMeshOffsetMtx();
					XMMATRIX xmmtxFrame = pMyAnimationInfo->GetCurFrameMtx(pMyOBB->GetMyJointIndex());
					Myobb.Transform(Myobb, xmmtxFrame * xmmtxOffset * GetWorldMtx());
					DEBUGER->RegistOBB(Myobb, GetUTag());

					BoundingOrientedBox Otherobb = pOtherOBB->GetOBB();
					xmmtxOffset = pOtherAnimater->GetMeshOffsetMtx();
					xmmtxFrame = pOtherAnimationInfo->GetCurFrameMtx(pOtherOBB->GetMyJointIndex());
					Otherobb.Transform(Otherobb, xmmtxFrame * xmmtxOffset * pOther->GetWorldMtx());
					DEBUGER->RegistOBB(Otherobb, pOther->GetUTag());

					if (Myobb.Intersects(Otherobb)) return true;
						//nCollisio++;
				}
			}
			//if (nCollisio > 0)
			//	val = true;
		}
		else if (MynObb > 0) {//animater - aabb
			val = false;
			//int nCollisio{ 0 };
			CAnimationInfo* pMyAnimationInfo = m_pAnimater->GetCurAnimationInfo();
			for (auto pMyOBB : m_pAnimater->GetCurAnimationInfo()->GetvActiveOBBs()) {

				BoundingOrientedBox Myobb = pMyOBB->GetOBB();
				XMMATRIX xmmtxOffset = m_pAnimater->GetMeshOffsetMtx();
				XMMATRIX xmmtxFrame = pMyAnimationInfo->GetCurFrameMtx(pMyOBB->GetMyJointIndex());
				Myobb.Transform(Myobb, xmmtxFrame * xmmtxOffset * GetWorldMtx());
				DEBUGER->RegistOBB(Myobb, GetUTag());
				if (Otheraabb.Intersects(Myobb)) return true;
				//	nCollisio++;
			}
			//if (nCollisio > 0)
			//	val = true;
		}
		else if (OthernObb > 0) {//aabb - animater
			val = false;
			int nCollisio{ 0 };
			CAnimationInfo* pOtherAnimationInfo = pOtherAnimater->GetCurAnimationInfo();
			for (auto pOtherOBB : pOtherAnimater->GetCurAnimationInfo()->GetvActiveOBBs()) {

				BoundingOrientedBox Otherobb = pOtherOBB->GetOBB();
				XMMATRIX xmmtxOffset = pOtherAnimater->GetMeshOffsetMtx();
				XMMATRIX xmmtxFrame = pOtherAnimationInfo->GetCurFrameMtx(pOtherOBB->GetMyJointIndex());
				Otherobb.Transform(Otherobb, xmmtxFrame * xmmtxOffset * pOther->GetWorldMtx());
				DEBUGER->RegistOBB(Otherobb, pOther->GetUTag());

				if (Myaabb.Intersects(Otherobb)) return true;
				//	nCollisio++;
			}
			//if (nCollisio > 0)
			//	val = true;
		}

	}
	return val;
}
bool CGameObject::IsCollisionAnimToMesh(CGameObject * pOther){

	CMesh* pOtherMesh = pOther->GetRenderContainer()->GetMesh(0);
	CAnimationInfo* pMyAnimationInfo = m_pAnimater->GetCurAnimationInfo();

	BoundingBox Myaabb = m_pAnimater->GetMainAABB()->GetAABB();
	BoundingBox Otheraabb = pOtherMesh->GetAABB();

	Myaabb.Transform(Myaabb, m_pAnimater->GetMeshOffsetMtx()*GetWorldMtx());
	Otheraabb.Transform(Otheraabb, pOther->GetWorldMtx());

	bool val = Myaabb.Intersects(Otheraabb);
	if (val) {

		//animation - mesh
		if (m_pAnimater) {
			int MynObb = pMyAnimationInfo->GetvActiveOBBs().size();
			int OthernObb = pOtherMesh->GetOBBCnt();
			//int nCollisio{ 0 };

			if (MynObb > 0 && OthernObb > 0) {//animater - mesh
				val = false;
				size_t nOtherObb = pOtherMesh->GetvOBBObject().size();
				for (auto pOBB : pMyAnimationInfo->GetvActiveOBBs()) {
					for (size_t j = 0; j < nOtherObb; ++j) {
						BoundingOrientedBox Myobb = pOBB->GetOBB();
						XMMATRIX xmmtxOffset = m_pAnimater->GetMeshOffsetMtx();
						XMMATRIX xmmtxFrame = pMyAnimationInfo->GetCurFrameMtx(pOBB->GetMyJointIndex());
						Myobb.Transform(Myobb, xmmtxFrame * xmmtxOffset * GetWorldMtx());
						DEBUGER->RegistOBB(Myobb, UTAG_COLLISION);

						BoundingOrientedBox Otherobb = pOtherMesh->GetvOBBObject()[j].GetOBB();
						Otherobb.Transform(Otherobb, pOther->GetWorldMtx());
						DEBUGER->RegistOBB(Otherobb, UTAG_COLLISION);

						if (Myobb.Intersects(Otherobb)) return true;
						//	nCollisio++;
					}
				}
				//if (nCollisio > 0)
				//	val = true;
			}
			else if (MynObb > 0) {//animater - aabb
				val = false;
				//int nCollisio{ 0 };
				for (auto pOBB : pMyAnimationInfo->GetvActiveOBBs()) {

					BoundingOrientedBox Myobb = pOBB->GetOBB();
					XMMATRIX xmmtxOffset = m_pAnimater->GetMeshOffsetMtx();
					XMMATRIX xmmtxFrame = pMyAnimationInfo->GetCurFrameMtx(pOBB->GetMyJointIndex());
					Myobb.Transform(Myobb, xmmtxFrame * xmmtxOffset * GetWorldMtx());
					DEBUGER->RegistOBB(Myobb, UTAG_COLLISION);
					if (Otheraabb.Intersects(Myobb)) return true;
						//nCollisio++;
				}
				//if (nCollisio > 0)
				//	val = true;
			}
			else if (OthernObb > 0) {//aabb - mesh
				val = false;
				//int nCollisio{ 0 };
				for (int i = 0; i < OthernObb; ++i) {

					BoundingOrientedBox Otherobb = pOther->GetRenderContainer()->GetMesh(0)->GetOBBObject(i).GetOBB();
					Otherobb.Transform(Otherobb, pOther->GetWorldMtx());
					DEBUGER->RegistOBB(Otherobb, UTAG_COLLISION);
					if (Myaabb.Intersects(Otherobb)) return true;
					//	nCollisio++;
				}
				//if (nCollisio > 0)
				//	val = true;
			}
		}
		//mesh - animation
		else if (pOther->GetAnimater()) {
			CAnimater* pOtherAniamter = pOther->GetAnimater();
			int MynObb = GetRenderContainer()->GetMesh(0)->GetOBBCnt();
			int OthernObb = pOtherAniamter->GetCurAnimationInfo()->GetvActiveOBBs().size();
			//int nCollisio{ 0 };
			CAnimationInfo* pOtherAnimationInfo = pOtherAniamter->GetCurAnimationInfo();

			if (MynObb > 0 && OthernObb > 0) {//mesh - animater
				val = false;
				for (auto pOtherOBB : pOtherAniamter->GetCurAnimationInfo()->GetvActiveOBBs()) {
					for (int j = 0; j < MynObb; ++j) {
						BoundingOrientedBox Otherobb = pOtherOBB->GetOBB();
						XMMATRIX xmmtxOffset = pOtherAniamter->GetMeshOffsetMtx();
						XMMATRIX xmmtxFrame = pOtherAnimationInfo->GetCurFrameMtx(pOtherOBB->GetMyJointIndex());
						Otherobb.Transform(Otherobb, xmmtxFrame * xmmtxOffset * pOther->GetWorldMtx());
						DEBUGER->RegistOBB(Otherobb, UTAG_COLLISION);

						BoundingOrientedBox Myobb = GetRenderContainer()->GetMesh(0)->GetOBBObject(j).GetOBB();
						Myobb.Transform(Myobb, GetWorldMtx());
						DEBUGER->RegistOBB(Myobb, UTAG_COLLISION);

						if (Myobb.Intersects(Otherobb)) return true;
						//	nCollisio++;
					}
				}
				//if (nCollisio > 0)
				//	val = true;
			}
			else if (MynObb > 0) {//mesh - aabb
				val = false;
				//int nCollisio{ 0 };
				for (int i = 0; i < MynObb; ++i) {

					BoundingOrientedBox Myobb = GetRenderContainer()->GetMesh(0)->GetOBBObject(i).GetOBB();
					Myobb.Transform(Myobb, GetWorldMtx());
					DEBUGER->RegistOBB(Myobb, UTAG_COLLISION);
					if (Myobb.Intersects(Otheraabb)) return true;
					//	nCollisio++;
				}
				//if (nCollisio > 0)
				//	val = true;

			}
			else if (OthernObb > 0) {//aabb - mesh

				val = false;
				//int nCollisio{ 0 };
				for (auto pOBB : m_pAnimater->GetCurAnimationInfo()->GetvActiveOBBs()) {

					BoundingOrientedBox Myobb = pOBB->GetOBB();
					XMMATRIX xmmtxOffset = m_pAnimater->GetMeshOffsetMtx();
					XMMATRIX xmmtxFrame = pOtherAnimationInfo->GetCurFrameMtx(pOBB->GetMyJointIndex());
					Myobb.Transform(Myobb, xmmtxFrame * xmmtxOffset * GetWorldMtx());
					DEBUGER->RegistOBB(Myobb, UTAG_COLLISION);
					if (Otheraabb.Intersects(Myobb)) return true;
					//	nCollisio++;
				}
				//if (nCollisio > 0)
				//	val = true;
			}
		}
	}

	return val;
}
//--------------------------------getter--------------------------------
XMVECTOR CGameObject::GetRight() {
	XMFLOAT3 xmvPos;

	xmvPos.x = m_xmf4x4World._11;
	xmvPos.y = m_xmf4x4World._12;
	xmvPos.z = m_xmf4x4World._13;

	return XMLoadFloat3(&xmvPos);
}
XMVECTOR CGameObject::GetUp() {
	XMFLOAT3 xmvPos;

	xmvPos.x = m_xmf4x4World._21;
	xmvPos.y = m_xmf4x4World._22;
	xmvPos.z = m_xmf4x4World._23;

	return XMLoadFloat3(&xmvPos);
}
XMVECTOR CGameObject::GetLook() {
	XMFLOAT3 xmvPos;

	xmvPos.x = m_xmf4x4World._31;
	xmvPos.y = m_xmf4x4World._32;
	xmvPos.z = m_xmf4x4World._33;

	return XMLoadFloat3(&xmvPos);
}
XMVECTOR CGameObject::GetPosition() {
	XMFLOAT3 xmvPos;

	xmvPos.x = m_xmf4x4World._41;
	xmvPos.y = m_xmf4x4World._42;
	xmvPos.z = m_xmf4x4World._43;

	return XMLoadFloat3(&xmvPos);
}
XMMATRIX CGameObject::GetWorldMtx() {
	return XMLoadFloat4x4(&m_xmf4x4World);
}
//--------------------------------getter--------------------------------

//-------------------------------componenet----------------------------
//같은 Family의 component는 set할 수 없다. 
bool CGameObject::SetComponent(CComponent* pComponenet) {
	if (!pComponenet) return false;

	//component id검사 이상한 id이면 패스
	//family검사도 해준다.
	for (int i = 0; i < COMPONENTID_END; ++i) {
		if (i == pComponenet->GetID()) {
			for (auto data : m_mapComponents) {
				//family id가 같은것이 있다면 안넣어 줌
				if (data.second->GetFamilyID() == pComponenet->GetFamilyID())
					return false;
			}

			//최종적으로 검사 다 끝나면 넣어줌
			m_mapComponents.insert(pairComponent(pComponenet->GetID(), pComponenet));
			pComponenet->SetOwner(this);
			return true;
		}
	}

	return false;
}

//Get
CComponent* CGameObject::GetComponenet(const component_id& componenetID) {
	mapComponent::const_iterator  find_iterator = m_mapComponents.find(componenetID);
	return find_iterator->second;
}
const CComponent* CGameObject::GetComponenet(const component_id& componenetID)const {
	mapComponent::const_iterator  find_iterator = m_mapComponents.find(componenetID);
	return find_iterator->second;
}

//Clear
void CGameObject::ClearComponents() {
	//모든 컴포넌트를 돌면서 Update실행
	for (auto i : m_mapComponents) {
		i.second->End();
		delete i.second;
	}
	m_mapComponents.clear();
}
//-------------------------------componenet----------------------------

//instance buffer controll base
void CGameObject::SetBufferInfo(void** ppMappedResources, int& nInstance,  CCamera* pCamera) {
	//형변환
	VS_VB_INSTANCE *pnInstances = (VS_VB_INSTANCE *)ppMappedResources[0];
	
	//transpose 이후 정보 주입
	if (m_pAnimater) {
		pnInstances[nInstance].m_xmmtxWorld = XMMatrixTranspose(m_pAnimater->GetMeshOffsetMtx()*GetWorldMtx());
		return;
	}
	pnInstances[nInstance].m_xmmtxWorld = XMMatrixTranspose(GetWorldMtx());

}
void CGameObject::RegistToContainer() {
	m_pRenderContainer->AddObject(this);
}

void CGameObject::RegistToDebuger(){
	BoundingBox BoundingBox;
	GetMainBoundingBox(BoundingBox);
	DEBUGER->RegistAABB(BoundingBox, m_utag);
}

//void CGameObject::SetRenderContainer(CRenderContainerSeller * pSeller) {
//	m_pRenderContainer = pSeller->GetRenderContainer(m_objectID);
//	if (m_pRenderContainer->GetMesh())//mesh가 있으면
//	{//aabb 해당 mesh에서 aabb를 얻어온다.
//		m_OriBoundingBox = m_pRenderContainer->GetMesh()->GetBoundingBox();
//	}
//	else {//없으면 최대 최소 aabb를 얻어온다.
//		BoundingBox::CreateFromPoints(m_OriBoundingBox, XMLoadFloat3(&XMFLOAT3(+FLT_MAX, +FLT_MAX, +FLT_MAX)), XMLoadFloat3(&XMFLOAT3(-FLT_MAX, -FLT_MAX, -FLT_MAX)));
//	}
//}
float CGameObject::GetTerrainHeight(){

	return m_pTerrainContainer->GetHeight(GetPosition(), m_indexNaviMesh);
//	return 100.0f;
}
//flustum culling
bool CGameObject::IsVisible( CCamera* pCamera){
	BoundingBox BoundingBox;
	
	m_bIsVisible = (m_bActive) ? true : false;
	if (m_bActive){
		GetMainBoundingBox(BoundingBox);
		if (pCamera) m_bIsVisible = pCamera->IsInFrustum(BoundingBox);
	}
	return(m_bIsVisible);
}

void CGameObject::GetMainBoundingBox(BoundingBox& out){
	out = m_OriBoundingBox;
	if (m_pAnimater) {
		out = m_pAnimater->GetMainAABB()->GetAABB();
		out.Transform(out, m_pAnimater->GetMeshOffsetMtx()*GetWorldMtx());
		return;
	}

	out.Transform(out, GetWorldMtx());
}

bool CGameObject::CheckPickObject(XMVECTOR xmvWorldCameraStartPos, XMVECTOR xmvRayDir, float & distance){
	BoundingBox BoundingBox;
	GetMainBoundingBox(BoundingBox);
	return BoundingBox.Intersects(xmvWorldCameraStartPos, xmvRayDir, distance);
}

void CGameObject::PickingProc(){
	
}

void CGameObject::LoadInfo(){
	//tag
	m_tag = (tag)IMPORTER->ReadUINT();
	m_utag = (utag)IMPORTER->ReadUINT();

	//name
	m_name = IMPORTER->Readstring();
	
	//worldmtx
	m_xmf4x4World = IMPORTER->ReadFloat4x4();
	SetWorldMtx(XMLoadFloat4x4(&m_xmf4x4World));
}

CGameObject* CGameObject::CreateObject(string name, tag t, XMMATRIX xmmtxWorld){
	CGameObject* pObject = new CGameObject(name, t);
	pObject->Begin();
	pObject->SetWorldMtx(xmmtxWorld);
	return pObject;
}


void CGameObject::SetNaviMeshIndex(int index) {
	if (CNaviObjectManager::IsValiableIndex(index)) {
		m_indexNaviMesh = index;
		SetPosition(CNaviObjectManager::GetNaviMeshPosition(index));
	}
}

void CGameObject::SetNaviMeshIndex() {
	m_indexNaviMesh = CNaviObjectManager::GetValiableIndex(GetPosition());

	if (m_indexNaviMesh == -1) SetNaviMeshIndex(0);
	else SetNaviMeshIndex(m_indexNaviMesh);
}

//생성자는 위에서부터 
CGameObject::CGameObject(string name, tag t) : CObject(name, t) {
	XMStoreFloat4x4(&m_xmf4x4World, XMMatrixIdentity());
}
CGameObject ::~CGameObject() {
	CGameObject* p = this;
}