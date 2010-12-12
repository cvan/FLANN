/***********************************************************************
 * Software License Agreement (BSD License)
 *
 * Copyright 2008-2009  Marius Muja (mariusm@cs.ubc.ca). All rights reserved.
 * Copyright 2008-2009  David G. Lowe (lowe@cs.ubc.ca). All rights reserved.
 *
 * THE BSD LICENSE
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *************************************************************************/

#ifndef RESULTSET_H
#define RESULTSET_H


#include <algorithm>
#include <limits>
#include <vector>

using namespace std;


namespace flann
{

/* This record represents a branch point when finding neighbors in
	the tree.  It contains a record of the minimum distance to the query
	point, as well as the node at which the search resumes.
*/

template <typename T>
struct BranchStruct {
	T node;           /* Tree node at which search resumes */
	float mindistsq;     /* Minimum distance to query for all nodes below. */

	BranchStruct() {};
	BranchStruct(const T& aNode, float dist) : node(aNode), mindistsq(dist) {};

	bool operator<(const BranchStruct<T>& rhs)
	{
        return mindistsq<rhs.mindistsq;
	}
};


class ResultSet
{
public:
	virtual ~ResultSet() {};

	virtual void init() = 0;

	virtual int* getNeighbors() = 0;

	virtual float* getDistances() = 0;

	virtual size_t size() const = 0;

	virtual bool full() const = 0;

	virtual void addPoint(float dist, int index) = 0;

	virtual float worstDist() const = 0;

};

class KNNResultSet : public ResultSet
{
	int* indices;
	float* dists;
    int capacity;
	int count;

public:
	KNNResultSet(int capacity_) : capacity(capacity_)
	{
        indices = new int[capacity_+1];
        dists = new float[capacity_+1];
        count = 0;
	}

	~KNNResultSet()
	{
		delete[] indices;
		delete[] dists;
	}

	void init()
	{
		count = 0;
		dists[capacity-1] = (numeric_limits<float>::max) ();
	}

	int* getNeighbors()
	{
		return indices;
	}

    float* getDistances()
    {
        return dists;
    }

    size_t size() const
    {
    	return count;
    }

	bool full() const
	{
		return count == capacity;
	}


	void addPoint(float dist, int index)
	{
//		for (int i=0;i<count;++i) {
//			if (indices[i]==index) return false;
//		}
		int i;
		for (i=count; i>0;--i) {
//			if ( (dists[i-1]>dist) || (dist==dists[i-1] && indices[i-1]>index) ) {
			if (dists[i-1]>dist) {
				dists[i] = dists[i-1];
				indices[i] = indices[i-1];
			}
			else break;
		}
		dists[i] = dist;
		indices[i] = index;
		if (count<capacity) count++;

//		if (count<capacity) {
//			indices[count] = index;
//			dists[count] = dist;
//			++count;
//		}
////		else if (dist < dists[count-1] || (dist == dists[count-1] && index < indices[count-1])) {
//         else if (dist < dists[count-1]) {
//			indices[count-1] = index;
//			dists[count-1] = dist;
//		}
//
//		register int i = count-1;
//		// bubble up
////		while (i>=1 && (dists[i]<dists[i-1] || (dists[i]==dists[i-1] && indices[i]<indices[i-1]) ) ) {
//         while (i>=1 && (dists[i]<dists[i-1]) ) {
//			swap(indices[i],indices[i-1]);
//			swap(dists[i],dists[i-1]);
//			i--;
//		}
	}

	float worstDist() const
	{
		return dists[capacity-1];
	}
};


/**
 * A result-set class used when performing a radius based search.
 */
class RadiusResultSet : public ResultSet
{
	struct Item {
		int index;
		float dist;

		bool operator<(Item rhs) {
			return dist<rhs.dist;
		}
	};

	vector<Item> items;
	float radius;

	bool sorted;
	int* indices;
	float* dists;
	size_t count;

private:
	void resize_vecs()
	{
		if (items.size()>count) {
			if (indices!=NULL) delete[] indices;
			if (dists!=NULL) delete[] dists;
			count = items.size();
			indices = new int[count];
			dists = new float[count];
		}
	}

public:
	RadiusResultSet(float radius_) :
		radius(radius_), indices(NULL), dists(NULL)
	{
		sorted = false;
		items.reserve(16);
		count = 0;
	}

	~RadiusResultSet()
	{
		if (indices!=NULL) delete[] indices;
		if (dists!=NULL) delete[] dists;
	}

	void init()
	{
		sorted = false;
		items.clear();
		count = 0;
	}

	int* getNeighbors()
	{
		if (!sorted) {
			sorted = true;
			sort_heap(items.begin(), items.end());
		}
		resize_vecs();
		for (size_t i=0;i<items.size();++i) {
			indices[i] = items[i].index;
		}
		return indices;
	}

    float* getDistances()
    {
		if (!sorted) {
			sorted = true;
			sort_heap(items.begin(), items.end());
		}
		resize_vecs();
		for (size_t i=0;i<items.size();++i) {
			dists[i] = items[i].dist;
		}
        return dists;
    }

    size_t size() const
    {
    	return items.size();
    }

	bool full() const
	{
		return true;
	}

	void addPoint(float dist, int index)
	{
		Item it;
		it.index = index;
		it.dist = dist;
		if (it.dist<=radius) {
			items.push_back(it);
			push_heap(items.begin(), items.end());
		}
	}

	float worstDist() const
	{
		return radius;
	}

};

}

#endif //RESULTSET_H
