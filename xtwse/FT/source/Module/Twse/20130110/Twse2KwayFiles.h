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
typedef  Kstl::char_ary<char, 31, 0>  TStr31;
typedef  Kstl::char_ary<char, 32, 0>  TStr32;
typedef  Kstl::char_ary<char, 34, 0>  TStr34;
typedef  Kstl::char_ary<char, 35, 0>  TStr35;
typedef  Kstl::char_ary<char, 36, 0>  TStr36;
typedef  Kstl::char_ary<char, 40, 0>  TStr40;
typedef  Kstl::char_ary<char, 42, 0>  TStr42;
typedef  Kstl::char_ary<char, 52, 0>  TStr52;
typedef  Kstl::char_ary<char, 72, 0>  TStr72;
typedef  Kstl::char_ary<char, 92, 0>  TStr92;

typedef  Kstl::char_ary<char, 26, 0>  TFreeStr26;
typedef  Kstl::char_ary<char, 34, 0>  TFreeStr34;

//---------------------------------------------------------------------------
enum  ESetType
{
   est_None,
   est_Appia,
   est_Tmp,
};
//---------------------------------------------------------------------------
//測試線路設定資料Data1(Len=100)
struct  SK01Data1
{
   TStr8   Date_;      //西元日期
   TStr92  Filler1_;      
};

//測試線路設定資料Data2(Len=100)
struct  SK01Data2
{
   
   TStr8       TestNo_;    //券商端之測試線路
   TStr5       PVCNo_;     //券商端之PVC NO或SOCKET PORT NO
   TStr1       Market_;    //T=TSE, O=OTC, X=資訊交換平台
   TStr1       IP_X25_;    //I=IP, X=X.25
   TStr10      Broker_;    
   TStr4       Passwd_;
   TFreeStr26  PortName_;  //TSE PORT NAME或GATEWAY
   TStr4       ApplySeqNo_;//申請收文序號
   TStr15      BrokerIP_;  //券商IP ADDRESS
   TStr1       Type_;      //線路別設定, F=FIX, K=Kway-TMP
   TStr2       TmpPVCID_;  //TMP線路PVCID
   TStr1       APCode_;    //TMP作業別, 0=委託下單, 3=成交回報
   TStr2       FixPVCID_;  //TMP所對應之FIX PVCID
   //TStr20      Filler2_;
   TStr1       FixVersion_;//FIX版本別, 1=4.2, 2=4.4, (Type_=F,才需有值)
   TStr19      Filler2_;

   K_mf(ESetType)  WhichType() { 
      return ((Type_[0]=='F' || Type_[0]=='K') ? (Type_[0]=='F'?est_Appia:est_Tmp) : est_None);
   }
};

//測試線路設定資料(Len=100 + 100)
struct  SK01 : public SK01Data1, public SK01Data2 
{
   K_mf(int)  GetDataSize(int i=0) { return (i==1 ? sizeof(SK01Data1) : sizeof(SK01Data2)); }
};
//---------------------------------------------------------------------------
//FIX測試結果回報(Len=120)
struct  SK02
{
   TStr8       TestNo_;    //券商端之測試線路 #K01#
   TStr5       PVCNo_;     //券商端之PVC NO或SOCKET PORT NO #K01#
   TStr1       Market_;    //#K01#
   TStr1       IP_X25_;    //#K01#
   TStr8       ApplyDate_; //申請測試日期
   TStr10      Broker_;    //#K01#
   TStr4       ApplySeqNo_;//申請收文序號 #K01#
   TStr4       Passwd_;    //#K01#
   TFreeStr26  PortName_;  //TSE PORT NAME或GATEWAY #K01#
   TStr8       OKDate_;    //測試完成日期
   TStr8       OKTime_;    //測試完成時間
   TStr3       Result_;    //測試結果
   TStr15      BrokerIP_;  //券商IP ADDRESS #K01#
   TStr1       Type_;      //線路別設定, F=FIX, K=Kway-TMP #K01#
   TStr2       FixPVCID_;  //TMP所對應之FIX PVCID #K01#
   //TStr16      Filler_;
   TStr1       FixVersion_;//FIX版本別, 1=4.2, 2=4.4, (Type_=F,才需有值)
   TStr15      Filler_;
   
   
   K_mf(int)  GetDataSize(int i=0) { return sizeof(SK02); }
};
//---------------------------------------------------------------------------
K_fn(void)  FillChar(std::string& str, int size, char c);
//---------------------------------------------------------------------------
//FIX系統異動通知Data1(Len=100)
struct  SK03Data1
{
   TStr8   Date_;      //西元日期
   TStr92  Filler1_;
   
   K_mf(std::string)  ToString();
};

//FIX系統異動通知Data2(Len=100)
struct  SK03Data2
{
   TStr1   WorkCode_;  //異動別, I=新增, D=刪除
   TStr1   Type_;      //線路別設定, F=FIX, K=Kway-TMP
   TStr7   PVCName_;   //PVC名稱
   TStr10  Broker_;
   TStr1   Market_;
   TStr2   FixPVCID_;  //TMP所對應之FIX PVCID
   TStr2   TmpPVCID_;  //TMP線路PVCID
   TStr1   APCode_;    //TMP作業別, 0=委託下單, 3=成交回報
   TStr8   EffDate_;   //生效日期
   TStr11  LineNo_;    //線路號碼
   TStr4   Passwd_;    //密碼
   TStr5   SrcPort_;   //Socket Port No
   TStr15  IP_;        //IP ADDRESS
   //TStr32  Filler2_;
   TStr1   FixVersion_;//FIX版本別, 1=4.2, 2=4.4, (Type_=F,才需有值)
   TStr31  Filler2_;
  
   K_mf(ESetType)  WhichType() { return ((Type_[0]=='F' || Type_[0]=='K') ? (Type_[0]=='F'?est_Appia:est_Tmp) : est_None);}
   K_mf(std::string)  ToString();
   K_mf(void)  Clear();
};

//FIX系統異動通知(Len=60 + 100)
struct  SK03 : public SK03Data1, public SK03Data2
{   
   K_mf(int)  GetDataSize(int i=0) { return (i==1 ? sizeof(SK03Data1) : sizeof(SK03Data2)); }
};
//----------------------------------------------------------------------------
//校對K03之確認檔Data1(Len=80),18:00後
struct  SK05Data1
{
   TStr8   Date_;      //西元日期
   TStr72  Filler1_;
};

//校對K03之確認檔Data2(Len=80),18:00後
struct  SK05Data2
{
   TStr7       PVCName_;   //PVC名稱
   TStr4       Broker_;
   TStr1       APCode_;    //應用程式代碼
   TStr2       TmpPVCID_;  //TMP線路PVCID
   TStr4       Passwd_;
   TStr8       ProgramID_; //應用程式名稱
   TFreeStr34  PhysicalName_; //實際位置
   TStr5       Node_;      //Pathway系統
   TStr5       Pathway_;   //Pathway名稱
   TStr1       Code_;      //連線傷勢
   TStr2       FixPVCID_;  //TMP所對應之FIX PVCID
   TStr2       FTGruop_;   //路別群組
   TStr1       Type_;      //線路別設定, F=FIX, K=Kway-TMP
   TStr4       Filler2_;

   K_mf(ESetType)  WhichType() { return ((Type_[0]=='F' || Type_[0]=='K') ? (Type_[0]=='F'?est_Appia:est_Tmp) : est_None);}
};

//校對K03之確認檔(Len=80 + 80),18:00後
struct  SK05 : public SK05Data1, public SK05Data2
{
   K_mf(int)  GetDataSize(int i=0) { return i==1 ? sizeof(SK05Data1) : sizeof(SK05Data2); } 
};
//---------------------------------------------------------------------------
//盤別資訊Data1(Len=50),17:00~17:30會進來
struct  SK04Data1
{
   TStr8   Date_;      //西元日期
   TStr8   NextDate_;  //次一交易日
   TStr34  Filler1_;
};

//盤別資訊Data2(Len=50),17:00~17:30會進來
struct  SK04Data2
{
   TStr1   APCode_;       //應用程式代碼
   TStr10  Name_;         //業務名稱
   TStr8   APProgram_;    //應用程式名稱
   TStr6   BeginTime_;    //作業起始時間
   TStr6   EndTime_;      //作業結束時間
   TStr6   SysBeginTime_; //系統執行起始時間
   TStr13  Filler2_;
};

//盤別資訊(Len=50 + 50),17:00~17:30會進來
struct  SK04 : public SK04Data1, public SK04Data2
{
   K_mf(int)  GetDataSize(int i=0) { return (i==1 ? sizeof(SK04Data1) : sizeof(SK04Data2)); }
};
//----------------------------------------------------------------------------
//開始/結束作業Data1(Len=50)
struct  SK06Data1
{
   TStr8   Date_;      //西元日期
   TStr42  Filler1_;
};

//開始/結束作業Data2(Len=50)
struct  SK06Data2
{
   TStr2   WorkCode_;  //作業代碼, 01=假開盤開始, 02=假開盤結束, 03=整股作業起始, 04=日結清檔作業
   TStr8   WorkTime_;  //LOG作業時間
   TStr40  Filler2_;
};

//開始/結束作業(Len=50 + 50)
struct  SK06 : public SK06Data1, public SK06Data2
{
   K_mf(int)  GetDataSize(int i=0) { return (i==1 ? sizeof(SK06Data1) : sizeof(SK06Data2)); }
};
//----------------------------------------------------------------------------
//假開盤股票檔Data1(Len=50)
struct  SK07Data1
{
   TStr8   Date_;      //西元日期
   TStr42  Filler1_;
};

//假開盤股票檔Data2(Len=50)
struct  SK07Data2
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
struct  SK07 : public SK07Data1, public SK07Data2
{
   K_mf(int)  GetDataSize(int i=0) { return (i==1 ? sizeof(SK07Data1) : sizeof(SK07Data2)); }
};
//----------------------------------------------------------------------------
//修改FIX密碼檔Data1(Len=30)
struct  SK08Data1
{
   TStr8   Date_;      //西元日期
   TStr22  Filler1_;
};

//修改FIX密碼檔Data2(Len=30)
struct  SK08Data2
{
   TStr4   Broker_;
   TStr2   FixPVCID_;  //FIX Socket ID
   TStr4   OldPasswd_; //舊密碼
   TStr4   Passwd_;    //新密碼
   TStr16  Filler2_;
};

struct  SK08 : public SK08Data1, public SK08Data2
{
   K_mf(int)  GetDataSize(int i=0) { return i==1 ? sizeof(SK08Data1) : sizeof(SK08Data2); }
};
//----------------------------------------------------------------------------
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
//----------------------------------------------------------------------------
//FIX證券商合併受讓作業檔
struct  SK11Data1  //(Len=50)
{
   TStr8   Date_;      //西元日期
   TStr42  Filler1_;
};

struct  SK11Data2  //(Len=50)
{
   TStr4   BfBroker_;   //原始證券商代號
   TStr4   AfBroker_;   //存續證券商代號
   TStr42  Filler2_;
};

struct  SK11 : public SK11Data1, public SK11Data2
{
   K_mf(int)  GetDataSize(int i=0) { return i==1 ? sizeof(SK11Data1) : sizeof(SK11Data2); }
};
//----------------------------------------------------------------------------
//FIX證券商流量檔
struct  SK12Data1  //(Len=30)
{
   TStr8   Date_;      //西元日期
   TStr4   BaseFlow_;  //基本流量單位值
   TStr18  Filler1_;
};

struct  SK12Data2  //(Len=30)
{
   TStr4   Broker_;    //證券商代號
   TStr2   FixPVCID_;  //FIX Socket ID
   TStr8   TotalFlow_; //總流量值
   TStr16  Filler2_;
};

struct  SK12 : public SK12Data1, public SK12Data2
{
   K_mf(int)  GetDataSize(int i=0) { return i==1 ? sizeof(SK12Data1) : sizeof(SK12Data2); }
};
//----------------------------------------------------------------------------
//FIX版本異動通知檔
struct  SK13Data1  //(Len=50)
{
   TStr8   Date_;      //西元日期
   TStr42  Filler1_;
};

struct  SK13Data2  //(Len=50)
{
   TStr10  Broker_;    //證券商代號
   TStr2   FixPVCID_;  //FIX Socket ID
   TStr1   BfFixVer_;  //FIX版本別, 1=4.2, 2=4.4
   TStr1   AfFixVer_;  //FIX版本別, 1=4.2, 2=4.4
   TStr36  Filler2_;
};

struct  SK13 : public SK13Data1, public SK13Data2
{
   K_mf(int)  GetDataSize(int i=0) { return i==1 ? sizeof(SK13Data1) : sizeof(SK13Data2); }
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
   ft_K01,
   ft_K02,
   ft_K03,
   ft_K04,
   ft_K05,
   ft_K06,
   ft_K07,
   ft_K08,
   ft_K09,
   //ft_K10, //K10與K03相同
   ft_K11,
   ft_K12,
   ft_K13,
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
