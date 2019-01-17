//---------------------------------------------------------------------------
#ifndef Twse2KwayFilesH
#define Twse2KwayFilesH
//---------------------------------------------------------------------------
#include "KwayBase.h"
#include "char_ary.h"
#include "GFieldsDef.h"
#include "kutility.h" 
//---------------------------------------------------------------------------
namespace Kway {
//---------------------------------------------------------------------------
typedef  Kstl::char_ary<char, 1, 0>   TStr1;
typedef  Kstl::char_ary<char, 2, 0>   TStr2;
typedef  Kstl::char_ary<char, 3, 0>   TStr3;
typedef  Kstl::char_ary<char, 4, 0>   TStr4;
typedef  Kstl::char_ary<char, 5, 0>   TStr5;
typedef  Kstl::char_ary<char, 6, 0>   TStr6;
typedef  Kstl::char_ary<char, 7, 0>   TStr7;
typedef  Kstl::char_ary<char, 8, 0>   TStr8;
typedef  Kstl::char_ary<char, 10, 0>  TStr10;
typedef  Kstl::char_ary<char, 11, 0>  TStr11;
typedef  Kstl::char_ary<char, 12, 0>  TStr12;
typedef  Kstl::char_ary<char, 13, 0>  TStr13;
typedef  Kstl::char_ary<char, 15, 0>  TStr15;
typedef  Kstl::char_ary<char, 16, 0>  TStr16;
typedef  Kstl::char_ary<char, 18, 0>  TStr18;
typedef  Kstl::char_ary<char, 19, 0>  TStr19;
typedef  Kstl::char_ary<char, 20, 0>  TStr20;
typedef  Kstl::char_ary<char, 22, 0>  TStr22;
typedef  Kstl::char_ary<char, 22, 0>  TStr27;
typedef  Kstl::char_ary<char, 31, 0>  TStr31;
typedef  Kstl::char_ary<char, 32, 0>  TStr32;
typedef  Kstl::char_ary<char, 34, 0>  TStr34;
typedef  Kstl::char_ary<char, 35, 0>  TStr35;
typedef  Kstl::char_ary<char, 36, 0>  TStr36;
typedef  Kstl::char_ary<char, 39, 0>  TStr39;
typedef  Kstl::char_ary<char, 40, 0>  TStr40;
typedef  Kstl::char_ary<char, 42, 0>  TStr42;
typedef  Kstl::char_ary<char, 44, 0>  TStr44;
typedef  Kstl::char_ary<char, 52, 0>  TStr52;
typedef  Kstl::char_ary<char, 56, 0>  TStr56;
typedef  Kstl::char_ary<char, 62, 0>  TStr62;
typedef  Kstl::char_ary<char, 64, 0>  TStr64;
typedef  Kstl::char_ary<char, 72, 0>  TStr72;
typedef  Kstl::char_ary<char, 92, 0>  TStr81;
typedef  Kstl::char_ary<char, 92, 0>  TStr92;
typedef  Kstl::char_ary<char,146, 0>  TStr146;
typedef  Kstl::char_ary<char,152, 0>  TStr152;

typedef  Kstl::char_ary<char, 26, 0>  TFreeStr26;
typedef  Kstl::char_ary<char, 34, 0>  TFreeStr34;

//---------------------------------------------------------------------------
enum  ESetType
{
   est_None,
   est_NewFix,
   est_Appia,
   est_Tmp,
};
//---------------------------------------------------------------------------
//測試線路設定資料Data1(Len=160)
struct  SF01Data1
{
   TStr8    Date_;      //西元日期
   TStr152  Filler1_;      
};

//測試線路設定資料Data2(Len=160)
struct  SF01Data2
{
   TStr8       TestNo_;    //券商端之測試線路
   TStr5       PVCNo_;     //券商端之PVC NO或SOCKET PORT NO
   TStr1       Market_;    //T=TSE, O=OTC
   TStr4       Broker_;    
   TStr2       FixPVCID_;  //FIX PVCID
   TStr1       APCode_;    //TMP作業別, 0=委託下單, 3=成交回報
   TStr4       Passwd_;
   TStr1       Type_;      //線路別設定, N：新FIX線路設定 
   TStr1       FixVersion_;//FIX版本別, 1=4.2, 2=4.4, (Type_=F,才需有值)
   TStr15      IP1_;       //第一路IP
   TStr15      IP2_;       //第二路IP
   TStr5       FWGroup_;   //Forwarder群組
   TStr5       AppiaGroup_;//Appia群組
   TStr15      DestIP_;    //DESTINATION IP
   TStr5       ListPort1_; //LISTEN PORT 1
   TStr5       ListPort2_; //LISTEN PORT 2
   TStr6       ApplySeqNo_;//申請收文序號
   TStr62      Filler2_;

   K_mf(ESetType)  WhichType() { return (Type_[0]=='N' ? est_NewFix : est_None);}
};

//測試線路設定資料
struct  SF01 : public SF01Data1, public SF01Data2 
{
   K_mf(int)  GetDataSize(int i=0) { return (i==1 ? sizeof(SF01Data1) : sizeof(SF01Data2)); }
};
//---------------------------------------------------------------------------
/*
//FIX測試結果回報(Len=120)
struct  SK02
{
   TStr8       TestNo_;    //券商端之測試線路 #F01#
   TStr5       PVCNo_;     //券商端之PVC NO或SOCKET PORT NO #F01#
   TStr1       Market_;    //#F01#
   TStr1       IP_X25_;    //#F01#
   TStr8       ApplyDate_; //申請測試日期
   TStr10      Broker_;    //#F01#
   TStr4       ApplySeqNo_;//申請收文序號 #F01#
   TStr4       Passwd_;    //#F01#
   TFreeStr26  PortName_;  //TSE PORT NAME或GATEWAY #F01#
   TStr8       OKDate_;    //測試完成日期
   TStr8       OKTime_;    //測試完成時間
   TStr3       Result_;    //測試結果
   TStr15      BrokerIP_;  //券商IP ADDRESS #F01#
   TStr1       Type_;      //線路別設定, F=FIX, K=Kway-TMP #F01#
   TStr2       FixPVCID_;  //TMP所對應之FIX PVCID #F01#
   //TStr16      Filler_;
   TStr1       FixVersion_;//FIX版本別, 1=4.2, 2=4.4, (Type_=F,才需有值)
   TStr15      Filler_;
   
   
   K_mf(int)  GetDataSize(int i=0) { return sizeof(SK02); }
};
*/
//---------------------------------------------------------------------------
K_fn(void)  FillChar(std::string& str, int size, char c);
//---------------------------------------------------------------------------
//FIX系統異動通知Data1(Len=160)
struct  SF03Data1
{
   TStr8   Date_;      //西元日期
   TStr6   Cnt_;       //異動筆數(不含首筆)
   TStr146  Filler1_;
   
   K_mf(std::string)  ToString();
};

//FIX系統異動通知Data2(Len=160)
struct  SF03Data2
{
   TStr1   WorkCode_;  //異動別, I=新增, D=刪除 (U:更新訊息流量)
   TStr8   LineNo_;    //線路號碼
   TStr5   SrcPort_;   //Socket Port No
   TStr1   Market_;
   TStr7   PVCName_;   //PVC名稱
   TStr4   Broker_;
   TStr2   PVCID_;     //PVCID
   TStr1   APCode_;    //應用程式代碼, 0=委託下單, 3=成交回報
   TStr4   Passwd_;    //密碼
   TStr1   Type_;      //線路別設定, N:新FIX線路設定
   TStr1   FixVersion_;//FIX版本別, 1=4.2, 2=4.4
   TStr15  IP1_;       //第一路IP
   TStr15  IP2_;       //第二路IP
   TStr5   FWGroup_;   //Forwarder群組
   TStr5   AppiaGroup_;//Appia群組
   TStr15  DestIP_;    //DESTINATION IP
   TStr5   ListPort1_; //LISTEN PORT 1
   TStr5   ListPort2_; //LISTEN PORT 2
   TStr4   FlowUnit_;  //訊息流量單位
   TStr4   TotalFlow_; //總訊息流量
   TStr8   EffDate_;   //生效日期
   TStr44  Filler2_;

   K_mf(ESetType)  WhichType() { return (Type_[0]=='N' ? est_NewFix : est_None);}
   K_mf(std::string)  ToString();
   K_mf(void)  Clear();
};

//FIX系統異動通知(Len=60 + 100)
struct  SF03 : public SF03Data1, public SF03Data2
{   
   K_mf(int)  GetDataSize(int i=0) { return (i==1 ? sizeof(SF03Data1) : sizeof(SF03Data2)); }
};
//----------------------------------------------------------------------------
//校對F03之確認檔Data1(Len=160),18:00後
struct  SF05Data1
{
   TStr8   Date_;      //西元日期
   TStr152  Filler1_;
};

//校對F03之確認檔Data2(Len=160),18:00後
struct  SF05Data2
{
   TStr7       PVCName_;   //PVC名稱
   TStr4       Broker_;   
   TStr1       APCode_;    //應用程式代碼, 0=委託下單, 3=成交回報
   TStr2       PVCID_;     //TMP線路PVCID
   TStr4       Passwd_;
   TStr5       FWGroup_;   //Forwarder群組
   TStr5       AppiaGroup_;//Appia群組
   TStr2       SMGroup_;   //Socket Middleware 群組
   TStr1       Type_;      //線路別設定, N:新FIX線路設定
   TStr1       FixVersion_;//FIX版本別, 1=4.2, 2=4.4
   TStr15      IP1_;       //第一路IP
   TStr15      IP2_;       //第二路IP
   TStr5       SrcPort_;   //Socket Port No
   TStr15      DestIP_;    //DESTINATION IP
   TStr5       ListPort1_; //LISTEN PORT 1
   TStr5       ListPort2_; //LISTEN PORT 2
   TStr4       TotalFlow_; //總訊息流量
   TStr64      Filler2_;

   K_mf(ESetType)  WhichType() { return (Type_[0]=='N' ? est_NewFix : est_None);}
};

//校對F03之確認檔(Len=80 + 80),18:00後
struct  SF05 : public SF05Data1, public SF05Data2
{
   K_mf(int)  GetDataSize(int i=0) { return i==1 ? sizeof(SF05Data1) : sizeof(SF05Data2); } 
};
//---------------------------------------------------------------------------
// 次交易日資訊 (Len=50),17:00~17:30會進來
struct  SF04
{
   TStr8   Date_;      //西元日期
   TStr8   NextDate_;  //次一交易日
   TStr34  Filler1_;

   K_mf(int)  GetDataSize(int i=0) { return sizeof(SF04); }
};
//----------------------------------------------------------------------------
//開始/結束作業Data1(Len=50)
struct  SF06Data1
{
   TStr8   Date_;      //西元日期
   TStr42  Filler1_;
};

//開始/結束作業Data2(Len=50)
struct  SF06Data2
{
   TStr3   WorkCode_;  //作業代碼, 01=假開盤開始, 02=假開盤結束, 03=整股作業起始, 04=日結清檔作業
   TStr8   WorkTime_;  //LOG作業時間
   TStr39  Filler2_;
};

//開始/結束作業(Len=50 + 50)
struct  SF06 : public SF06Data1, public SF06Data2
{
   K_mf(int)  GetDataSize(int i=0) { return (i==1 ? sizeof(SF06Data1) : sizeof(SF06Data2)); }
};
//----------------------------------------------------------------------------
//假開盤股票檔Data1(Len=50)
struct  SF07Data1
{
   TStr8   Date_;      //西元日期
   TStr42  Filler1_;
};

//假開盤股票檔Data2(Len=50)
struct  SF07Data2
{
   TStr8   Date_;      //西元日期
   TStr6   StkNo_;     //證券代號
   TStr6   StkName_;   //證券名稱
   TStr6   PriUp_;     //漲停價 9(04)V99
   TStr6   PriRef_;    //參考價 9(04)V99
   TStr6   PriDown_;   //跌停價 9(04)V99
//#ifdef __20100825__
   TStr1   TradeOn_;   //零股註記, Y表示可委託
   TStr11  Filler2_;
//#else //__20100825__
//   TStr12      Filler2_;
//#endif //__20100825__
   K_mf(std::string)  as_price(TStr6& p) { std::string a = p.as_string(); return std::string(a.substr(0,4)+"."+a.substr(4,2)); }
};

//假開盤股票檔(Len=50 + 50)
struct  SF07 : public SF07Data1, public SF07Data2
{
   K_mf(int)  GetDataSize(int i=0) { return (i==1 ? sizeof(SF07Data1) : sizeof(SF07Data2)); }
};
//----------------------------------------------------------------------------
//修改FIX密碼檔Data1(Len=60)
struct  SF08Data1
{
   TStr8   Date_;      //西元日期
   TStr52  Filler1_;
};

//修改FIX密碼檔Data2(Len=60)
struct  SF08Data2
{
   TStr4   Broker_;
   TStr2   FixPVCID_;  //FIX Socket ID
   TStr4   OldPasswd_; //舊密碼
   TStr4   Passwd_;    //新密碼
   TStr5   FWGroup_;   //Forwarder群組
   TStr5   AppiaGroup_;//Appia群組
   TStr36  Filler2_;
};

struct  SF08 : public SF08Data1, public SF08Data2
{
   K_mf(int)  GetDataSize(int i=0) { return i==1 ? sizeof(SF08Data1) : sizeof(SF08Data2); }
};
//----------------------------------------------------------------------------
/*
//FIX清除某盤別錯誤次數檔
struct  SK09  //(Len=50)
{
   TStr4   Broker_;
   TStr2   FixPVCID_;  //FIX Socket ID
   TStr1   APCode_;    //應用程式代碼
   TStr8   TransTime_; //LOG作業時間
   TStr35  Filler_;
  
   K_mf(int)  GetDataSize(int i=0) { return sizeof(SK09); }
   K_mf(std::string)  ToString();
};
*/
//---------------------------------------------------------------------------
//FIX完整環境檔Data1(Len=160)
struct  SF10Data1
{
   TStr8   Date_;      //西元日期
   TStr152  Filler1_;
   
   K_mf(std::string)  ToString();
};

//FIX完整環境檔Data2(Len=160)
struct  SF10Data2
{
   TStr7   PVCName_;   //PVC名稱
   TStr4   Broker_;
   TStr1   APCode_;    //應用程式代碼, 0=委託下單, 3=成交回報
   TStr2   PVCID_;     //Fix Socket ID
   TStr4   Passwd_;    //密碼
   TStr5   FWGroup_;   //Forwarder群組
   TStr5   AppiaGroup_;//Appia群組
   TStr2   SMGroup_;   //Socket Middleware 群組
   TStr1   Type_;      //線路別設定, N:新FIX線路設定
   TStr1   FixVersion_;//FIX版本別, 1=4.2, 2=4.4
   TStr15  IP1_;       //第一路IP
   TStr15  IP2_;       //第二路IP
   TStr5   SrcPort_;   //Socket Port No
   TStr15  DestIP_;    //DESTINATION IP
   TStr5   ListPort1_; //LISTEN PORT 1
   TStr5   ListPort2_; //LISTEN PORT 2
   TStr4   TotalFlow_; //總訊息流量
   TStr64  Filler2_;
   
   K_mf(std::string)  ToString();
   K_mf(void)  Clear();
};

//FIX完整環境檔
struct  SF10 : public SF10Data1, public SF10Data2
{   
   K_mf(int)  GetDataSize(int i=0) { return (i==1 ? sizeof(SF10Data1) : sizeof(SF10Data2)); }
};
//----------------------------------------------------------------------------
//FIX證券商合併受讓作業檔
struct  SF11Data1  //(Len=50)
{
   TStr8   Date_;      //西元日期
   TStr42  Filler1_;
};

struct  SF11Data2  //(Len=50)
{
   TStr4   BfBroker_;   //原始證券商代號
   TStr4   AfBroker_;   //存續證券商代號
   TStr42  Filler2_;
};

struct  SF11 : public SF11Data1, public SF11Data2
{
   K_mf(int)  GetDataSize(int i=0) { return i==1 ? sizeof(SF11Data1) : sizeof(SF11Data2); }
};
//----------------------------------------------------------------------------
/*
//FIX證券商流量檔
struct  SF12Data1  //(Len=30)
{
   TStr8   Date_;      //西元日期
   TStr22  Filler1_;
};

struct  SF12Data2  //(Len=30)
{
   TStr4   Broker_;    //證券商代號
   TStr2   FixPVCID_;  //FIX Socket ID
   TStr4   TotalFlow_; //總流量值
   TStr20  Filler2_;
};

struct  SF12 : public SF12Data1, public SF12Data2
{
   K_mf(int)  GetDataSize(int i=0) { return i==1 ? sizeof(SF12Data1) : sizeof(SF12Data2); }
};
*/
//----------------------------------------------------------------------------
/*
//FIX版本異動通知檔
struct  SF13Data1  //(Len=50)
{
   TStr8   Date_;      //西元日期
   TStr42  Filler1_;
};

struct  SF13Data2  //(Len=50)
{
   TStr10  Broker_;    //證券商代號
   TStr2   FixPVCID_;  //FIX Socket ID
   TStr1   BfFixVer_;  //FIX版本別, 1=4.2, 2=4.4
   TStr1   AfFixVer_;  //FIX版本別, 1=4.2, 2=4.4
   TStr36  Filler2_;
};

struct  SF13 : public SF13Data1, public SF13Data2
{
   K_mf(int)  GetDataSize(int i=0) { return i==1 ? sizeof(SF13Data1) : sizeof(SF13Data2); }
};
*/
//----------------------------------------------------------------------------
//證券商檔
struct  SF14  //(Len=19)
{
   TStr19  Filler_;
   K_mf(int)  GetDataSize(int i=0) { return sizeof(SF14); }
};
//----------------------------------------------------------------------------
//APPIA連線定義檔
struct  SF15  //(Len=13)
{
   TStr13  Filler_;
   K_mf(int)  GetDataSize(int i=0) { return sizeof(SF15); }
};
//----------------------------------------------------------------------------
//可交易商品檔
struct  SF16  //(Len=81)
{
   TStr81  Filler_;
   K_mf(int)  GetDataSize(int i=0) { return sizeof(SF16); }
};
//----------------------------------------------------------------------------
//可交易商品檔
struct  SF17  //(Len=27)
{
   TStr27  Filler_;
   K_mf(int)  GetDataSize(int i=0) { return sizeof(SF17); }
};
//----------------------------------------------------------------------------
#ifdef K_EnumPacked_Bug
   typedef byte  EFileType;
   enum  EFileType_t
#else
   enum  EFileType
#endif
{
   ft_None,
   ft_F01,
   //ft_K02,
   ft_F03,
   ft_F04,
   ft_F05,
   ft_F06,
   ft_F07,
   ft_F08,
   //ft_K09,
   //ft_F10, //F10與F03相同
   ft_F11,
   //ft_F12,
   //ft_F13,
   ft_F14,
   ft_F15,
   ft_F16,
   ft_F17,
} K_EnumPacked;

K_class  TFileType : public Kstl::EnumT<EFileType>
{
   typedef  Kstl::EnumT<EFileType>  inherited;
public:
   inline K_ctor  TFileType(EFileType a = ft_None) : inherited(a) {}
   
   K_mf(std::string)  as_string() const;
   K_mf(void)  assign(const std::string&);
   K_mf(int)   GetDataSize(int i=0);
};
//----------------------------------------------------------------------------
};  // Kway
//---------------------------------------------------------------------------
#endif
