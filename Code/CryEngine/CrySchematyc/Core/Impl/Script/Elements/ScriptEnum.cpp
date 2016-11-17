// Copyright 2001-2016 Crytek GmbH / Crytek Group. All rights reserved.

#include "StdAfx.h"
#include "Script/Elements/ScriptEnum.h"

#include <CrySerialization/BlackBox.h>
#include <CrySerialization/IArchiveHost.h>
#include <CrySerialization/STL.h>
#include <Schematyc/Env/IEnvRegistry.h>
#include <Schematyc/SerializationUtils/ISerializationContext.h>
#include <Schematyc/SerializationUtils/SerializationUtils.h>
#include <Schematyc/Utils/Assert.h>
#include <Schematyc/Utils/IGUIDRemapper.h>

namespace Schematyc
{
CScriptEnum::CScriptEnum()
	: CScriptElementBase(EScriptElementFlags::CanOwnScript)
{}

CScriptEnum::CScriptEnum(const SGUID& guid, const char* szName)
	: CScriptElementBase(guid, szName, EScriptElementFlags::CanOwnScript)
{}

EScriptElementAccessor CScriptEnum::GetAccessor() const
{
	return EScriptElementAccessor::Private;
}

void CScriptEnum::EnumerateDependencies(const ScriptDependencyEnumerator& enumerator, EScriptDependencyType type) const {}

void CScriptEnum::RemapDependencies(IGUIDRemapper& guidRemapper) {}

void CScriptEnum::ProcessEvent(const SScriptEvent& event)
{
	CScriptElementBase::ProcessEvent(event);

	switch (event.id)
	{
	case EScriptEventId::EditorAdd:
	case EScriptEventId::EditorPaste:
		{
			m_userDocumentation.SetCurrentUserAsAuthor();
			break;
		}
	}
}

void CScriptEnum::Serialize(Serialization::IArchive& archive)
{
	LOADING_TIME_PROFILE_SECTION;

	CScriptElementBase::Serialize(archive);

	switch (SerializationContext::GetPass(archive))
	{
	case ESerializationPass::LoadDependencies:
	case ESerializationPass::Save:
		{
			archive(m_constants, "constants", "Constants");
			archive(m_userDocumentation, "userDocumentation", "Documentation");
			break;
		}
	case ESerializationPass::Edit:
		{
			archive(m_constants, "constants", "Constants");
			archive(m_userDocumentation, "userDocumentation", "Documentation");
			break;
		}
	}

	CScriptElementBase::SerializeExtensions(archive);
}

uint32 CScriptEnum::GetConstantCount() const
{
	return m_constants.size();
}

uint32 CScriptEnum::FindConstant(const char* szConstant) const
{
	SCHEMATYC_CORE_ASSERT(szConstant);
	if (szConstant)
	{
		for (Constants::const_iterator itConstant = m_constants.begin(), itEndConstant = m_constants.end(); itConstant != itEndConstant; ++itConstant)
		{
			if (strcmp(itConstant->c_str(), szConstant) == 0)
			{
				return static_cast<uint32>(itConstant - m_constants.begin());
			}
		}
	}
	return InvalidIdx;
}

const char* CScriptEnum::GetConstant(uint32 constantIdx) const
{
	return constantIdx < m_constants.size() ? m_constants[constantIdx].c_str() : "";
}
} // Schematyc
