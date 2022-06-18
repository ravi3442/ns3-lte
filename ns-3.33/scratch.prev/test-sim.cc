
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
#include "ns3/lte-enb-mac.h"

//Added for flow monitor
#include "ns3/flow-monitor.h"
#include "ns3/flow-monitor-helper.h"

//---------------------------------------------
using namespace ns3;


NS_LOG_COMPONENT_DEFINE ("test-sim");

 uint32_t ByteCounterEmbb = 0;
 uint32_t oldByteCounterEmbb= 0;

uint32_t ByteCounterUrllc = 0;
 uint32_t oldByteCounterUrllc= 0;

 uint16_t embbAllocation;
 uint16_t urllcAllocation;

 uint16_t embbHolDelay;
 uint16_t urllcHolDelay;

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
 

void NotifyRbgAllocation (std::string context,uint16_t _embbAllocation,  uint16_t _urllcAllocation, uint16_t _embbHolDelay, uint16_t _urllcHolDelay)
{
  /*std::cout << " Embb and Urllc Rbg Allocation "
            << embbAllocation << " " << urllcAllocation
            << std::endl;*/

  embbAllocation = _embbAllocation;
  urllcAllocation = _urllcAllocation;
  embbHolDelay= _embbHolDelay;
  urllcHolDelay=_urllcHolDelay;

}


void dlScheduleInfo(std::string path, struct DlSchedulingCallbackInfo temp){
       return;
     }

 /* void dlScheduleInfo(struct DlSchedulingCallbackInfo temp){
       return;
     }   */

//---------------------------------# Added for Throughput Calculation

 void
 ReceivePacketEmbb (Ptr<const Packet> packet, const Address &)
 {
   ByteCounterEmbb += packet->GetSize ();
  // std::cout << Simulator::Now() << "  Packet Size " << packet->GetSize () << std::endl;
 }

void
 ReceivePacketUrllc (Ptr<const Packet> packet, const Address &)
 {
   ByteCounterUrllc += packet->GetSize ();
   //std::cout << Simulator::Now() << "  Packet Size " << packet->GetSize () << std::endl;
 }


void writeRbgAllocation(bool firstWrite, Time binSize, std::string fileName){

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

     output << Simulator::Now().GetSeconds() << " " << embbAllocation << " " <<urllcAllocation<<" "<<embbHolDelay<<" "<<urllcHolDelay<<std::endl;
     Simulator::Schedule (binSize, &writeRbgAllocation, firstWrite, binSize, fileName);

     std::cout<<"Embb/URLLC Allocation "<<embbAllocation<<" " <<urllcAllocation<<std::endl;
     std::cout<<"Embb/URLLC Hol Delay "<<embbHolDelay<<" " <<urllcHolDelay<<std::endl;
}

 void
 ThroughputEmbb(bool firstWrite, Time binSize, std::string fileName)
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
   double  throughput = (ByteCounterEmbb - oldByteCounterEmbb)*8/binSize.GetSeconds ()/1024/1024;
   //double  throughput = (ByteCounter)*8/Simulator::Now().GetSeconds ()/1024/1024;
   //double  throughput = (ByteCounter);
   output << Simulator::Now().GetSeconds() << " " << throughput << std::endl;
   oldByteCounterEmbb = ByteCounterEmbb;
   Simulator::Schedule (binSize, &ThroughputEmbb, firstWrite, binSize, fileName);
   std::cout<<"Throughput Embb "<<throughput<<std::endl;
 }

 void
 ThroughputUrllc(bool firstWrite, Time binSize, std::string fileName)
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
   double  throughput = (ByteCounterUrllc - oldByteCounterUrllc)*8/binSize.GetSeconds ()/1024/1024;
   //double  throughput = (ByteCounter)*8/Simulator::Now().GetSeconds ()/1024/1024;
   //double  throughput = (ByteCounter);
   output << Simulator::Now().GetSeconds() << " " << throughput << std::endl;
   oldByteCounterUrllc = ByteCounterUrllc;
   Simulator::Schedule (binSize, &ThroughputUrllc, firstWrite, binSize, fileName);
   std::cout<<"Throughput Urllc "<<throughput<<std::endl;
 }

//---------------------------------------------------------------------------


static void 
CourseChange (std::string foo, Ptr<const MobilityModel> mobility)
{
  //Vector pos = mobility->GetPosition ();
  //Vector vel = mobility->GetVelocity ();
  //std::cout << Simulator::Now () << ", model=" << mobility << ", POS: x=" << pos.x << ", y=" << pos.y
    //        << ", z=" << pos.z << "; VEL:" << vel.x << ", y=" << vel.y
      //      << ", z=" << vel.z << std::endl;
}

using namespace ns3;

int main (int argc, char *argv[])
{

LogLevel logLevel = (LogLevel)(LOG_PREFIX_FUNC | LOG_PREFIX_TIME | LOG_LEVEL_ALL);
//LtePdcp LteRlc LteRlcUm lte-amc lte-enb-phy PfFfMacUrllcScheduler

/*LogComponentEnable ("PfFfMacScheduler", logLevel);
LogComponentEnable ("PfFfMacUrllcScheduler", logLevel);
 LogComponentEnable ("LteEnbMac", logLevel);
 LogComponentEnable ("LtePdcp", logLevel);
 LogComponentEnable ("LteRlc", logLevel);
 LogComponentEnable ("LteRlcUm", logLevel);
 LogComponentEnable ("NoOpComponentCarrierManager", logLevel);
 //LogComponentEnable ("LteAmc", logLevel);
 LogComponentEnable ("LteEnbPhy", logLevel);
 LogComponentEnable("FfMacScheduler", logLevel);*/

//LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);

uint16_t numberOfUes = 2;
uint16_t numberOfEnbs = 1;
//uint16_t distance = 1000;
uint16_t numBearersPerUe = 1;
double enbTxPowerDbm = 45.0;
//uint16_t c = 0;

//-----------------------
bool useIdealRrc = true;
//-----------------------
   Config::SetDefault ("ns3::UdpClient::Interval", TimeValue (MicroSeconds (100))); //128 Kbps make it 64000
   Config::SetDefault ("ns3::UdpClient::MaxPackets", UintegerValue (100000));
   Config::SetDefault ("ns3::UdpClient::PacketSize", UintegerValue (1024));
   Config::SetDefault ("ns3::LteHelper::UseIdealRrc", BooleanValue (true));

  Config::SetDefault ("ns3::RandomWalk2dMobilityModel::Mode", StringValue ("Time"));
  Config::SetDefault ("ns3::RandomWalk2dMobilityModel::Time", StringValue ("2s"));
  Config::SetDefault ("ns3::RandomWalk2dMobilityModel::Speed", StringValue ("ns3::ConstantRandomVariable[Constant=1.0]"));
  Config::SetDefault ("ns3::RandomWalk2dMobilityModel::Bounds", StringValue ("0|200|0|200"));

  Config::SetDefault ("ns3::LteEnbMac::PreambleTransMax",UintegerValue(100));//abhishek
 

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

/* Another schedular for handling URLLC flows*/
lteHelper->SetUrllcSchedulerType("ns3::PfFfMacUrllcScheduler");
Config::SetDefault ("ns3::PfFfMacScheduler::HarqEnabled", BooleanValue (false));

//disable HARQ
Config::SetDefault ("ns3::PfFfMacUrllcScheduler::HarqEnabled", BooleanValue (false));

lteHelper->SetHandoverAlgorithmType ("ns3::A3RsrpHandoverAlgorithm");
lteHelper->SetHandoverAlgorithmAttribute ("Hysteresis",DoubleValue (3.0));
lteHelper->SetHandoverAlgorithmAttribute ("TimeToTrigger",TimeValue (MilliSeconds (256)));

//bandwidth Configuration
uint16_t bandwidth =25 ;
lteHelper->SetEnbDeviceAttribute ("DlBandwidth", UintegerValue (bandwidth));
lteHelper->SetEnbDeviceAttribute ("UlBandwidth", UintegerValue (bandwidth));

Ptr<UniformRandomVariable> coordinates = CreateObject<UniformRandomVariable> ();
  coordinates->SetAttribute ("Min", DoubleValue (0));
  coordinates->SetAttribute ("Max", DoubleValue (500));

//-----------------------------------------------
Ptr<Node> pgw = epcHelper->GetPgwNode ();
Ptr<Node> sgw= epcHelper->GetSgwNode();
//Ptr<Node> mme= lteHelper->Get

std::cout<<"PGW id is "<<pgw->GetId()<<std::endl;
std::cout<<"SGw id is "<<sgw->GetId()<<std::endl;
//std::cout<<"Mme id is "<<mme->GetId()<<std::endl;



  // Create a single RemoteHost
  NodeContainer remoteHostContainer;
  remoteHostContainer.Create (1);
  Ptr<Node> remoteHost = remoteHostContainer.Get (0);

  InternetStackHelper internet;
  internet.Install (remoteHostContainer);

std::cout<<"Remote host id is "<<remoteHostContainer.Get (0)->GetId()<<std::endl;


//-----------------------------------------------


  // Create the links
  PointToPointHelper p2ph;
  p2ph.SetDeviceAttribute ("DataRate", DataRateValue (DataRate ("100Gb/s")));
  p2ph.SetDeviceAttribute ("Mtu", UintegerValue (1500));
  p2ph.SetChannelAttribute ("Delay", TimeValue (Seconds (0.010)));
  NetDeviceContainer internetDevices = p2ph.Install (pgw, remoteHost);
  Ipv4AddressHelper ipv4h;
  ipv4h.SetBase ("1.0.0.0", "255.0.0.0");
  Ipv4InterfaceContainer internetIpIfaces = ipv4h.Assign (internetDevices);
  //Ipv4Address remoteHostAddr = internetIpIfaces.GetAddress (1); 

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
 // for (uint16_t i = 0; i < numberOfEnbs/2; i++)
   // {
     // for (uint16_t j = 0; j < numberOfEnbs/2; j++)
       // {
         // Vector enbPosition (distance * (j + 1), distance * (i + 1), 0);
         Vector enbPosition (1000, 1000, 0);
         enbPositionAlloc->Add (enbPosition); 

        //  std::cout<<"ENodeB ID is : "<< enbNodes.GetN()<<std::endl;

        //}
     
    //}
  MobilityHelper enbMobility;
  enbMobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  enbMobility.SetPositionAllocator (enbPositionAlloc);
  enbMobility.Install (enbNodes);


//-----------------------------------------------
// Install Mobility Model in mobile UE
  MobilityHelper mobility;
  mobility.SetMobilityModel ("ns3::ConstantVelocityMobilityModel");
 /* mobility.SetPositionAllocator ("ns3::RandomDiscPositionAllocator",
                                 "X", StringValue ("1500.0"),
                                 "Y", StringValue ("1500.0"),
                                 "Rho", StringValue ("ns3::ConstantRandomVariable[Constant=10.0]"));
  mobility.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
                             "Mode", StringValue ("Time"),
                             "Time", StringValue ("500ms"),
                             "Speed", StringValue ("ns3::ConstantRandomVariable[Constant=10.0]"),
                             "Bounds", StringValue ("0|2000|0|2000"));*/

for (uint16_t i = 0; i < numberOfEnbs; i++)
    {
    //  NS_LOG_INFO("EnodeB Node ID"<<enbNodes.Get(i)->GetId());
     std::cout<<"EnodeB Node ID "<<enbNodes.Get(i)->GetId()<<std::endl;
    //  c = i ; 
    }

 for (uint16_t i = 0; i < numberOfUes; i++)
    {
      mobility.Install (ueNodes.Get(i));
      ueNodes.Get(i)->GetObject<MobilityModel> ()->SetPosition (Vector (1000+ coordinates->GetValue(),1000+coordinates->GetValue(),0));
      std::cout<<"UE Node ID "<<ueNodes.Get(i)->GetId()<<std::endl;
    //  c = i ; 
    }

/*for (uint16_t i = numberOfUes/2; i < numberOfUes; i++)
    {
      mobility.Install (ueNodes.Get(i));
      ueNodes.Get(i)->GetObject<MobilityModel> ()->SetPosition (Vector (10000+ coordinates->GetValue(),10000+coordinates->GetValue(),0));
      std::cout<<"UE Node ID "<<ueNodes.Get(i)->GetId()<< std::endl;
    //  c = i ; 
    }*/
   

   mobility.Install(pgw);
   pgw->GetObject<MobilityModel> ()->SetPosition (Vector (500+ coordinates->GetValue(),500+coordinates->GetValue(),0));

   mobility.Install(sgw);
   sgw->GetObject<MobilityModel> ()->SetPosition (Vector (500+ coordinates->GetValue(),500+coordinates->GetValue(),0));

   mobility.Install(remoteHost);
   remoteHost->GetObject<MobilityModel> ()->SetPosition (Vector (500+ coordinates->GetValue(),500+coordinates->GetValue(),0));




//--------------------------------------------------

 // Install LTE Devices in eNB and UEs
  Config::SetDefault ("ns3::LteEnbPhy::TxPower", DoubleValue (enbTxPowerDbm));
  NetDeviceContainer enbLteDevs = lteHelper->InstallEnbDevice (enbNodes);
  NetDeviceContainer ueLteDevs = lteHelper->InstallUeDevice (ueNodes);

   for (NodeList::Iterator it = NodeList::Begin (); it != NodeList::End (); ++it)
     {
       Ptr<Node> node = *it;
       int nDevs = node->GetNDevices ();
       for (int j = 0; j < nDevs; j++)
         {
           Ptr<LteUeNetDevice> uedev = node->GetDevice (j)->GetObject <LteUeNetDevice> ();
           if (uedev)
             {
               //Vector pos = node->GetObject<MobilityModel> ()->GetPosition ();
               std::cout << "Node Id "<<node->GetId()<< " IMSI " << uedev->GetImsi () << std::endl;
             }
         }
     }


//--------------------------------------------------

// Install the IP stack on the UEs
  internet.Install (ueNodes);
  Ipv4InterfaceContainer ueIpIfaces;
  ueIpIfaces = epcHelper->AssignUeIpv4Address (NetDeviceContainer (ueLteDevs));

//--------------------------------------------------

// Attach all UEs to the eNodeB
lteHelper->AttachToClosestEnb (ueLteDevs, enbLteDevs);
 // for (uint16_t i = 0; i < numberOfUes; i++)
   // {
      // lteHelper->Attach (ueLteDevs.Get (i), enbLteDevs.Get (0));
    //}

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
          PacketSinkHelper dlPacketSinkHelper ("ns3::UdpSocketFactory",InetSocketAddress (Ipv4Address::GetAny (), dlPort));
          serverApps.Add (dlPacketSinkHelper.Install (ue));

          /*NS_LOG_LOGIC ("installing UDP UL app for UE " << u);
          UdpClientHelper ulClientHelper (remoteHostAddr, ulPort);
          clientApps.Add (ulClientHelper.Install (ue));
          PacketSinkHelper ulPacketSinkHelper ("ns3::UdpSocketFactory",
                                               InetSocketAddress (Ipv4Address::GetAny (), ulPort));
          serverApps.Add (ulPacketSinkHelper.Install (remoteHost));*/

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
  lteHelper->EnableDlMacTraces();

  Ptr<RadioBearerStatsCalculator> rlcStats = lteHelper->GetRlcStats ();
  rlcStats->SetAttribute ("EpochDuration", TimeValue (Seconds (1.0)));
  Ptr<RadioBearerStatsCalculator> pdcpStats = lteHelper->GetPdcpStats ();
  pdcpStats->SetAttribute ("EpochDuration", TimeValue (Seconds (1.0)));

 // connect custom trace sinks for RRC connection establishment and handover notification
  Config::Connect ("/NodeList/4/DeviceList/0/LteEnbRrc/ConnectionEstablished",
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
  std::ostringstream oss0, oss1;
  //std::ostringstream oss0, oss1;
   //std::ostringstream oss0,oss1,oss2,oss3,oss4,oss5,oss6,oss7,oss8,oss9,oss10;
  //std::ostringstream oss0,
    //                  oss1,oss2,oss3,oss4,oss5,oss6,oss7,oss8,oss9,oss10,oss11,oss12,oss13,oss14,oss15,oss16,
      //                oss17,oss18,oss19,oss20,oss21,oss22,oss23,oss24;
     //std::ostringstream oss17,oss18,oss19,oss20,oss21,oss22,oss23,oss24;

for (uint32_t u = 0; u < numberOfUes; ++u)
    {
      Ptr<Node> ue = ueNodes.Get (u);

      if (ueNodes.Get (u)->GetId () == 5 ){

        oss1 << "/NodeList/" << ueNodes.Get (u)->GetId () << "/ApplicationList/0/$ns3::PacketSink/Rx";
        Config::ConnectWithoutContext (oss1.str (), MakeCallback (&ReceivePacketEmbb));
      }

      if (ueNodes.Get (u)->GetId () == 6 ){

        oss0 << "/NodeList/" << ueNodes.Get (u)->GetId () << "/ApplicationList/0/$ns3::PacketSink/Rx";
        Config::ConnectWithoutContext (oss0.str (), MakeCallback (&ReceivePacketUrllc));
      }

      

    }

//Ravi connect to the trace source for rbg allocation in the mac
oss1.str("");
oss1 << "/NodeList/" << enbNodes.Get(0)->GetId ()<<"/DeviceList/*" <<"/ComponentCarrierMap/*"<< "/LteEnbMac/RbgAllocation"; 
Config::Connect ("/NodeList/*/DeviceList/*/ComponentCarrierMap/*/LteEnbMac/DlScheduling",MakeCallback (&dlScheduleInfo));
Config::Connect (oss1.str(),MakeCallback (&NotifyRbgAllocation));
/*NetDeviceContainer::Iterator it;
for (it = enbNodes.begin(); it != enbNodes.end(); it++)
{
  Ptr<NetDevice> dev = *it;
  Ptr<LteEnbNetDevice> enbDev = dev->GetObject<LteEnbNetDevice> ();
  Ptr<LteEnbMac> enbMac = enbDev->GetMac()

 // uint8_t measId = enbRrc->AddUeMeasReportConfig (config);
  //measIdList.push_back (measId); // remember the measId created

  enbMac->TraceConnect ("RbgAllocation",
                        "context",
                        MakeCallback (&NotifyRbgAllocation));
}*/
 
  
bool firstWrite = true;
std::string rrcType = useIdealRrc == 1 ? "ideal_rrc":"real_rrc";
std::string fileNameEmbb = "rlf_dl_thrput_" + std::to_string (enbNodes.GetN ()) + "_eNB_" + "EMBB" + "5"+ rrcType;
std::string fileNameUrllc = "rlf_dl_thrput_" + std::to_string (enbNodes.GetN ()) + "_eNB_" + "URLLC" + "6"+ rrcType;

std::string rbgAllocfile = "rbgfile" + std::to_string (enbNodes.GetN ()) + "_eNB_" + "URLLC" + std::to_string(4)+ rrcType;

//std::string rbgAllocfile = "rbgfile_" + "Alloc" + "4"+ rrcType;

Time binSize = Seconds (0.01);
Time binSizeRbg = Seconds (0.01);

Simulator::Schedule (Seconds(0.1), &ThroughputEmbb, firstWrite, binSize, fileNameEmbb);
Simulator::Schedule (Seconds(0.1), &ThroughputUrllc, firstWrite, binSize, fileNameUrllc);
Simulator::Schedule (Seconds(0.1), &writeRbgAllocation, firstWrite, binSizeRbg, rbgAllocfile);

//---------------------------------------------
   AnimationInterface anim ("test-sim.xml");
   anim.SetMaxPktsPerTraceFile(10000000); // abhishek
//---------------------------------------------

//flow monitor
Ptr<FlowMonitor> flowmon;
FlowMonitorHelper flowmonHelper;
flowmon = flowmonHelper.InstallAll ();

//---------------------------------------------

Simulator::Stop (Seconds (2));
Simulator::Run ();


//flow monitor
//flowmon->SerializeToXmlFile ("test10.flowmon", true, true);


//---------------------------------------------

Simulator::Destroy ();
return 0;
}

//---------------------------------------------