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

// Default Network Topology
//默认网络拓扑
//                          |
//                 Rank 0   |   Rank 1
// -------------------------|----------------------------
//   Wifi_left 10.1.3.0
//                                 AP
//  *     *     *     *   *   *    |
//  |     |     |     |   |   |    |      10.1.1.0
// n12   n11   n10   n9   n8  n7   n6 ------------------  n0    n1  n2   n3   n4  n5  
//                                      point-to-point    |     |   |    |    |   |
//                                                        |    ================
//                                                        AP    Wifi_right 10.1.2.0

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("ThirdScriptExample");		//定义记录组件

int 
main (int argc, char *argv[])
{
  bool verbose = true;
 // uint32_t nCsma = 3;			//csma节点数量
  uint32_t nWifi_left = 6;				//wifi节点数量
  uint32_t nWifi_right = 6;
   bool tracing = true;


  CommandLine cmd;
  cmd.AddValue ("nWifi", "Number of the left wifi STA devices", nWifi_left); 
  cmd.AddValue ("nWifi", "Number of the right wifi STA devices", nWifi_right);
  cmd.AddValue ("verbose", "Tell echo applications to log if true", verbose);
  cmd.AddValue ("tracing", "Enable pcap tracing", tracing);

  cmd.Parse (argc,argv);

  // Check for valid number of csma or wifi nodes
  // 250 should be enough, otherwise IP addresses 
  // soon become an issue		//判断是否超过了250个，超过报错 , 原因？
  if (nWifi_left > 250 || nWifi_right > 250)
    {
      std::cout << "Too many wifi nodes, no more than 250 each." << std::endl;
      return 1;
    }

  if (verbose)
    {
      LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
      LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);	//启动记录组件
    }


  //创建2个节点，p2p链路两端
  NodeContainer p2pNodes;
  p2pNodes.Create (2);

  //创建信道，设置信道参数，在设备安装到节点上
  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));

  NetDeviceContainer p2pDevices;
  p2pDevices = pointToPoint.Install (p2pNodes);

  //创建csma节点，包含一个p2p节点

/*  NodeContainer csmaNodes;
  csmaNodes.Add (p2pNodes.Get (1));
  csmaNodes.Create (nCsma);

  //创建信道，设置信道参数，在设备安装到节点上
  CsmaHelper csma;
  csma.SetChannelAttribute ("DataRate", StringValue ("100Mbps"));
  csma.SetChannelAttribute ("Delay", TimeValue (NanoSeconds (6560)));

  NetDeviceContainer csmaDevices;
  csmaDevices = csma.Install (csmaNodes);*/
  

  //创建左边的wifista无线终端，AP接入点
  NodeContainer wifiStaNodes_left;
  wifiStaNodes_left.Create (nWifi_left);
  NodeContainer wifiApNode_left = p2pNodes.Get (0);

  //创建无线设备于无线节点之间的互联通道，并将通道对象与物理层对象关联
  //确保所有物理层对象使用相同的底层信道，即无线信道
  YansWifiChannelHelper channel_left = YansWifiChannelHelper::Default ();
  YansWifiPhyHelper phy_left = YansWifiPhyHelper::Default ();
  phy_left.SetChannel (channel_left.Create ());

  //配置速率控制算法，AARF算法
  WifiHelper wifi_left = WifiHelper::Default ();
  wifi_left.SetRemoteStationManager ("ns3::AarfWifiManager");

  NqosWifiMacHelper mac_left = NqosWifiMacHelper::Default ();

  //配置mac类型为sta模式，不发送探测请求
  Ssid ssid_left = Ssid ("ns-3-ssid");
  mac_left.SetType ("ns3::StaWifiMac",
               "Ssid", SsidValue (ssid_left),
               "ActiveProbing", BooleanValue (false));

  //创建无线设备，将mac层和phy层安装到设备上
  NetDeviceContainer staDevices_left;
  staDevices_left = wifi_left.Install (phy_left, mac_left, wifiStaNodes_left);

  //配置AP节点的mac层为AP模式，创建AP设备
  mac_left.SetType ("ns3::ApWifiMac",
               "Ssid", SsidValue (ssid_left));

  NetDeviceContainer apDevices_left;
  apDevices_left = wifi_left.Install (phy_left, mac_left, wifiApNode_left);
  
  





  //创建wifista无线终端，AP接入点
  NodeContainer wifiStaNodes_right;
  wifiStaNodes_right.Create (nWifi_right);
  NodeContainer wifiApNode_right = p2pNodes.Get (1);

  //创建无线设备于无线节点之间的互联通道，并将通道对象与物理层对象关联
  //确保所有物理层对象使用相同的底层信道，即无线信道
  YansWifiChannelHelper channel_right = YansWifiChannelHelper::Default ();
  YansWifiPhyHelper phy_right = YansWifiPhyHelper::Default ();
  phy_right.SetChannel (channel_right.Create ());

  //配置速率控制算法，AARF算法
  WifiHelper wifi_right = WifiHelper::Default ();
  wifi_right.SetRemoteStationManager ("ns3::AarfWifiManager");

  NqosWifiMacHelper mac_right = NqosWifiMacHelper::Default ();

  //配置mac类型为sta模式，不发送探测请求
  Ssid ssid_right = Ssid ("ns-3-ssid_a");
  mac_right.SetType ("ns3::StaWifiMac",
               "Ssid", SsidValue (ssid_right),
               "ActiveProbing", BooleanValue (false));

  //创建无线设备，将mac层和phy层安装到设备上
  NetDeviceContainer staDevices_right;
  staDevices_right = wifi_right.Install (phy_right, mac_right, wifiStaNodes_right);

  //配置AP节点的mac层为AP模式，创建AP设备
  mac_right.SetType ("ns3::ApWifiMac",
               "Ssid", SsidValue (ssid_right));

  NetDeviceContainer apDevices_right;
  apDevices_right = wifi_right.Install (phy_right, mac_right, wifiApNode_right);








  //配置移动模型，起始位置
  MobilityHelper mobility;

  mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
                                 "MinX", DoubleValue (0.0),
                                 "MinY", DoubleValue (0.0),
                                 "DeltaX", DoubleValue (5.0),
                                 "DeltaY", DoubleValue (10.0),
                                 "GridWidth", UintegerValue (3),
                                 "LayoutType", StringValue ("RowFirst"));

  //配置STA移动方式，RandomWalk2dMobilityModel，随机游走模型
  mobility.SetMobilityModel ("ns3::ConstantVelocityMobilityModel");
  mobility.Install (wifiStaNodes_right);
  mobility.Install (wifiStaNodes_left);

  for (uint n = 0; n < wifiStaNodes_left.GetN(); n++)
  {
    Ptr<ConstantVelocityMobilityModel> mob = wifiStaNodes_left.Get(n)->GetObject<ConstantVelocityMobilityModel>();
    if (n == 1 || n == 3 || n == 5)
    {
    mob->SetVelocity(Vector(2.0, 2.0, 0.0));
    mob->SetPosition(Vector(0.0, 1.0, 2.0));
    }
    else
    {
    mob->SetVelocity(Vector(-2.0, -2.0, 0.0));
    mob->SetPosition(Vector(2.0, 1.0, 0.0));
    }
  }

//配置AP移动方式，ConstantPositionMobilityModel，固定位置模型
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (wifiApNode_left);
  mobility.Install (wifiApNode_right);

  //已经创建了节点，设备，信道和移动模型，接下来配置协议栈
  InternetStackHelper stack;
  //stack.Install (csmaNodes);
  stack.Install (wifiApNode_left);
  stack.Install (wifiStaNodes_left);
  stack.Install (wifiApNode_right);
  stack.Install (wifiStaNodes_right);

  //分配IP地址
  Ipv4AddressHelper address;
 //P2P信道
  address.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces;
  p2pInterfaces = address.Assign (p2pDevices);


 //wifi_right
  address.SetBase ("10.1.2.0", "255.255.255.0");
  Ipv4InterfaceContainer wifiInterfaces_right;
  wifiInterfaces_right =  address.Assign (staDevices_right);
  address.Assign (apDevices_right);
  //Ipv4InterfaceContainer csmaInterfaces;
  //csmaInterfaces = address.Assign (csmaDevices);


 //wifi_left
  address.SetBase ("10.1.3.0", "255.255.255.0");
  Ipv4InterfaceContainer wifiInterfaces_left;
  wifiInterfaces_left = address.Assign (staDevices_left);
  address.Assign (apDevices_left);

  //放置echo服务端程序在最右边的csma节点,端口为9
  UdpEchoServerHelper echoServer (9);

  ApplicationContainer serverApps = echoServer.Install (wifiStaNodes_right.Get (nWifi_right - 1));
  serverApps.Start (Seconds (1.0));
  serverApps.Stop (Seconds (10.0));

  //回显客户端放在最后的STA节点，指向CSMA网络的服务器，上面的节点地址，端口为9
  UdpEchoClientHelper echoClient (wifiInterfaces_right.GetAddress(nWifi_right - 1), 9);
  echoClient.SetAttribute ("MaxPackets", UintegerValue (1));
  echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  echoClient.SetAttribute ("PacketSize", UintegerValue (1024));

  //安装其他节点应用程序
  ApplicationContainer clientApps_a = 
    echoClient.Install (wifiStaNodes_left.Get (nWifi_left - 1));
  clientApps_a.Start (Seconds (2.0));
  clientApps_a.Stop (Seconds (10.0));

 ApplicationContainer clientApps_b = 
    echoClient.Install (wifiStaNodes_left.Get (nWifi_left - 2));
  clientApps_b.Start (Seconds (2.0));
  clientApps_b.Stop (Seconds (10.0));

 ApplicationContainer clientApps_c = 
    echoClient.Install (wifiStaNodes_left.Get (nWifi_left - 3));
  clientApps_c.Start (Seconds (2.0));
  clientApps_c.Stop (Seconds (10.0));

 ApplicationContainer clientApps_d = 
    echoClient.Install (wifiStaNodes_left.Get (nWifi_left - 4));
  clientApps_d.Start (Seconds (3.0));
  clientApps_d.Stop (Seconds (10.0));


  //启动互联网络路由
  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  Simulator::Stop (Seconds (10.0));

  if (tracing == true)
    {
      pointToPoint.EnablePcapAll ("myWifiProject2_1_P2P");
      phy_left.EnablePcap ("myWifiProject2_1_WifiLeft", apDevices_left.Get (0), true);
      phy_right.EnablePcap ("myWifiProject2_1_WifiRight", apDevices_right.Get (0), true);
    }

  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}
