/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
 /*
  * Copyright (c) 2011-2018 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
  *
  * This program is free software; you can redistribute it and/or modify
  * it under the terms of the GNU General Public License version 2 as
  * published by the Free Software Foundation;
  *
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with this program; if not, write to the Free Software
  * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
  *
  * Authors: Jaume Nin <jaume.nin@cttc.cat>
  *          Manuel Requena <manuel.requena@cttc.es>
  */
  
 #include "ns3/core-module.h"
 #include "ns3/point-to-point-module.h"
 #include "ns3/internet-module.h"
 #include "ns3/applications-module.h"
 #include "ns3/mobility-module.h"
 #include "ns3/config-store-module.h"
 #include "ns3/lte-module.h"
 //#include "ns3/gtk-config-store.h"
 #include "ns3/netanim-module.h"

 //Added for flow monitor
#include "ns3/flow-monitor.h"
#include "ns3/flow-monitor-helper.h"
  
 using namespace ns3;
  
 //NS_LOG_COMPONENT_DEFINE ("LenaSimpleEpc");



 NS_LOG_COMPONENT_DEFINE ("test-http-2");

 uint32_t ByteCounterEmbb = 0;
 uint32_t oldByteCounterEmbb= 0;

uint32_t ByteCounterUrllc = 0;
 uint32_t oldByteCounterUrllc= 0;

 uint16_t embbAllocation;
 uint16_t urllcAllocation;

 uint16_t embbHolDelay;
 uint16_t urllcHolDelay;

 void
 ServerConnectionEstablished (Ptr<const ThreeGppHttpServer>, Ptr<Socket>)
 {
   NS_LOG_INFO ("Client has established a connection to the server.");
 }
  
 void
 MainObjectGenerated (uint32_t size)
 {
   NS_LOG_INFO ("Server generated a main object of " << size << " bytes.");
 }
  
 void
 EmbeddedObjectGenerated (uint32_t size)
 {
   NS_LOG_INFO ("Server generated an embedded object of " << size << " bytes.");
 }
  
 void
 ServerTx (Ptr<const Packet> packet)
 {
   NS_LOG_INFO ("Server sent a packet of " << packet->GetSize () << " bytes.");
 }
  
 void
 ClientRx (Ptr<const Packet> packet, const Address &address)
 {
   NS_LOG_INFO ("Client received a packet of " << packet->GetSize () << " bytes from " << address);
 }


void
 ClientMainObjectReceived (Ptr<const ThreeGppHttpClient>, Ptr<const Packet> packet)
 {
   Ptr<Packet> p = packet->Copy ();
   ThreeGppHttpHeader header;
   p->RemoveHeader (header);
   if (header.GetContentLength () == p->GetSize ()
       && header.GetContentType () == ThreeGppHttpHeader::MAIN_OBJECT)
     {
       NS_LOG_INFO ("Client has successfully received a main object of "
                    << p->GetSize () << " bytes.");
     }
   else
     {
       NS_LOG_INFO ("Client failed to parse a main object. ");
     }
 }
  
 void
 ClientEmbeddedObjectReceived (Ptr<const ThreeGppHttpClient>, Ptr<const Packet> packet)
 {
   Ptr<Packet> p = packet->Copy ();
   ThreeGppHttpHeader header;
   p->RemoveHeader (header);
   if (header.GetContentLength () == p->GetSize ()
       && header.GetContentType () == ThreeGppHttpHeader::EMBEDDED_OBJECT)
     {
       NS_LOG_INFO ("Client has successfully received an embedded object of "
                    << p->GetSize () << " bytes.");
     }
   else
     {
       NS_LOG_INFO ("Client failed to parse an embedded object. ");
     }
 }

/*void ReceivePacketUrllc (Ptr<const Packet> packet, const Address &)
 {
  // ByteCounterUrllc += packet->GetSize ();
  std::cout << Simulator::Now() << "  Packet Size " << packet->GetSize () << std::endl;
 } */

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

/*void ReceivePdcpPacket(uint16_t m_rnti, uint8_t lcid, uint32_t packetSize, uint64_t delay){
  
  std::cout<<" PDCP Rnti "<<m_rnti<<" lcid "<<lcid<< " Packet Size "<<packetSize<< "Delay "<<delay<<std::endl;

}*/

void
ReceivePdcpPacket (std::string path, uint16_t rnti, uint8_t lcid, uint32_t packetSize, uint64_t delay)
{
  NS_LOG_INFO (path << rnti << (uint16_t)lcid << packetSize << delay);
  //arg->stats->DlRxPdu (arg->cellId, arg->imsi, rnti, lcid, packetSize, delay);
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


  
 int
 main (int argc, char *argv[])
 {
   uint16_t numNodePairs = 2;
   Time simTime = MilliSeconds (11000);
   double distance = 60.0;
   Time interPacketInterval = MilliSeconds (100);
   bool useCa = false;
   bool disableDl = false;
   bool disableUl = false;
   bool disablePl = false;
  

   LogLevel logLevel = (LogLevel)(LOG_PREFIX_FUNC | LOG_PREFIX_TIME | LOG_LEVEL_ALL);

   //test-http-2
  LogComponentEnable ("test-http-2", logLevel);

/*
  LogComponentEnable ("PfFfMacScheduler", logLevel);
  LogComponentEnable ("PfFfMacUrllcScheduler", logLevel);
  LogComponentEnable ("LteEnbMac", logLevel);
  LogComponentEnable ("LtePdcp", logLevel);
  LogComponentEnable ("LteRlc", logLevel);
  LogComponentEnable ("LteRlcUm", logLevel);
  LogComponentEnable ("NoOpComponentCarrierManager", logLevel);*/
  //LogComponentEnable ("LteAmc", logLevel);
  /*LogComponentEnable ("LteEnbPhy", logLevel);
  LogComponentEnable("FfMacScheduler", logLevel);*/

   // Command line arguments
   CommandLine cmd (__FILE__);
   cmd.AddValue ("numNodePairs", "Number of eNodeBs + UE pairs", numNodePairs);
   cmd.AddValue ("simTime", "Total duration of the simulation", simTime);
   cmd.AddValue ("distance", "Distance between eNBs [m]", distance);
   cmd.AddValue ("interPacketInterval", "Inter packet interval", interPacketInterval);
   cmd.AddValue ("useCa", "Whether to use carrier aggregation.", useCa);
   cmd.AddValue ("disableDl", "Disable downlink data flows", disableDl);
   cmd.AddValue ("disableUl", "Disable uplink data flows", disableUl);
   cmd.AddValue ("disablePl", "Disable data flows between peer UEs", disablePl);
   cmd.Parse (argc, argv);
  
   ConfigStore inputConfig;
   inputConfig.ConfigureDefaults ();
  
   // parse again so you can override default values from the command line
   cmd.Parse(argc, argv);
  
   if (useCa)
    {
      Config::SetDefault ("ns3::LteHelper::UseCa", BooleanValue (useCa));
      Config::SetDefault ("ns3::LteHelper::NumberOfComponentCarriers", UintegerValue (2));
      Config::SetDefault ("ns3::LteHelper::EnbComponentCarrierManager", StringValue ("ns3::RrComponentCarrierManager"));
    }
  
   Ptr<LteHelper> lteHelper = CreateObject<LteHelper> ();
   Ptr<PointToPointEpcHelper> epcHelper = CreateObject<PointToPointEpcHelper> ();
   lteHelper->SetEpcHelper (epcHelper);
  
   Ptr<Node> pgw = epcHelper->GetPgwNode ();
  
    // Create a single RemoteHost
   NodeContainer remoteHostContainer;
   remoteHostContainer.Create (1);
   Ptr<Node> remoteHost = remoteHostContainer.Get (0);
   InternetStackHelper internet;
   internet.Install (remoteHostContainer);


   //create a http address
   NodeContainer remoteHttpHostContainer;
   remoteHttpHostContainer.Create (1);
   Ptr<Node> remoteHttpHost = remoteHttpHostContainer.Get (0);
   internet.Install (remoteHttpHostContainer);
   std::cout<<"Http Remote host id is "<<remoteHttpHostContainer.Get (0)->GetId()<<std::endl;
  
   // Create the Internet
   PointToPointHelper p2ph;
   p2ph.SetDeviceAttribute ("DataRate", DataRateValue (DataRate ("100Gb/s")));
   p2ph.SetDeviceAttribute ("Mtu", UintegerValue (1500));
   p2ph.SetChannelAttribute ("Delay", TimeValue (MilliSeconds (10)));
   NetDeviceContainer internetDevices = p2ph.Install (pgw, remoteHost);
   Ipv4AddressHelper ipv4h;
   ipv4h.SetBase ("1.0.0.0", "255.0.0.0");
   Ipv4InterfaceContainer internetIpIfaces = ipv4h.Assign (internetDevices);
   // interface 0 is localhost, 1 is the p2p device
   Ipv4Address remoteHostAddr = internetIpIfaces.GetAddress (1);
  
   Ipv4StaticRoutingHelper ipv4RoutingHelper;
   Ptr<Ipv4StaticRouting> remoteHostStaticRouting = ipv4RoutingHelper.GetStaticRouting (remoteHost->GetObject<Ipv4> ());

   remoteHostStaticRouting->AddNetworkRouteTo (Ipv4Address ("7.0.0.0"), Ipv4Mask ("255.0.0.0"), 1);


   //create link between PGw and the Http server
   //create link between Pgw and the remoteHttpHost
  NetDeviceContainer internetHttpDevices = p2ph.Install (pgw, remoteHttpHost);
  Ipv4AddressHelper ipv4hHttp;
  ipv4hHttp.SetBase ("2.0.0.0", "255.0.0.0");
  Ipv4InterfaceContainer internetIpHttpIfaces = ipv4hHttp.Assign (internetHttpDevices);
  Ipv4Address remoteHostHttpAddr = internetIpHttpIfaces.GetAddress (1);
  // Routing of the Internet Host (towards the LTE network)
  Ipv4StaticRoutingHelper ipv4RoutingHelper2;
  Ptr<Ipv4StaticRouting> remoteHttpHostStaticRouting = ipv4RoutingHelper2.GetStaticRouting (remoteHttpHost->GetObject<Ipv4> ());
  // interface 0 is localhost, 1 is the p2p device
  remoteHttpHostStaticRouting->AddNetworkRouteTo (Ipv4Address ("7.0.0.0"), Ipv4Mask ("255.0.0.0"), 1);


  NS_LOG_INFO("Remote HTTP Ip Add: ");
  remoteHostHttpAddr.Print(std::cout);
  std::cout<<"\n";

  
   NodeContainer ueNodes;
   NodeContainer enbNodes;
   enbNodes.Create (numNodePairs);
   ueNodes.Create (numNodePairs);
  
   // Install Mobility Model
   Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
   for (uint16_t i = 0; i < numNodePairs; i++)
     {
       positionAlloc->Add (Vector (distance * i, 0, 0));
     }
   MobilityHelper mobility;
   mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
   mobility.SetPositionAllocator(positionAlloc);
   mobility.Install(enbNodes);
   mobility.Install(ueNodes);
  
   // Install LTE Devices to the nodes
   NetDeviceContainer enbLteDevs = lteHelper->InstallEnbDevice (enbNodes);
   NetDeviceContainer ueLteDevs = lteHelper->InstallUeDevice (ueNodes);
  
   // Install the IP stack on the UEs
   internet.Install (ueNodes);
   Ipv4InterfaceContainer ueIpIface;
   ueIpIface = epcHelper->AssignUeIpv4Address (NetDeviceContainer (ueLteDevs));
   // Assign IP address to UEs, and install applications
   for (uint32_t u = 0; u < ueNodes.GetN (); ++u)
     {
       Ptr<Node> ueNode = ueNodes.Get (u);
       // Set the default gateway for the UE
       Ptr<Ipv4StaticRouting> ueStaticRouting = ipv4RoutingHelper.GetStaticRouting (ueNode->GetObject<Ipv4> ());
       ueStaticRouting->SetDefaultRoute (epcHelper->GetUeDefaultGatewayAddress (), 1);
     }

// Install HTTP server
ThreeGppHttpServerHelper serverHelper (remoteHostHttpAddr);
ApplicationContainer serverAppsHttp = serverHelper.Install (remoteHttpHost);
Ptr<ThreeGppHttpServer> httpServer = serverAppsHttp.Get (0)->GetObject<ThreeGppHttpServer> ();

// Example of connecting to the trace sources
httpServer->TraceConnectWithoutContext ("ConnectionEstablished",
                                        MakeCallback (&ServerConnectionEstablished));
httpServer->TraceConnectWithoutContext ("MainObject", MakeCallback (&MainObjectGenerated));
httpServer->TraceConnectWithoutContext ("EmbeddedObject", MakeCallback (&EmbeddedObjectGenerated));
httpServer->TraceConnectWithoutContext ("Tx", MakeCallback (&ServerTx));

Ptr<UniformRandomVariable> startTimeSeconds = CreateObject<UniformRandomVariable> ();
startTimeSeconds->SetAttribute ("Min", DoubleValue (0));
startTimeSeconds->SetAttribute ("Max", DoubleValue (0.010));

  
// Attach one UE per eNodeB
for (uint16_t i = 0; i < numNodePairs; i++)
  {
    lteHelper->Attach (ueLteDevs.Get(i), enbLteDevs.Get(i));
    // side effect: the default EPS bearer will be activated
  }
  
  
   // Install and start applications on UEs and remote host
   uint16_t dlPort = 1100;
   uint16_t ulPort = 2000;
   uint16_t otherPort = 3000;
   ApplicationContainer clientApps;
   ApplicationContainer serverApps;
   for (uint32_t u = 0; u < ueNodes.GetN (); ++u)
     {
       Ptr<Node> ue = ueNodes.Get (u);

       std::cout<<"UE id "<<ue->GetId()<<std::endl;
       if (!disableDl)
         {
           std::cout<<"Here DL"<<std::endl;

           PacketSinkHelper dlPacketSinkHelper ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), dlPort));
           serverApps.Add (dlPacketSinkHelper.Install (ueNodes.Get (u)));
  
           UdpClientHelper dlClient (ueIpIface.GetAddress (u), dlPort);
           dlClient.SetAttribute ("Interval", TimeValue (interPacketInterval));
           dlClient.SetAttribute ("MaxPackets", UintegerValue (1000000));
           clientApps.Add (dlClient.Install (remoteHost));
         }
  
       if (!disableUl)
         {
           std::cout<<"Here UL"<<std::endl;
           ++ulPort;
           PacketSinkHelper ulPacketSinkHelper ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), ulPort));
           serverApps.Add (ulPacketSinkHelper.Install (remoteHost));
  
           UdpClientHelper ulClient (remoteHostAddr, ulPort);
           ulClient.SetAttribute ("Interval", TimeValue (interPacketInterval));
           ulClient.SetAttribute ("MaxPackets", UintegerValue (1000000));
           clientApps.Add (ulClient.Install (ueNodes.Get(u)));
         }
  
       if (!disablePl && numNodePairs > 1)
         {
           std::cout<<"Here PL"<<std::endl;
           ++otherPort;
           PacketSinkHelper packetSinkHelper ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), otherPort));
           serverApps.Add (packetSinkHelper.Install (ueNodes.Get (u)));
  
           UdpClientHelper client (ueIpIface.GetAddress (u), otherPort);
           client.SetAttribute ("Interval", TimeValue (interPacketInterval));
           client.SetAttribute ("MaxPackets", UintegerValue (1000000));
           clientApps.Add (client.Install (ueNodes.Get ((u + 1) % numNodePairs)));
         }
     }

//Install HTTP in the UE nodes

for (uint32_t u = 0; u < ueNodes.GetN (); ++u){

  Ptr<Node> ue = ueNodes.Get (u);
   // Set the default gateway for the UE
  Ptr<Ipv4StaticRouting> ueStaticRouting = ipv4RoutingHelper.GetStaticRouting (ue->GetObject<Ipv4> ());
  ueStaticRouting->SetDefaultRoute (epcHelper->GetUeDefaultGatewayAddress (), 1);

  NS_LOG_INFO("HTTP Client Add: ");
  Ipv4Address ueClientAddr =ue->GetObject<Ipv4>()->GetAddress(1,0).GetLocal();
  ueClientAddr.Print(std::cout);

  if(ue->GetId()==8){


      ThreeGppHttpClientHelper clientHelper (remoteHostHttpAddr);
      NodeContainer ueHttpNodes(ue);
      ApplicationContainer clientApps_http;
      clientApps_http = clientHelper.Install (ueHttpNodes);

      Ptr<ThreeGppHttpClient> httpClient ;

      //for (int i=0; i<numberOfUes ; i++){
      httpClient = clientApps_http.Get (0)->GetObject<ThreeGppHttpClient> ();

      // Example of connecting to the trace sources
      httpClient->TraceConnectWithoutContext ("RxMainObject", MakeCallback (&ClientMainObjectReceived));
      httpClient->TraceConnectWithoutContext ("RxEmbeddedObject", MakeCallback (&ClientEmbeddedObjectReceived));
      httpClient->TraceConnectWithoutContext ("Rx", MakeCallback (&ClientRx));

      Time startTime = Seconds (startTimeSeconds->GetValue ());
      serverAppsHttp.Start(startTime);
      clientApps_http.Start(startTime);

  }

}

std::ostringstream oss0, oss1;
  
for (uint32_t u = 0; u < ueNodes.GetN (); ++u)
    {
      Ptr<Node> ue = ueNodes.Get (u);

      NS_LOG_INFO("UE id "<<ue->GetId());
      std::cout<<"UE id "<<ue->GetId()<<std::endl;

      //if (ueNodes.Get (u)->GetId () == 5 ){

     // oss1 << "/NodeList/" << ueNodes.Get (u)->GetId () << "/ApplicationList/0/$ns3::PacketSink/Rx";
       // Config::ConnectWithoutContext (oss1.str (), MakeCallback (&ReceivePacketEmbb));
        //ns3::LtePdcp::PduRxTracedCallback /NodeList/[i]/DeviceList/[i]/$ns3::LteUeNetDevice/LteUeRrc/DataRadioBearerMap/[i]/LtePdcp
        //NS_LOG_FUNCTION (this << imsi << cellId << rnti);
        //std::string ueRrcPath =  context.substr (0, context.rfind ("/"));
        
      //oss3.str("");
        ///NodeList/4/DeviceList/0/LteEnbRrc/UeMap/2/DataRadioBearerMap/1/LtePdcp/RxPDU
        //oss1 << "/NodeList/" << ueNodes.Get (u)->GetId () << "/DeviceList/*/LteUeRrc/DataRadioBearerMap/*/LtePdcp/RxPDU";
    //  oss3<<"/NodeList/*/DeviceList/*/LteUeRrc/DataRadioBearerMap/*/LtePdcp/RxPDU";
        //Config::Connect(oss1.str (), MakeCallback (&ReceivePdcpPacket));
   //   Config::Connect(oss3.str(), MakeCallback (&ReceivePdcpPacket));
     // }

      if (ueNodes.Get (u)->GetId () == 7 ){

        oss0 << "/NodeList/" << ueNodes.Get (u)->GetId () << "/ApplicationList/0/$ns3::PacketSink/Rx";
       // oss0 << "/NodeList/" << "*"<< "/ApplicationList/0/$ns3::PacketSink/Rx";
        Config::ConnectWithoutContext (oss0.str (), MakeCallback (&ReceivePacketUrllc));
      }

    
    }

   serverApps.Start (MilliSeconds (500));
   clientApps.Start (MilliSeconds (500));
   lteHelper->EnableTraces ();
   // Uncomment to enable PCAP tracing
   //p2ph.EnablePcapAll("lena-simple-epc");


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
    
    bool useIdealRrc = true;

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

  
   Simulator::Stop (simTime);
   Simulator::Run ();
  
   /*GtkConfigStore config;
   config.ConfigureAttributes();*/
  
   Simulator::Destroy ();
   return 0;
 }

