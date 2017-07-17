#ifndef COEVENT_CLASS_H_
#define COEVENT_CLASS_H_

/*
Copyright or © or Copr. CNRS

This software is a computer program whose purpose is to estimate
phylogenies and evolutionary parameters from a dataset according to
the maximum likelihood principle.

This software is governed by the CeCILL  license under French law and
abiding by the rules of distribution of free software.  You can  use, 
modify and/ or redistribute the software under the terms of the CeCILL
license as circulated by CEA, CNRS and INRIA at the following URL
"http://www.cecill.info". 

As a counterpart to the access to the source code and  rights to copy,
modify and redistribute granted by the license, users are provided only
with a limited warranty  and the software's author,  the holder of the
economic rights,  and the successive licensors  have only  limited
liability. 

In this respect, the user's attention is drawn to the risks associated
with loading,  using,  modifying and/or developing or reproducing the
software by the user in light of its specific status of free software,
that may mean  that it is complicated to manipulate,  and  that  also
therefore means  that it is reserved for developers  and  experienced
professionals having in-depth computer knowledge. Users are therefore
encouraged to load and test the software's suitability as regards their
requirements in conditions enabling the security of their systems and/or 
data to be ensured and,  more generally, to use and operate it in the 
same conditions as regards security. 

The fact that you are presently reading this means that you have had
knowledge of the CeCILL license and that you accept its terms.
*/

/*

This file contains a class for co events -> events that span several nodes of gene tree(s)

Created the: 22-01-2016
by: Wandrille Duchemin

Last modified the: 07-07-2016
by: Wandrille Duchemin

*/

#include "ReconciledTree.h"
#include "AdjTree.h"

//#include <Bpp/Graph/AssociationGraphObserver.h>
#include <Bpp/Exceptions.h>

#include <set>

using namespace bpp;

struct Gene
{
	int Gfam;// GeneFamily index
	int NodeId;// node id of the gene in the reconciled tree of the corresponding GeneFamily

	int UTS;//upper time slice of the gene 
	int LTS;//lower time slice of the gene
};

struct AdjNode
{
	int EqClassId;// EquivalenceClass index
	int NodeId;// node id of the adj in the adjacency tree the the corresponding EquivalenceClass

	int n1; // index in AdjNode
	int n2;

};

class CoEvent
{
protected:
	vector < AdjNode > AdjList;//each element is a AdjNode struct 
	vector < Gene > GeneList; // each element is a Gene struct

	int UpperTS;//should be initialized at -1 if TSs don't matter
	int LowerTS;//should be equal to UpperTS outside of the BoundedTS framework

	int event; // following the codes defined in ReconciledTree.h
	int species;// id the the species the node is in

	int nbConnexComponents;

//	SimpleAssociationGraphObserver<Gene, AdjNode , SimpleGraph> grObs; //the relation between genes is modeled as a graph where nodes are Gene and edges are AdjNode.

	bool HasNeighbors(int id);

	vector < int > exploreOneConnexComponent(int SourceId);

public:

	CoEvent() //: grObs(false)
	{
		UpperTS = -1;
		LowerTS = UpperTS;
		nbConnexComponents = -1;
	}

	~CoEvent(){}

	//getters
	int getEvent(){return event;}
	int getSpecies(){return species;}
	int getUpperTS(){return UpperTS;}
	int getLowerTS(){return LowerTS;}

	//setters
	void setEvent(int e){event = e;}
	void setSpecies(int s){species = s;}


	int getGeneNumberOfNeighbors(int id);

	int getNumberOfGene();
	int getNumberOfAdj();
	int getNbConnexComponents();

	Gene getGene(int i);
	AdjNode getAdj(int i);

	int getGeneIndex(int gfam, int nodeid);//returns -1 if the Gene is absent
	int getAdjIndex(int eqclassid, int nodeid);//returns -1 if the Gene is absent

	//adding Gene or Adj
	int addGene(int Gfam, int NodeId);// also checks if the gene already exists; Does not set the UTS/LTS
	int addGene(int Gfam, int NodeId, ReconciledTree * Rtree);//also checks if the gene already exists and sets the UTS/LTS
	

	int addAdj(int EqClassId, int NodeId, AdjTree * Atree, int Gfam1, int Gfam2);//also checks if the AdjNode already exists, as well as the Genes. 
	int addAdj(int EqClassId, int NodeId, AdjTree * Atree, int Gfam1, int Gfam2, ReconciledTree * Rtree1, ReconciledTree * Rtree2);//also checks if the AdjNode already exists, as well as the Genes. Sets the UTS/LTS 

	//removing gene or Adj
	void removeGene(int i); //removes Gene at index i
	//void removeGeneAndLinkedAdj(int i); // removes Gene at index i as well as all NodeAdj implicating this Gene; Also updates UTS and LTS of the CoEvent
	void removeAdj(int i); // removes NodeAdj at index i;
	void removeAdjAndLinkedGene(int i); // removes NodeAdj at index i as well as all Gene that are not participating to another edge in the CoEvent; Also updates UTS and LTS of the CoEvent

	void updateTimeSlices();

	bool isCompatible(int NodeId, AdjTree * Atree);
	bool isCompatible(int NodeId, AdjTree * Atree, int UTS, int LTS); // also checks timeslices

	double getLinearScore(double Dcost, double Tcost, double Lcost);


	vector <int> getGeneNeighbors(int Gid);
	vector < vector <int> > getConnexComponents();


	bool removeGeneFam(int GfamId);
	bool removeECF(int ECFId);
};


#endif