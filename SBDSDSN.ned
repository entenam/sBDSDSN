//*******************************************************************************
//*  Copyright: RMIT University,  2012 - 2013		                        *
//*  Author(s): Khandakar Entenam Unayes Ahmed	and Nazmus S. Nafi              *
//*  This file is distributed under the terms in the attached LICENSE file.     *
//*  If you do not find this file, copies can be found by writing to:           *
//*                                                                             *
//*      Khandakar Entenam Unayes Ahmed, SECE, RMIT University                  *
//*      e-mail: khandakar.ahmed@rmit.edu.au			                *
//*      Attention:  License Inquiry.                                           *
//*                                                                             *  
//*******************************************************************************/

package node.communication.routing.sBDSDSN;

simple SBDSDSN like node.communication.routing.iRouting {
 parameters:
	bool collectTraceInfo = default (false);
	int sectorHeadID = default(0);
	int parentSectorID = default(0);
	int maxNetFrameSize = default (0);				// bytes, 0 means no limit
	int netBufferSize = default (32);				// number of messages
	int netDataFrameOverhead = default (14);			// bytes
	int nodeStartTimeout = default(30);
	int topologyPacketTimeout = default(50);
	int segmentsPerSector = default (3);
	int nodesPerSegment = default(3);

	double sectorSlotLength = default(4000);
	int totalRound = default(1);
	double childSelectionRoundTime = default(20);
	double segmentationTime = default(5000);
	double sendBeaconTime = default(2000); 
	double TDMASchedulingTime = default(6000);
	double joinSectorTime = default(4000);
 gates:
	output toCommunicationModule;
	output toMacModule;
	input fromCommunicationModule;
	input fromMacModule;
	input fromCommModuleResourceMgr;
}

