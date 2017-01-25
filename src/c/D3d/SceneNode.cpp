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

#include "SceneNode.h"
#include "Global/Globals.h"
#include "Global/ErrorMsg.h"

#undef max

// Class method implementations
SceneNode::SceneNode(int index, GraphicObjectTypes type)
	: parentNode(NULL), index(index), type(type)
{
	localToParentTransform = DirectX::XMMatrixIdentity();
	parentToWorld = DirectX::XMMatrixIdentity();
}

SceneNode::SceneNode(GraphicObjectTypes type)
	: SceneNode(-1, type)
{}

SceneNode::~SceneNode()
{
	for (std::vector<SceneNode*>::iterator it=childrenNodes.begin(); it!=childrenNodes.end(); ++it)
	{
		SceneNode* cur = *it;
		cur->parentNode = NULL;
		delete cur;
	}

	childrenNodes.clear();

	parentNode = NULL;
}

void SceneNode::update()
{
	updateTransforms(parentToWorld);
}

void SceneNode::attachToParentNode(SceneNode* parent)
{
	if ( !parent->addChildNode(this) )
		return;

	setParentNode(parent);

	// Propagate type counts up the tree
	updateAddTypes();

	updateTransforms(parent->getLocalToWorldTransform());
	requestUpdate();
}

void SceneNode::detatchFromParentNode()
{
	parentNode->detatchChildNode(this);

	// Remove type counts up the tree
	updateSubtractTypes();

	setParentNode(NULL);
}


void SceneNode::setLocalToParent(DirectX::XMMATRIX transform)
{
	localToParentTransform = transform;
	updateTransforms(parentToWorld);
}

DirectX::XMMATRIX SceneNode::getLocalToWorldTransform() const
{
	return localToParentTransform*parentToWorld;
}


SceneNode* SceneNode::pickNode(Vec<float> pnt, Vec<float> direction, GraphicObjectTypes filter, float& depthOut)
{
	SceneNode* nodeOut = NULL;
	depthOut = std::numeric_limits<float>::max();

	for (int i=0; i<childrenNodes.size(); ++i)
	{
		float curDepth;

		SceneNode* node = childrenNodes[i]->pickNode(pnt,direction,filter, curDepth);

		if ( !node )
			continue;

		if (curDepth < depthOut)
		{
			nodeOut = node;
			depthOut = curDepth;
		}
	}

	return nodeOut;
}


void SceneNode::setParentNode(SceneNode* parent)
{
	parentNode = parent;
}

const std::vector<SceneNode*>& SceneNode::getChildren()
{
	return childrenNodes;
}

void SceneNode::updateTransforms(DirectX::XMMATRIX parentToWorldIn)
{
	parentToWorld = parentToWorldIn;

	for (std::vector<SceneNode*>::iterator it=childrenNodes.begin(); it!=childrenNodes.end(); ++it)
	{
		(*it)->updateTransforms(localToParentTransform*parentToWorld);
	}
}

bool SceneNode::addChildNode(SceneNode* child)
{
	//TODO check for dups (cycles)
	childrenNodes.push_back(child);

	return true;
}

void SceneNode::requestUpdate()
{
	if (parentNode!=NULL)
		parentNode->requestUpdate();
}

void SceneNode::updateAddTypes()
{
	Histogram deltas = childTypes;
	deltas(type) += 1;

	parentNode->addTypes(deltas);
}

void SceneNode::updateSubtractTypes()
{
	Histogram deltas = childTypes;
	deltas(type) += 1;

	parentNode->subtractTypes(deltas);
}

void SceneNode::addTypes(const Histogram& deltas)
{
	childTypes += deltas;

	if ( parentNode )
		parentNode->addTypes(deltas);
}

void SceneNode::subtractTypes(const Histogram& deltas)
{
	childTypes -= deltas;
	if ( parentNode )
		parentNode->subtractTypes(deltas);
}

void SceneNode::detatchChildNode(SceneNode* child)
{
	for (std::vector<SceneNode*>::iterator it=childrenNodes.begin(); it!=childrenNodes.end(); ++it)
	{
		if (*it==child)
		{
			childrenNodes.erase(it,it+1);
			break;
		}
	}
}



GraphicObjectNode::GraphicObjectNode(int index, GraphicObjectTypes type, std::shared_ptr<MeshPrimitive> mesh, std::shared_ptr<Material> material)
	: SceneNode(index, type), mesh(mesh), material(material)
{
	renderable = true;
}

void GraphicObjectNode::releaseRenderResources()
{
	if (parentNode!=NULL)
		detatchFromParentNode();
	
	renderable = false;
}

void GraphicObjectNode::setRenderable(bool render)
{
	renderable = render;
	requestUpdate();
}

void GraphicObjectNode::setWireframe(bool wireframe)
{
	material->setWireframe(wireframe);
}


SceneNode* GraphicObjectNode::pickNode(Vec<float> pnt, Vec<float> direction, GraphicObjectTypes filter, float& depthOut)
{
	if ( type != filter )
		return NULL;

	DirectX::XMVECTOR det;
	DirectX::XMMATRIX locl = DirectX::XMMatrixInverse(&det,getLocalToWorldTransform());

	DirectX::XMFLOAT3 viewPntF(pnt.x,pnt.y,pnt.z);
	DirectX::XMVECTOR viewPntV = DirectX::XMLoadFloat3(&viewPntF);
	DirectX::XMFLOAT3 viewDirF(direction.x,direction.y,direction.z);
	DirectX::XMVECTOR viewDirV = DirectX::XMLoadFloat3(&viewDirF);

	DirectX::XMVECTOR lclPnt = DirectX::XMVector3TransformCoord(viewPntV,locl);
	DirectX::XMVECTOR lclDir = DirectX::XMVector3TransformNormal(viewDirV,locl);

	Vec<float> lclPntVec(DirectX::XMVectorGetX(lclPnt),DirectX::XMVectorGetY(lclPnt),DirectX::XMVectorGetZ(lclPnt));
	Vec<float> lclDirVec(DirectX::XMVectorGetX(lclDir),DirectX::XMVectorGetY(lclDir),DirectX::XMVectorGetZ(lclDir));

	if ( mesh->checkIntersect(lclPntVec,lclDirVec,depthOut) )
		return this;

	return NULL;
}


void GraphicObjectNode::updateTransforms(DirectX::XMMATRIX parentToWorldIn)
{
	parentToWorld = parentToWorldIn;
	localToWorld = mesh->computeLocalToWorld(localToParentTransform * parentToWorld);
}

GraphicObjectNode::~GraphicObjectNode()
{
	releaseRenderResources();
}



RenderFilter::RenderFilter(SceneNode* rootNode, GraphicObjectTypes filter)
	: root(rootNode), filter(filter), checkIdx(0)
{}

GraphicObjectNode* RenderFilter::first()
{
	checkIdx = 0;
	queue.swap(queue);

	if ( checkChildren(root, filter) )
		queue.push(&root->childrenNodes);

	// Return the root node if it's renderable
	if ( checkFilter(root, filter) )
		return dynamic_cast<GraphicObjectNode*>(root);

	// Otherwise just start traversing in breadth-first order
	return next();
}

GraphicObjectNode* RenderFilter::next()
{
	SceneNode* nextNode = getNext();
	++checkIdx;

	if ( nextNode )
		return dynamic_cast<GraphicObjectNode*>(nextNode);

	return NULL;
}

SceneNode* RenderFilter::getNext()
{
	while ( !queue.empty() )
	{
		std::vector<SceneNode*>& childList = *queue.front();
		for ( ; checkIdx < childList.size(); ++checkIdx )
		{
			if ( checkChildren(childList[checkIdx],filter) )
				queue.push(&childList[checkIdx]->childrenNodes);

			if ( checkFilter(childList[checkIdx],filter) )
				return childList[checkIdx];
		}

		queue.pop();
		checkIdx = 0;
	}

	return NULL;
}

bool RenderFilter::checkFilter(SceneNode* node, GraphicObjectTypes filter)
{
	if ( !node->isRenderable() )
		return false;

	return (node->type == filter);
}

bool RenderFilter::checkChildren(SceneNode* node, GraphicObjectTypes filter)
{
	return (node->childTypes(filter) > 0);
}



RootSceneNode::RootSceneNode()
	: SceneNode(GraphicObjectTypes::Group)
{
	for (int i=0; i<Renderer::Section::SectionEnd; ++i)
	{
		rootChildrenNodes[i].push_back(new SceneNode(GraphicObjectTypes::Group));
		rootChildrenNodes[i][0]->setParentNode(this);
	}

	resetWorldTransform();
	makeRenderableList();
}

void RootSceneNode::resetWorldTransform()
{
	origin = Vec<float>(0.0f,0.0f,0.0f);
	rootRotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(0.0f,0.0f,DirectX::XM_PI);
	updateTransforms(parentToWorld);
}

RootSceneNode::~RootSceneNode()
{
	for (int i=0; i<Renderer::Section::SectionEnd; ++i)
	{
		for (int j=0; j<rootChildrenNodes[i].size(); ++j)
			delete rootChildrenNodes[i][j];
		rootChildrenNodes[i].clear();
	}
}

SceneNode* RootSceneNode::getRenderSectionNode(Renderer::Section section, int frame)
{
	if (frame>=rootChildrenNodes[section].size())
	{
		int oldSize = int(rootChildrenNodes[section].size());
		rootChildrenNodes[section].resize(frame+1);
		for (int i=oldSize; i<=frame; ++i)
		{
			rootChildrenNodes[section][i] = new SceneNode(GraphicObjectTypes::Group);
			rootChildrenNodes[section][i]->setParentNode(this); //TODO rethink this
		}
	}

	return rootChildrenNodes[section][frame];
}

const std::vector<GraphicObjectNode*>& RootSceneNode::getRenderableList(Renderer::Section section,unsigned int frame)
{
	frame = frame % rootChildrenNodes[section].size();
	if (renderList[section].size()<=frame)
		sendErrMessage("Render list is malformed!");

	return renderList[section][frame];
}

void RootSceneNode::updateTransforms(DirectX::XMMATRIX parentToWorldIn)
{
	parentToWorld = parentToWorldIn;

	DirectX::XMMATRIX transMatrix = DirectX::XMMatrixTranslation(-origin.x,-origin.y,-origin.z);

	for(int i=0; i<rootChildrenNodes[Renderer::Section::Pre].size(); ++i)
	{
		SceneNode* node = rootChildrenNodes[Renderer::Section::Pre][i];
		node->updateTransforms(transMatrix * rootRotationMatrix * parentToWorld);
	}

	for(int i=0; i<rootChildrenNodes[Renderer::Section::Main].size(); ++i)
	{
		SceneNode* node = rootChildrenNodes[Renderer::Section::Main][i];
		node->updateTransforms(transMatrix * rootRotationMatrix * parentToWorld);
	}

	for(int i=0; i<rootChildrenNodes[Renderer::Section::Post].size(); ++i)
	{
		SceneNode* node = rootChildrenNodes[Renderer::Section::Post][i];
		node->updateTransforms(rootRotationMatrix * parentToWorld);
	}
}

int RootSceneNode::getNumFrames()
{
	int maxFrames = 0;

	for (int i=0; i<Renderer::Section::SectionEnd; ++i)
		if (maxFrames < rootChildrenNodes[i].size())
			maxFrames = int(rootChildrenNodes[i].size());

	return maxFrames;
}


SceneNode* RootSceneNode::pickNode(Vec<float> pnt, Vec<float> direction, unsigned int currentFrame, GraphicObjectTypes filter, float& depthOut)
{
	std::vector<SceneNode*> children = rootChildrenNodes[Renderer::Section::Main][currentFrame]->getChildren();

	SceneNode* nodeOut = NULL;
	depthOut = std::numeric_limits<float>::max();

	for (int i=0; i<children.size(); ++i)
	{
		float curDepth;
		SceneNode* node = children[i]->pickNode(pnt,direction,filter, curDepth);

		if ( !node )
			continue;

		if (curDepth < depthOut)
		{
			nodeOut = node;
			depthOut = curDepth;
		}
	}

	return nodeOut;
}

DirectX::XMMATRIX RootSceneNode::getWorldRotation()
{
	return rootRotationMatrix;
}

bool RootSceneNode::addChildNode(SceneNode* child)
{
	sendErrMessage("You cannot attach to a root node using this method!");
	return false;
}

void RootSceneNode::requestUpdate()
{
	bRenderListDirty = true;
}

void RootSceneNode::updateRenderableList()
{
	if ( !bRenderListDirty )
		return;

	makeRenderableList();
}


void RootSceneNode::updateTranslation(Vec<float> origin)
{
	this->origin = origin;
	updateTransforms(parentToWorld);
}

void RootSceneNode::updateRotation(DirectX::XMMATRIX& rotation)
{
	rootRotationMatrix = rotation;
	updateTransforms(parentToWorld);
}

void RootSceneNode::makeRenderableList()
{
	for (int i=0; i<Renderer::Section::SectionEnd; ++i)
	{
		renderList[i].clear();
		renderList[i].resize(rootChildrenNodes[i].size());

		for (int k=0; k<rootChildrenNodes[i].size(); ++k)
		{
			std::vector<SceneNode*> curChildList;
			curChildList.push_back(rootChildrenNodes[i][k]);

			size_t j = 0;
			while (j<curChildList.size())
			{
				const std::vector<SceneNode*>& children = curChildList[j]->getChildren();
				curChildList.insert(curChildList.end(),children.begin(),children.end());

				if (curChildList[j]->isRenderable())
					renderList[i][k].push_back((GraphicObjectNode*)curChildList[j]);

				++j;
			}

			curChildList.clear();
		}
	}

	bRenderListDirty = false;
}
