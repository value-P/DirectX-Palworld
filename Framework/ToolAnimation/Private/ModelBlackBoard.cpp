#include "pch.h"
#include "ModelBlackBoard.h"
#include "ToolAnimObject.h"
#include "Level.h"

#include "MeshComponent.h"
#include "Model.h"
#include "StringUtility.h"

IMPLEMENT_SINGLETON(CModelBlackBoard);

void ToolAnimation::CModelBlackBoard::SetCurrentLevel(shared_ptr<CLevel> _spCurrentLevel)
{
	m_wpCurrentLevel = _spCurrentLevel;
}

const vector<string>* ToolAnimation::CModelBlackBoard::GetCurrentAnimations()
{
	if (m_strCurrentMainObjectName == "")
		return nullptr;

	return &m_umapObjectAnimationNames[m_strCurrentMainObjectName];
}

optional<FAnimationData> ToolAnimation::CModelBlackBoard::GetCurrentModelAnimData()
{
	if (!m_spMainObject) { return nullopt; }

	optional<FAnimationData> tOptAnimationData = m_spMainObject->GetMeshComponent()->GetModelAnimationData().m_tBaseAnimData;

	return tOptAnimationData;
}

HRESULT ToolAnimation::CModelBlackBoard::Initialize()
{
	m_matPivot = Matrix::CreateScale(0.0001f) * Matrix::CreateFromQuaternion(Quaternion::CreateFromAxisAngle(Vector3(0.0f, 1.0f, 0.0f), XMConvertToRadians(180.f)));

	LoadModelFile();

	return S_OK;
}

void ToolAnimation::CModelBlackBoard::Release()
{
	for (auto& pair : m_umapAnimObject)
	{
		pair.second = nullptr;
	}
	m_umapAnimObject.clear();

	if (m_spMainObject) { m_spMainObject = nullptr; }

	m_umapObjectAnimationNames.clear();
	m_vecObjectNames.clear();

	m_spInstance = nullptr;
}

void ToolAnimation::CModelBlackBoard::ChangeMainObject(const string& _strObjectName)
{
	if (m_umapAnimObject.contains(_strObjectName))
	{
		if (m_spMainObject) 
		{
			m_spMainObject->SetActive(false); 
			m_spMainObject->GetMeshComponent()->GetModel()->SetStopAnimation(false);
		}

		m_spMainObject = m_umapAnimObject[_strObjectName];

		m_spMainObject->SetActive(true);
		m_strCurrentMainObjectName = _strObjectName;

		m_bStopAnimation = false;
	}
}

bool ToolAnimation::CModelBlackBoard::EmplaceNotify(const FAnimationData& _tAnimData)
{
	if (!m_umapModelsNotify.contains(m_strCurrentMainObjectName)) { return false; }

	string strCurAnim = CStringUtility::ToString(_tAnimData.m_wstrCurrentAnimName);

	if (!m_umapModelsNotify[m_strCurrentMainObjectName].contains(strCurAnim))
	{
		KeyFrameNotify keyFrameNotify;
		m_umapModelsNotify[m_strCurrentMainObjectName].emplace(strCurAnim, keyFrameNotify);
	}

	if (!m_umapModelsNotify[m_strCurrentMainObjectName][strCurAnim].contains((int32)_tAnimData.m_fTrackPosition))
	{
		list <FToolNotifyData> lstNotify;
		m_umapModelsNotify[m_strCurrentMainObjectName][strCurAnim].emplace((int32)_tAnimData.m_fTrackPosition, lstNotify);
	}

	return true;
}

void ToolAnimation::CModelBlackBoard::AddCollisionNotify(const string& _strAttachBoneName, const FCollisionOffset& _tCollisionOffset, float _fEndTrackPos)
{
	FAnimationData tAnimData = m_spMainObject->GetMeshComponent()->GetModelAnimationData().m_tBaseAnimData;
	string strCurAnim = CStringUtility::ToString(tAnimData.m_wstrCurrentAnimName);

	if (!EmplaceNotify(tAnimData)) { return; }

	for (auto& tNotify : m_umapModelsNotify[m_strCurrentMainObjectName][strCurAnim][(int32)tAnimData.m_fTrackPosition])
	{
		if (tNotify.m_strNotifyType == "Collision")
		{
			if ((tNotify.m_tCollisionNotify.m_strAttachBoneName == _strAttachBoneName) &&
				(tNotify.m_fEndTrackPos == _fEndTrackPos))
			{
				tNotify.m_tCollisionNotify.m_lstOffsetData.push_back(_tCollisionOffset);
				return;
			}
		}
	}

	FToolNotifyData tNotify;
	tNotify.iNotifyID = g_iNotifyID;
	++g_iNotifyID;

	tNotify.m_strNotifyType = "Collision";
	tNotify.m_fStartTrackPos = tAnimData.m_fTrackPosition;
	tNotify.m_fEndTrackPos = _fEndTrackPos;

	tNotify.m_tCollisionNotify.m_strAttachBoneName = _strAttachBoneName;
	tNotify.m_tCollisionNotify.m_lstOffsetData.push_back(_tCollisionOffset);

	m_umapModelsNotify[m_strCurrentMainObjectName][strCurAnim][(int32)tAnimData.m_fTrackPosition].push_back(tNotify);
}

void ToolAnimation::CModelBlackBoard::AddSoundNotify(const FToolSoundNotify& _tSoundNotify)
{
	FAnimationData tAnimData = m_spMainObject->GetMeshComponent()->GetModelAnimationData().m_tBaseAnimData;
	string strCurAnim = CStringUtility::ToString(tAnimData.m_wstrCurrentAnimName);

	if (!EmplaceNotify(tAnimData)) { return; }

	for (auto& tNotify : m_umapModelsNotify[m_strCurrentMainObjectName][strCurAnim][(int32)tAnimData.m_fTrackPosition])
	{
		if (tNotify.m_strNotifyType == "Sound")
		{
			tNotify.m_tSoundNotify = _tSoundNotify;
			return;
		}
	}

	FToolNotifyData tNotify;
	tNotify.iNotifyID = g_iNotifyID;
	++g_iNotifyID;

	tNotify.m_strNotifyType = "Sound";
	tNotify.m_fStartTrackPos = tAnimData.m_fTrackPosition;
	tNotify.m_fEndTrackPos = tAnimData.m_fTrackPosition;

	tNotify.m_tSoundNotify = _tSoundNotify;

	m_umapModelsNotify[m_strCurrentMainObjectName][strCurAnim][(int32)tAnimData.m_fTrackPosition].push_back(tNotify);
}

void ToolAnimation::CModelBlackBoard::AddEffectNotify(const FToolEffectNotify& _tEffectNotify)
{
	FAnimationData tAnimData = m_spMainObject->GetMeshComponent()->GetModelAnimationData().m_tBaseAnimData;
		
	string strCurAnim = CStringUtility::ToString(tAnimData.m_wstrCurrentAnimName);

	if (!EmplaceNotify(tAnimData)) { return; }

	FToolNotifyData tNotify;
	tNotify.iNotifyID = g_iNotifyID;
	++g_iNotifyID;

	tNotify.m_strNotifyType = "Effect";
	tNotify.m_fStartTrackPos = tAnimData.m_fTrackPosition;
	tNotify.m_fEndTrackPos = tAnimData.m_fTrackPosition;

	tNotify.m_tEffectNotify = _tEffectNotify;

	m_umapModelsNotify[m_strCurrentMainObjectName][strCurAnim][(int32)tAnimData.m_fTrackPosition].push_back(tNotify);
}

void ToolAnimation::CModelBlackBoard::AddFunctionNotify(const string& _strFunctionName)
{
	FAnimationData tAnimData = m_spMainObject->GetMeshComponent()->GetModelAnimationData().m_tBaseAnimData;

	string strCurAnim = CStringUtility::ToString(tAnimData.m_wstrCurrentAnimName);

	if (!EmplaceNotify(tAnimData)) { return; }

	FToolNotifyData tNotify;
	tNotify.iNotifyID = g_iNotifyID;
	++g_iNotifyID;

	tNotify.m_strNotifyType = "Function";
	tNotify.m_fStartTrackPos = tAnimData.m_fTrackPosition;
	tNotify.m_fEndTrackPos = tAnimData.m_fTrackPosition;

	tNotify.m_strFunctionNotify = _strFunctionName;

	m_umapModelsNotify[m_strCurrentMainObjectName][strCurAnim][(int32)tAnimData.m_fTrackPosition].push_back(tNotify);
}

void ToolAnimation::CModelBlackBoard::DeleteNotify(int32 _iNotifyIndex)
{
	FAnimationData tAnimData = m_spMainObject->GetMeshComponent()->GetModelAnimationData().m_tBaseAnimData;
	string strCurAnim = CStringUtility::ToString(tAnimData.m_wstrCurrentAnimName);

	if (m_umapModelsNotify[m_strCurrentMainObjectName][strCurAnim][(int32)tAnimData.m_fTrackPosition].empty()) { return; }

	if (m_umapModelsNotify[m_strCurrentMainObjectName][strCurAnim][(int32)tAnimData.m_fTrackPosition].size() <= _iNotifyIndex) { return; }

	auto notifyIter = m_umapModelsNotify[m_strCurrentMainObjectName][strCurAnim][(int32)tAnimData.m_fTrackPosition].begin();

	for (int i = 0; i < _iNotifyIndex; ++i)
	{
		++notifyIter;
	}

	m_umapModelsNotify[m_strCurrentMainObjectName][strCurAnim][(int32)tAnimData.m_fTrackPosition].erase(notifyIter);
}

list<FToolNotifyData>* ToolAnimation::CModelBlackBoard::GetCurrentKeyFrameNotify()
{
	if (!m_spMainObject) { return nullptr; }

	FAnimationData tAnimData = m_spMainObject->GetMeshComponent()->GetModelAnimationData().m_tBaseAnimData;
	string strCurAnim = CStringUtility::ToString(tAnimData.m_wstrCurrentAnimName);

	if (!m_umapModelsNotify[m_strCurrentMainObjectName].contains(strCurAnim))
	{
		return nullptr;
	}

	if (!m_umapModelsNotify[m_strCurrentMainObjectName][strCurAnim].contains((int32)tAnimData.m_fTrackPosition))
	{
		return nullptr;
	}

	return &m_umapModelsNotify[m_strCurrentMainObjectName][strCurAnim][(int32)tAnimData.m_fTrackPosition];
}

void ToolAnimation::CModelBlackBoard::ToggleAnimation()
{
	if (!m_spMainObject) { return; }

	m_bStopAnimation = !m_bStopAnimation;
	m_spMainObject->GetMeshComponent()->GetModel()->SetStopAnimation(m_bStopAnimation);
}

void ToolAnimation::CModelBlackBoard::StopAnimation()
{
	if (!m_spMainObject) { return; }

	m_bStopAnimation = true;
	m_spMainObject->GetMeshComponent()->GetModel()->SetStopAnimation(m_bStopAnimation);
}

void ToolAnimation::CModelBlackBoard::ResumeAnimation()
{
	if (!m_spMainObject) { return; }

	m_bStopAnimation = false;
	m_spMainObject->GetMeshComponent()->GetModel()->SetStopAnimation(m_bStopAnimation);
}

void ToolAnimation::CModelBlackBoard::LoadModelFile()
{
	filesystem::path path(m_strFilePath);
	filesystem::directory_iterator iter(path);

	while (iter != filesystem::end(iter))
	{
		if (iter->is_regular_file())
		{
			FToolAnimObjectDesc tObjDesc;
			tObjDesc.m_matPivot = m_matPivot;
			tObjDesc.m_wstrGameObjectName = iter->path().filename().wstring();
			tObjDesc.m_wstrAnimModelPath = iter->path().wstring();

			shared_ptr<CToolAnimObject> spAnimObj = CToolAnimObject::Create(make_shared<FToolAnimObjectDesc>(tObjDesc));
			string strModelName = iter->path().filename().string();

			// 모델 오브젝트 저장
			m_umapAnimObject.emplace(strModelName, spAnimObj);
			spAnimObj->SetActive(false);
			m_wpCurrentLevel.lock()->AddGameObject(spAnimObj);

			// 모델 이름 저장
			m_vecObjectNames.push_back(strModelName);

			// 애니메이션 이름 저장
			vector<wstring> vecAnimNames = spAnimObj->GetMeshComponent()->GetModel()->GetAnimationNames();
			vector<string> vecString;
			vecString.reserve(vecAnimNames.size());

			m_umapObjectAnimationNames.emplace(strModelName, vecString);
			for (auto& wstrAnimName : vecAnimNames)
			{
				m_umapObjectAnimationNames[strModelName].push_back(CStringUtility::ToString(wstrAnimName));
			}

			// 애니메이션 노티파이 정보 저장
			for (auto& strAnimName : m_umapObjectAnimationNames[strModelName])
			{
				InsertNotifys(strModelName, strAnimName);
			}
		}

		++iter;
	}
}

void ToolAnimation::CModelBlackBoard::InsertNotifys(const string& _strObjName, const string& _strAnimationName)
{	
	// 해당 모델의 애니메이션 정보가 없으면 채워넣기
	if (!m_umapModelsNotify.contains(_strObjName))
	{
		AnimationNotify m_umapNotifyDat;
		m_umapModelsNotify.emplace(_strObjName, m_umapNotifyDat);
	}

	// 애니메이션 이름과 대응하는 Notify파일 있는지 검색
	int32 iIndex = (int32)_strAnimationName.find('_');
	string strAMFilePath = "../../Resource/Models/AnimNotify/AM_" + _strAnimationName.substr(iIndex + 1) + ".json";

	ifstream animNotify(strAMFilePath);

	if (animNotify.fail())
		return;

	// 해당 모델에 <애니메이션 이름을 키>값으로 하는 <키프레임 노티파이 데이터들>의 집합 삽입
	if (!m_umapModelsNotify[_strObjName].contains(_strAnimationName))
	{
		KeyFrameNotify umapKeyFrameNotify;
		m_umapModelsNotify[_strObjName].emplace(_strAnimationName, umapKeyFrameNotify);
	}	

	// 키프레임 노티파이 데이터들 삽입
	Json::Value notify;
	animNotify >> notify;

	for (auto jsonIter = notify.begin(); jsonIter != notify.end(); ++jsonIter)
	{
		FToolNotifyData tNotifyData;
		tNotifyData.iNotifyID = g_iNotifyID;
		++g_iNotifyID;

		tNotifyData.m_strNotifyType = (*jsonIter)["Type"].asString();

		// 이벤트 시작 프레임 / 끝 프레임
		tNotifyData.m_fStartTrackPos = (*jsonIter)["StartTime"].asFloat();
		tNotifyData.m_fEndTrackPos = (*jsonIter)["EndTime"].asFloat();

		tNotifyData.m_fStartTrackPos *= 30.f;
		tNotifyData.m_fEndTrackPos *= 30.f;

		int32 iStartKeyFrame = static_cast<int32>(tNotifyData.m_fStartTrackPos);

		// 해당 키프레임에 해당하는 리스트 정보가 없다면 채워주기
		if (!m_umapModelsNotify[_strObjName][_strAnimationName].contains(iStartKeyFrame))
		{
			list<FToolNotifyData> lstNotifyDat;
			m_umapModelsNotify[_strObjName][_strAnimationName].emplace(iStartKeyFrame, lstNotifyDat);
		}

		// 타입별 데이터 생성 후 삽입
		if (tNotifyData.m_strNotifyType == "Collision")
		{
			tNotifyData.m_tCollisionNotify.m_strAttachBoneName = (*jsonIter)["AttachBoneName"].asString();

			Json::Value offsetDatas = (*jsonIter)["OffsetDataArray"];
			for (auto offsetData = offsetDatas.begin(); offsetData != offsetDatas.end(); ++offsetData)
			{				
				// 충돌체 오프셋
				FCollisionOffset tOffset;
				tOffset.m_fSphereRadius = (*offsetData)["SphereRadius"].asFloat() * 0.01f;
				tOffset.m_vRelativeLocation = Vector3((*offsetData)["vRelativeLocationX"].asFloat() * 0.01f, (*offsetData)["vRelativeLocationY"].asFloat() * 0.01f, (*offsetData)["vRelativeLocationZ"].asFloat() * 0.01f);
				
				tNotifyData.m_tCollisionNotify.m_lstOffsetData.push_back(tOffset);
			}			
		}
		else if (tNotifyData.m_strNotifyType == "Sound")
		{
			tNotifyData.m_tSoundNotify.m_strSoundID = (*jsonIter)["SoundID"].asString();
		}
		else if (tNotifyData.m_strNotifyType == "Effect")
		{
			tNotifyData.m_tEffectNotify.m_strEffectName = (*jsonIter)["EffectName"].asString();
			tNotifyData.m_tEffectNotify.m_strAttachBoneName = (*jsonIter)["AttachBoneName"].asString();
			tNotifyData.m_tEffectNotify.m_vOffset = Vector3((*jsonIter)["vRelativeLocationX"].asFloat() * 0.01f, (*jsonIter)["vRelativeLocationY"].asFloat() * 0.01f, (*jsonIter)["vRelativeLocationZ"].asFloat() * 0.01f);
		}
		else if (tNotifyData.m_strNotifyType == "Function")
		{
			tNotifyData.m_strFunctionNotify = (*jsonIter)["FunctionName"].asString();
		}

		m_umapModelsNotify[_strObjName][_strAnimationName][iStartKeyFrame].push_back(tNotifyData);
	}
}

void ToolAnimation::CModelBlackBoard::SaveModelNotify(const string& _strModelName, bool _bAnnounce)
{
	if (!m_umapModelsNotify.contains(_strModelName)) { return; }

	//KeyFrameNotify
	for (auto& pair : m_umapModelsNotify[_strModelName])
	{
		string strAnimationName = pair.first;
		int32 iIndex = (int32)pair.first.find('_');
		string strAMFilePath = "../../Resource/Models/AnimNotify/AM_" + pair.first.substr(iIndex + 1) + ".json";

		ofstream animNotify(strAMFilePath);
		if (animNotify.fail())
		{
			assert("invalid output filePath");
			animNotify.close();
			return;
		}

		Json::Value Output;

		//list<FToolNotifyData>
		for (auto& keyFrameData : pair.second)
		{
			// FToolNotifyData
			for (auto& Notify : keyFrameData.second)
			{
				Json::Value Data;

				float fStartTime = Notify.m_fStartTrackPos / 30.f;
				float fEndTime = Notify.m_fEndTrackPos / 30.f;

				Data["Type"] = Notify.m_strNotifyType;
				Data["StartTime"] = fStartTime;
				Data["EndTime"] = fEndTime;
				Data["Duration"] = fEndTime - fStartTime;

				if (Notify.m_strNotifyType == "Collision")
				{
					Data["AttachBoneName"] = Notify.m_tCollisionNotify.m_strAttachBoneName;

					for (auto& tColOffset : Notify.m_tCollisionNotify.m_lstOffsetData)
					{
						Json::Value ColliderOffset;
						ColliderOffset["SphereRadius"] = tColOffset.m_fSphereRadius * 100.f;
						ColliderOffset["vRelativeLocationX"] = tColOffset.m_vRelativeLocation.x * 100.f;
						ColliderOffset["vRelativeLocationY"] = tColOffset.m_vRelativeLocation.y * 100.f;
						ColliderOffset["vRelativeLocationZ"] = tColOffset.m_vRelativeLocation.z * 100.f;

						Data["OffsetDataArray"].append(ColliderOffset);
					}
				}
				else if (Notify.m_strNotifyType == "Sound")
				{
					Data["SoundID"] = Notify.m_tSoundNotify.m_strSoundID;
				}
				else if (Notify.m_strNotifyType == "Effect")
				{
					Data["AttachBoneName"] = Notify.m_tEffectNotify.m_strAttachBoneName;
					Data["EffectName"] = Notify.m_tEffectNotify.m_strEffectName;
					Data["vRelativeLocationX"] = Notify.m_tEffectNotify.m_vOffset.x * 100.f;
					Data["vRelativeLocationY"] = Notify.m_tEffectNotify.m_vOffset.y * 100.f;
					Data["vRelativeLocationZ"] = Notify.m_tEffectNotify.m_vOffset.z * 100.f;
					Data["UseFollowBone"] = Notify.m_tEffectNotify.m_bFollowBone;
				}
				else if (Notify.m_strNotifyType == "Function")
				{
					Data["FunctionName"] = Notify.m_strFunctionNotify;
				}

				Output.append(Data);
			}
		}

		animNotify << Output;

		animNotify.close();
	}

	if (_bAnnounce)
	{
		MESSAGE_BOX(L"Save Model Notify Success");
	}
}

void ToolAnimation::CModelBlackBoard::SaveCurrentModelNotify()
{
	if (!m_spMainObject) { return; }

	SaveModelNotify(m_strCurrentMainObjectName);
}

void ToolAnimation::CModelBlackBoard::SaveAllModelNotifyData()
{
	for (auto& pair : m_umapModelsNotify)
	{
		SaveModelNotify(pair.first, false);
	}

	MESSAGE_BOX(L"Save Model Notify Success");
}