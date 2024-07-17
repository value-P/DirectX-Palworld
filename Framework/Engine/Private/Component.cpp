// default
#include "pch.h"
#include "Component.h"

HRESULT Engine::CComponent::InitializeComponent(const std::shared_ptr<FComponentDesc>& _spComponentDesc)
{
	CHECKF(!_spComponentDesc->m_wstrComponentName.empty(), L"Component name is not designated.");

	m_wstrComponentName = _spComponentDesc->m_wstrComponentName;
	m_eRenderType		= _spComponentDesc->m_eRenderType;
	m_eComponentType	= _spComponentDesc->m_eComponentType;
	m_wpOwnerGameObject = _spComponentDesc->m_wpOwnerGameObject;
	return S_OK;
}

const std::shared_ptr<CGameObject> Engine::CComponent::GetOwnerGameObject() const
{
	return m_wpOwnerGameObject.expired() ? nullptr : m_wpOwnerGameObject.lock();
}
