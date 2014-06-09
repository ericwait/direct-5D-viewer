#pragma once
#include "GraphicObject.h"
#include <DirectXMath.h>
#include <vector>

class SceneNode
{
public:
	friend class RootSceneNode;
	SceneNode();
	virtual ~SceneNode();

	void update();

	virtual void attachToParentNode(SceneNode* parent);

	void setLocalToParent(DirectX::XMMATRIX transform);

	virtual bool isRenderable(){return false;}
	DirectX::XMMATRIX getLocalToWorldTransform();

protected:
	void setParentNode(SceneNode* parent);
	virtual const std::vector<SceneNode*>& getChildren();
	virtual void updateTransforms(DirectX::XMMATRIX parentToWorldIn);
	virtual void addChildNode(SceneNode* child);

	virtual void requestUpdate();

	DirectX::XMMATRIX localToParentTransform;
	DirectX::XMMATRIX parentToWorld;
	std::vector<SceneNode*> childrenNodes;
	SceneNode* parentNode;
};


class GraphicObjectNode : public SceneNode
{
public:
	GraphicObjectNode(GraphicObject* graphicObjectIn);
	~GraphicObjectNode(){graphicObject = NULL;}

	virtual bool isRenderable(){return true;}

	virtual void attachToParentNode(SceneNode* parent);

	const RendererPackage* getRenderPackage();

protected:
	void updateTransforms(DirectX::XMMATRIX parentToWorldIn);

private:
	GraphicObjectNode();
	GraphicObject* graphicObject;
};


class RootSceneNode : public SceneNode
{
public:
	RootSceneNode();
	~RootSceneNode();

	virtual void attachToParentNode(SceneNode* parent){}
	SceneNode* getRenderSectionNode(Renderer::Section section, int frame);
	int getNumRenderableObjects(Renderer::Section section){return renderList[section].size();}
	const std::vector<GraphicObjectNode*>& getRenderableList(Renderer::Section section, unsigned int frame);
	void updateTransforms(DirectX::XMMATRIX parentToWorldIn);
	int getNumFrames();

	virtual void requestUpdate();

private:
	void makeRenderableList();
	std::vector<SceneNode*> rootChildrenNodes[Renderer::Section::SectionEnd];
	std::vector<std::vector<GraphicObjectNode*>> renderList[Renderer::Section::SectionEnd];
};
