#include "stdafx.h"
#include "Loading.h"
#include "SceneMgr.h"

#include "SCHeroSel.h"
#include "SCOriTown.h"
#include "SCTitle.h"

#include "LoadingBack.h"
#include "ImageUI.h"

#include "EffectMgr.h"

CLoading::CLoading(SCENE_ID eID, CDirectXFramework* pFrameWork)
	:CScene(eID)
{
	m_pFrameWork = pFrameWork;
}
CLoading::~CLoading()
{
	End();
}

bool CLoading::Begin()
{
	m_bIsLoading = true;
	UPDATER->GetSkyBoxContainer()->SetActive(false);
	UPDATER->GetTerrainContainer()->SetActive(false);

	RESOURCEMGR->ReleaseAllResource();
	RCSELLER->ClearStempRenderContainer();

	RESOURCEMGR->CreateTexture("Logo", _T("../../Assets/Loading/Logo.png"), PS_TEXTURE, BIND_PS);
	RESOURCEMGR->CreateTexture("Logo2", _T("../../Assets/Loading/Logo2.png"), PS_TEXTURE, BIND_PS);

	string strName = rand() % 2 == 0 ? "Logo" : "Logo2";
	RESOURCEMGR->CreateTexture("Loading", _T("../../Assets/Loading/LoadingText.png"), PS_TEXTURE, BIND_PS);

	CUIObject* pUI = CLoadingBack::Create(XMLoadFloat2(&XMFLOAT2(WINSIZEX * 0.5f, WINSIZEY * 0.5f)), XMLoadFloat2(&XMFLOAT2(250.f, 250.f)), StringToTCHAR(strName));
	m_vecUI.push_back(pUI);

	strName = "Loading";
	pUI = CImageUI::Create(XMLoadFloat2(&XMFLOAT2(WINSIZEX * 0.8f, WINSIZEY * 0.9f)), XMLoadFloat2(&XMFLOAT2(120.f, 30.f)), StringToTCHAR(strName), 1.f);
	m_vecUI.push_back(pUI);

	m_hLoadThread = CreateThread(NULL, 0, FuncLoadResourceThread, this, 0, NULL);

	return true;
}
bool CLoading::End()
{
	RENDERER->GetUIRenderer()->ClearData();
	size_t iVecSize = m_vecUI.size();
	for (size_t i = 0; i < iVecSize; ++i)
	{
		delete m_vecUI[i];
		m_vecUI[i] = nullptr;
	}
	m_vecUI.clear();

	return true;
}

void CLoading::Animate(float fTimeElapsed)
{
	Sleep(10);
	size_t iVecSize = m_vecUI.size();
	for (size_t i = 0; i < iVecSize; ++i)
	{
		m_vecUI[i]->Update(fTimeElapsed);
	}

	if (true == m_bComplete)
	{
		CScene*	pScene = nullptr;
		switch (m_eSceneID) {
		case SCN_TITLE:			pScene = new CSCTitle(m_eSceneID, m_pFrameWork);			break;
		case SCN_HEROSEL:		pScene = new CSCHeroSel(m_eSceneID, m_pFrameWork);			break;
		case SCN_ORITOWN:		pScene = new CSCOriTown(m_eSceneID, m_pFrameWork);			break;
		}

		m_bIsLoading = false;
		UPDATER->GetSkyBoxContainer()->SetActive(true);
		UPDATER->GetTerrainContainer()->SetActive(true);
		CSceneMgr::GetInstance()->SetPresentScene(m_eSceneID, pScene);

		End();
		delete this;
	}
}

void CLoading::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
}

void CLoading::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
}

void CLoading::ProcessInput(float fTimeElapsed) 
{

}

DWORD WINAPI CLoading::FuncLoadResourceThread(LPVOID arg)
{
	// 계속 추가하는애들 함수로 ㄱㄱ
	RESOURCEMGR->CreateTexture("DEFAULT", _T("../../Assets/default.jpg"), PS_TEXTURE, BIND_PS);

	CLoading*	pMyThread = (CLoading*)arg;
	switch (pMyThread->GetSceneID())
	{
	case SCN_TITLE:
		pMyThread->LoadScene_TITLE();
		break;
	case SCN_LOBBY:
		pMyThread->LoadScene_LOBBY();
		break;
	case SCN_HEROSEL:
		pMyThread->LoadScene_HEROSEL();
		break;
	case SCN_ORITOWN:
		pMyThread->LoadScene_ORITOWN();
		break;
	case SCN_REPAIRTOWN:
		pMyThread->LoadScene_REPAIRTOWN();
		break;
	case SCN_ALDENAD:
		pMyThread->LoadScene_ALDENAD();
		break;
	case SCN_BOSS:
		pMyThread->LoadScene_BOSS();
		break;
	}
	pMyThread->SetLoadComplete(true);

	return 0;
}

void CLoading::LoadScene_TITLE()
{
	RESOURCEMGR->CreateTexture("Title", _T("../../Assets/Scene_Title/Title.png"), PS_TEXTURE, BIND_PS);
	RESOURCEMGR->CreateTexture("Press", _T("../../Assets/Scene_Title/Press.png"), PS_TEXTURE, BIND_PS);
}

void CLoading::LoadScene_LOBBY()
{
}
void CLoading::LoadScene_HEROSEL()
{
	RESOURCEMGR->CreateTexture("Back", _T("../../Assets/Scene_HeroSel/background.jpg"), PS_TEXTURE, BIND_PS);
	RESOURCEMGR->CreateTexture("CButton_0", _T("../../Assets/Scene_HeroSel/Char_Button_1.jpg"), PS_TEXTURE, BIND_PS);
	RESOURCEMGR->CreateTexture("CButton_1", _T("../../Assets/Scene_HeroSel/Char_Button_2.jpg"), PS_TEXTURE, BIND_PS);
	RESOURCEMGR->CreateTexture("CButton_2", _T("../../Assets/Scene_HeroSel/Char_Button_3.jpg"), PS_TEXTURE, BIND_PS);
	RESOURCEMGR->CreateTexture("CButton_3", _T("../../Assets/Scene_HeroSel/Char_Button_4.jpg"), PS_TEXTURE, BIND_PS);
	RESOURCEMGR->CreateTexture("CButton_4", _T("../../Assets/Scene_HeroSel/Char_Button_5.jpg"), PS_TEXTURE, BIND_PS);
	RESOURCEMGR->CreateTexture("CButton_5", _T("../../Assets/Scene_HeroSel/Char_Button_6.jpg"), PS_TEXTURE, BIND_PS);

	RESOURCEMGR->CreateTexture("Char_Select_0", _T("../../Assets/Scene_HeroSel/Char_Select_1.jpg"), PS_TEXTURE, BIND_PS);
	RESOURCEMGR->CreateTexture("Char_Select_1", _T("../../Assets/Scene_HeroSel/Char_Select_2.jpg"), PS_TEXTURE, BIND_PS);
	RESOURCEMGR->CreateTexture("Char_Select_2", _T("../../Assets/Scene_HeroSel/Char_Select_3.jpg"), PS_TEXTURE, BIND_PS);
	RESOURCEMGR->CreateTexture("Char_Select_3", _T("../../Assets/Scene_HeroSel/Char_Select_4.jpg"), PS_TEXTURE, BIND_PS);
	RESOURCEMGR->CreateTexture("Char_Select_4", _T("../../Assets/Scene_HeroSel/Char_Select_5.jpg"), PS_TEXTURE, BIND_PS);
	RESOURCEMGR->CreateTexture("Char_Select_5", _T("../../Assets/Scene_HeroSel/Char_Select_6.jpg"), PS_TEXTURE, BIND_PS);
	RESOURCEMGR->CreateTexture("Char_Select_D", _T("../../Assets/Scene_HeroSel/Char_Select_D.jpg"), PS_TEXTURE, BIND_PS);
	RESOURCEMGR->CreateTexture("Char_Select_N", _T("../../Assets/Scene_HeroSel/Char_Select_N.jpg"), PS_TEXTURE, BIND_PS);

	RESOURCEMGR->CreateTexture("Button_Ready", _T("../../Assets/Scene_HeroSel/Button_Ready.png"), PS_TEXTURE, BIND_PS);

	RESOURCEMGR->CreateTexture("Ready_0", _T("../../Assets/Scene_HeroSel/Ready_0.png"), PS_TEXTURE, BIND_PS);
	RESOURCEMGR->CreateTexture("Ready_1", _T("../../Assets/Scene_HeroSel/Ready_1.png"), PS_TEXTURE, BIND_PS);

	RESOURCEMGR->CreateMultiMesh("../../Assets/Scene_HeroSel/SelectElf01F.gjm", "SelectElf01F");
	RESOURCEMGR->CreateMultiMesh("../../Assets/Scene_HeroSel/SelectHum01M.gjm", "SelectHum01M");
	RESOURCEMGR->CreateMultiMesh("../../Assets/Scene_HeroSel/SelectHum02F.gjm", "SelectHum02F");
}
void CLoading::LoadScene_ORITOWN()
{
	RESOURCEMGR->CreateTexture("Char_Select_0", _T("../../Assets/Scene_HeroSel/Char_Select_1.jpg"), PS_TEXTURE, BIND_PS);
	RESOURCEMGR->CreateTexture("Char_Select_1", _T("../../Assets/Scene_HeroSel/Char_Select_2.jpg"), PS_TEXTURE, BIND_PS);
	RESOURCEMGR->CreateTexture("Char_Select_2", _T("../../Assets/Scene_HeroSel/Char_Select_3.jpg"), PS_TEXTURE, BIND_PS);
	RESOURCEMGR->CreateTexture("Char_Select_3", _T("../../Assets/Scene_HeroSel/Char_Select_4.jpg"), PS_TEXTURE, BIND_PS);
	RESOURCEMGR->CreateTexture("Char_Select_4", _T("../../Assets/Scene_HeroSel/Char_Select_5.jpg"), PS_TEXTURE, BIND_PS);
	RESOURCEMGR->CreateTexture("Char_Select_5", _T("../../Assets/Scene_HeroSel/Char_Select_6.jpg"), PS_TEXTURE, BIND_PS);

	RESOURCEMGR->CreateTexture("SkillBack", _T("../../Assets/Game_UI/SkillBack.png"), PS_TEXTURE, BIND_PS);

	RESOURCEMGR->CreateTexture("skicon1", _T("../../Assets/Game_UI/skicon1.tga"), PS_TEXTURE, BIND_PS);
	RESOURCEMGR->CreateTexture("skicon2", _T("../../Assets/Game_UI/skicon2.tga"), PS_TEXTURE, BIND_PS);
	RESOURCEMGR->CreateTexture("skicon3", _T("../../Assets/Game_UI/skicon3.tga"), PS_TEXTURE, BIND_PS);
	RESOURCEMGR->CreateTexture("skicon4", _T("../../Assets/Game_UI/skicon4.tga"), PS_TEXTURE, BIND_PS);

	CEffectMgr::GetInstance()->Load_EffectData(L"../../Assets/EffectData/Arrow_Skill1Shot.dat", L"Arrow_Skill1Shot");
	CEffectMgr::GetInstance()->Load_EffectData(L"../../Assets/EffectData/ShotArrow.dat", L"ShotArrow");
	CEffectMgr::GetInstance()->Load_EffectData(L"../../Assets/EffectData/Test2.dat", L"Test2");

	CEffectMgr::GetInstance()->Load_EffectData(L"../../Assets/EffectData/elf_sk1.dat", L"elf_sk1");
	CEffectMgr::GetInstance()->Load_EffectData(L"../../Assets/EffectData/elf_sk2.dat", L"elf_sk2");
	CEffectMgr::GetInstance()->Load_EffectData(L"../../Assets/EffectData/elf_sk3.dat", L"elf_sk3");
	CEffectMgr::GetInstance()->Load_EffectData(L"../../Assets/EffectData/elf_sk3-1.dat", L"elf_sk3-1");
	CEffectMgr::GetInstance()->Load_EffectData(L"../../Assets/EffectData/elf_sk4.dat", L"elf_sk4");

	CEffectMgr::GetInstance()->Load_EffectData(L"../../Assets/EffectData/hum2_sk1.dat", L"hum2_sk1");
	CEffectMgr::GetInstance()->Load_EffectData(L"../../Assets/EffectData/hum2_sk2.dat", L"hum2_sk2");
	CEffectMgr::GetInstance()->Load_EffectData(L"../../Assets/EffectData/hum2_sk3.dat", L"hum2_sk3");
	CEffectMgr::GetInstance()->Load_EffectData(L"../../Assets/EffectData/hum2_sk4.dat", L"hum2_sk4");

	CEffectMgr::GetInstance()->Load_EffectData(L"../../Assets/EffectData/TestBlood.dat", L"TestBlood");
	CEffectMgr::GetInstance()->Load_EffectData(L"../../Assets/EffectData/SparkTest.dat", L"SparkTest");

	CEffectMgr::GetInstance()->Load_EffectData(L"../../Assets/EffectData/hum3_sk1.dat", L"hum3_sk1");

	LoadScene("../../Assets/SceneResource/test/test.scn");

	RESOURCEMGR->CreateTexture("Bar", _T("../../Assets/Game_UI/HpBar.png"), PS_TEXTURE, BIND_PS);
	RESOURCEMGR->CreateTexture("Bar_Fill", _T("../../Assets/Game_UI/HpPoint.png"), PS_TEXTURE, BIND_PS);
}

void CLoading::LoadScene_REPAIRTOWN()
{
}

void CLoading::LoadScene_ALDENAD()
{
}

void CLoading::LoadScene_BOSS()
{
}
