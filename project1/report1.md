##ns3-lec1-project1
### NS3是什么
>Ns-3是一个离散事件驱动网络模拟器。除了系统状态变量和系统事件发生逻辑外，基于事件仿真还包括以下组成部分： 
>（1）时钟（Clock） 
>仿真系统必须要保持对当前仿真时间的跟踪。离散事件仿真与实时仿真不同，在离散事件仿真中时间是跳跃的（ time ‘hops’ ），因为事件是瞬时发生的– 随着仿真的进展，时钟跳跃到下一事件的开始时间。
>（2）事件列表（Events List） 
>仿真系统至少要维护一个仿真事件列表，一个事件用事件发生的时刻和类型来描述，事件类型标识用于仿真事件的代码，一般事件代码都是参数化的，事件描述中还包含表示事件代码的参数。
>Ns-3的事件列表由Scheduler类及其派生类实现，Simulator类提供创建具体的Scheduler对象的方法，以及插入各种事件的静态接口函数。
>（3）随机数发生器
>根据系统模型，仿真系统需要产生各种类型的随机变量（random variables ）。
这由一个或多个伪随机数发生器（Pseudorandom number generators）产生。
>（4）统计（Statistics） 
>仿真系统通常会记录系统的统计数据，用以表示感兴趣的一些统计量。 
>（5）结束条件（ Ending Condition） 
>理论上来说离散事件仿真系统可以永远运行下去。因此，仿真系统设计者必须决定仿真什么时候结束。典型的选择是“在事件t”（ “at time t” ）或者“在处理n个事件后”（ “after processing n number of events”）或者，更一般地，“当统计量X
达到值x时”（ “when statistical measure X reaches the value x”）。

###为什么要仿真
>在硬件设计与编程前都某种算法或策略就行验证，节约成本
>网络仿真：节点数目多，直接在实际设备上测试耗时耗力
>仿真在科研、工程中应用非常广泛：
> * 电路仿真：LabView multisim ModelSim 
> * 物理引擎：Nvidia PhyX 

###关键代码
* 命令行读入参数
```
cmd.AddValue ("name", "my name", name);
cmd.AddValue ("stid", "my student id", stid);
```
三个参数分别是参数名称、参数说明、参数存储变量

* 打印参数函数
```
static void printHello(std::string word, std::string stid) { 
	std::cout<<Simulator::Now()<< " " << word << " " << stid << std::endl; 
	Simulator::Schedule(Seconds(1),&printHello, word, stid); 
}
```
读入参数并加入到仿真器schedule中去

* 定时执行
```
Simulator::Stop(Seconds(10)); 
Simulator::Run (); 
Simulator::Destroy (); 
```

###命令行运行及结果
* 简单运行
```
./waf --run "scratch/hello-simulator --name=余烜 --stid=2013010917017"
```
* 过滤运行
```
./waf --run "scratch/hello-simulator --name=余烜 --stid=2013010917017"|grep '余烜'
```
* 过滤并统计运行
```
./waf --run "scratch/hello-simulator --name=余烜 --stid=2013010917017"|grep '余烜'|wc -l
```
![](http://diycode.b0.upaiyun.com/photo/2016/f8adc8e93a9720e955774952618b168a.png)
