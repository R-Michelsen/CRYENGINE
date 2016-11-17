// Copyright 2001-2016 Crytek GmbH / Crytek Group. All rights reserved.

#include "StdAfx.h"
#include "Script/Elements/ScriptVariable.h"

#include <CrySerialization/IArchiveHost.h>
#include <CrySerialization/STL.h>
#include <Schematyc/Env/IEnvRegistry.h>
#include <Schematyc/Env/Elements/IEnvInterface.h>
#include <Schematyc/SerializationUtils/ISerializationContext.h>
#include <Schematyc/SerializationUtils/SerializationUtils.h>
#include <Schematyc/Utils/Any.h>
#include <Schematyc/Utils/Assert.h>
#include <Schematyc/Utils/IGUIDRemapper.h>

namespace Schematyc
{
CScriptVariable::CScriptVariable()
	: m_accessor(EScriptElementAccessor::Private)
	, m_data(SElementId(), true)
	, m_overridePolicy(EOverridePolicy::Default)
{}

CScriptVariable::CScriptVariable(const SGUID& guid, const char* szName, const SElementId& typeId, const SGUID& baseGUID)
	: CScriptElementBase(guid, szName)
	, m_accessor(EScriptElementAccessor::Private)
	, m_data(typeId, true)
	, m_baseGUID(baseGUID)
	, m_overridePolicy(EOverridePolicy::Default)
{}

EScriptElementAccessor CScriptVariable::GetAccessor() const
{
	return m_accessor;
}

void CScriptVariable::EnumerateDependencies(const ScriptDependencyEnumerator& enumerator, EScriptDependencyType type) const
{
	SCHEMATYC_CORE_ASSERT(!enumerator.IsEmpty());
	if (!enumerator.IsEmpty())
	{
		m_data.EnumerateDependencies(enumerator, type);

		if (!GUID::IsEmpty(m_baseGUID))
		{
			enumerator(m_baseGUID);
		}

		CScriptElementBase::EnumerateDependencies(enumerator, type);
	}
}

void CScriptVariable::RemapDependencies(IGUIDRemapper& guidRemapper)
{
	CScriptElementBase::RemapDependencies(guidRemapper);

	m_data.RemapDependencies(guidRemapper);

	m_baseGUID = guidRemapper.Remap(m_baseGUID);
}

void CScriptVariable::ProcessEvent(const SScriptEvent& event) {}

void CScriptVariable::Serialize(Serialization::IArchive& archive)
{
	// #SchematycTODO : Shouldn't this be handled by CScriptElementBase itself?
	CScriptElementBase::Serialize(archive);
	CMultiPassSerializer::Serialize(archive);
	CScriptElementBase::SerializeExtensions(archive);
}

SElementId CScriptVariable::GetTypeId() const
{
	return m_data.GetTypeId();
}

bool CScriptVariable::IsArray() const
{
	return m_data.IsArray();
}

CAnyConstPtr CScriptVariable::GetData() const
{
	return m_data.GetValue();
}

SGUID CScriptVariable::GetBaseGUID() const
{
	return m_baseGUID;
}

EOverridePolicy CScriptVariable::GetOverridePolicy() const
{
	return m_overridePolicy;
}

void CScriptVariable::LoadDependencies(Serialization::IArchive& archive, const ISerializationContext& context)
{
	m_data.SerializeTypeId(archive);
	archive(m_baseGUID, "baseGUID");
}

void CScriptVariable::Load(Serialization::IArchive& archive, const ISerializationContext& context)
{
	archive(m_accessor, "accessor");
	m_data.SerializeValue(archive);
	archive(m_overridePolicy, "overridePolicy");
}

void CScriptVariable::Save(Serialization::IArchive& archive, const ISerializationContext& context)
{
	archive(m_accessor, "accessor");
	m_data.SerializeTypeId(archive);
	m_data.SerializeValue(archive);
	archive(m_baseGUID, "baseGUID");
	archive(m_overridePolicy, "overridePolicy");
}

void CScriptVariable::Edit(Serialization::IArchive& archive, const ISerializationContext& context)
{
	{
		ScriptVariableData::CScopedSerializationConfig serializationConfig(archive);

		const SGUID guid = CScriptElementBase::GetGUID();
		serializationConfig.DeclareEnvDataTypes(guid);
		serializationConfig.DeclareScriptEnums(guid);
		serializationConfig.DeclareScriptStructs(guid);

		m_data.SerializeTypeId(archive);
	}

	bool bPublic = m_accessor == EScriptElementAccessor::Public;
	archive(bPublic, "bPublic", "Public");
	if (archive.isInput())
	{
		m_accessor = bPublic ? EScriptElementAccessor::Public : EScriptElementAccessor::Private;
	}
	
	const bool bDerived = !GUID::IsEmpty(m_baseGUID);
	if (bDerived)
	{
		archive(m_overridePolicy, "overridePolicy", "Override");
	}
	if (!bDerived || (m_overridePolicy != EOverridePolicy::Default))
	{
		m_data.SerializeValue(archive);
	}
}
} // Schematyc
