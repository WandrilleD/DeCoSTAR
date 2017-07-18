#ifndef ADJ_MATRIX_H_
#define ADJ_MATRIX_H_

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

This file contains a class for a matrix for adjacency history

Created the: 30-11-2015
by: Wandrille Duchemin

Last modified the: 17-07-2017
by: Wandrille Duchemin

*/

#define MYINFINIY numeric_limits<double>::max();
#define MYMINFINIY -numeric_limits<double>::max();


#include <stdlib.h>

#include <iostream>
#include <fstream>

#include "MyMatrix.h"
#include "ReconciledTree.h"
#include "AdjTree.h"
#include "MyMatrixAdjSolution.h"

const double BOLTZMANN_K = 1; // not true but for now this will do

using namespace std;
using namespace bpp;

//helper functions
void printTuple(boost::tuples::tuple<int,int,bool> t);
void printTupleV(vector< boost::tuples::tuple<int,int,bool> >  V);
void printTupleVV(vector< vector< boost::tuples::tuple<int,int,bool> > > V);
void printTupleVVM(map< int , vector< vector< boost::tuples::tuple<int,int,bool> > > > M);


bool identVectors( vector< boost::tuples::tuple<int,int,bool> >  V1, vector< boost::tuples::tuple<int,int,bool> >  V2);
vector< vector< boost::tuples::tuple<int,int,bool> > > DiffVectors(vector< vector< boost::tuples::tuple<int,int,bool> > > V1,vector< vector< boost::tuples::tuple<int,int,bool> > > V2);
vector< vector< boost::tuples::tuple<int,int,bool> > > UnionVectors(vector< vector< boost::tuples::tuple<int,int,bool> > > V1,vector< vector< boost::tuples::tuple<int,int,bool> > > V2);
vector< vector< boost::tuples::tuple<int,int,bool> > > combineVectors(vector< vector< boost::tuples::tuple<int,int,bool> > > V1,vector< vector< boost::tuples::tuple<int,int,bool> > > V2);

vector< vector< boost::tuples::tuple<int,int,bool> > > AddUpVectors( vector< vector< boost::tuples::tuple<int,int,bool> > > V1,vector< vector< boost::tuples::tuple<int,int,bool> > > V2);  // LA modif

class AdjMatrix
{
protected:

	///Backtrack Count correction
	vector< vector< boost::tuples::tuple<int,int,bool> > > getListListReturnCaseAdj( AdjSolution s, bool IncompOnly);
	vector< vector< boost::tuples::tuple<int,int,bool> > >	getListListReturnCase(int id1, int id2, bool IncompOnly,bool c1=false);

	// matrix of maps linking adjSolution indexes to the list of C1 roots they allow backtrack to
	vector < vector< map<int , vector< vector< boost::tuples::tuple<int,int,bool> > > > > > C1BacktrackRootMaps;
	
	map<int , vector< vector< boost::tuples::tuple<int,int,bool> > > > getC1BacktrackRootMaps(int id1, int id2);
	void setC1BacktrackRootMaps(int id1, int id2, map<int , vector< vector< boost::tuples::tuple<int,int,bool> > > > M);
	void setC1BacktrackRootMaps(int id1, int id2, int i , vector< vector< boost::tuples::tuple<int,int,bool> > > V);

	double setMapsBacktrackC0(int id1, int id2);

	void clearC1BacktrackRootMaps()
	{
		int d1 = SolutionMatrixC1.getDim1();
		int d2 = SolutionMatrixC1.getDim2(); 

		for(unsigned i = 0 ; i <d1 ;i++)
		{
			for(unsigned j = 0 ; j < d2 ;j++)
			{
				for(map<int , vector< vector< boost::tuples::tuple<int,int,bool> > > >::iterator itM = C1BacktrackRootMaps[i][j].begin(); itM != C1BacktrackRootMaps[i][j].end() ; ++itM)
				{
					for(vector< vector< boost::tuples::tuple<int,int,bool> > >::iterator itV1 = itM->second.begin(); itV1 != itM->second.end() ; ++itV1)
					{
						(*itV1).clear();
					}
					itM->second.clear();
				}
				C1BacktrackRootMaps[i][j].clear();
			}
			C1BacktrackRootMaps[i].clear();
		}
		C1BacktrackRootMaps.clear();
	}

	///Backtrack count

	vector < vector<double> > NbBacktrackC1;
	vector < vector<double> > NbBacktrackC0;

	vector< vector< map< int,double > > > MapNbBacktrackC1;
	vector< vector< map< int,double > > > MapNbBacktrackC0;

	void InitNbBacktrackMatrices(); // also inits the map ones
	void computeNbBacktrackMatrix(bool doC1, bool doC0);

	double getNbBacktrack(AdjSolution s);
	double getNbBacktrack(AdjSolution s, vector < pair<int,int> > &nonSet, vector <bool> &nonSetC1);
	double getNbBacktrackV(vector<AdjSolution> Vsol, bool keepOnlyBest=true);
	map<int,double> getMapNbBacktrackV(vector<AdjSolution> Vsol, bool keepOnlyBest=true);
	map<int,double> getMapNbBacktrackV(vector<AdjSolution> Vsol, vector < pair<int,int> > &nonSet, vector <bool> &nonSetC1, bool keepOnlyBest=true);

	void setNbBacktrackMatrix(int id1, int id2, double nbbt , bool c1=true);
	double getNbBacktrackMatrix(int id1, int id2, bool c1=true);

	void setMapNbBacktrackMatrix(int id1, int id2, map< int,double > nbbtMap , bool c1=true);
	map< int,double > getMapNbBacktrackMatrix(int id1, int id2, bool c1=true);

	AdjSolution chooseBestSolutionUsingBacktrackCount(int id1, int id2, bool c1=true);
	AdjSolution chooseRandomSolutionUsingBacktrackCount(int id1, int id2, bool c1=true);

	bool backtrackCountMatrixDone;
	///



	MyMatrix RootMatrix;
	vector < pair<int,int> > rootList;
	bool rootComputed;

	void backtrackAux( vector< AdjTree *> * AdjacencyTrees, bool stochastic, bool alwaysGainAtTop , double c1proba , int Root1, int Root2);


	bool isImpossibleCase(vector<AdjSolution> aSolC1, vector<AdjSolution> aSolC0);

	void initRootMatrix();
	void scanSolutionForRooting(int id1, int id2);

	void scanForRoots();
	vector < pair<int,int> > getMatrixRoots();



	double GainCost; // cost of a single gain
	double BreakCost; // cost of a single break

	double WeightedDupCost ;//Cost of a single reconciliation event, weighted so that this score can be added to an adjacency score. This is defaulting at 0 and set only if requested when computing the matrix. Used to subtract a reconciliation event to the adjacency scores of co-events.
	double WeightedLossCost;//Cost of a single reconciliation event, weighted so that this score can be added to an adjacency score. This is defaulting at 0 and set only if requested when computing the matrix. Used to subtract a reconciliation event to the adjacency scores of co-events.
	double WeightedHgtCost ;//Cost of a single reconciliation event, weighted so that this score can be added to an adjacency score. This is defaulting at 0 and set only if requested when computing the matrix. Used to subtract a reconciliation event to the adjacency scores of co-events.	

	double worstScore; // worst possible score (infinity for the DeCo case; 0 for the DeCoBoltzmann case)
	double bestScore; // best possible score (0 for the DeCo case; 1 for the DeCoBoltzmann case)
	double defaultScore; // default score for cases that haven't been computed yet ( -1 in both case)
	double worstAbsenceScore; // worst possible score for an absent adjacency presence( == worstScore unless some option is set at the AdjMatrix creation)

	map<int,vector<float> > speciesC0C1;
	map<int, map<string,int> > speGeneAdjNb;
	map<int, map<string,pair<int,int> > > speGeneExtremitiesAdjNb;

	ReconciledTree Rtree1;
	ReconciledTree Rtree2;


	MyMatrix MatrixC0;
	MyMatrix MatrixC1;

	MyMatrixAdjSolution SolutionMatrixC0;
	MyMatrixAdjSolution SolutionMatrixC1;

	//matrix ids go from 0 to Dim without interuption; while tree ids have no particular obligations baout that -> we need map linking one to the other

	map <int,int> TreeToMatrixId1;
	map <int,int> TreeToMatrixId2;

	map <int,int> MatrixToTreeId1;
	map <int,int> MatrixToTreeId2;

	bool matrixComputed;

	bool verbose;

	bool useBoltzmann;
	double Temperature;

	bool decoLTalgo;

	//LA modif
	bool LossAware;
	pair < vector < pair <string, string> >, bool > currFreeAdjacencies;

	int Gfam1;
	int Gfam2;

	int Gsens1;
	int Gsens2;
	//end LA modif



//// methods that will be used by the score algebra
	double addition(double const& a, double const& b);
	double multiplication(double const& a, double const& b);

	double getminimum(vector <double> const& v);
	double getsum(vector <double> const& v);

/////pointers to these methods
	typedef double (AdjMatrix::*ScoreAggregator)(double const& a,double const& b);
	typedef double (AdjMatrix::*ScoreComparator)(vector <double> const& v);  	

	ScoreAggregator scoreAggregatorfunc;
	ScoreComparator scoreComparatorfunc;


///methods


	void AdjMatrixAux(map<int,vector<float> > &speciesC0C1, map<int, map<string,int> > &speGeneAdjNb,
							map<int, map<string,pair<int,int> > > &speGeneExtremitiesAdjNb,
							 vector <double> &adjacencyScoreVec, double Gcost, double Bcost, 
							 ReconciledTree * rtree1, ReconciledTree * rtree2, vector< pair <int,int> > &adjacencies,
							 int Gfamily1, int Gfamily2,																//LAmodif
							 bool lossaware, pair < vector < pair <string, string> >, bool > FamiliesFreeAdjacencies,	//LAmodif
							 bool VERBOSE, bool boltzmann , double temp , double absencePenalty, double adjScoreLogBase,
							  int sens1 =0, int sens2 =0);


	void BuildIdMaps();

	void addAdjacency(pair <int,int> adjacency);
	void addAdjacency(pair <int,int> adjacency, double score, double adjScoreLogBase);

	void initMatrix();
	void initAdjAbsence(map<int,vector<float> > &speciesC0C1, map<int, map<string,int> > &speGeneAdjNb, 
						map<int, map<string,pair<int,int> > > &speGeneExtremitiesAdjNb, int sens1 =0, int sens2 =0);
	void initMatrix(vector< pair <int,int> > &adjacencies, map<int,vector<float> > &speciesC0C1, 
					map<int, map<string,int> > &speGeneAdjNb, map<int, map<string,pair<int,int> > > &speGeneExtremitiesAdjNb,
					vector <double> &adjacencyScoreVec, double adjScoreLogBase , int sens1 =0, int sens2 =0);


	//void setC0(int id1, int id2, double value);
	//void setC1(int id1, int id2, double value);

	void resetCase(int id1, int id2);

///// Boltzmann specific function /////
	void setComputationToBoltzmann();

//// Backtrack Functions ////
	AdjSolution chooseRandomSolution(vector <AdjSolution> Vsolution);
	AdjSolution chooseBestSolution(vector <AdjSolution> Vsolution);
	AdjSolution chooseRandomSolutionWeighted(vector <AdjSolution> Vsolution);

	Node * backtrackAuxC1(int id1, int id2, vector< AdjTree *> * AdjacencyTrees, bool stochastic);
	void backtrackAuxC0(int id1, int id2, vector< AdjTree *> * AdjacencyTrees, bool stochastic);
	pair <int,int> getAdjEventAndSpecies(int NodeId1, int NodeId2);

///////////////////////////////////
/////// Cost functions //////////// in another file
///////////////////////////////////


/////////////////////////////////// OLD FUNCTIONS ////////////////////////////////////////////////////////

	double AggregateScore(vector <double> scores, int nbGain = 0, int nbBreak = 0);
/*
	pair <double, double> computeScore(int NodeId1, int NodeId2);

// simple DeCo cases
	double C1ExtantWithExtant(int NodeId1, int NodeId2);
	double C0ExtantWithExtant(int NodeId1, int NodeId2);

	double C1LossWithOther(int NodeId1, int NodeId2);//Do not presume wether NodeId1 or NodeId2 is the loss; Other may be anything but a loss
	double C0LossWithOther(int NodeId1, int NodeId2);//Do not presume wether NodeId1 or NodeId2 is the loss; Other may be anything but a loss

	double C1LossWithLoss(int NodeId1, int NodeId2); //add something later to account for possible co-events
	double C0LossWithLoss(int NodeId1, int NodeId2); //add something later to account for possible co-events


	double D1(int NodeIdDup, int NodeIdOther, ReconciledTree * RtreeDup, ReconciledTree * Rtreeother,bool invert ); //auxiliary of C1DupWithOther 
	double D0(int NodeIdDup, int NodeIdOther, ReconciledTree * RtreeDup, ReconciledTree * Rtreeother,bool invert ); //auxiliary of C0DupWithOther 
	double D12(int NodeId1, int NodeId2);

	double C1DupWithOther(int NodeId1, int NodeId2, bool firstDup );
	double C0DupWithOther(int NodeId1, int NodeId2, bool firstDup );

	double C1SpecWithSpec(int NodeId1, int NodeId2);
	double C0SpecWithSpec(int NodeId1, int NodeId2);

	double C1DupWithDup(int NodeId1, int NodeId2); //add something later to account for possible co-events
	double C0DupWithDup(int NodeId1, int NodeId2); //add something later to account for possible co-events


//DeCoLT specific cases

	double C1NullWithNull(int NodeId1, int NodeId2);
	double C0NullWithNull(int NodeId1, int NodeId2);

	pair <int,int> CostSoutWithExtantOrSpecOrNullAux(int NodeId1, int NodeId2, bool firstSout);

	double C1SoutWithExtantOrSpecOrNull(int NodeId1, int NodeId2, bool firstSout);
	double C0SoutWithExtantOrSpecOrNull(int NodeId1, int NodeId2, bool firstSout);

	double C1SoutWithSoutSynchronous(int NodeId1, int NodeId2);
	double C1SoutWithSoutaSynchronous(int NodeId1, int NodeId2);

	double C1SoutWithSout(int NodeId1, int NodeId2); //add something later to account for possible co-events
	double C0SoutWithSout(int NodeId1, int NodeId2); //add something later to account for possible co-events

	double C1RecWithRec(int NodeId1, int NodeId2); //add something later to account for possible co-events
	double C0RecWithRec(int NodeId1, int NodeId2); //add something later to account for possible co-events

	double B1(int NodeIdBout, int NodeIdOther, ReconciledTree * RtreeBout, ReconciledTree * Rtreeother, bool invert); // auxiliary of C1BoutWithBout and C1BoutWithOther
	double B0(int NodeIdBout, int NodeIdOther, ReconciledTree * RtreeBout, ReconciledTree * Rtreeother, bool invert); // auxiliary of C0BoutWithBout and C0BoutWithOther
	double B12(int NodeId1, int NodeId2);// auxiliary of C1BoutWithBout and C1BoutWithOther

	double C1BoutWithBout(int NodeId1, int NodeId2); //add something later to account for possible co-events
	double C0BoutWithBout(int NodeId1, int NodeId2); //add something later to account for possible co-events

	double C1BoutWithRec(int NodeId1, int NodeId2, bool firstBout);
	double C0BoutWithRec(int NodeId1, int NodeId2, bool firstBout);
/////////////////////////////////// END OLD FUNCTIONS ////////////////////////////////////////////////////////
*/

///score functions with AdjSolutions ///
	void computeSolution(int NodeId1, int NodeId2, vector <AdjSolution> &VsolutionC1, vector <AdjSolution> &VsolutionC0 );
	double compareScore( vector <AdjSolution> Vsolution);

	vector<AdjSolution> SolutionC1DefaultImpossibleCase();
	vector <AdjSolution> SolutionC1ExtantWithExtant(int NodeId1, int NodeId2);
	vector <AdjSolution> SolutionC0ExtantWithExtant(int NodeId1, int NodeId2);
	vector <AdjSolution> SolutionC1LossWithOther(int NodeId1, int NodeId2);
	vector <AdjSolution> SolutionC0LossWithOther(int NodeId1, int NodeId2);
	vector<AdjSolution> SolutionC1LossWithLoss(int NodeId1, int NodeId2);
	vector<AdjSolution> SolutionC0LossWithLoss(int NodeId1, int NodeId2);

	vector<AdjSolution> SolutionC1synchronousOneChild(int NodeId1, int NodeId2, bool inTheDead, bool iscoevent);
	vector<AdjSolution> SolutionC1synchronousTwoChildren(int NodeId1, int NodeId2, bool inTheDead, bool iscoevent, bool isSout);
	vector<AdjSolution> SolutionC1synchronousTwoAndOneChildren(int NodeId1, int NodeId2, bool inTheDead);
	vector<AdjSolution> SolutionC1asynchronousOneChild(int NodeId1, int NodeId2, bool NodeId2First, bool inTheDead, bool isRec);
	vector<AdjSolution> SolutionC1asynchronousTwoChildren(int NodeId1, int NodeId2, bool NodeId2First, bool inTheDead);

	vector<AdjSolution> SolutionC0DefaultImpossibleCase();
	vector<AdjSolution> SolutionC0DefaultImpossibleCaseNew(int NodeId1, int NodeId2);
	vector<AdjSolution> SolutionC0DefaultImpossibleCaseAux(int NodeId1, int NodeId2, bool invert);

	vector<AdjSolution> SolutionC0synchronousOneChild(int NodeId1, int NodeId2);
	vector<AdjSolution> SolutionC0asynchronousOneChild(int NodeId1, int NodeId2, bool NodeId2First);
	vector<AdjSolution> SolutionC0synchronousTwoAndOneChildren(int NodeId1, int NodeId2);
	vector<AdjSolution> SolutionC0asynchronousTwoChildren(int NodeId1, int NodeId2, bool NodeId2First);
	vector<AdjSolution> SolutionC0synchronousTwoChildren(int NodeId1, int NodeId2);


	//vector<AdjSolution> SolutionC1General(int NodeId1, int NodeId2);
	//vector<AdjSolution> SolutionC0General(int NodeId1, int NodeId2);

/*
	vector<AdjSolution> SolutionC1ExtantWithExtant(int NodeId1, int NodeId2);
	vector<AdjSolution> SolutionC0ExtantWithExtant(int NodeId1, int NodeId2);
	vector<AdjSolution> SolutionC1LossWithOther(int NodeId1, int NodeId2);//Do not presume wether NodeId1 or NodeId2 is the loss; Other may be anything but a loss
	vector<AdjSolution> SolutionC0LossWithOther(int NodeId1, int NodeId2);//Do not presume wether NodeId1 or NodeId2 is the loss; Other may be anything but a loss
	vector<AdjSolution> SolutionC1LossWithLoss(int NodeId1, int NodeId2); //add something later to account for possible co-events
	vector<AdjSolution> SolutionC0LossWithLoss(int NodeId1, int NodeId2); //add something later to account for possible co-events
	vector<AdjSolution> SolutionD1(int NodeIdDup, int NodeIdOther, ReconciledTree * RtreeDup, ReconciledTree * Rtreeother,bool invert );
	vector<AdjSolution> SolutionD0(int NodeIdDup, int NodeIdOther, ReconciledTree * RtreeDup, ReconciledTree * Rtreeother,bool invert );
	vector<AdjSolution> SolutionD12(int NodeId1, int NodeId2);
	vector<AdjSolution> SolutionC1DupWithOther(int NodeId1, int NodeId2, bool firstDup );
	vector<AdjSolution> SolutionC0DupWithOther(int NodeId1, int NodeId2, bool firstDup );
	vector<AdjSolution> SolutionC1SpecWithSpec(int NodeId1, int NodeId2);
	vector<AdjSolution> SolutionC0SpecWithSpec(int NodeId1, int NodeId2);
	vector<AdjSolution> SolutionC1DupWithDup(int NodeId1, int NodeId2);
	vector<AdjSolution> SolutionC0DupWithDup(int NodeId1, int NodeId2);
	/// decolt ///
	vector<AdjSolution> SolutionC1NullWithNull(int NodeId1, int NodeId2);
	vector<AdjSolution> SolutionC0NullWithNull(int NodeId1, int NodeId2);
	vector<AdjSolution> SolutionC1SoutWithExtantOrSpecOrNull(int NodeId1, int NodeId2, bool firstSout);
	vector<AdjSolution> SolutionC0SoutWithExtantOrSpecOrNull(int NodeId1, int NodeId2, bool firstSout);
	vector<AdjSolution> SolutionC1SoutWithSoutSynchronous(int NodeId1, int NodeId2);
	vector<AdjSolution> SolutionC1SoutWithSoutaSynchronous(int NodeId1, int NodeId2);
	vector<AdjSolution> SolutionC1SoutWithSout(int NodeId1, int NodeId2); //add something later to account for possible co-events
	vector<AdjSolution> SolutionC0SoutWithSout(int NodeId1, int NodeId2); //add something later to account for possible co-events
	vector<AdjSolution> SolutionC1RecWithRec(int NodeId1, int NodeId2); //add something later to account for possible co-events
	vector<AdjSolution> SolutionC0RecWithRec(int NodeId1, int NodeId2); //add something later to account for possible co-events
	vector<AdjSolution> SolutionC1RecWithOther(int NodeId1, int NodeId2, bool firstRec); 
	vector<AdjSolution> SolutionC0RecWithOther(int NodeId1, int NodeId2, bool firstRec); 
	vector<AdjSolution> SolutionB1(int NodeIdBout, int NodeIdOther, ReconciledTree * RtreeBout, ReconciledTree * Rtreeother, bool invert); // auxiliary of C1BoutWithBout and C1BoutWithOther
	vector<AdjSolution> SolutionB0(int NodeIdBout, int NodeIdOther, ReconciledTree * RtreeBout, ReconciledTree * Rtreeother, bool invert); // auxiliary of C0BoutWithBout and C0BoutWithOther
	vector<AdjSolution> SolutionB12(int NodeId1, int NodeId2);// auxiliary of C1BoutWithBout and C1BoutWithOther
	vector<AdjSolution> SolutionC1BoutWithBout(int NodeId1, int NodeId2); //add something later to account for possible co-events
	vector<AdjSolution> SolutionC0BoutWithBout(int NodeId1, int NodeId2); //add something later to account for possible co-events
	vector<AdjSolution> SolutionC1BoutWithRec(int NodeId1, int NodeId2, bool firstBout);
	vector<AdjSolution> SolutionC0BoutWithRec(int NodeId1, int NodeId2, bool firstBout);
	vector<AdjSolution> SolutionC1DefaultImpossibleCase();
	vector<AdjSolution> SolutionC0DefaultImpossibleCase();


	vector<AdjSolution> SolutionC1RecWithSout(int NodeId1, int NodeId2, bool firstSout);
	vector<AdjSolution> SolutionC0RecWithSout(int NodeId1, int NodeId2, bool firstSout);
	vector<AdjSolution> SolutionC1DupWithSout(int NodeId1, int NodeId2, bool firstDup );
	vector<AdjSolution> SolutionC0DupWithSout(int NodeId1, int NodeId2, bool firstDup );
	vector<AdjSolution> SolutionC1DupWithRec(int NodeId1, int NodeId2, bool firstDup );
	vector<AdjSolution> SolutionC0DupWithRec(int NodeId1, int NodeId2, bool firstDup );


	vector<AdjSolution> SolutionC0DefaultImpossibleCaseNew(int NodeId1, int NodeId2);
	vector<AdjSolution> SolutionC0DefaultImpossibleCaseAux(int NodeId1, int NodeId2, bool invert);
*/

public:

//	AdjMatrix(double Gcost, double Bcost, bool VERBOSE);
	AdjMatrix(map<int,vector<float> > &speciesC0C1, map<int, map<string,int> > &speGeneAdjNb, 
				map<int, map<string,pair<int,int> > > &speGeneExtremitiesAdjNb,
				vector <double> &adjacencyScoreVec,
				double Gcost, double Bcost,
				ReconciledTree * rtree1, ReconciledTree * rtree2, 
				vector< pair <string,string> > &adjacencies, 
				int Gfamily1, int Gfamily2,																	//LA modif
				bool lossaware, pair < vector < pair <string, string> >, bool > FamiliesFreeAdjacencies,	//LA modif
				bool VERBOSE,
				bool boltzmann = false,	double temp = 1 , double absencePenalty = -1, double adjScoreLogBase = 10000, int sens1 = 0, int sens2 = 0);
				
	AdjMatrix(map<int,vector<float> > &speciesC0C1, map<int, map<string,int> > &speGeneAdjNb, 
				map<int, map<string,pair<int,int> > > &speGeneExtremitiesAdjNb,
				vector <double> &adjacencyScoreVec,
				double Gcost, double Bcost, 
				ReconciledTree * rtree1, ReconciledTree * rtree2, 
				vector< pair <int,int> > &adjacencies, 
				int Gfamily1, int Gfamily2,																	// LA modif
				bool lossaware, pair < vector < pair <string, string> >, bool > FamiliesFreeAdjacencies,	// LA modif
				bool VERBOSE,
				bool boltzmann = false,double temp = 1  , double absencePenalty = -1, double adjScoreLogBase = 1000, int sens1 = 0, int sens2 = 0);




	AdjMatrix(){}

	~AdjMatrix()
	{
		//cout << "ploup" << endl;
		/*
		if(TreeToMatrixId1.size() >0)
			TreeToMatrixId1.clear();
		if(TreeToMatrixId2.size() >0)
			TreeToMatrixId2.clear();

		if(MatrixToTreeId1.size() > 0)
			MatrixToTreeId1.clear();
		if(MatrixToTreeId2.size() > 0)
			MatrixToTreeId2.clear();
		*/
		/*
		if(Rtree1 !=NULL)
			delete Rtree1;

		if(Rtree2 != NULL)
			delete Rtree2;
		*/

	}

	void resetMatrix();
	void partiallyResetMatrix(int id1, int id2);

	double getC1(int id1, int id2, bool invert = false);
	double getC0(int id1, int id2, bool invert = false);

	vector <AdjSolution> getSolutionC1(int id1, int id2);
	vector <AdjSolution> getSolutionC0(int id1, int id2);
	void setSolutionC1(int id1, int id2, vector <AdjSolution> &Vsolution);
	void setSolutionC0(int id1, int id2, vector <AdjSolution> &Vsolution);


	bool issetC1(int id1, int id2);
	bool issetC0(int id1, int id2);

	void setdecoLTalgo(bool decolt);

	bool isComputed()
	{return matrixComputed;}

	void setComputed()
	{matrixComputed = true;}

	double getGainCost();
	double getBreakCost();
	double getBoltzmannGainBreakCost(int nbGain, int nbBreak);


	void setRtree1(ReconciledTree * rtree);
	void setRtree2(ReconciledTree * rtree);

	void addAdjacencies(vector < pair <int,int> > adjacencies, vector <double> adjacencyScoreVec, double adjScoreLogBase);

	void computeMatrix();
	void computeMatrix(double WDupCost, double WLossCost, double WHgtCost);


	void printC0();
	void printC1();
	void printMe();

	void backtrack( vector< AdjTree *> * AdjacencyTrees, bool stochastic, bool alwaysGainAtTop = true, double c1proba = 0.5 );

	///////////////WMODIF
	bool getRootMatrix(int id1, int id2);
	void setRootMatrixPossible(int id1, int id2);
	void setRootMatrixNotPossible(int id1, int id2);

	AdjMatrix* getClone();

	void setC0(int id1, int id2, double value);
	void setC1(int id1, int id2, double value);


	int getNumberScoreWithAbsLog10Above(double threshold = 200); 

	
	void clear();							//LA modif
	bool hasFreeAdj(int node1, int node2);	//LA modif

};


#endif