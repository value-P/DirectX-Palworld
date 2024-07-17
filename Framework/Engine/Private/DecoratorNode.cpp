#include "pch.h"
#include "DecoratorNode.h"

HRESULT Engine::CDecoratorNode::Initialize(const shared_ptr<FBTNodeDesc>& _spNodeDesc)
{
	if (FAILED(CBTNode::Initialize(_spNodeDesc))) { return E_FAIL; };

	return S_OK;
}

void Engine::CDecoratorNode::Release()
{
	CBTNode::Release();
}
