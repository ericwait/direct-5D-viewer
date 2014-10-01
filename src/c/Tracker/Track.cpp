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

#include "Track.h"
#include "tracker.h"

std::vector<Track> gTracks;

Track::Track()
{
	clear();
}

Track::Track(unsigned int label)
{
	clear();
	for (unsigned int i=0; i<gTracks.size(); ++i)
	{
		if (gTracks.at(i).getLabel()==label)
		{

#ifdef _DEBUG
			char buffer[255];
			sprintf_s(buffer,255,"Track wanted label %d, gave it label %d",label,label*2);
			printf(buffer);
#endif // _DEBUG

			label *= 2;
			break;
		}
	}
	this->label = label;
}

Track::~Track()
{
	clear();
}

void Track::clear()
{
	hulls.clear();
	startFrame = UNSET_VAR;
	endFrame = UNSET_VAR;
	label = UNSET_VAR;
	color.x = 0;
	color.y = 0;
	color.z = 0;
	deathFrame = UNSET_VAR;
}

unsigned int Track::getHullOnFrame(unsigned int frame)
{
	if (0==hulls.count(frame))
		return UNSET_VAR;

	return hulls.find(frame)->second;
}

unsigned int Track::getFirstHull()
{
	std::map<unsigned int, unsigned int>::iterator it = hulls.begin();
	return it->second;
}

const std::map<unsigned int, unsigned int>* Track::getHulls()
{
	return &hulls;
}

/************************************************************************
Will insert a hull at the given frame and will return the number of the
hull it replaced or UNSET_VAR otherwise
************************************************************************/
unsigned int Track::addHull(unsigned int frame, unsigned int hull)
{
	std::pair<std::map<unsigned int,unsigned int>::iterator,bool> returnValue;
	returnValue = hulls.insert(std::pair<unsigned int,unsigned int>(frame,hull));

	if (!returnValue.second)
	{
		unsigned int existingHull = returnValue.first->second;
		hulls[frame] = hull;

		return existingHull;
	}

	if(((unsigned int) UNSET_VAR)==startFrame || startFrame>frame)
		startFrame = frame;

	if(((unsigned int) UNSET_VAR)==endFrame || endFrame<frame)
		endFrame = frame;

	gHulls[hull].setTrack(this->label);

	return UNSET_VAR;
}


/************************************************************************
split will make a new track that consists of all the hulls between the given
frame and the end (inclusive).
split will take the new track and attach it to this as a child.
************************************************************************/
void Track::split(unsigned int frame)
{
	unsigned int newTrack = (unsigned int)gTracks.size();
	Track track(newTrack);

	//move the hulls over
	std::map<unsigned int, unsigned int>::iterator it = this->hulls.begin();
	for (; it!=this->hulls.end(); ++it)
	{
		if (it->first>=frame)
		{
			track.hulls.insert(std::pair<unsigned int, unsigned int>(it->first,it->second));
			//this->hulls.erase(it);
		}
	}

	it = hulls.begin();
	for (; it!=hulls.end(); ++it)
	{
		if (it->first>=frame)
			break;
	}

	if (it!=hulls.end())
	{
		this->hulls.erase(it,this->hulls.end());
	}

	//update the start and end frames
	track.updateTrackDuration();
	this->updateTrackDuration();

	//add the new track to the global container
	gTracks.push_back(track);
}

/************************************************************************
Updates start and end times
************************************************************************/
void Track::updateTrackDuration()
{
	startFrame = -1;
	endFrame = 0;

	std::map<unsigned int, unsigned int>::iterator it = hulls.begin();
	for (; it!=hulls.end(); ++it)
	{
		if (it->first<startFrame)
			startFrame = it->second;
		if (it->first>endFrame)
			endFrame = it->second;
	}
}

void Track::changeLabel(unsigned int newLabel,unsigned int frame)
{
	if (startFrame>frame || endFrame<frame)
	{
		return;
	}

	unsigned int newTrack = UNSET_VAR;

	//Remove subtree from this point forth
	if (startFrame<frame)
	{
		split(frame);
	}else
	{
		newTrack = this->label;
	}

	//Check for an existing label to attach to
	if (gTracks.size()-1>newLabel)
	{
		gTracks[newLabel].combineTracks(newTrack);
	}
}

void Track::combineTracks(unsigned int otherTrack)
{
	// is the otherTrack just one hull
	if (0==gTracks.at(otherTrack).endFrame-gTracks.at(otherTrack).startFrame)
	{
		if (UNSET_VAR==addHull(gTracks.at(otherTrack).getStartFrame(),gTracks.at(otherTrack).getHullOnFrame(gTracks.at(otherTrack).getStartFrame())))
		{
			;//TODO deal with this error
		}else
		{
			gTracks.at(otherTrack).clear();
			return;
		}
	}else if (startFrame<gTracks.at(otherTrack).startFrame && endFrame<gTracks.at(otherTrack).startFrame)
	{// does the otherTrack exist after this
		std::map<unsigned int,unsigned int>::iterator it = gTracks.at(otherTrack).hulls.begin();
		for (; it!=gTracks.at(otherTrack).hulls.end(); ++it)
		{
			if (UNSET_VAR==this->addHull(it->first,it->second))
			{
				//TODO Handle this error
				return;
			}
		}
		// clear out otherTrack
		gTracks.at(otherTrack).clear();
	}
}

void Track::removeHull(unsigned int hull, int frame)
{
	std::map<unsigned int, unsigned int>::iterator del = hulls.find(frame);
	if (del->second!=hull)
	{
		for (del=hulls.begin(); del!=hulls.end(); ++del)
		{
			if (del->second==hull)
				break;
		}
	}

	if (del->second!=hull)
		return;

	hulls.erase(del);

	if (hulls.empty())
	{
		clear();
	}else
	{
		updateTrackDuration();
	}
}