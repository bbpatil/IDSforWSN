package idsforwsn.nic;

import idsforwsn.ids.IWirelessIDS;
import idsforwsn.ids.IWirelessIDSSender;
import org.mixim.base.modules.IWirelessMac;
import org.mixim.base.phyLayer.IWirelessPhy;
import org.mixim.modules.nic.IWirelessNicUpperCtrl;


//
// Basic wireless nic definition
//
module WirelessNicIDS like IWirelessNicUpperCtrl
{
    parameters:
        string connectionManagerName = default("");   //name of the ConnectionManager module
        string phyType = default("org.mixim.modules.phy.PhyLayer");
        string macType = default("org.mixim.modules.mac.CSMAMacLayer");
        string idsType = default("idsforwsn.ids.IDSSimpleLayer");
        string idsSenderType = default("idsforwsn.ids.IDSSender");

        //publishRSSIAlways = false;
        phy.headerLength = 48 bit; // ieee 802.15.4                
        phy.thermalNoise = default(-110 dBm);
        // From TI CC1100 datasheet rev. C
        phy.timeSleepToRX = 0.001792 s;
        phy.timeSleepToTX = 0.001792 s;
        phy.timeRXToTX = 0.000192 s;
        phy.timeTXToRX = 0.000192 s;
        phy.timeRXToSleep = 0 s;
        phy.timeTXToSleep = 0 s;

    gates:
        input upperLayerIn; // to upper layers
        output upperLayerOut; // from upper layers
        output upperControlOut; // control information 
        input upperControlIn; // control information 
        input radioIn; // radioIn gate for sendDirect

    submodules:
        idsSender: <idsSenderType> like IWirelessIDSSender {
            @display("p=137,96;i=block/process_s");
        }

        mac: <macType> like IWirelessMac {
            @display("p=52,51;i=block/layer");
        }

        ids: <idsType> like IWirelessIDS {
            @display("p=52,127;i=block/cogwheel");
        }

        phy: <phyType> like IWirelessPhy {
            @display("p=52,200;i=block/process_s");
        }

    connections:
        
        idsSender.upperLayerOut --> { @display("ls=black;m=m,25,50,25,0"); } --> upperLayerOut;
        idsSender.upperLayerIn <-- { @display("ls=black;m=m,15,50,15,0"); } <-- upperLayerIn;
        idsSender.upperControlOut --> { @display("ls=red;m=m,75,50,75,0"); } --> upperControlOut;
        idsSender.upperControlIn <-- { @display("ls=red;m=m,85,0,85,0"); } <-- upperControlIn;
        
        
        mac.upperLayerOut --> { @display("ls=black;m=m,25,50,25,0"); } --> idsSender.lowerLayerIn;
        mac.upperLayerIn <-- { @display("ls=black;m=m,15,50,15,0"); } <-- idsSender.lowerLayerOut;
        mac.upperControlOut --> { @display("ls=red;m=m,75,50,75,0"); } --> idsSender.lowerControlIn;
        mac.upperControlIn <-- { @display("ls=red;m=m,85,0,85,0"); } <-- idsSender.lowerControlOut;
        
//        mac.upperLayerOut --> { @display("ls=black;m=m,25,50,25,0"); } --> upperLayerOut;
//        mac.upperLayerIn <-- { @display("ls=black;m=m,15,50,15,0"); } <-- upperLayerIn;
//        mac.upperControlOut --> { @display("ls=red;m=m,75,50,75,0"); } --> upperControlOut;
//        mac.upperControlIn <-- { @display("ls=red;m=m,85,0,85,0"); } <-- upperControlIn;

        ids.upperLayerOut --> { @display("ls=black;m=m,25,50,25,0"); } --> mac.lowerLayerIn;
        ids.upperLayerIn <-- { @display("ls=black;m=m,15,50,15,0"); } <-- mac.lowerLayerOut;
        ids.upperControlOut --> { @display("ls=red;m=m,75,50,75,0"); } --> mac.lowerControlIn;
        ids.upperControlIn <-- { @display("ls=red;m=m,85,0,85,0"); } <-- mac.lowerControlOut;

        ids.idsSenderControlOut --> { @display("ls=red;m=m,75,50,75,0"); } --> idsSender.idsControlIn;

        phy.upperLayerOut --> { @display("ls=black;m=m,25,50,25,0"); } --> ids.lowerLayerIn;
        phy.upperLayerIn <-- { @display("ls=black;m=m,15,50,15,0"); } <-- ids.lowerLayerOut;
        phy.upperControlOut --> { @display("ls=black;m=m,75,50,75,0"); } --> ids.lowerControlIn;
        phy.upperControlIn <-- { @display("ls=red;m=m,85,0,85,0"); } <-- ids.lowerControlOut;

        radioIn --> phy.radioIn;
}
