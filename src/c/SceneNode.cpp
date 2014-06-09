#include "SceneNode.h"

SceneNode::SceneNode()
{
	localToParentTransform = DirectX::XMMatrixIdentity();
	parentToWorld = DirectX::XMMatrixIdentity();
	parentNode=NULL;
}

SceneNode::~SceneNode()
{
	for (std::vector<SceneNode*>::iterator it=childrenNodes.begin(); it!=childrenNodes.end(); ++it)
		delete *it;

	childrenNodes.clear();
	parentNode = NULL;
}

void SceneNode::update()
{
	updateTransforms(parentToWorld);
}

void SceneNode::attachToParentNode(SceneNode* parent)
{
	parent->addChildNode(this);
	parentNode = parent;
	updateTransforms(parent->getLocalToWorldTransform());
	requestUpdate();
}

void SceneNode::setLocalToParent(DirectX::XMMATRIX transform)
{
	localToParentTransform = transform;
	updateTransforms(parentToWorld);
}

void SceneNode::updateTransforms(DirectX::XMMATRIX parentToWorldIn)
{
	parentToWorld = parentToWorldIn;

	for (std::vector<SceneNode*>::iterator it=childrenNodes.begin(); it!=childrenNodes.end(); ++it)
	{
		(*it)->updateTransforms(localToParentTransform*parentToWorld);
	}
}

DirectX::XMMATRIX SceneNode::getLocalToWorldTransform()
{
	return localToParentTransform*parentToWorld;
}

const std::vector<SceneNode*>& SceneNode::getChildren()
{
	return childrenNodes;
}

void SceneNode::requestUpdate()
{
	if (parentNode!=NULL)
		parentNode->requestUpdate();
}

void SceneNode::setParentNode(SceneNode* parent)
{
	parentNode = parent;
}

void SceneNode::addChildNode(SceneNode* child)
{
	//TODO check for dups
	childrenNodes.push_back(child);
}

GraphicObjectNode::GraphicObjectNode(GraphicObject* graphicObjectIn)
{
	graphicObject = graphicObjectIn;
}

void GraphicObjectNode::updateTransforms(DirectX::XMMATRIX parentToWorldIn)
{
	parentToWorld = parentToWorldIn;
	graphicObject->makeLocalToWorld(localToParentTransform*parentToWorld);
}

const RendererPackage* GraphicObjectNode::getRenderPackage()
{
	return graphicObject->getRenderPackage();
}

void GraphicObjectNode::attachToParentNode(SceneNode* parent)
{
	SceneNode::attachToParentNode(parent);
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

			std::vector<SceneNode*>::iterator it = curChildList.begin();
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
}

const std::vector<GraphicObjectNode*>& RootSceneNode::getRenderableList(Renderer::Section section,unsigned int frame)
{
	frame = frame % rootChildrenNodes[section].size();
	if (renderList[section].size()<=frame)
		throw std::runtime_error("Renderlist is malformed!");

	return renderList[section][frame];
}

void RootSceneNode::requestUpdate()
{
	makeRenderableList();
}

RootSceneNode::RootSceneNode() : SceneNode()
{
	for (int i=0; i<Renderer::Section::SectionEnd; ++i)
	{
		rootChildrenNodes[i].push_back(new SceneNode);
		rootChildrenNodes[i][0]->setParentNode(this);
	}

	makeRenderableList();
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
		int oldSize = rootChildrenNodes[section].size();
		rootChildrenNodes[section].resize(frame+1);
		for (int i=oldSize; i<=frame; ++i)
		{
			rootChildrenNodes[section][i] = new SceneNode;
		}
	}

	return rootChildrenNodes[section][frame];
}

void RootSceneNode::updateTransforms(DirectX::XMMATRIX parentToWorldIn)
{
	parentToWorld = parentToWorldIn;

	for (int i=0; i<Renderer::Section::SectionEnd; ++i)
		for (int j=0; j<rootChildrenNodes[i].size(); ++j)
			rootChildrenNodes[i][j]->updateTransforms(localToParentTransform*parentToWorld);
}

int RootSceneNode::getNumFrames()
{
	int maxFrames = 0;

	for (int i=0; i<Renderer::Section::SectionEnd; ++i)
		if (maxFrames < rootChildrenNodes[i].size())
			maxFrames = rootChildrenNodes[i].size();

	return maxFrames;
}
