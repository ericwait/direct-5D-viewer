////////////////////////////////////////////////////////////////////////////////
//Copyright 2014 Andrew Cohen, Eric Wait, and Mark Winter
//This file is part of LEVER 3-D - the tool for 5-D stem cell segmentation,
//tracking, and lineaging. See http://bioimage.coe.drexel.edu 'software' section
//for details. LEVER 3-D is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by the Free
//Software Foundation, either version 3 of the License, or (at your option) any
//later version.
//LEVER 3-D is distributed in the hope that it will be useful, but WITHOUT ANY
//WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
//A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//You should have received a copy of the GNU General Public License along with
//LEVer in file "gnu gpl v3.txt".  If not, see  <http://www.gnu.org/licenses/>.
////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "GraphicObject.h"
#include <DirectXMath.h>
#include <vector>

// A few of global helper functions for managing the graphic object node list
void insertGlobalGraphicsObject(GraphicObjectTypes objType, GraphicObjectNode* node, int forceLabel = -1);
void removeGlobalGraphicsObject(GraphicObjectTypes objType, unsigned int uniqueID);
GraphicObjectNode* getGlobalGraphicsObject(GraphicObjectTypes objType, unsigned int uniqueID);

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

	virtual int getPolygon(Vec<float> pnt, Vec<float> direction,float& depthOut);
	virtual int getPolygonLabel(){return -1;}

	SceneNode* getParentNode(){return parentNode;}

protected:
	void setParentNode(SceneNode* parent);
	virtual const std::vector<SceneNode*>& getChildren();
	virtual void updateTransforms(DirectX::XMMATRIX parentToWorldIn);
	virtual bool addChildNode(SceneNode* child);
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

	void releaseRenderResources();
	void setLightOn(bool on){graphicObject->setLightOn(on);}
	void setRenderable(bool render);
	void setWireframe(bool wireframe);

	virtual bool isRenderable(){return renderable;}
	const RendererPackage* getRenderPackage();
	virtual int getPolygon(Vec<float> pnt, Vec<float> direction,float& depthOut);
	virtual int getPolygonLabel(){return graphicObject->getIndex();}
	GraphicObject* getGraphicObjectPtr() { return graphicObject; }

protected:
	virtual bool addChildNode(SceneNode* child) { return false; }; //TODO: should probably be an error
	virtual void updateTransforms(DirectX::XMMATRIX parentToWorldIn);

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
	int getNumFrames();
	int getPolygon(Vec<float> pnt, Vec<float> direction,float& depthOut){return -1;}
	int getPolygon(Vec<float> pnt, Vec<float> direction, unsigned int currentFrame,float& depthOut);
	DirectX::XMMATRIX getWorldRotation();

	void resetWorldTransform();
	virtual void requestUpdate();
	void updateRenderableList();
	void updateTranslation(Vec<float> origin);
	void updateRotation(DirectX::XMMATRIX& rotation);

protected:
	virtual bool addChildNode(SceneNode* child);
	void updateTransforms(DirectX::XMMATRIX parentToWorldIn);

private:
	void makeRenderableList();

	bool bRenderListDirty;
	Vec<float> origin;
	DirectX::XMMATRIX curRotationMatrix;

	std::vector<SceneNode*> rootChildrenNodes[Renderer::Section::SectionEnd];
	std::vector<std::vector<GraphicObjectNode*>> renderList[Renderer::Section::SectionEnd];
};
