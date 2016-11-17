// Copyright 2001-2016 Crytek GmbH / Crytek Group. All rights reserved.

#include "StdAfx.h"
#include "Script/Elements/ScriptClass.h"

#include <CrySerialization/IArchiveHost.h>
#include <Schematyc/Script/IScriptGraph.h>
#include <Schematyc/Script/IScriptRegistry.h>
#include <Schematyc/Script/Elements/IScriptSignalReceiver.h>
#include <Schematyc/SerializationUtils/ISerializationContext.h>
#include <Schematyc/Utils/IGUIDRemapper.h>

#include "CVars.h"
#include "CoreEnv/CoreEnvSignals.h"
#include "Script/Graph/ScriptGraphNode.h"
#include "Script/Graph/Nodes/ScriptGraphReceiveSignalNode.h"

namespace Schematyc
{
CScriptClass::CScriptClass()
	: CScriptElementBase(EScriptElementFlags::MustOwnScript)
{}

CScriptClass::CScriptClass(const SGUID& guid, const char* szName)
	: CScriptElementBase(guid, szName, EScriptElementFlags::MustOwnScript)
{}

EScriptElementAccessor CScriptClass::GetAccessor() const
{
	return EScriptElementAccessor::Private;
}

void CScriptClass::EnumerateDependencies(const ScriptDependencyEnumerator& enumerator, EScriptDependencyType type) const {}

void CScriptClass::RemapDependencies(IGUIDRemapper& guidRemapper) {}

void CScriptClass::ProcessEvent(const SScriptEvent& event)
{
	CScriptElementBase::ProcessEvent(event);

	switch (event.id)
	{
	case EScriptEventId::EditorAdd:
		{
			// #SchematycTODO : Should we be doing this in editor code?

			IScriptRegistry& scriptRegistry = GetSchematycCore().GetScriptRegistry();
			scriptRegistry.AddConstructor("ConstructionGraph", this);

			IScriptSignalReceiver* pSignalReceiver = scriptRegistry.AddSignalReceiver("SignalGraph", EScriptSignalReceiverType::Universal, SGUID(), this);
			IScriptGraph* pGraph = static_cast<IScriptGraph*>(pSignalReceiver->GetExtensions().QueryExtension(EScriptExtensionType::Graph));
			if(pGraph)
			{
				IScriptGraphNodePtr pStartNode = std::make_shared<CScriptGraphNode>(GetSchematycCore().CreateGUID(), stl::make_unique<CScriptGraphReceiveSignalNode>(SElementId(EDomain::Env, g_startSignalGUID))); // #SchematycTODO : Shouldn't we be using CScriptGraphNodeFactory::CreateNode() instead of instantiating the node directly?!?
				pStartNode->SetPos(Vec2(0.0f, 0.0f));
				pGraph->AddNode(pStartNode);

				IScriptGraphNodePtr pStopNode = std::make_shared<CScriptGraphNode>(GetSchematycCore().CreateGUID(), stl::make_unique<CScriptGraphReceiveSignalNode>(SElementId(EDomain::Env, g_stopSignalGUID))); // #SchematycTODO : Shouldn't we be using CScriptGraphNodeFactory::CreateNode() instead of instantiating the node directly?!?
				pStopNode->SetPos(Vec2(0.0f, 100.0f));
				pGraph->AddNode(pStopNode);

				IScriptGraphNodePtr pUpdateNode = std::make_shared<CScriptGraphNode>(GetSchematycCore().CreateGUID(), stl::make_unique<CScriptGraphReceiveSignalNode>(SElementId(EDomain::Env, g_updateSignalGUID))); // #SchematycTODO : Shouldn't we be using CScriptGraphNodeFactory::CreateNode() instead of instantiating the node directly?!?
				pUpdateNode->SetPos(Vec2(0.0f, 200.0f));
				pGraph->AddNode(pUpdateNode);
			}

			m_userDocumentation.SetCurrentUserAsAuthor();
			break;
		}
	case EScriptEventId::EditorPaste:
		{
			m_userDocumentation.SetCurrentUserAsAuthor();
			break;
		}
	}
}

void CScriptClass::Serialize(Serialization::IArchive& archive)
{
	// #SchematycTODO : Shouldn't this be handled by CScriptElementBase itself?
	CScriptElementBase::Serialize(archive);
	CMultiPassSerializer::Serialize(archive);
	CScriptElementBase::SerializeExtensions(archive);
}

const char* CScriptClass::GetAuthor() const
{
	return m_userDocumentation.author.c_str();
}

const char* CScriptClass::GetDescription() const
{
	return m_userDocumentation.description.c_str();
}

void CScriptClass::Load(Serialization::IArchive& archive, const ISerializationContext& context)
{
	archive(m_userDocumentation, "userDocumentation", "Documentation");
}

void CScriptClass::Save(Serialization::IArchive& archive, const ISerializationContext& context)
{
	archive(m_userDocumentation, "userDocumentation", "Documentation");
}

void CScriptClass::Edit(Serialization::IArchive& archive, const ISerializationContext& context)
{
	archive(m_userDocumentation, "userDocumentation", "Documentation");
}
} // Schematyc
