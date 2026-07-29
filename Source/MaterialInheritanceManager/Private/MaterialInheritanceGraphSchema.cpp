// Copyright Epic Games, Inc. All Rights Reserved.

#include "MaterialInheritanceGraphSchema.h"
#include "MaterialInheritanceGraphNode.h"
#include "ConnectionDrawingPolicy.h"
#include "EdGraph/EdGraph.h"
#include "EdGraph/EdGraphPin.h"
#include "ScopedTransaction.h"

#define LOCTEXT_NAMESPACE "MaterialInheritanceGraphSchema"

// ============================================================================
// Custom connection drawing policy for material inheritance graph
// ============================================================================
class FMaterialInheritanceConnectionDrawingPolicy : public FConnectionDrawingPolicy
{
public:
	FMaterialInheritanceConnectionDrawingPolicy(int32 InBackLayerID, int32 InFrontLayerID, float InZoomFactor, const FSlateRect& InClippingRect, FSlateWindowElementList& InDrawElements, UEdGraph* InGraphObj)
		: FConnectionDrawingPolicy(InBackLayerID, InFrontLayerID, InZoomFactor, InClippingRect, InDrawElements)
		, GraphObj(InGraphObj)
	{
		ArrowImage = nullptr;
		ArrowRadius = FVector2D::ZeroVector;
	}

	virtual void DetermineWiringStyle(UEdGraphPin* OutputPin, UEdGraphPin* InputPin, /*inout*/ FConnectionParams& Params) override
	{
		// Determine wire color based on node type
		Params.WireColor = FLinearColor(0.5f, 0.7f, 1.0f); // Light blue wires
		Params.bDrawBubbles = false;
		Params.WireThickness = 1.5f;
	}

private:
	UEdGraph* GraphObj;
};

// ============================================================================
// Schema methods
// ============================================================================

void UMaterialInheritanceGraphSchema::GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const
{
	// No context actions for read-only graph
}

const FPinConnectionResponse UMaterialInheritanceGraphSchema::CanCreateConnection(const UEdGraphPin* A, const UEdGraphPin* B) const
{
	return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, LOCTEXT("NoManualConnect","Connections are set automatically for inheritance relationships."));
}

FLinearColor UMaterialInheritanceGraphSchema::GetPinTypeColor(const FEdGraphPinType& PinType) const
{
	return FLinearColor(0.5f, 0.7f, 1.0f);
}

void UMaterialInheritanceGraphSchema::BreakPinLinks(UEdGraphPin& TargetPin, bool bSendsNodeNotifcation) const
{
	// Disallow manual breaking of links
}

void UMaterialInheritanceGraphSchema::BreakSinglePinLink(UEdGraphPin* SourcePin, UEdGraphPin* TargetPin) const
{
	// Disallow manual breaking of links
}

FConnectionDrawingPolicy* UMaterialInheritanceGraphSchema::CreateConnectionDrawingPolicy(
	int32 InBackLayerID, int32 InFrontLayerID, float InZoomFactor,
	const FSlateRect& InClippingRect, FSlateWindowElementList& InDrawElements,
	UEdGraph* InGraphObj) const
{
	return new FMaterialInheritanceConnectionDrawingPolicy(InBackLayerID, InFrontLayerID, InZoomFactor, InClippingRect, InDrawElements, InGraphObj);
}

#undef LOCTEXT_NAMESPACE
