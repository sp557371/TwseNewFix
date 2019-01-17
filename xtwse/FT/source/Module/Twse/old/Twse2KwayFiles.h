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
//namespace Tw   {
//namespace Bx   {
//namespace FT   {
//---------------------------------------------------------------------------
typedef  Kstl::char_ary<char, 1, ' '>   TStr1;
typedef  Kstl::char_ary<char, 2, ' '>   TStr2;
typedef  Kstl::char_ary<char, 3, ' '>   TStr3;
typedef  Kstl::char_ary<char, 4, ' '>   TStr4;
typedef  Kstl::char_ary<char, 5, ' '>   TStr5;
typedef  Kstl::char_ary<char, 6, ' '>   TStr6;
typedef  Kstl::char_ary<char, 7, ' '>   TStr7;
typedef  Kstl::char_ary<char, 8, ' '>   TStr8;
typedef  Kstl::char_ary<char, 10, ' '>  TStr10;
typedef  Kstl::char_ary<char, 11, ' '>  TStr11;
typedef  Kstl::char_ary<char, 13, ' '>  TStr13;
typedef  Kstl::char_ary<char, 15, ' '>  TStr15;
typedef  Kstl::char_ary<char, 16, ' '>  TStr16;
typedef  Kstl::char_ary<char, 20, ' '>  TStr20;
typedef  Kstl::char_ary<char, 34, ' '>  TStr34;
typedef  Kstl::char_ary<char, 40, ' '>  TStr40;
typedef  Kstl::char_ary<char, 42, ' '>  TStr42;
typedef  Kstl::char_ary<char, 52, ' '>  TStr52;
typedef  Kstl::char_ary<char, 72, ' '>  TStr72;
typedef  Kstl::char_ary<char, 92, ' '>  TStr92;

typedef  Kstl::char_ary<char, 26, 0>   TFreeStr26;
typedef  Kstl::char_ary<char, 34, 0>   TFreeStr34;

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
   TStr8       Date_;      //西元日期
   TStr92      Filler1_;      
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
   TStr20      Filler2_;

   K_mf(ESetType)  WhichType() { return ((Type_[0]=='F' || Type_[0]=='K') ? (Type_[0]=='F'?est_Appia:est_Tmp) : est_None);}
};

//測試線路設定資料(Len=100 + 100)
struct  SK01 : public SK01Data1, public SK01Data2 
{
   K_mf(int)  GetDataSize(int i=0) { return i==1 ? (sizeof(SK01Data1)) 
                                                 : (sizeof(SK01)-sizeof(SK01Data1)); }
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
   TStr16      Filler_;
   
   K_mf(int)  GetDataSize(int i=0) { return sizeof(SK02); }
};
//---------------------------------------------------------------------------
//FIX系統異動通知Data1(Len=60)
struct  SK03Data1
{
   TStr8       Date_;      //西元日期
   TStr52      Filler1_;
};

//FIX系統異動通知Data2(Len=60)
struct  SK03Data2
{
   TStr1       WorkCode_;  //異動別, I=新增, D=刪除
   TStr1       Type_;      //線路別設定, F=FIX, K=Kway-TMP
   TStr7       PVCName_;   //PVC名稱
   TStr10      Broker_;
   TStr1       Market_;
   TStr2       FixPVCID_;  //TMP所對應之FIX PVCID
   TStr2       TmpPVCID_;  //TMP線路PVCID
   TStr1       APCode_;    //TMP作業別, 0=委託下單, 3=成交回報
   TStr8       EffDate_;   //生效日期
   TStr11      LineNo_;    //線路號碼
   TStr16      Filler2_;
  
  K_mf(ESetType)  WhichType() { return ((Type_[0]=='F' || Type_[0]=='K') ? (Type_[0]=='F'?est_Appia:est_Tmp) : est_None);}
};

//FIX系統異動通知(Len=60 + 60)
struct  SK03 : public SK03Data1, public SK03Data2
{   
   K_mf(int)  GetDataSize(int i=0) { return i==1 ? (sizeof(SK03Data1)) 
                                                 : (sizeof(SK03)-sizeof(SK03Data1)); }
};
//----------------------------------------------------------------------------
//校對K03之確認檔Data1(Len=80),18:00後
struct  SK05Data1
{
   TStr8       Date_;      //西元日期
   TStr72      Filler1_;
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
   K_mf(int)  GetDataSize(int i=0) { return i==1 ? (sizeof(SK05Data1)) 
                                                 : (sizeof(SK05)-sizeof(SK05Data1)); }
};
//---------------------------------------------------------------------------
//盤別資訊Data1(Len=50),17:00~17:30會進來
struct  SK04Data1
{
   TStr8       Date_;      //西元日期
   TStr8       NextDate_;  //次一交易日
   TStr34      Filler1_;
};

//盤別資訊Data2(Len=50),17:00~17:30會進來
struct  SK04Data2
{
   TStr1       APCode_;    //應用程式代碼
   TStr10      Name_;      //業務名稱
   TStr8       APProgram_; //應用程式名稱
   TStr6       BeginTime_; //作業起始時間
   TStr6       EndTime_;   //作業結束時間
   TStr6       SysBeginTime_; //系統執行起始時間
   TStr13      Filler2_;
};

//盤別資訊(Len=50 + 50),17:00~17:30會進來
struct  SK04 : public SK04Data1, public SK04Data2
{
   K_mf(int)  GetDataSize(int i=0) { return i==1 ? (sizeof(SK04Data1)) 
                                                 : (sizeof(SK04)-sizeof(SK04Data1)); }
};
//----------------------------------------------------------------------------
//開始/結束作業Data1(Len=50)
struct  SK06Data1
{
   TStr8       Date_;      //西元日期
   TStr42      Filler1_;
};

//開始/結束作業Data2(Len=50)
struct  SK06Data2
{
   TStr2       WorkCode_;  //作業代碼, 01=假開盤開始, 02=假開盤結束, 03=整股作業起始, 04=日結清檔作業
   TStr8       WorkTime_;  //LOG作業時間
   TStr40      Filler2_;
};

//開始/結束作業(Len=50 + 50)
struct  SK06 : public SK06Data1, public SK06Data2
{
   K_mf(int)  GetDataSize(int i=0) { return i==1 ? (sizeof(SK06Data1)) 
                                                 : (sizeof(SK06)-sizeof(SK06Data1)); }
};
//----------------------------------------------------------------------------
struct  SK07
{
   K_mf(int)  GetDataSize(int i=0) { return 0; }
};
//----------------------------------------------------------------------------
struct  SK08
{
   K_mf(int)  GetDataSize(int i=0) { return 0; }
};
//----------------------------------------------------------------------------
#ifdef K_EnumPacked_Bug
   typedef byte   EFileType;
   enum EFileType_t
#else
   enum EFileType
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
} K_EnumPacked;

K_class TFileType : public Kstl::EnumT<EFileType>
{
   typedef  Kstl::EnumT<EFileType>   inherited;
public:
   inline K_ctor  TFileType(EFileType a = ft_None) : inherited(a) {}
   K_mf(std::string)  as_string  () const;
   K_mf(void)  assign(const std::string&);
   K_mf(int)   GetDataSize(int i=0);
};
//----------------------------------------------------------------------------- 
//----------------------------------------------------------------------------
//};  // FT
//};  // Bx
//};  // Tw
};  // Kway
//---------------------------------------------------------------------------
#endif
