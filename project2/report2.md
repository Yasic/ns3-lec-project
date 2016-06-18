# ns3-lec1-project2

### 网络拓扑图

* 拓扑一(project2_1)

```
   Wifi_left 10.1.3.0
                                 AP
  *     *     *     *   *   *    |
  |     |     |     |   |   |    |      10.1.1.0
 n12   n11   n10   n9   n8  n7   n6 ------------------  n0    n1  n2   n3   n4  n5  
                                      point-to-point    |     |   |    |    |   |
                                                        |    ================
                                                        AP    Wifi_right 10.1.2.0
```

* 拓扑二(project2_2)

```
         csma
      ===================  
     n0    n1    n2    n3
    p|     |p
    2|     |2
    p|     |p
     |     |
     |     n4
     |     |
     |     |p
     |     |2
     |     |p
     \     /
      \   /
       \ /
        n5
```

### STA移动模型部分代码

```
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
```

### 运行结果截图
![](http://diycode.b0.upaiyun.com/photo/2016/0d41547b637d82a5257b7fd1ff77829b.png)

### 相关问题

* 关于同时发起四个客户端对服务器进行访问的问题

同时发起4个客户端对服务器进行访问会导致最后运行时服务器的回应少一次，原因不明，采取办法是，最多三个客户端在同一时刻对服务器发送报文，等待一段时间后再启动第4个客户端发送保温

* 关于wifi设置phy和channel的问题

```
YansWifiChannelHelper channel_left = YansWifiChannelHelper::Default ();
YansWifiPhyHelper phy_left = YansWifiPhyHelper::Default ();
```
虽然两者是使用同一个方法执行产生的，但是从逻辑上讲每一次调用产生的channel对象和phy对象都是独立的，不能对多个wifi子拓扑使用，否则会产生错误信息

* 关于ESCP问题

一开始尝试设计如下一种网络拓扑
```
      n2 
    /  | \
   /   |  \
 n1---n5---n4
   \   |  /
    \  | /
      n3
    
```

其中周围4个节点在同一个csma环境下，同时每个节点与中心的n5节点构成P2P信道，运行时n5充当服务器，n1-n4充当客户机，间隔1s依次发送报文

报错信息为
```
Assumed there is at most one exit from the root to this vertex
```

考虑是有多条从源点到达目的点的链路造成了路由疑惑，于是定位错误信息
```
===========================启动互联网络路由=============================
  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();
```

将这一句注释掉后则可以发现客户机可以成功将报文发送至服务器，因此排除客户机端的路由疑惑

接下来减少p2p节点至2个，即只留下n1-n5信道，则可以成功运行程序

将n2-n5路径上添加一个节点，形成如下拓扑
```
         csma
      ===================  
     n0    n1    n2    n3
    p|     |p
    2|     |2
    p|     |p
     |     |
     |     n4
     |     |
     |     |p
     |     |2
     |     |p
     \     /
      \   /
       \ /
        n5
```
此时代码可以正确运行

经过搜索相关内容，最终估计原因是，当服务器n5需要发送回应报文给4个csma节点时，由于每一条链路P2P的代价都是相同的，形成了Equal-cost multi-path（ecmp）情况，所以会造成路由疑惑，也就是说，单纯使用PopulateRoutingTables()这个最简路由表生成方法是不能满足要求的。

但是另一个疑惑是，为什么第一个报文到达时也不能正确返回回应报文，有可能是路由信息在所有客户端尚未开始发送报文时就已经生成了，所以程序编译成功后还没有发送报文就检测出了路由的二义性。