//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#include "StaticMobility.h"

#include <FWMath.h>
#include <FindModule.h>

#include <iostream>
#include <fstream>
using namespace std;

Define_Module(StaticMobility);

void StaticMobility::initialize(int stage){
	StationaryMobility::initialize(stage);
	if (stage==0){
//		helper = FindModule<SimHelper*>::findGlobalModule();

/*
 	 	ALTERNATIVE
		if (helper->hasNodePos(findHost()->getIndex())){
			Coord pos = helper->getNodePos(findHost()->getIndex());
			Coord borderStep;
			if (checkIfOutside(pos,borderStep)){
				opp_warning("StaticMobility: position outside playground");
			} else {
				move.setStart(pos);
		        //move.setSpeed(0);
		        //move.setDirectionByVector(Coord::ZERO);
				//updatePosition();
			}
		}
*/
	} else if (stage==1){

	}
}

/**
 * Sets current node position
 */
void StaticMobility::setPosition(Coord pos){
//	Coord borderStep;
//	if (checkIfOutside(pos,borderStep)){
//		opp_warning("BasicMobility: position outside playground");
//	} else {
//		move.setStart(pos);
//		updatePosition();
//	}
    lastPosition = pos;
    emitMobilityStateChangedSignal();
    updateVisualRepresentation();

}

/**
 * Finalize module
 */
void StaticMobility::finish(){
    StationaryMobility::finish();
    recordScalar("x", getCurrentPosition().x);
    recordScalar("y", getCurrentPosition().y);
}
