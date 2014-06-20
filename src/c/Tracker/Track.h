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