#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/lte-module.h"
#include "ns3/applications-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/config-store-module.h"
//#include "ns3/netanim-module.h"


//-------------------------------------------------------


using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("LenaX2HandoverMeasures");

 uint32_t ByteCounter = 0;
 uint32_t oldByteCounter = 0;
uint32_t packet_counter=0;

void
NotifyConnectionEstablishedUe (std::string context,
                               uint64_t imsi,
                               uint16_t cellid,
                               uint16_t rnti)
{
  std::cout << context
            << " UE IMSI " << imsi
            << ": connected to CellId " << cellid
            << " with RNTI " << rnti
            << std::endl;
}

void
NotifyHandoverStartUe (std::string context,
                       uint64_t imsi,
                       uint16_t cellid,
                       uint16_t rnti,
                       uint16_t targetCellId)
{
  std::cout << context
            << " UE IMSI " << imsi
            << ": previously connected to CellId " << cellid
            << " with RNTI " << rnti
            << ", doing handover to CellId " << targetCellId
            << std::endl;
}

void
NotifyHandoverEndOkUe (std::string context,
                       uint64_t imsi,
                       uint16_t cellid,
                       uint16_t rnti)
{
  std::cout << context
            << " UE IMSI " << imsi
            << ": successful handover to CellId " << cellid
            << " with RNTI " << rnti
            << std::endl;
}

void
NotifyConnectionEstablishedEnb (std::string context,
                                uint64_t imsi,
                                uint16_t cellid,
                                uint16_t rnti)
{
  std::cout << context
            << " eNB CellId " << cellid
            << ": successful connection of UE with IMSI " << imsi
            << " RNTI " << rnti
            << std::endl;
}

void
NotifyHandoverStartEnb (std::string context,
                        uint64_t imsi,
                        uint16_t cellid,
                        uint16_t rnti,
                        uint16_t targetCellId)
{
  std::cout << context
            << " eNB CellId " << cellid
            << ": start handover of UE with IMSI " << imsi
            << " RNTI " << rnti
            << " to CellId " << targetCellId
            << std::endl;
}

void
NotifyHandoverEndOkEnb (std::string context,
                        uint64_t imsi,
                        uint16_t cellid,
                        uint16_t rnti)
{
  std::cout << context
            << " eNB CellId " << cellid
            << ": completed handover of UE with IMSI " << imsi
            << " RNTI " << rnti
            << std::endl;
}

//-----------------------------------------------
//-----------------------------------------------


 void
 ReceivePacket (Ptr<const Packet> packet, const Address &)
 {
   ByteCounter += packet->GetSize ();
   packet_counter+=1;
   //std::cout << Simulator::Now() << "  Packet Size " << packet->GetSize ()<< " Total No. of Packets "<< packet_counter << std::endl;
 }
 

 void
 Throughput(bool &firstWrite, Time binSize, std::string fileName)
 {
   std::ofstream output;
 
   if (firstWrite == true)
     {
       output.open (fileName.c_str (), std::ofstream::out);
       firstWrite = false;
     }
   else
     {
       output.open (fileName.c_str (), std::ofstream::app);
     }
 
   //Instantaneous throughput every 200 ms
   double  throughput = (ByteCounter - oldByteCounter)*8/binSize.GetSeconds ()/1024/1024;
double no_packets=packet_counter;
   //double  throughput = (ByteCounter)*8/Simulator::Now().GetSeconds ()/1024/1024;
   //double  throughput = (ByteCounter);
   output << Simulator::Now().GetSeconds() << " " << throughput <<" "<< no_packets <<std::endl;
   //oldByteCounter = ByteCounter;
   ByteCounter=0;
   Simulator::Schedule (binSize, &Throughput, firstWrite, binSize, fileName);
 }

using namespace ns3;

int main (int argc, char *argv[])
{

uint16_t numberOfUes = 10;
uint16_t numberOfEnbs = 2;
double distance = 500.0;
uint16_t numBearersPerUe = 1;

double enbTxPowerDbm = 46.0;
bool useIdealRrc = true;

Config::SetDefault ("ns3::UdpClient::Interval", TimeValue (MilliSeconds (2)));
Config::SetDefault ("ns3::UdpClient::MaxPackets", UintegerValue (1000000));
Config::SetDefault ("ns3::UdpClient::PacketSize", UintegerValue (1024));
Config::SetDefault ("ns3::LteHelper::UseIdealRrc", BooleanValue (true));

CommandLine cmd;
cmd.AddValue ("enbTxPowerDbm", "TX power [dBm] used by HeNBs (default = 46.0)", enbTxPowerDbm);
cmd.AddValue ("useIdealRrc", "Use ideal RRC protocol", useIdealRrc);
cmd.Parse (argc, argv);


//-----------------------------------------------

Ptr<LteHelper> lteHelper = CreateObject<LteHelper> ();

Ptr<PointToPointEpcHelper> epcHelper = CreateObject<PointToPointEpcHelper> ();
lteHelper->SetEpcHelper (epcHelper);
lteHelper->SetSchedulerType ("ns3::PfFfMacScheduler");

lteHelper->SetHandoverAlgorithmType ("ns3::A3RsrpHandoverAlgorithm");
lteHelper->SetHandoverAlgorithmAttribute ("Hysteresis",DoubleValue (3.0));
lteHelper->SetHandoverAlgorithmAttribute ("TimeToTrigger",TimeValue (MilliSeconds (256)));

//------------------------------------------------

Ptr<Node> pgw = epcHelper->GetPgwNode ();
// Create a single RemoteHost
  NodeContainer remoteHostContainer;
  remoteHostContainer.Create (1);
  Ptr<Node> remoteHost = remoteHostContainer.Get (0);
  InternetStackHelper internet;
  internet.Install (remoteHostContainer);

  // Create the Internet
  PointToPointHelper p2ph;
  p2ph.SetDeviceAttribute ("DataRate", DataRateValue (DataRate ("100Gb/s")));
  p2ph.SetDeviceAttribute ("Mtu", UintegerValue (1500));
  p2ph.SetChannelAttribute ("Delay", TimeValue (Seconds (0.010)));
  NetDeviceContainer internetDevices = p2ph.Install (pgw, remoteHost);
  Ipv4AddressHelper ipv4h;
  ipv4h.SetBase ("1.0.0.0", "255.0.0.0");
  Ipv4InterfaceContainer internetIpIfaces = ipv4h.Assign (internetDevices);
  Ipv4Address remoteHostAddr = internetIpIfaces.GetAddress (1);

// Routing of the Internet Host (towards the LTE network)
  Ipv4StaticRoutingHelper ipv4RoutingHelper;
  Ptr<Ipv4StaticRouting> remoteHostStaticRouting = ipv4RoutingHelper.GetStaticRouting (remoteHost->GetObject<Ipv4> ());
  // interface 0 is localhost, 1 is the p2p device
  remoteHostStaticRouting->AddNetworkRouteTo (Ipv4Address ("7.0.0.0"), Ipv4Mask ("255.0.0.0"), 1);




//-----------------------------------------------

NodeContainer enbNodes;
enbNodes.Create (numberOfEnbs);
NodeContainer ueNodes;
ueNodes.Create (numberOfUes);

//-----------------------------------------------

// Install Mobility Model in eNB
  Ptr<ListPositionAllocator> enbPositionAlloc = CreateObject<ListPositionAllocator> ();
  for (uint16_t i = 0; i < numberOfEnbs; i++)
    {
      Vector enbPosition (distance * (i + 1), distance, 0);
      enbPositionAlloc->Add (enbPosition);
    }
  MobilityHelper enbMobility;
  enbMobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  enbMobility.SetPositionAllocator (enbPositionAlloc);
  enbMobility.Install (enbNodes);

//--------------------------------------------------

// Install Mobility Model in UE
  MobilityHelper ueMobility;
  ueMobility.SetMobilityModel ("ns3::ConstantVelocityMobilityModel");
  ueMobility.Install (ueNodes);
  ueNodes.Get (0)->GetObject<MobilityModel> ()->SetPosition (Vector (750,250,0));
  ueNodes.Get (1)->GetObject<MobilityModel> ()->SetPosition (Vector (750,500,0));
  ueNodes.Get (2)->GetObject<MobilityModel> ()->SetPosition (Vector (750,750,0));
  ueNodes.Get (3)->GetObject<MobilityModel> ()->SetPosition (Vector (625,375,0));  
  ueNodes.Get (4)->GetObject<MobilityModel> ()->SetPosition (Vector (625,625,0));
  ueNodes.Get (5)->GetObject<MobilityModel> ()->SetPosition (Vector (375,375,0));
  ueNodes.Get (6)->GetObject<MobilityModel> ()->SetPosition (Vector (375,625,0));
  ueNodes.Get (7)->GetObject<MobilityModel> ()->SetPosition (Vector (250,500,0));
  ueNodes.Get (8)->GetObject<MobilityModel> ()->SetPosition (Vector (250,375,0));
  ueNodes.Get (9)->GetObject<MobilityModel> ()->SetPosition (Vector (250,625,0));
   
   ueNodes.Get (0)->GetObject<ConstantVelocityMobilityModel> ()->SetVelocity (Vector (0, 0, 0));
   ueNodes.Get (1)->GetObject<ConstantVelocityMobilityModel> ()->SetVelocity (Vector (0, 0, 0));
ueNodes.Get (2)->GetObject<ConstantVelocityMobilityModel> ()->SetVelocity (Vector (0, 0, 0));
ueNodes.Get (3)->GetObject<ConstantVelocityMobilityModel> ()->SetVelocity (Vector (0, 0, 0));
ueNodes.Get (4)->GetObject<ConstantVelocityMobilityModel> ()->SetVelocity (Vector (0, 0, 0));
ueNodes.Get (5)->GetObject<ConstantVelocityMobilityModel> ()->SetVelocity (Vector (0, 0, 0));
ueNodes.Get (6)->GetObject<ConstantVelocityMobilityModel> ()->SetVelocity (Vector (0, 0, 0));
ueNodes.Get (7)->GetObject<ConstantVelocityMobilityModel> ()->SetVelocity (Vector (0, 0, 0));
ueNodes.Get (8)->GetObject<ConstantVelocityMobilityModel> ()->SetVelocity (Vector (0, 0, 0));
ueNodes.Get (9)->GetObject<ConstantVelocityMobilityModel> ()->SetVelocity (Vector (0, 0, 0));
  //ueNodes.Get (0)->GetObject<MobilityModel> ()->SetPosition (Vector (750,250,0));
  //ueNodes.Get (1)->GetObject<MobilityModel> ()->SetPosition (Vector (750,500,0));
  //ueNodes.Get (2)->GetObject<MobilityModel> ()->SetPosition (Vector (750,750,0));
  //ueNodes.Get (0)->GetObject<MobilityModel> ()->SetPosition (Vector (625,375,0));  
  //ueNodes.Get (1)->GetObject<MobilityModel> ()->SetPosition (Vector (625,625,0));
  //ueNodes.Get (2)->GetObject<MobilityModel> ()->SetPosition (Vector (375,375,0));
  //ueNodes.Get (3)->GetObject<MobilityModel> ()->SetPosition (Vector (375,625,0));
  //ueNodes.Get (4)->GetObject<MobilityModel> ()->SetPosition (Vector (250,500,0));
  //ueNodes.Get (5)->GetObject<MobilityModel> ()->SetPosition (Vector (250,375,0));
  //ueNodes.Get (6)->GetObject<MobilityModel> ()->SetPosition (Vector (250,625,0));



//--------------------------------------------------
 // Install LTE Devices in eNB and UEs
  Config::SetDefault ("ns3::LteEnbPhy::TxPower", DoubleValue (enbTxPowerDbm));
  NetDeviceContainer enbLteDevs = lteHelper->InstallEnbDevice (enbNodes);
  NetDeviceContainer ueLteDevs = lteHelper->InstallUeDevice (ueNodes);

//--------------------------------------------------

// Install the IP stack on the UEs
  internet.Install (ueNodes);
  Ipv4InterfaceContainer ueIpIfaces;
  ueIpIfaces = epcHelper->AssignUeIpv4Address (NetDeviceContainer (ueLteDevs));

//--------------------------------------------------

// Attach all UEs to the first eNodeB
  for (uint16_t i = 0; i < numberOfUes; i++)
    {
      lteHelper->Attach (ueLteDevs.Get (i), enbLteDevs.Get (0));
    }

//-------------------------------------------------

NS_LOG_LOGIC ("setting up applications");

  // Install and start applications on UEs and remote host
  uint16_t dlPort = 10000;
  uint16_t ulPort = 20000;

  // randomize a bit start times to avoid simulation artifacts
  // (e.g., buffer overflows due to packet transmissions happening
  // exactly at the same time)
  Ptr<UniformRandomVariable> startTimeSeconds = CreateObject<UniformRandomVariable> ();
  startTimeSeconds->SetAttribute ("Min", DoubleValue (0));
  startTimeSeconds->SetAttribute ("Max", DoubleValue (0.010));

//--------------------------------------------------

//enum EpsBearer::Qci q = EpsBearer::GBR_CONV_VOICE;
//EpsBearer bearer (q);
//lteHelper->ActivateDataRadioBearer (ueDevs, bearer);

//--------------------------------------------------

 for (uint32_t u = 0; u < numberOfUes; ++u)
    {
      Ptr<Node> ue = ueNodes.Get (u);
      // Set the default gateway for the UE
      Ptr<Ipv4StaticRouting> ueStaticRouting = ipv4RoutingHelper.GetStaticRouting (ue->GetObject<Ipv4> ());
      ueStaticRouting->SetDefaultRoute (epcHelper->GetUeDefaultGatewayAddress (), 1);

      for (uint32_t b = 0; b < numBearersPerUe; ++b)
        {
          ++dlPort;
          ++ulPort;

          ApplicationContainer clientApps;
          ApplicationContainer serverApps;

          NS_LOG_LOGIC ("installing UDP DL app for UE " << u);
          UdpClientHelper dlClientHelper (ueIpIfaces.GetAddress (u), dlPort);
          clientApps.Add (dlClientHelper.Install (remoteHost));
          PacketSinkHelper dlPacketSinkHelper ("ns3::UdpSocketFactory",
                                               InetSocketAddress (Ipv4Address::GetAny (), dlPort));
          serverApps.Add (dlPacketSinkHelper.Install (ue));

          NS_LOG_LOGIC ("installing UDP UL app for UE " << u);
          UdpClientHelper ulClientHelper (remoteHostAddr, ulPort);
          clientApps.Add (ulClientHelper.Install (ue));
          PacketSinkHelper ulPacketSinkHelper ("ns3::UdpSocketFactory",
                                               InetSocketAddress (Ipv4Address::GetAny (), ulPort));
          serverApps.Add (ulPacketSinkHelper.Install (remoteHost));

          Ptr<EpcTft> tft = Create<EpcTft> ();
          EpcTft::PacketFilter dlpf;
          dlpf.localPortStart = dlPort;
          dlpf.localPortEnd = dlPort;
          tft->Add (dlpf);
          EpcTft::PacketFilter ulpf;
          ulpf.remotePortStart = ulPort;
          ulpf.remotePortEnd = ulPort;
          tft->Add (ulpf);
          EpsBearer bearer (EpsBearer::NGBR_VIDEO_TCP_DEFAULT);
          lteHelper->ActivateDedicatedEpsBearer (ueLteDevs.Get (u), bearer, tft);

          Time startTime = Seconds (startTimeSeconds->GetValue ());
          serverApps.Start (startTime);
          clientApps.Start (startTime);

        } // end for b
    }


//-----------------------------------------------

// Add X2 interface
  lteHelper->AddX2Interface (enbNodes);

  // X2-based Handover
  //lteHelper->HandoverRequest (Seconds (0.100), ueLteDevs.Get (0), enbLteDevs.Get (0), enbLteDevs.Get (1));

  // Uncomment to enable PCAP tracing
  // p2ph.EnablePcapAll("lena-x2-handover-measures");

  lteHelper->EnablePhyTraces ();
  lteHelper->EnableMacTraces ();
  lteHelper->EnableRlcTraces ();
  lteHelper->EnablePdcpTraces ();
  Ptr<RadioBearerStatsCalculator> rlcStats = lteHelper->GetRlcStats ();
  rlcStats->SetAttribute ("EpochDuration", TimeValue (Seconds (1.0)));
  Ptr<RadioBearerStatsCalculator> pdcpStats = lteHelper->GetPdcpStats ();
  pdcpStats->SetAttribute ("EpochDuration", TimeValue (Seconds (1.0)));

  // connect custom trace sinks for RRC connection establishment and handover notification
  Config::Connect ("/NodeList/*/DeviceList/*/LteEnbRrc/ConnectionEstablished",
                   MakeCallback (&NotifyConnectionEstablishedEnb));
  Config::Connect ("/NodeList/*/DeviceList/*/LteUeRrc/ConnectionEstablished",
                   MakeCallback (&NotifyConnectionEstablishedUe));
  Config::Connect ("/NodeList/*/DeviceList/*/LteEnbRrc/HandoverStart",
                   MakeCallback (&NotifyHandoverStartEnb));
  Config::Connect ("/NodeList/*/DeviceList/*/LteUeRrc/HandoverStart",
                   MakeCallback (&NotifyHandoverStartUe));
  Config::Connect ("/NodeList/*/DeviceList/*/LteEnbRrc/HandoverEndOk",
                   MakeCallback (&NotifyHandoverEndOkEnb));
  Config::Connect ("/NodeList/*/DeviceList/*/LteUeRrc/HandoverEndOk",
                   MakeCallback (&NotifyHandoverEndOkUe));


//-----------------------------------------------



//----------------------------------------------



//---------------------------------------------


  //Trace sink for the packet sink of UE
   std::ostringstream oss0,oss1,oss2,oss3,oss4,oss5,oss6,oss7,oss8,oss9;



   oss8 << "/NodeList/" << ueNodes.Get (8)->GetId () << "/ApplicationList/0/$ns3::PacketSink/Rx";
   Config::ConnectWithoutContext (oss8.str (), MakeCallback (&ReceivePacket));

   //oss0 << "/NodeList/" << enbNodes.Get (0)->GetId () << "/ApplicationList/0/$ns3::PacketSink/Rx";
   //Config::ConnectWithoutContext (oss0.str (), MakeCallback (&ReceivePacket));


 
   bool firstWrite = true;
   std::string rrcType = useIdealRrc == 1 ? "ideal_rrc":"real_rrc";
   std::string fileName = "rlf_dl_thrput_" + std::to_string (enbNodes.GetN ()) + "_eNB_" + rrcType;
   Time binSize = Seconds (0.2);
 Simulator::Schedule (Seconds(0.2), &Throughput, firstWrite, binSize, fileName);
//---------------------------------------------

//AnimationInterface anim ("abhi.xml"); 
// anim.SetConstantPosition (ueNodes.Get(0),750,250); 
// anim.SetConstantPosition (ueNodes.Get(1),750,500);
// anim.SetConstantPosition (ueNodes.Get(2),750,750);
// anim.SetConstantPosition (ueNodes.Get(3),625,375);
// anim.SetConstantPosition (ueNodes.Get(4),625,625);
// anim.SetConstantPosition (ueNodes.Get(5),375,375);
// anim.SetConstantPosition (ueNodes.Get(6),375,625);
// anim.SetConstantPosition (ueNodes.Get(7),250,500);
// anim.SetConstantPosition (ueNodes.Get(8),250,375);
// anim.SetConstantPosition (ueNodes.Get(9),250,625);
// anim.SetConstantPosition (enbNodes.Get(0),500.0,500.0);
// anim.SetConstantPosition (enbNodes.Get(1),1000.0,500.0);

//---------------------------------------------

Simulator::Stop (Seconds (3));
Simulator::Run ();

//---------------------------------------------

Simulator::Destroy ();
return 0;
}

//---------------------------------------------