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

package idsforwsn.nic;

//
// Basic wireless nic definition
//
module WirelessNicIDSBattery extends WirelessNicIDS
{
    parameters:
        phyType = default("org.mixim.modules.phy.PhyLayerBattery");

//        double sleepCurrent @unit(mA);
//        double rxCurrent @unit(mA);
//        double decodingCurrentDelta @unit(mA);
//        double txCurrent @unit(mA);
//        double setupRxCurrent @unit(mA);
//        double setupTxCurrent @unit(mA);
//        double rxTxCurrent @unit(mA);
//        double txRxCurrent @unit(mA);
        double sleepCurrent @unit(mA)         = 0.000021mA; // 0.021 µA
        double rxCurrent @unit(mA)            = 18.8 mA;
        double decodingCurrentDelta @unit(mA) = 0 mA;
        double txCurrent @unit(mA)            = 17.4 mA; // 8.5 for lowest transmission power
        double setupRxCurrent @unit(mA)       = 0.6391mA; // 0.002109 W = (ESetupPower+ESetupXtal+ETxCalibrate)/TSetupTx
        double setupTxCurrent @unit(mA)       = 0.6845mA; // 0.002259 W = (ESetupPower+ESetupXtal+ERxCalibrate)/TSetupRx
        double rxTxCurrent @unit(mA)          = 18.8 mA; // Upper bound
        double txRxCurrent @unit(mA)          = 18.8 mA; // idem
}
