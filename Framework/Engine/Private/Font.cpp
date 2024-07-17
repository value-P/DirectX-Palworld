#include "pch.h"
#include "Font.h"

#include "EngineModule.h"
#include "DeviceManager.h"

#include "TextComponent.h"

std::shared_ptr<CFont> Engine::CFont::Create(const std::wstring& _wstrFontFilePath)
{
	const std::shared_ptr<CFont> spFont = make_shared<CFont>();
	spFont->Initialize(_wstrFontFilePath);
	return spFont;
}

HRESULT Engine::CFont::Initialize(const std::wstring& _wstrFontFilePath)
{
	shared_ptr<CDeviceManager> spDeviceManager = CEngineModule::GetInstance()->GetDeviceManager();
	m_spSpriteBatch = make_shared<SpriteBatch>(spDeviceManager->GetDeviceContext().Get());
	m_spSpriteFont = make_shared<SpriteFont>(spDeviceManager->GetDevice().Get(), _wstrFontFilePath.c_str());

	D3D11_BLEND_DESC blendDesc;
	ZeroMemory(&blendDesc, sizeof(D3D11_BLEND_DESC));

	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	CEngineModule::GetInstance()->GetDeviceManager()->GetDevice()->CreateBlendState(&blendDesc, m_cpBlendState.GetAddressOf());

	return S_OK;
}

void Engine::CFont::Render()
{
	CEngineModule::GetInstance()->GetDeviceManager()->GetDeviceContext()->GSSetShader(nullptr, nullptr, 0);

	m_spSpriteBatch->Begin(SpriteSortMode_Deferred, m_cpBlendState.Get());

	switch (m_iOption)
	{
	case 0x0:
		if (nullptr != m_spTextComponent)
		{
			//Vector4 vTextSize = m_spSpriteFont->MeasureString(m_wstrText.c_str());
			RECT rcText = m_spSpriteFont->MeasureDrawBounds(m_wstrText.c_str(), m_vScreenPosition);
			//m_spTextComponent->SetRect(rcText);
			Vector2 vCenter;
			vCenter.x = (rcText.left + rcText.right) / 2.0f;
			vCenter.y = (rcText.top + rcText.bottom) / 2.0f;
			Vector2 vSize;
			vSize.x = static_cast<float>(rcText.right - rcText.left);
			vSize.y = static_cast<float>(rcText.bottom - rcText.top);

			if (ETextType::LEFT == m_spTextComponent->GetTextType())
			{
				m_spTextComponent->SetRect(rcText);
				// 왼쪽 상단을 기준으로 잡습니다.
				m_spSpriteFont->DrawString(m_spSpriteBatch.get(), m_wstrText.c_str(), m_vScreenPosition, m_vColor, m_fRadian, m_vOrigin, m_fScale);
				break;
			}
			else if (ETextType::CENTER == m_spTextComponent->GetTextType())
			{
				m_vOrigin.x = vSize.x / 2.0f;
				vCenter.x = vCenter.x - m_vOrigin.x;

				RECT rcTextRect =
				{
					static_cast<LONG>(vCenter.x - (vSize.x * 0.5f)),
					static_cast<LONG>(vCenter.y - (vSize.y * 0.5f)),
					static_cast<LONG>(vCenter.x + (vSize.x * 0.5f)),
					static_cast<LONG>(vCenter.y + (vSize.y * 0.5f))
				};
				m_spTextComponent->SetRect(rcTextRect);
				// 가운데 상단을 기준으로 잡습니다.
				m_spSpriteFont->DrawString(m_spSpriteBatch.get(), m_wstrText.c_str(), m_vScreenPosition, m_vColor, m_fRadian, m_vOrigin, m_fScale);
				break;
			}
			else if (ETextType::RIGHT == m_spTextComponent->GetTextType())
			{
				m_vOrigin.x = vSize.x;
				vCenter.x = vCenter.x - m_vOrigin.x;

				RECT rcTextRect =
				{
					static_cast<LONG>(vCenter.x - (vSize.x * 0.5f)),
					static_cast<LONG>(vCenter.y - (vSize.y * 0.5f)),
					static_cast<LONG>(vCenter.x + (vSize.x * 0.5f)),
					static_cast<LONG>(vCenter.y + (vSize.y * 0.5f))
				};
				m_spTextComponent->SetRect(rcTextRect);
				// 오른쪽 상단을 기준으로 잡습니다.
				m_spSpriteFont->DrawString(m_spSpriteBatch.get(), m_wstrText.c_str(), m_vScreenPosition, m_vColor, m_fRadian, m_vOrigin, m_fScale);
				break;
			}
		}
		// 아무것도 없는 경우
		m_spSpriteFont->DrawString(m_spSpriteBatch.get(), m_wstrText.c_str(), m_vScreenPosition, m_vColor, m_fRadian, m_vOrigin, m_fScale);
		break;

	default:
		break;
	}

	m_spSpriteBatch->End();
}

void Engine::CFont::Release()
{
	m_spSpriteBatch = nullptr;
	m_spSpriteFont = nullptr;

	m_cpBlendState = nullptr;
}
