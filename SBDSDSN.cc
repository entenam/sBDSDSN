/*******************************************************************************
 *  Copyright: RMIT University,  2012 - 2013		                       *
 *  Author(s): Khandakar Entenam Unayes Ahmed		                       *
 *  This file is distributed under the terms in the attached LICENSE file.     *
 *  If you do not find this file, copies can be found by writing to:           *
 *                                                                             *
 *      Khandakar Entenam Unayes Ahmed, SECE, RMIT University                  *
 *      e-mail: khandakar.ahmed@rmit.edu.au			               *
 *      Attention:  License Inquiry.                                           *
 *                                                                             *  
 *******************************************************************************/

#include "SBDSDSN.h"
#include "algorithm"

#define NEXT_HOP_ADDRESS nextHopAddress.c_str()
#define SECTOR_HEAD_ADDRESS sectorHeadAddress.c_str()
#define MEMBER_NODE_ADDRESS memberNodeAddress.c_str()
#define SEGMENT_ADDRESS segmentAddress.c_str()
#define SELF_SEGMENT_ADDRESS selfSegmentID.c_str();

Define_Module(SBDSDSN);

/* Application layer sends a packet together with a dest network layer address.
 * Network layer is responsible to route that packet by selecting an appropriate
 * MAC address.
 */
void SBDSDSN::startup()
{
	parseStringParams();	//read the string params from omnet.ini and initialize the variables
	transmissionLength = 0;
	numberOfTracks = 4;
	numberOfSectors = 4;
	roundCounter = 0;
	SHCandidates.clear();	
	storageInMemberNodeTable.clear();
	initialisationLength = TDMASchedulingTime + 2.0;
	for(int i=0;i<numberOfTracks;i++){
		for(int j=0;j<numberOfSectors;j++){
			sectorSlotArray.push_back(-1);
		}
	}
	roundLength = 4 * sectorSlotLength + initialisationLength + TDMASchedulingTime + queryTimeLength + 2.0; //2.0 = roundSpacing
	txInterval_total = (roundLength * totalRound);
	trace()<< "Simulation Time: " << strtod(ev.getConfig()->getConfigValue("sim-time-limit"),NULL) << "TX Interval Time: " << txInterval_total << "Round Length: " << roundLength << "Total Round: " << totalRound << "SectorSlotLength: " << sectorSlotLength;
	if (strtod(ev.getConfig()->getConfigValue("sim-time-limit"), NULL) < txInterval_total) {
		trace() << "ERROR: Total sim time should be at least = " << txInterval_total;
		opp_error("\nError: simulation time not large enough for the Sector Based Distance Routing\n");
	}

	addPadRemUp2Segment = 100;
	addPadRangeQuery = 200;
	addPadKNNQuery = 300;
	addPadSkyLineQuery = 400;
	addPadController = 500;
	addPadDecider = 100;

	setTimer(START_ROUND, 0.0);
	if(isController){
		trace()<< "Is a Controller";
	}
}

void SBDSDSN::timerFiredCallback(int index){
	double timer = uniform(0,1);
	switch(index){
		case START_ROUND:{
			if(roundCounter < totalRound){
				SHCandidates.clear();
				sectorMembers.clear();
				sectorMembersList.clear();
				roundCounter++;
				currentRoundStartTime = (roundCounter-1)*roundLength;
				if(isSectorHead)
					setTimer(SECTOR_SLOT_SELECTION_ROUND, 0.0);
					trace() << "From START_ROUND: " << roundCounter << " Total Round: " << totalRound << " Round Length: " << roundLength << "currentStartTime " << currentRoundStartTime << "Child Selection Round " << currentRoundStartTime+childSelectionRoundTime << "Segmentation Time" << segmentationTime << "Round Start Time " << roundCounter*roundLength;
				setTimer(CHILD_SELECTION_ROUND,childSelectionRoundTime);
				setTimer(START_ROUND, roundLength);
			}
		}
		case SECTOR_SLOT_SELECTION_ROUND:{
			int m = numberOfTracks;
			int n = numberOfSectors;
			for(int j=1;j<=m;j+=2){
				for(int i=(j-1)*n;i<j*n-1;i+=2){
					if(i<n*j){
						sectorSlotArray[i]=1;
					}
					if(i+1<n*j){
						sectorSlotArray[i+1]=2;
					}
					if(i+n<m*n){
						sectorSlotArray[i+n]=3;
					}	
					if(i+n+1<m*n){
						sectorSlotArray[i+n+1]=4;
					}	
				}
			}
			break;
		}		
		case CHILD_SELECTION_ROUND:{
			if(isSectorHead){
				setTimer(SEND_BEACON,sendBeaconTime);
				trace() << "Child Selection Round: " << currentRoundStartTime+sendBeaconTime;
				setTimer(SEGMENTATION_ROUND, segmentationTime);
				trace() << "Segmentation Round: " << currentRoundStartTime+segmentationTime;				
			}
			else{
				setTimer(JOIN_SECTOR,joinSectorTime);
			}	
			break;
		}	
		case SEND_BEACON:{
			SBDSDSNPacket *ctrlPkt = new SBDSDSNPacket("Sector Head Announcement Packet", NETWORK_LAYER_PACKET);
			ctrlPkt->setSBDSDSNPacketKind(SBD_BEACON_PACKET);
			ctrlPkt->setSource(SELF_NETWORK_ADDRESS);
			toMacLayer(ctrlPkt, BROADCAST_MAC_ADDRESS);
			break;
		}
		case JOIN_SECTOR:{
			if(SHCandidates.size()!=0){
				sort(SHCandidates.begin(),SHCandidates.end(),cmpRSSI_SBDSDSN);		
				SBDSDSNPacket *ctrlPkt = new SBDSDSNPacket("Sector Member Joining Packet", NETWORK_LAYER_PACKET);		
				ctrlPkt->setSBDSDSNPacketKind(SBD_JOIN_PACKET);
				ctrlPkt->setSource(SELF_NETWORK_ADDRESS);
				nodeInfo info = (nodeInfo)SHCandidates.back();
				stringstream convert;
				convert << info.src;
				sectorHeadAddress = convert.str();
				ctrlPkt->setDestination(SECTOR_HEAD_ADDRESS);
				toMacLayer(ctrlPkt, resolveNetworkAddress(SECTOR_HEAD_ADDRESS));
			}
			break;
		}
		case SEGMENTATION_ROUND:{
			sort(sectorMembersList.begin(),sectorMembersList.end(),cmpRSSI_SBDSDSN);
			int index=0;
			trace()<< "SegmentMemberListSize: " << sectorMembersList.size() << "SegmentsPerSector: " << segmentsPerSector;
			nodesPerSegment = ceil((double)sectorMembersList.size()/(double)segmentsPerSector);
			int segmentCounter = 0;
			while(index<sectorMembersList.size()&&segmentCounter<segmentsPerSector){
				//trace() << "Segmentation Round ......................" << index << "Node Per Segment: ..." << nodesPerSegment;			
				segmentNode rec;
				for(int i=index;i<nodesPerSegment*(segmentCounter+1) && i<sectorMembersList.size();i++){
					(rec.ID).push_back(sectorMembersList[i].src);
					(rec.rssi).push_back(sectorMembersList[i].rssi);
					(rec.tally).push_back(0);
				}
				segmentList.push_back(rec);
				index = (++segmentCounter)*nodesPerSegment;
			}
			segmentsPerSector = segmentList.size();
			trace()<< "Number of Segments: " << segmentsPerSector;
			setTimer(TDMA_SCHEDULING,TDMASchedulingTime);
			trace() << "TDMA Scheduling Round: " << currentRoundStartTime+TDMASchedulingTime;
			break;
		}
		case TDMA_SCHEDULING:{
			transmissionLength = sectorSlotLength/sectorMembers.size();
			trace() << "Sector Member Size: " << sectorMembers.size();
			if(sectorMembers.size()!=0){
				SBDSDSNPacket *ctrlPkt = new SBDSDSNPacket("TDMA Scheduling Packet", NETWORK_LAYER_PACKET);
				ctrlPkt->setSBDSDSNPacketKind(SBD_TDMA_SCHEDULE_PACKET);
				ctrlPkt->setSource(SELF_NETWORK_ADDRESS);
				ctrlPkt->setDestination(BROADCAST_NETWORK_ADDRESS);
				sectorSize = sectorMembers.size();
				ctrlPkt->setSlotArraySize(sectorSize);
				ctrlPkt->setSegmentAddressArraySize(sectorSize);
				ctrlPkt->setSectorSlotNumber(sectorSlotArray[atoi(SELF_NETWORK_ADDRESS)]);
				for(int i=0;i<sectorSize;i++){
					ctrlPkt->setSlot(i,sectorMembers[i]);
					for(int k=0;k<(int)segmentList.size();k++){
						for(int j=0;j<(segmentList[k].ID).size();j++){
							if(segmentList[k].ID[j]==sectorMembers[i]){
								ctrlPkt->setSegmentAddress(i,k);
							}		
						}
					}
				}
				toMacLayer(ctrlPkt, BROADCAST_MAC_ADDRESS);
				double slot = (sectorSlotArray[atoi(SELF_NETWORK_ADDRESS)]) * sectorSlotLength + initialisationLength +timer;
				trace() << "transmission Length: " << transmissionLength << " Slot:" << slot << " sectorSlotarray " << sectorSlotArray[atoi(SELF_NETWORK_ADDRESS)] ;
				setTimer(TRANSMISSION_SLOT,slot);
			}
			else{
				double slot = (sectorSlotArray[atoi(SELF_NETWORK_ADDRESS)]) * sectorSlotLength + transmissionLength + initialisationLength + timer;
				setTimer(TRANSMISSION_SLOT,slot);
			}
			break;
		}
		case TRANSMISSION_SLOT:{
			if(!isSectorHead){
				trace() << "Remote Packet:   ...........";
				transmitBufferedPacket();	
			}
			else{
				transmitBufferedPacket();
			}
			break;
		}
	}
}

void SBDSDSN::fromApplicationLayer(cPacket * pkt, const char *destination)
{
	string dst(destination);
	if(!isSectorHead){
		//trace() << "#######################################################################################################" <<pkt->getAppName();
		SBDSDSNPacket *netPacket = new SBDSDSNPacket("SDSN Traffic FROM MN TO SH (Network Layer)", NETWORK_LAYER_PACKET);
		netPacket->setSource(SELF_NETWORK_ADDRESS);
		netPacket->setSBDSDSNPacketKind(SBD_SDSN_TRAFFIC_GENERAL);
		netPacket->setDestination(SECTOR_HEAD_ADDRESS);
		encapsulatePacket(netPacket,pkt);
		if((int)TXBuffer.size() >= netBufferSize)
			TXBuffer.pop();
		else
			bufferPacket(netPacket);
	}
	else{	
		if(atoi(destination) == atoi(SELF_NETWORK_ADDRESS)){
			toApplicationLayer(pkt);
		}
		else if(atoi(destination)<0){	
			SBDSDSNPacket *netPacket = new SBDSDSNPacket("Remote Data Update Packet", NETWORK_LAYER_PACKET);
			stringstream convert;
			switch(-atoi(destination)/addPadDecider){
				case 1:{
					netPacket->setSBDSDSNPacketKind(SBD_DATA_PACKET_REMOTE_UPDATE);
					convert << destin4MemberNode(-atoi(destination)%addPadRemUp2Segment);
					memberNodeAddress = convert.str();
					netPacket->setDestination(MEMBER_NODE_ADDRESS);
					encapsulatePacket(netPacket, pkt);
					toMacLayer(netPacket, resolveNetworkAddress(MEMBER_NODE_ADDRESS));	
					break;
				}
				case 2:{
					netPacket->setSBDSDSNPacketKind(SBD_RANGE_QUERY_PACKET);
					convert << (-atoi(destination)%addPadRemUp2Segment)-1;
					segmentAddress = convert.str();
					netPacket->setDestination(SEGMENT_ADDRESS);
					encapsulatePacket(netPacket, pkt);
					toMacLayer(netPacket, BROADCAST_MAC_ADDRESS);
					break;
				}
				case 5:{
					netPacket->setSBDSDSNPacketKind(SDSN_FLOW_REQ_CONTROLLER);
					convert << (-atoi(destination)%addPadController)-1;
					segmentAddress = convert.str();
					netPacket->setDestination(SEGMENT_ADDRESS);
					trace() << "Received in Network Layer for Packet to Controller" << SEGMENT_ADDRESS;
					encapsulatePacket(netPacket, pkt);
					toMacLayer(netPacket, BROADCAST_MAC_ADDRESS);
					break;
				}
			}
		}
		else{	
			int nextHop = searchNextHop(atoi(destination));
			trace() << "------------------------------------------------------------------------" << nextHop;
			stringstream out;
	 		out << nextHop;
			nextHopAddress = out.str();
			SBDSDSNPacket *netPacket = new SBDSDSNPacket("Remote Data Update Packet", NETWORK_LAYER_PACKET);
			netPacket->setSource(SELF_NETWORK_ADDRESS);
			netPacket->setSBDSDSNPacketKind(SBD_DATA_PACKET_REMOTE_UPDATE);
			netPacket->setDestination(destination);
			netPacket->setPrevHop(SELF_NETWORK_ADDRESS);
			netPacket->setNextHop(NEXT_HOP_ADDRESS);
			encapsulatePacket(netPacket, pkt);
			bufferPacket(netPacket);
		}
	}
}

void SBDSDSN::parseStringParams(void){
	nodeStartTimeout = (double)par("nodeStartTimeout") / 1000;
	topologyPacketTimeout = (double)par("topologyPacketTimeout") / 1000;
	cModule *appModule = getParentModule()->getParentModule()->getSubmodule("Application");
	parentSectorID = appModule->par("parentSectorID");
	packetsPerChildNode = appModule->par("packetsPerChildNode");
	queryTimeLength = appModule->par("queryTimeLength");
	totalRound = (int)par("totalRound");
	sendBeaconTime = (double)par("sendBeaconTime") / 1000;
	TDMASchedulingTime = (double)par("TDMASchedulingTime") / 1000;
	joinSectorTime = (double)par("joinSectorTime") / 1000;
	childSelectionRoundTime = (double)par("childSelectionRoundTime") / 1000;
	segmentationTime = (double)par("segmentationTime") / 1000;
	sectorSlotLength = (double)par("sectorSlotLength") / 1000;
	segmentsPerSector = (int)par("segmentsPerSector");
	nodesPerSegment = (int)par("nodesPerSegment");
	if (appModule->hasPar("isSectorHead"))
		isSectorHead = appModule->par("isSectorHead");
	else
		opp_error("\nSector Based Distance Routing Must Have Sector Head");
	if (appModule->hasPar("isController")){
		isController = appModule->par("isController");
	}
}

int SBDSDSN::getSegmentsPerSector(void){
	return segmentsPerSector;
}

void SBDSDSN::fromMacLayer(cPacket * pkt, int srcMacAddress, double rssi, double lqi)
{
	SBDSDSNPacket *netPacket = dynamic_cast <SBDSDSNPacket*>(pkt);
	string destination(netPacket->getDestination());
	if (!netPacket) {
		return;
	}
	switch(netPacket->getSBDSDSNPacketKind()){
		case SBD_SDSN_TRAFFIC_GENERAL:{
			if(isSectorHead){
				trace()<< "From: " << srcMacAddress << "Destination" << destination;
				if(destination.compare(SELF_NETWORK_ADDRESS) == 0){
					toApplicationLayer(decapsulatePacket(pkt));
				}
			}
			break;
		}
		case SBD_DATA_PACKET_REMOTE_UPDATE:{
			string thisHop(netPacket->getNextHop());
			if(isSectorHead){				
				if(destination.compare(SELF_NETWORK_ADDRESS)==0 && thisHop.compare(SELF_NETWORK_ADDRESS)==0){
					trace()<< "Previous Hop:" << netPacket->getPrevHop() <<" Source:" << netPacket->getSource() << " Next Hop:" << thisHop << " Destination:" <<netPacket->getDestination()<<" Sequence: " <<netPacket->getSequenceNumber();
					toApplicationLayer(decapsulatePacket(pkt));
				}
				else if(thisHop.compare(SELF_NETWORK_ADDRESS)==0){
					int nextHop = searchNextHop(atoi(netPacket->getDestination()));
					stringstream out;
					out << nextHop;
					string nextHopAddress = out.str();
					SBDSDSNPacket *dataPkt = netPacket->dup();
					dataPkt->setSBDSDSNPacketKind(SBD_DATA_PACKET_REMOTE_UPDATE);
					dataPkt->setSource(netPacket->getSource());
					dataPkt->setDestination(netPacket->getDestination());
					dataPkt->setPrevHop(SELF_NETWORK_ADDRESS);
					dataPkt->setNextHop(NEXT_HOP_ADDRESS);
					toMacLayer(dataPkt, resolveNetworkAddress(NEXT_HOP_ADDRESS));
				}
			}
			else if(destination.compare(SELF_NETWORK_ADDRESS)==0){ 
					toApplicationLayer(decapsulatePacket(pkt));
			}
			break;
		}

		case SBD_RANGE_QUERY_PACKET:{
			/*if(destination.compare(SELF_NETWORK_ADDRESS)==0){ 
					toApplicationLayer(decapsulatePacket(pkt));
			}*/
			if(!isSectorHead){
				//if(destination.compare(NEXT_HOP_ADDRESS)==0){
					trace() << "Segment Address: !!!!!!!!!!!!!!!!!!!!!!" << SELF_SEGMENT_ADDRESS;
					trace() << "Range Query Packet Destination Segment................: " << netPacket->getDestination();
				//}
			}
			break;
		}

		case SDSN_FLOW_REQ_CONTROLLER:{
			string thisHop(netPacket->getNextHop());
			if(isController){
  			   trace()<< "Previous Hop:" << netPacket->getPrevHop() <<" Source:" << netPacket->getSource() << " Next Hop:" << thisHop << " Destination:" <<netPacket->getDestination()<<" Sequence: " <<netPacket->getSequenceNumber();
			   toApplicationLayer(decapsulatePacket(pkt));
			}
			else if(isSectorHead){
			   trace() << "Needs to implement an algorithm to forward toward controller";	
			}
			break;
		}

		case SBD_BEACON_PACKET:{
			if(!isSectorHead){
				nodeInfo rec;
				rec.src = atoi(netPacket->getSource());
				rec.rssi = rssi;
				SHCandidates.push_back(rec);
			}				
			break;
		}
		case SBD_JOIN_PACKET:{
			string dst(netPacket->getDestination());
			if(isSectorHead && dst.compare(SELF_NETWORK_ADDRESS)==0){
				sectorMembers.push_back(atoi(netPacket->getSource()));
				nodeInfo rec;
				storageInMemberNode tempRec;
				rec.src=tempRec.memberNodeID = atoi(netPacket->getSource());
				rec.rssi = rssi;
				tempRec.tally = 0;
				storageInMemberNodeTable.push_back(tempRec);
				sectorMembersList.push_back(rec);
			}
			break;
		}
		case SBD_TDMA_SCHEDULE_PACKET:{
			double timer = uniform(0,1);
			if(!isSectorHead){
				if(atoi(netPacket->getSource()) == atoi(SECTOR_HEAD_ADDRESS)){
					sectorSize = netPacket->getSlotArraySize();
					transmissionLength = sectorSlotLength/sectorSize;
					for(int i=0;i<sectorSize;i++)
					{
						if(netPacket->getSlot(i) == atoi(SELF_NETWORK_ADDRESS)){
							send(createRadioCommand(SET_STATE,SLEEP),"toMacModule");
							double slot = ((netPacket->getSectorSlotNumber()-1) * sectorSlotLength) + (i * transmissionLength) + initialisationLength +timer;
							trace() << "transmission Length: " << transmissionLength << "Slot:" << slot;
							stringstream convert;
							convert << netPacket->getSegmentAddress(i);
							selfSegmentID = convert.str();
							trace() << "Segment Address: " << SELF_SEGMENT_ADDRESS;
							setTimer(TRANSMISSION_SLOT, slot);
							break;
						}
					}
				}
			}
			break;
		}
	}
}

void SBDSDSN::finishSpecific()
{
	for(int i=0;i< (int)sectorMembers.size();i++){
		trace() << "Sector: " << SELF_NETWORK_ADDRESS << "Members: " << sectorMembers[i];
		trace() << "Sector: " << SELF_NETWORK_ADDRESS << "Members: " << sectorMembersList[i].src << "RSSI" << sectorMembersList[i].rssi;		
	}
	for(int i=0;i< (int)sectorSlotArray.size();i++){
		trace() << "Sector Number: " << i << "Slot Number: " << sectorSlotArray[i];
	}
	for(int i=0;i<(int)segmentList.size();i++){
		trace()<< "Segment: " << i;
		for(int j=0;j<(segmentList[i].ID).size();j++)
			trace()<< "Segment Members: " << segmentList[i].ID[j];
	}
}


int SBDSDSN::searchNextHop(int destination){
	int destCol, destRow, currCol, currRow, nextHopCol, nextHopRow;	
	destCol = destination%4;
	destRow = destination/4;
	currCol = nextHopCol = atoi(SELF_NETWORK_ADDRESS)%4;
	currRow = nextHopRow = atoi(SELF_NETWORK_ADDRESS)/4;
	if(currCol<destCol){
		nextHopCol = nextHopCol + 1;
		return nextHopRow * 4 + nextHopCol;
	}
	else if(currCol > destCol){
		nextHopCol = nextHopCol - 1;
		return nextHopRow * 4 + nextHopCol;
	}
	else if(currCol == destCol){
		if(currRow < destRow){
			nextHopRow = nextHopRow  + 1;
			return nextHopRow * 4 + nextHopCol;
		}
		else if(currRow > destRow){
			nextHopRow = nextHopRow  - 1;
			return nextHopRow * 4 + nextHopCol;
		}
	}
	return -1;
}

void SBDSDSN::bufferNaivePacket(cPacket *rcvFrame){
}

void SBDSDSN::transmitBufferedPacket(){
	int numberOfPacketsSent = 0;
	while(!TXBuffer.empty()){
		SBDSDSNPacket *netPacket = check_and_cast < SBDSDSNPacket * >(TXBuffer.front());
		if(!isSectorHead){
			if(numberOfPacketsSent < packetsPerChildNode){
				trace() << "Number of Packets: " << packetsPerChildNode;
				netPacket->setDestination(SECTOR_HEAD_ADDRESS);
				toMacLayer(netPacket,resolveNetworkAddress(SECTOR_HEAD_ADDRESS));
			}
		}
		else{
			int nextHop = atoi(netPacket->getNextHop());		
			stringstream out;
			out << nextHop;
			string nextHopAddress = out.str();
			trace() << "Transmitting Packet: " << "Destination: " << netPacket->getDestination()<< " Next Hop: " << netPacket->getNextHop() << " Next Hop: " << NEXT_HOP_ADDRESS;
			toMacLayer(netPacket,resolveNetworkAddress(NEXT_HOP_ADDRESS));
		}
		TXBuffer.pop();
		numberOfPacketsSent++;
	}	
}

void SBDSDSN::transmitAggregatedBufferedPacket(){
	// Not Used Yet
}

int SBDSDSN::destin4MemberNode(int destinationSegmentPivotIndex){
	int index=0;
	//trace() << "Segment Index Pivot Value:.........................................." << destinationSegmentPivotIndex;
	if(destinationSegmentPivotIndex <= (int)segmentList.size())
		index = destinationSegmentPivotIndex-1;
	else 
		index = 0;
	sortSegmentMemberNode(index);
	int memberNodeID = segmentList[index].ID[0];
	trace() << "Segment: " << index << "Member ID: " << memberNodeID << "Tally: " << segmentList[index].tally[0];
	segmentList[index].tally[0]++;
	for(int i=0;i<(int)segmentList.size();i++){
		trace()<< "Segment: " << i;
		for(int j=0;j<(segmentList[i].ID).size();j++)
			trace()<< "Member ID: " << segmentList[i].ID[j] << "Tally: " << segmentList[i].tally[j];
	}
	return memberNodeID;
}

int SBDSDSN::sortSegmentMemberNode(int index){
	int k,j,temp_tally, temp_ID;
	double temp_rssi;
	for (int i = 1; i < (segmentList[index].ID).size(); i++) {
		temp_tally = segmentList[index].tally[i];
		temp_ID = segmentList[index].ID[i];
		temp_rssi = segmentList[index].rssi[i]; 
		j = i;
 		while (j>0 && segmentList[index].tally[j-1] > temp_tally){
			segmentList[index].tally[j] = segmentList[index].tally[j-1];
			segmentList[index].ID[j] = segmentList[index].ID[j-1];	
			segmentList[index].rssi[j] = segmentList[index].rssi[j-1];			
			j--;
		}
		segmentList[index].tally[j] = temp_tally;
		segmentList[index].ID[j] = temp_ID;	
		segmentList[index].rssi[j] = temp_rssi;
	}
}

bool cmpRSSI_SBDSDSN(nodeInfo a, nodeInfo b){
	return (a.rssi < b.rssi);
}

bool cmpTALLY_SBDSDSN(storageInMemberNode a, storageInMemberNode b){
	return (a.tally < b.tally);
}
