/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
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
 */

#include "ns3/core-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"


//         csma
//      ===================  
//     n0    n1    n2    n3
//    p|     |p
//    2|     |2
//    p|     |p
//     |     |
//     |     n4
//     |     |
//     |     |p
//     |     |2
//     |     |p
//     \     /
//      \   /
//       \ /
//        n5

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("ThirdScriptExample");		//定义记录组件

int 
main (int argc, char *argv[])
{
  bool verbose = true;
  uint32_t nP2P = 4;
  bool tracing = true;


  CommandLine cmd;
  cmd.AddValue ("verbose", "Tell echo applications to log if true", verbose);
  cmd.AddValue ("tracing", "Enable pcap tracing", tracing);
  cmd.Parse (argc,argv);

  if (verbose)
    {
      LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
      LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);	//启动记录组件
    }



//============================创建节点====================================
  //创建4个节点，p2p链路两端
  NodeContainer p2pNodes;
  p2pNodes.Create (nP2P);

  NodeContainer c0c3 = NodeContainer (p2pNodes.Get(0), p2pNodes.Get(3));
  NodeContainer c1c2 = NodeContainer (p2pNodes.Get(1), p2pNodes.Get(2));
  NodeContainer c2c3 = NodeContainer (p2pNodes.Get(2), p2pNodes.Get(3));

  //创建信道，设置信道参数，在设备安装到节点上
  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));

  NetDeviceContainer p2pDevices03 = pointToPoint.Install (c0c3);
  NetDeviceContainer p2pDevices12 = pointToPoint.Install (c1c2);
  NetDeviceContainer p2pDevices23 = pointToPoint.Install (c2c3);



  //创建csma节点，包含2个p2p节点
  NodeContainer csmaNodes;
  csmaNodes.Create (2);
  for (uint i = 0; i < 2; i++)
  {
    csmaNodes.Add (p2pNodes.Get (i));;
  }

  //创建信道，设置信道参数，在设备安装到节点上
  CsmaHelper csma;
  csma.SetChannelAttribute ("DataRate", StringValue ("100Mbps"));
  csma.SetChannelAttribute ("Delay", TimeValue (NanoSeconds (6560)));

  NetDeviceContainer csmaDevices;
  csmaDevices = csma.Install (csmaNodes);
  

//===========================配置协议栈=====================================
  //已经创建了节点，设备，信道和移动模型，接下来配置协议栈
  InternetStackHelper stack;
  stack.Install (csmaNodes);
  stack.Install (p2pNodes.Get(2));
  stack.Install (p2pNodes.Get(3));


//==========================分配IP地址========================================
  Ipv4AddressHelper address;

//p2p信道
  address.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces03;
  p2pInterfaces03 = address.Assign (p2pDevices03);

//p2p信道
  address.SetBase ("10.1.2.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces12;
  p2pInterfaces12 = address.Assign (p2pDevices12);

//p2p信道
  address.SetBase ("10.1.3.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces23;
  p2pInterfaces23 = address.Assign (p2pDevices23);

//csma信道
  address.SetBase ("10.1.4.0", "255.255.255.0");
  Ipv4InterfaceContainer csmaInterfaces;
  csmaInterfaces = address.Assign (csmaDevices);




//======================配置服务器和客户端=====================================
  //放置echo服务端程序在最右边的csma节点,端口为9
  UdpEchoServerHelper echoServer (9);

  ApplicationContainer serverApps = echoServer.Install (p2pNodes.Get (nP2P - 1));
  serverApps.Start (Seconds (1.0));
  serverApps.Stop (Seconds (10.0));

  //回显客户端放在最后的STA节点，指向CSMA网络的服务器，上面的节点地址，端口为9
  UdpEchoClientHelper echoClient (p2pInterfaces03.GetAddress(1), 9);
  echoClient.SetAttribute ("MaxPackets", UintegerValue (1));
  echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  echoClient.SetAttribute ("PacketSize", UintegerValue (1024));

  //安装其他节点应用程序
  ApplicationContainer clientApps_a = 
    echoClient.Install (csmaNodes.Get (0));
  clientApps_a.Start (Seconds (2.0));
  clientApps_a.Stop (Seconds (10.0));

 ApplicationContainer clientApps_b = 
    echoClient.Install (csmaNodes.Get (1));
  clientApps_b.Start (Seconds (3.0));
  clientApps_b.Stop (Seconds (10.0));

 ApplicationContainer clientApps_c = 
    echoClient.Install (csmaNodes.Get (2));
  clientApps_c.Start (Seconds (4.0));
  clientApps_c.Stop (Seconds (10.0));

 ApplicationContainer clientApps_d = 
    echoClient.Install (csmaNodes.Get (3));
  clientApps_d.Start (Seconds (5.0));
  clientApps_d.Stop (Seconds (10.0));


//=======================启动互联网络路由========================================
  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();
 
  Simulator::Stop (Seconds (10.0));

  if (tracing == true)
    {
      pointToPoint.EnablePcapAll ("myProject2_2_P2P");
      csma.EnablePcap ("myProject2_2_Csma1", p2pDevices03.Get (0), true);
      csma.EnablePcap ("myProject2_2_Csma2", p2pDevices12.Get (0), true);
      csma.EnablePcap ("myProject2_2_Csma3", p2pDevices23.Get (0), true);
    }

  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}
