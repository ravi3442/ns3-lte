/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
 /*
  * Copyright (c) 2016 Magister Solutions
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
  * Author: Lauri Sormunen <lauri.sormunen@magister.fi>
  */
  
 #include "ns3/core-module.h"
 #include "ns3/network-module.h"
 #include "ns3/internet-module.h"
 #include "ns3/point-to-point-module.h"
 #include "ns3/applications-module.h"
 #include "ns3/csma-module.h"
  
 using namespace ns3;
  
 NS_LOG_COMPONENT_DEFINE ("ThreeGppHttpExample");
  
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
 ClientRx (std::string context, Ptr<const Packet> packet, const Address &address)
 {

   NS_LOG_INFO (context <<" Client received a packet of " << packet->GetSize () << " bytes from " << address);
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
  
 int
 main (int argc, char *argv[])
 {
   double simTimeSec = 300;
   CommandLine cmd (__FILE__);
   cmd.AddValue ("SimulationTime", "Length of simulation in seconds.", simTimeSec);
   cmd.Parse (argc, argv);
  
   Time::SetResolution (Time::NS);
   LogComponentEnableAll (LOG_PREFIX_TIME);
   //LogComponentEnableAll (LOG_PREFIX_FUNC);
   //LogComponentEnable ("ThreeGppHttpClient", LOG_INFO);
   LogComponentEnable ("ThreeGppHttpExample", LOG_INFO);
  
  NodeContainer lanNodes;

  lanNodes.Create(3);

  CsmaHelper csma1;
  csma1.SetChannelAttribute("DataRate", StringValue("100Mbps"));
  csma1.SetChannelAttribute("Delay", TimeValue(NanoSeconds(6560)));

  NetDeviceContainer lanDevices;
  lanDevices = csma1.Install(lanNodes);
   // Setup two nodes
  // NodeContainer nodes;
   //nodes.Create (3);

   //NodeContainer n1n3= NodeContainer(nodes.Get(0), nodes.Get(2));
   //NodeContainer n2n3= NodeContainer(nodes.Get(1), nodes.Get(2);
  
   PointToPointHelper pointToPoint;
   pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
   pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));
  
   //NetDeviceContainer devicesn1n3, devicesn2n3;
   //devicesn1n3 = pointToPoint.Install (n1n3);
   //devicesn2n3 = pointToPoint.Install(n2n3);
  
   InternetStackHelper stack;
   stack.Install (lanNodes);
  
   Ipv4AddressHelper address;
   address.SetBase ("10.1.1.0", "255.255.255.0");
  
   Ipv4InterfaceContainer interfacesn1n3 = address.Assign (lanDevices);

   //Ipv4AddressHelper address2;
   //address.SetBase ("10.1.2.0", "255.255.255.0");

   //Ipv4InterfaceContainer interfacesn2n3 = address.Assign (devicesn2n3);
  
   //Ipv4Address serverAddress = interfacesn1n3.GetAddress (1);
  
   Ptr<Node> server_p= lanNodes.Get(2);

   Ipv4Address serverAddress = server_p->GetObject<Ipv4>()->GetAddress(1,0).GetLocal();

   // Create HTTP server helper
   ThreeGppHttpServerHelper serverHelper (serverAddress);
  
   // Install HTTP server
   ApplicationContainer serverApps = serverHelper.Install (lanNodes.Get (2));
   Ptr<ThreeGppHttpServer> httpServer = serverApps.Get (0)->GetObject<ThreeGppHttpServer> ();
  
   // Example of connecting to the trace sources
   httpServer->TraceConnectWithoutContext ("ConnectionEstablished",MakeCallback (&ServerConnectionEstablished));
   httpServer->TraceConnectWithoutContext ("MainObject", MakeCallback (&MainObjectGenerated));
   httpServer->TraceConnectWithoutContext ("EmbeddedObject", MakeCallback (&EmbeddedObjectGenerated));
   httpServer->TraceConnectWithoutContext ("Tx", MakeCallback (&ServerTx));
  
   // Setup HTTP variables for the server
   PointerValue varPtr;
   httpServer->GetAttribute ("Variables", varPtr);
   Ptr<ThreeGppHttpVariables> httpVariables = varPtr.Get<ThreeGppHttpVariables> ();
   httpVariables->SetMainObjectSizeMean (102400); // 100kB
   httpVariables->SetMainObjectSizeStdDev (40960); // 40kB
  
  
   // Create HTTP client helper
   ThreeGppHttpClientHelper clientHelper (serverAddress);

   serverAddress.Print(std::cout) ;

  
   // Install HTTP client
   NodeContainer ueHttpNodes(lanNodes.Get(0), lanNodes.Get(1));
   ApplicationContainer clientApps_http;
   clientApps_http = clientHelper.Install (ueHttpNodes); 


  //ApplicationContainer clientApps = clientHelper.Install (nodes.Get (0));
   Ptr<ThreeGppHttpClient> httpClient = clientApps_http.Get (0)->GetObject<ThreeGppHttpClient> ();

  // ApplicationContainer clientApps1 = clientHelper.Install (nodes.Get (2));
   Ptr<ThreeGppHttpClient> httpClient1 = clientApps_http.Get (1)->GetObject<ThreeGppHttpClient> ();

  
   // Example of connecting to the trace sources
   httpClient->TraceConnectWithoutContext ("RxMainObject", MakeCallback (&ClientMainObjectReceived));
   httpClient->TraceConnectWithoutContext ("RxEmbeddedObject", MakeCallback (&ClientEmbeddedObjectReceived));
   httpClient->TraceConnect ("Rx", "Client 1", MakeCallback (&ClientRx));

   // Example of connecting to the trace sources
   httpClient1->TraceConnectWithoutContext ("RxMainObject", MakeCallback (&ClientMainObjectReceived));
   httpClient1->TraceConnectWithoutContext ("RxEmbeddedObject", MakeCallback (&ClientEmbeddedObjectReceived));
   httpClient1->TraceConnect("Rx", "Client 2", MakeCallback (&ClientRx));
  
   // Stop browsing after 30 minutes
   clientApps_http.Stop (Seconds (simTimeSec));
  // clientApps1.Stop(Seconds (simTimeSec));
  
   Simulator::Run ();
   Simulator::Destroy ();
   return 0;
 }

