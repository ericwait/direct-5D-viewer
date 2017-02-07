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
#include "Global/Vec.h"
#include "Renderer.h"

#include "MeshPrimitive.h"
#include "Material.h"

#include "Eigen/Eigen"

#include <DirectXMath.h>
#include <vector>
#include <queue>

class GraphicObjectNode;

// A few of global helper functions for managing the graphic object node list
void insertGlobalGraphicsObject(GraphicObjectTypes objType, GraphicObjectNode* node, int forceLabel = -1);
void removeGlobalGraphicsObject(GraphicObjectTypes objType, unsigned int uniqueID);
GraphicObjectNode* getGlobalGraphicsObject(GraphicObjectTypes objType, unsigned int uniqueID);

class SceneNode
{
protected:
	typedef Eigen::Matrix<size_t, GraphicObjectTypes::VTend, 1> Histogram;

public:
	friend class RootSceneNode;
	friend class RenderFilter;

	SceneNode(GraphicObjectTypes type);

	virtual ~SceneNode();

	void update();

	virtual void attachToParentNode(SceneNode* parent);
	void detatchFromParentNode();

	void setLocalToParent(DirectX::XMMATRIX transform);
	DirectX::XMMATRIX getLocalToWorldTransform() const;

	virtual bool isRenderable(){return false;}
	virtual SceneNode* pickNode(Vec<float> pnt, Vec<float> direction, GraphicObjectTypes filter, float& depthOut);

	int getIndex() const {return index;}
	const std::string& getLabel() const {return label;}
	void setLabel(const std::string& labelIn){label = labelIn;}

	SceneNode* getParentNode(){return parentNode;}

protected:
	SceneNode(int index, GraphicObjectTypes type);

	void setParentNode(SceneNode* parent);

	virtual bool addChildNode(SceneNode* child);
	virtual void detatchChildNode(SceneNode* child);

	virtual const std::vector<SceneNode*>& getChildren();
	virtual void updateTransforms(DirectX::XMMATRIX parentToWorldIn);
	virtual void requestUpdate();

	void updateAddTypes();
	void updateSubtractTypes();

	virtual void addTypes(const Histogram& deltas);
	virtual void subtractTypes(const Histogram& deltas);

	DirectX::XMMATRIX localToParentTransform;
	DirectX::XMMATRIX parentToWorld;

	// Scene graph
	SceneNode* parentNode;
	std::vector<SceneNode*> childrenNodes;
	
	// Object properites
	int index;
	std::string label;

	// Type filter and child type listing
	Histogram childTypes;
	GraphicObjectTypes type;

	Vec<float> BoundingBox[2];
};


class GraphicObjectNode : public SceneNode
{
public:
	friend class Renderer;

	GraphicObjectNode(int index, GraphicObjectTypes type, std::shared_ptr<MeshPrimitive> mesh, std::shared_ptr<Material> material);
	~GraphicObjectNode();

	void releaseRenderResources();
	void setLightOn(bool on){material->getParams()->setLightOn(on);}

	void setColor(Vec<float> color, float alpha);
	void setRenderable(bool render);
	void setWireframe(bool wireframe);

	virtual bool isRenderable(){return renderable;}
	virtual SceneNode* pickNode(Vec<float> pnt, Vec<float> direction, GraphicObjectTypes filter, float& depthOut);

	DirectX::XMMATRIX getLocalToWorld() const {return localToWorld;};

	std::shared_ptr<MeshPrimitive>& getMesh(){return mesh;}
	std::shared_ptr<Material>& getMaterial(){return material;}

protected:
	virtual bool addChildNode(SceneNode* child) { return false; }; //TODO: should probably be an error
	virtual void updateTransforms(DirectX::XMMATRIX parentToWorldIn);

private:
	GraphicObjectNode();

	// TODO: Should this be in the material?
	DirectX::XMMATRIX localToWorld;

	bool renderable;

	// Render properties
	std::shared_ptr<MeshPrimitive> mesh;
	std::shared_ptr<Material> material;
};


class RenderFilter
{
public:
	RenderFilter(SceneNode* rootNode, GraphicObjectTypes filter);

	GraphicObjectNode* first();
	GraphicObjectNode* next();

private:
	SceneNode* getNext();
	bool checkFilter(SceneNode* node, GraphicObjectTypes filter);
	bool checkChildren(SceneNode* node, GraphicObjectTypes filter);

	SceneNode* root;
	GraphicObjectTypes filter;

	int checkIdx;
	std::queue<std::vector<SceneNode*>*> queue;
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

	SceneNode* pickNode(Vec<float> pnt, Vec<float> direction, unsigned int currentFrame, GraphicObjectTypes filter,float& depthOut);
	virtual SceneNode* pickNode(Vec<float> pnt, Vec<float> direction, GraphicObjectTypes filter, float& depthOut){return NULL;}

	// Transform updates
	DirectX::XMMATRIX getWorldRotation();

	void resetWorldTransform();
	void updateTranslation(Vec<float> origin);
	void updateRotation(DirectX::XMMATRIX& rotation);

	// Propagate rendering info
	virtual void requestUpdate();
	void updateRenderableList();

protected:
	virtual bool addChildNode(SceneNode* child);
	void updateTransforms(DirectX::XMMATRIX parentToWorldIn);

private:
	void makeRenderableList();

	bool bRenderListDirty;

	Vec<float> origin;
	DirectX::XMMATRIX rootRotationMatrix;

	std::vector<SceneNode*> rootChildrenNodes[Renderer::Section::SectionEnd];
	std::vector<std::vector<GraphicObjectNode*>> renderList[Renderer::Section::SectionEnd];
};
