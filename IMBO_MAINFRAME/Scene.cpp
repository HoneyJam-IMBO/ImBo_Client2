#include "stdafx.h"
#include "Scene.h"
#include "DirectXFramework.h"

bool CScene::Begin(string path){


	return true;
}
//m_vecUI delete가 없어서 추가함 나중에 문제되면 지움 다른데서 delete중일 수 있으나 없었음
bool CScene::End(){
	for (size_t i = 0; i < m_vecUI.size(); ++i) {
		delete m_vecUI[i];
		m_vecUI[i] = nullptr;
	}
	m_vecUI.clear();
	return true;
}

void CScene::Animate(float fTimeElapsed){

}

CScene* CScene::CreateScene(string name, CDirectXFramework* pFramework, SCENE_ID eID){
	CScene* pScene = new CScene(eID);
	pScene->LoadScene(name);
	pScene->SetFrameWork(pFramework);
	return pScene;
	//return nullptr;
}

void CScene::LoadScene(string path){

	//load scene
	string name = GetFileName(path);
	char meshRoot[256] = { "../../Assets/SceneResource/" };
	wstring wsMeshRoot;
	strcat(meshRoot, name.c_str());
	string sMeshRoot{ meshRoot };

	wsMeshRoot.assign(sMeshRoot.cbegin(), sMeshRoot.cend());
	LoadResource(wsMeshRoot);
}

void CScene::LoadResource(wstring wsMeshRoot){
	//read resource root 

	//RESOURCEMGR->ReleaseAllResource();
	//RCSELLER->ClearStempRenderContainer();

	//해당 dir아래의 모든 gjm파일을 로드
	vector<wstring> vFile;
	DIRECTORYFINDER->GetFiles(vFile, wsMeshRoot, true, true, L".gjm");
	DIRECTORYFINDER->GetFiles(vFile, wsMeshRoot, true, true, L".GJM");

	string name;
	for (auto fileName : vFile) {
		string sPath{ "" };
		sPath.assign(fileName.begin(), fileName.end());
		/*여기서 file 이름을 가지고 name을 정하도록 한다.
		file 명에서 확장자를 제거하면 될 것이다.
		sprintf(name, "파 일 명", i);*/
		name = GetFileName(sPath);
		RESOURCEMGR->CreateMultiMesh(sPath, name);
		//sprintf(name, "StempMesh%d", i++);
	}

	//RCSELLER->TestingRCAdd();
	RCSELLER->CreateStempRenderContainer();
}
