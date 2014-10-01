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

#ifndef TRACK_H
#define TRACK_H

#include "Hull.h"
#include "Utility.h"

#include <map>
#include <vector>

class Track
{
public:
	Track();
	Track(unsigned int label);
	~Track();

	unsigned int getHullOnFrame(unsigned int frame);
	unsigned int getFirstHull();
	const std::map<unsigned int, unsigned int>* getHulls();
	unsigned int getStartFrame(){return startFrame;}
	unsigned int getEndFrame(){return endFrame;}
	unsigned int getTrackLength(){return endFrame-startFrame;}
	Vec<float> getColor(){return color;}
	unsigned int getLabel(){return label;}

	unsigned int addHull(unsigned int frame, unsigned int hull);
	void setColor(Vec<float> color){this->color=color;}
	void setLabel(unsigned int label){this->label=label;}
	void updateTrackDuration();
	void changeLabel(unsigned int newLabel,unsigned int frame);
	void combineTracks(unsigned int track);
	void removeHull(unsigned int hull, int frame);

private:
	unsigned int label;
	std::map<unsigned int, unsigned int> hulls; // key = frame, value = hull index
	unsigned int startFrame;
	unsigned int endFrame;
	unsigned int deathFrame;
	Vec<float> color;

	void clear();
	void split(unsigned int frame);
};

extern std::vector<Track> gTracks;

#endif