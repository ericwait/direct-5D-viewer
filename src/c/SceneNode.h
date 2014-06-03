#pragma once
#include "GraphicObject.h"
#include <DirectXMath.h>
#include <set>

class SceneNode
{
public:
	SceneNode()
	{
		localToParentTransform = DirectX::XMMatrixIdentity();
		parentToWorld = DirectX::XMMatrixIdentity();
		parentNode=NULL;
	}
	virtual ~SceneNode()
	{
		for (std::set<SceneNode*>::iterator it=childrenNodes.begin(); it!=childrenNodes.end(); ++it)
			delete *it;

		childrenNodes.clear();
		parentNode = NULL;
	}

	void update(){updateTransforms(parentToWorld);}

	virtual void attachChildNode(SceneNode* child)//TODO look over the tree and ensure that there is no duplicates
	{
		if (0<childrenNodes.count(child))
			return;

		childrenNodes.insert(child);
		child->updateTransforms(localToParentTransform*parentToWorld);
	}

	void setLocalToParent(DirectX::XMMATRIX transform)
	{
		localToParentTransform = transform;
		updateTransforms(parentToWorld);
	}

protected:
	virtual void updateTransforms(DirectX::XMMATRIX parentToWorldIn)
	{
		parentToWorld = parentToWorldIn;

		for (std::set<SceneNode*>::iterator it=childrenNodes.begin(); it!=childrenNodes.end(); ++it)
		{
			(*it)->updateTransforms(localToParentTransform*parentToWorld);
		}
	}

	DirectX::XMMATRIX localToParentTransform;
	DirectX::XMMATRIX parentToWorld;
	std::set<SceneNode*> childrenNodes;
	SceneNode* parentNode;
};

class GraphicObjectNode : public SceneNode
{
public:
	GraphicObjectNode(GraphicObject* graphicObjectIn){graphicObject = graphicObjectIn;}
	~GraphicObjectNode(){graphicObject = NULL;}

	void attachChildNode(SceneNode* child){throw std::runtime_error("You cannot attach SceneNode to a GraphicObjectNode!");}

protected:
	void updateTransforms(DirectX::XMMATRIX parentToWorldIn)
	{
		parentToWorld = parentToWorldIn;
		graphicObject->makeLocalToWorld(localToParentTransform*parentToWorld);
	}

private:
	GraphicObjectNode();
	GraphicObject* graphicObject;
};
