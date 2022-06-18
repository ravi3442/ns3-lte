
//NOTE: For running this model you need to change SRSPeriodicity to 160  
//and need to comment one Fatal error as given below in lte-enb-rrc.cc file
      //RemoveUe (rnti); //abhishek
      //NS_FATAL_ERROR ("should trigger HO Preparation Failure, but it is not implemented"); //abhishek
      //NS_FATAL_ERROR ("X2-U data received but no X2uTeidInfo found");  //abhishek


#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/lte-module.h"
#include "ns3/applications-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/config-store-module.h"
#include "ns3/netanim-module.h"

//Added for flow monitor
#include "ns3/flow-monitor.h"
#include "ns3/flow-monitor-helper.h"

//---------------------------------------------
using namespace ns3;


NS_LOG_COMPONENT_DEFINE ("LenaX2HandoverMeasures");

 uint32_t ByteCounter = 0;
 uint32_t oldByteCounter = 0;


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
 


//---------------------------------# Added for Throughput Calculation

 void
 ReceivePacket (Ptr<const Packet> packet, const Address &)
 {
   ByteCounter += packet->GetSize ();
   std::cout << Simulator::Now() << "  Packet Size " << packet->GetSize () << std::endl;
 }

 void
 Throughput(bool firstWrite, Time binSize, std::string fileName)
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
   //double  throughput = (ByteCounter)*8/Simulator::Now().GetSeconds ()/1024/1024;
   //double  throughput = (ByteCounter);
   output << Simulator::Now().GetSeconds() << " " << throughput << std::endl;
   oldByteCounter = ByteCounter;
   Simulator::Schedule (binSize, &Throughput, firstWrite, binSize, fileName);
 }

//---------------------------------------------------------------------------


static void 
CourseChange (std::string foo, Ptr<const MobilityModel> mobility)
{
  //Vector pos = mobility->GetPosition ();
  //Vector vel = mobility->GetVelocity ();
  //std::cout << Simulator::Now () << ", model=" << mobility << ", POS: x=" << pos.x << ", y=" << pos.y
    //        << ", z=" << pos.z << "; VEL: x=" << vel.x << ", y=" << vel.y
      //    << ", z=" << vel.z << std::endl;
}

using namespace ns3;

int main (int argc, char *argv[])
{
uint16_t numberOfUes = 60;
uint16_t numberOfEnbs = 16;
uint16_t distance = 1000;
uint16_t numBearersPerUe = 1;
double enbTxPowerDbm = 44.0;
uint16_t c = 0;

//-----------------------
bool useIdealRrc = true;
//-----------------------
   Config::SetDefault ("ns3::UdpClient::Interval", TimeValue (MicroSeconds (128000))); //128 Kbps make it 64000
   //Config::SetDefault ("ns3::UdpClient::Interval", TimeValue (MicroSeconds (32000))); //256 Kbps make it 32000
   Config::SetDefault ("ns3::UdpClient::MaxPackets", UintegerValue (1000000));
   Config::SetDefault ("ns3::UdpClient::PacketSize", UintegerValue (1024));
   Config::SetDefault ("ns3::LteHelper::UseIdealRrc", BooleanValue (true));

  Config::SetDefault ("ns3::RandomWalk2dMobilityModel::Mode", StringValue ("Time"));
  Config::SetDefault ("ns3::RandomWalk2dMobilityModel::Time", StringValue ("2s"));
  Config::SetDefault ("ns3::RandomWalk2dMobilityModel::Speed", StringValue ("ns3::ConstantRandomVariable[Constant=1.0]"));
  Config::SetDefault ("ns3::RandomWalk2dMobilityModel::Bounds", StringValue ("0|200|0|200"));

  Config::SetDefault ("ns3::LteEnbMac::PreambleTransMax",UintegerValue(100));//abhishek

//-------
//Config::SetDefault ("ns3::LteEnbNetDevice::UlBandwidth", StringValue ("100"));
//Config::SetDefault ("ns3::LteEnbNetDevice::DlBandwidth", StringValue ("100"));

//default ns3::LteEnbNetDevice::UlBandwidth "25"
//default ns3::LteEnbNetDevice::DlBandwidth "25"
//-------

  CommandLine cmd;
cmd.AddValue ("enbTxPowerDbm", "TX power [dBm] used by HeNBs (default = 46.0)", enbTxPowerDbm);
cmd.AddValue ("useIdealRrc", "Use ideal RRC protocol", useIdealRrc);
  cmd.Parse (argc, argv);
//---------------------------------------
//-----------------------------------------------

Ptr<LteHelper> lteHelper = CreateObject<LteHelper> ();

Ptr<PointToPointEpcHelper> epcHelper = CreateObject<PointToPointEpcHelper> ();
lteHelper->SetEpcHelper (epcHelper);
lteHelper->SetSchedulerType ("ns3::PfFfMacScheduler");

lteHelper->SetHandoverAlgorithmType ("ns3::A3RsrpHandoverAlgorithm");
lteHelper->SetHandoverAlgorithmAttribute ("Hysteresis",DoubleValue (3.0));
lteHelper->SetHandoverAlgorithmAttribute ("TimeToTrigger",TimeValue (MilliSeconds (256)));

//-----------------------------------------------
Ptr<Node> pgw = epcHelper->GetPgwNode ();

  // Create a single RemoteHost
  NodeContainer remoteHostContainer;
  remoteHostContainer.Create (1);
  Ptr<Node> remoteHost = remoteHostContainer.Get (0);

  InternetStackHelper internet;
  internet.Install (remoteHostContainer);


//-----------------------------------------------

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

//---------------------------------------

NodeContainer enbNodes;
enbNodes.Create (numberOfEnbs);
NodeContainer ueNodes;
ueNodes.Create (numberOfUes);
//-----------------------------------------------

//-----------------------------------------------


// Install Mobility Model in eNB
  Ptr<ListPositionAllocator> enbPositionAlloc = CreateObject<ListPositionAllocator> ();
  for (uint16_t i = 0; i < numberOfEnbs/4; i++)
    {
      for (uint16_t j = 0; j < numberOfEnbs/4; j++)
        {
          Vector enbPosition (distance * (j + 1), distance * (i + 1), 0);
          enbPositionAlloc->Add (enbPosition); 
        }
     
    }
  MobilityHelper enbMobility;
  enbMobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  enbMobility.SetPositionAllocator (enbPositionAlloc);
  enbMobility.Install (enbNodes);

//-----------------------------------------------
// Install Mobility Model in mobile UE
  MobilityHelper mobility;
  mobility.SetPositionAllocator ("ns3::RandomDiscPositionAllocator",
                                 "X", StringValue ("2500"),
                                 "Y", StringValue ("2500"),
                                 "Rho", StringValue ("ns3::ConstantRandomVariable[Constant=10.0]"));
  mobility.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
                             "Mode", StringValue ("Time"),
                             "Time", StringValue ("500ms"),
                             "Speed", StringValue ("ns3::ConstantRandomVariable[Constant=10.0]"),
                             "Bounds", StringValue ("0|50000|0|50000"));
   
 for (uint16_t i = 0; i < 33; i++)
    {
      mobility.Install (ueNodes.Get(i));
      c = i ; 
    }

ueNodes.Get (0)->GetObject<MobilityModel> ()->SetPosition (Vector (1358,1358,0));
ueNodes.Get (1)->GetObject<MobilityModel> ()->SetPosition (Vector (2358,1358,0));
ueNodes.Get (2)->GetObject<MobilityModel> ()->SetPosition (Vector (3358,1358,0));
ueNodes.Get (3)->GetObject<MobilityModel> ()->SetPosition (Vector (1358,2358,0));
ueNodes.Get (4)->GetObject<MobilityModel> ()->SetPosition (Vector (2358,2358,0));
ueNodes.Get (5)->GetObject<MobilityModel> ()->SetPosition (Vector (3358,2358,0));
ueNodes.Get (6)->GetObject<MobilityModel> ()->SetPosition (Vector (1358,3358,0));
ueNodes.Get (7)->GetObject<MobilityModel> ()->SetPosition (Vector (2358,3358,0));
ueNodes.Get (8)->GetObject<MobilityModel> ()->SetPosition (Vector (3358,3358,0));

ueNodes.Get (9)->GetObject<MobilityModel> ()->SetPosition (Vector (1358,1000,0));
ueNodes.Get (10)->GetObject<MobilityModel> ()->SetPosition (Vector (2358,1000,0));
ueNodes.Get (11)->GetObject<MobilityModel> ()->SetPosition (Vector (3358,1000,0));
ueNodes.Get (12)->GetObject<MobilityModel> ()->SetPosition (Vector (1358,2000,0));
ueNodes.Get (13)->GetObject<MobilityModel> ()->SetPosition (Vector (2358,2000,0));
ueNodes.Get (14)->GetObject<MobilityModel> ()->SetPosition (Vector (3358,2000,0));
ueNodes.Get (15)->GetObject<MobilityModel> ()->SetPosition (Vector (1358,3000,0));
ueNodes.Get (16)->GetObject<MobilityModel> ()->SetPosition (Vector (2358,3000,0));
ueNodes.Get (17)->GetObject<MobilityModel> ()->SetPosition (Vector (3358,3000,0));
ueNodes.Get (18)->GetObject<MobilityModel> ()->SetPosition (Vector (1358,4000,0));
ueNodes.Get (19)->GetObject<MobilityModel> ()->SetPosition (Vector (2358,4000,0));
ueNodes.Get (20)->GetObject<MobilityModel> ()->SetPosition (Vector (3358,4000,0));
ueNodes.Get (21)->GetObject<MobilityModel> ()->SetPosition (Vector (1000,1358,0));
ueNodes.Get (22)->GetObject<MobilityModel> ()->SetPosition (Vector (2000,1358,0));
ueNodes.Get (23)->GetObject<MobilityModel> ()->SetPosition (Vector (3000,1358,0));
ueNodes.Get (24)->GetObject<MobilityModel> ()->SetPosition (Vector (4000,1358,0));
ueNodes.Get (25)->GetObject<MobilityModel> ()->SetPosition (Vector (1000,2358,0));
ueNodes.Get (26)->GetObject<MobilityModel> ()->SetPosition (Vector (2000,2358,0));
ueNodes.Get (27)->GetObject<MobilityModel> ()->SetPosition (Vector (3000,2358,0));
ueNodes.Get (28)->GetObject<MobilityModel> ()->SetPosition (Vector (4000,2358,0));
ueNodes.Get (29)->GetObject<MobilityModel> ()->SetPosition (Vector (1000,3358,0));
ueNodes.Get (30)->GetObject<MobilityModel> ()->SetPosition (Vector (2000,3358,0));
ueNodes.Get (31)->GetObject<MobilityModel> ()->SetPosition (Vector (3000,3358,0));
ueNodes.Get (32)->GetObject<MobilityModel> ()->SetPosition (Vector (4000,3358,0));


//c=33;

// Install Mobility Model in static UE

  mobility.SetMobilityModel ("ns3::ConstantVelocityMobilityModel");
for (uint16_t i = c+1; i < numberOfUes; i++)
    {
      mobility.Install (ueNodes.Get(i));
    }

  for (uint16_t i = 0; i < 2; i++)
    {
     for (uint16_t j = 0; j < 1; j++)
      {
         c++;
         ueNodes.Get (c)->GetObject<MobilityModel> ()->SetPosition (Vector ((700+150*j),(700+200*i),0));
         
      }
    }

  for (uint16_t i = 0; i < 2; i++)
    {
     for (uint16_t j = 0; j < 1; j++)
      {
         c++;
         ueNodes.Get (c)->GetObject<MobilityModel> ()->SetPosition (Vector ((700+150*j),(1700+200*i),0));
         
      }
    }

  for (uint16_t i = 0; i < 2; i++)
    {
     for (uint16_t j = 0; j < 1; j++)
      {
         c++;
         ueNodes.Get (c)->GetObject<MobilityModel> ()->SetPosition (Vector ((700+150*j),(2700+200*i),0));
         
      }
    }

  for (uint16_t i = 0; i < 2; i++)
    {
     for (uint16_t j = 0; j < 1; j++)
      {
         c++;
         ueNodes.Get (c)->GetObject<MobilityModel> ()->SetPosition (Vector ((700+150*j),(3700+200*i),0));
         
      }
    }

    for (uint16_t i = 0; i < 1; i++)
    {
     for (uint16_t j = 0; j < 1; j++)
      {
         c++;
         ueNodes.Get (c)->GetObject<MobilityModel> ()->SetPosition (Vector ((1700+150*j),(700+200*i),0));
         
      }
    }

    for (uint16_t i = 0; i < 2; i++)
    {
     for (uint16_t j = 0; j < 1; j++)
      {
         c++;
         ueNodes.Get (c)->GetObject<MobilityModel> ()->SetPosition (Vector ((1700+150*j),(1700+200*i),0));
         
      }
    }

    for (uint16_t i = 0; i < 1; i++)
    {
     for (uint16_t j = 0; j < 1; j++)
      {
         c++;
         ueNodes.Get (c)->GetObject<MobilityModel> ()->SetPosition (Vector ((1700+150*j),(2700+200*i),0));
         
      }
    }

    for (uint16_t i = 0; i < 1; i++)
    {
     for (uint16_t j = 0; j < 1; j++)
      {
         c++;
         ueNodes.Get (c)->GetObject<MobilityModel> ()->SetPosition (Vector ((1700+150*j),(3700+200*i),0));
         
      }
    }


    for (uint16_t i = 0; i < 1; i++)
    {
     for (uint16_t j = 0; j < 1; j++)
      {
         c++;
         ueNodes.Get (c)->GetObject<MobilityModel> ()->SetPosition (Vector ((2700+150*j),(700+300*i),0));
         
      }
    }

    for (uint16_t i = 0; i < 2; i++)
    {
     for (uint16_t j = 0; j < 1; j++)
      {
         c++;
         ueNodes.Get (c)->GetObject<MobilityModel> ()->SetPosition (Vector ((2700+150*j),(1700+300*i),0));
         
      }
    }

    for (uint16_t i = 0; i < 1; i++)
    {
     for (uint16_t j = 0; j < 1; j++)
      {
         c++;
         ueNodes.Get (c)->GetObject<MobilityModel> ()->SetPosition (Vector ((2700+150*j),(2700+300*i),0));
         
      }
    }

    for (uint16_t i = 0; i < 2; i++)
    {
     for (uint16_t j = 0; j < 1; j++)
      {
         c++;
         ueNodes.Get (c)->GetObject<MobilityModel> ()->SetPosition (Vector ((2700+150*j),(3700+300*i),0));
         
      }
    }



    for (uint16_t i = 0; i < 2; i++)
    {
     for (uint16_t j = 0; j < 1; j++)
      {
         c++;
         ueNodes.Get (c)->GetObject<MobilityModel> ()->SetPosition (Vector ((3700+150*j),(700+200*i),0));
         
      }
    }

    for (uint16_t i = 0; i < 2; i++)
    {
     for (uint16_t j = 0; j < 1; j++)
      {
         c++;
         ueNodes.Get (c)->GetObject<MobilityModel> ()->SetPosition (Vector ((3700+150*j),(1700+200*i),0));
         
      }
    }

    for (uint16_t i = 0; i < 2; i++)
    {
     for (uint16_t j = 0; j < 1; j++)
      {
         c++;
         ueNodes.Get (c)->GetObject<MobilityModel> ()->SetPosition (Vector ((3700+150*j),(2700+200*i),0));
         
      }
    }

    for (uint16_t i = 0; i < 2; i++)
    {
     for (uint16_t j = 0; j < 1; j++)
      {
         c++;
         ueNodes.Get (c)->GetObject<MobilityModel> ()->SetPosition (Vector ((3700+150*j),(3700+200*i),0));
         
      }
    }
   
//--Set Velocity = 0 for static UEs
for (uint16_t i = 33; i < numberOfUes; i++)
    {
     ueNodes.Get (i)->GetObject<ConstantVelocityMobilityModel> ()->SetVelocity (Vector (0, 0, 0));
    }

 
//-------------------------------------------------


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

// Attach all UEs to the eNodeB
lteHelper->AttachToClosestEnb (ueLteDevs, enbLteDevs);

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

//------------------------------------------------------------------
  Config::Connect ("/NodeList/*/$ns3::MobilityModel/CourseChange",
                   MakeCallback (&CourseChange));
//------------------------------------------------------------------

//--------------------------------------------------
  //Trace sink for the packet sink of UE
  std::ostringstream oss3;
   //std::ostringstream oss0,oss1,oss2,oss3,oss4,oss5;
                        //oss6,oss7,oss8,oss9,oss10;
  //std::ostringstream oss0,
    //                  oss1,oss2,oss3,oss4,oss5,oss6,oss7,oss8,oss9,oss10,oss11,oss12,oss13,oss14,oss15,oss16,
      //                oss17,oss18,oss19,oss20,oss21,oss22,oss23,oss24;
     //std::ostringstream oss17,oss18,oss19,oss20,oss21,oss22,oss23,oss24;

  //oss0 << "/NodeList/" << ueNodes.Get (0)->GetId () << "/ApplicationList/0/$ns3::PacketSink/Rx";
  //Config::ConnectWithoutContext (oss0.str (), MakeCallback (&ReceivePacket));
 
  //oss1 << "/NodeList/" << ueNodes.Get (1)->GetId () << "/ApplicationList/0/$ns3::PacketSink/Rx";
  //Config::ConnectWithoutContext (oss1.str (), MakeCallback (&ReceivePacket));

  //oss2 << "/NodeList/" << ueNodes.Get (2)->GetId () << "/ApplicationList/0/$ns3::PacketSink/Rx";
  //Config::ConnectWithoutContext (oss2.str (), MakeCallback (&ReceivePacket));

  oss3 << "/NodeList/" << ueNodes.Get (3)->GetId () << "/ApplicationList/0/$ns3::PacketSink/Rx";
  Config::ConnectWithoutContext (oss3.str (), MakeCallback (&ReceivePacket));

 // oss4 << "/NodeList/" << ueNodes.Get (4)->GetId () << "/ApplicationList/0/$ns3::PacketSink/Rx";
 // Config::ConnectWithoutContext (oss4.str (), MakeCallback (&ReceivePacket));

  //oss5 << "/NodeList/" << ueNodes.Get (5)->GetId () << "/ApplicationList/0/$ns3::PacketSink/Rx";
  //Config::ConnectWithoutContext (oss5.str (), MakeCallback (&ReceivePacket));

  //oss6 << "/NodeList/" << ueNodes.Get (6)->GetId () << "/ApplicationList/0/$ns3::PacketSink/Rx";
  //Config::ConnectWithoutContext (oss6.str (), MakeCallback (&ReceivePacket));

  //oss7 << "/NodeList/" << ueNodes.Get (7)->GetId () << "/ApplicationList/0/$ns3::PacketSink/Rx";
  //Config::ConnectWithoutContext (oss7.str (), MakeCallback (&ReceivePacket));

  //oss8 << "/NodeList/" << ueNodes.Get (8)->GetId () << "/ApplicationList/0/$ns3::PacketSink/Rx";
  //Config::ConnectWithoutContext (oss8.str (), MakeCallback (&ReceivePacket));

  //oss9 << "/NodeList/" << ueNodes.Get (9)->GetId () << "/ApplicationList/0/$ns3::PacketSink/Rx";
  //Config::ConnectWithoutContext (oss9.str (), MakeCallback (&ReceivePacket));

  //oss10 << "/NodeList/" << ueNodes.Get (10)->GetId () << "/ApplicationList/0/$ns3::PacketSink/Rx";
  //Config::ConnectWithoutContext (oss10.str (), MakeCallback (&ReceivePacket));

  //oss11 << "/NodeList/" << ueNodes.Get (11)->GetId () << "/ApplicationList/0/$ns3::PacketSink/Rx";
  //Config::ConnectWithoutContext (oss11.str (), MakeCallback (&ReceivePacket));

  //oss12 << "/NodeList/" << ueNodes.Get (12)->GetId () << "/ApplicationList/0/$ns3::PacketSink/Rx";
  //Config::ConnectWithoutContext (oss12.str (), MakeCallback (&ReceivePacket));

  //oss13 << "/NodeList/" << ueNodes.Get (13)->GetId () << "/ApplicationList/0/$ns3::PacketSink/Rx";
  //Config::ConnectWithoutContext (oss13.str (), MakeCallback (&ReceivePacket));

  //oss14 << "/NodeList/" << ueNodes.Get (14)->GetId () << "/ApplicationList/0/$ns3::PacketSink/Rx";
  //Config::ConnectWithoutContext (oss14.str (), MakeCallback (&ReceivePacket));

  //oss15 << "/NodeList/" << ueNodes.Get (15)->GetId () << "/ApplicationList/0/$ns3::PacketSink/Rx";
  //Config::ConnectWithoutContext (oss15.str (), MakeCallback (&ReceivePacket));

  //oss16 << "/NodeList/" << ueNodes.Get (16)->GetId () << "/ApplicationList/0/$ns3::PacketSink/Rx";
  //Config::ConnectWithoutContext (oss16.str (), MakeCallback (&ReceivePacket));

  //oss17 << "/NodeList/" << ueNodes.Get (17)->GetId () << "/ApplicationList/0/$ns3::PacketSink/Rx";
  //Config::ConnectWithoutContext (oss17.str (), MakeCallback (&ReceivePacket));

  //oss18 << "/NodeList/" << ueNodes.Get (18)->GetId () << "/ApplicationList/0/$ns3::PacketSink/Rx";
  //Config::ConnectWithoutContext (oss18.str (), MakeCallback (&ReceivePacket));

  //oss19 << "/NodeList/" << ueNodes.Get (19)->GetId () << "/ApplicationList/0/$ns3::PacketSink/Rx";
  //Config::ConnectWithoutContext (oss19.str (), MakeCallback (&ReceivePacket));

  //oss20 << "/NodeList/" << ueNodes.Get (20)->GetId () << "/ApplicationList/0/$ns3::PacketSink/Rx";
  //Config::ConnectWithoutContext (oss20.str (), MakeCallback (&ReceivePacket));

  //oss21 << "/NodeList/" << ueNodes.Get (21)->GetId () << "/ApplicationList/0/$ns3::PacketSink/Rx";
  //Config::ConnectWithoutContext (oss21.str (), MakeCallback (&ReceivePacket));

  //oss22 << "/NodeList/" << ueNodes.Get (22)->GetId () << "/ApplicationList/0/$ns3::PacketSink/Rx";
  //Config::ConnectWithoutContext (oss22.str (), MakeCallback (&ReceivePacket));

  //oss23 << "/NodeList/" << ueNodes.Get (23)->GetId () << "/ApplicationList/0/$ns3::PacketSink/Rx";
  //Config::ConnectWithoutContext (oss23.str (), MakeCallback (&ReceivePacket));

  //oss24 << "/NodeList/" << ueNodes.Get (24)->GetId () << "/ApplicationList/0/$ns3::PacketSink/Rx";
  //Config::ConnectWithoutContext (oss24.str (), MakeCallback (&ReceivePacket));


bool firstWrite = true;
   std::string rrcType = useIdealRrc == 1 ? "ideal_rrc":"real_rrc";
   std::string fileName = "rlf_dl_thrput_" + std::to_string (enbNodes.GetN ()) + "_eNB_" + rrcType;
   //Time binSize = Seconds (0.2);
 //Simulator::Schedule (Seconds(0.2), &Throughput, firstWrite, binSize, fileName);
 Time binSize = Seconds (1);
 Simulator::Schedule (Seconds(1), &Throughput, firstWrite, binSize, fileName);

//---------------------------------------------
   AnimationInterface anim ("test14.xml");
   anim.SetMaxPktsPerTraceFile(10000000); // abhishek
//---------------------------------------------

//flow monitor
Ptr<FlowMonitor> flowmon;
FlowMonitorHelper flowmonHelper;
flowmon = flowmonHelper.InstallAll ();

//---------------------------------------------

Simulator::Stop (Seconds (5));
Simulator::Run ();


//flow monitor
flowmon->SerializeToXmlFile ("test14.flowmon", true, true);


//---------------------------------------------

Simulator::Destroy ();
return 0;
}

//---------------------------------------------
  
