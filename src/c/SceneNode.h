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
	void detatchFromParentNode();
	void setLocalToParent(DirectX::XMMATRIX transform);
	virtual bool isRenderable(){return false;}
	DirectX::XMMATRIX getLocalToWorldTransform();

protected:
	void setParentNode(SceneNode* parent);
	virtual const std::vector<SceneNode*>& getChildren();
	virtual void updateTransforms(DirectX::XMMATRIX parentToWorldIn);
	virtual void addChildNode(SceneNode* child);
	virtual void requestUpdate();
	virtual void detatchChildNode(SceneNode* child);

	DirectX::XMMATRIX localToParentTransform;
	DirectX::XMMATRIX parentToWorld;
	std::vector<SceneNode*> childrenNodes;
	SceneNode* parentNode;
};


class GraphicObjectNode : public SceneNode
{
public:
	GraphicObjectNode(GraphicObject* graphicObjectIn);
	~GraphicObjectNode();

	virtual void attachToParentNode(SceneNode* parent);
	void releaseRenderResources();
	void setLightOn(bool on){graphicObject->setLightOn(on);}
	void setRenderable(bool render);

	virtual bool isRenderable(){return renderable;}
	const RendererPackage* getRenderPackage();

protected:
	void updateTransforms(DirectX::XMMATRIX parentToWorldIn);

private:
	GraphicObjectNode();
	GraphicObject* graphicObject;
	bool renderable;
};


class RootSceneNode : public SceneNode
{
public:
	RootSceneNode();
	~RootSceneNode();

	virtual void attachToParentNode(SceneNode* parent){}
	SceneNode* getRenderSectionNode(Renderer::Section section, int frame);
	size_t getNumRenderableObjects(Renderer::Section section){return renderList[section].size();}
	const std::vector<GraphicObjectNode*>& getRenderableList(Renderer::Section section, unsigned int frame);
	void updateTransforms(DirectX::XMMATRIX parentToWorldIn);
	void resetWorldTransform();
	int getNumFrames();
	virtual void addChildNode(SceneNode* child){throw std::runtime_error("You cannot attach to a root node using this method!");}

	virtual void requestUpdate();

private:
	void makeRenderableList();
	std::vector<SceneNode*> rootChildrenNodes[Renderer::Section::SectionEnd];
	std::vector<std::vector<GraphicObjectNode*>> renderList[Renderer::Section::SectionEnd];
};
