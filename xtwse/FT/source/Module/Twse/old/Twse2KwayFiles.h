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
//���սu���]�w���Data1(Len=100)
struct  SK01Data1
{
   TStr8       Date_;      //�褸���
   TStr92      Filler1_;      
};

//���սu���]�w���Data2(Len=100)
struct  SK01Data2
{
   
   TStr8       TestNo_;    //��Ӻݤ����սu��
   TStr5       PVCNo_;     //��Ӻݤ�PVC NO��SOCKET PORT NO
   TStr1       Market_;    //T=TSE, O=OTC, X=��T�洫���x
   TStr1       IP_X25_;    //I=IP, X=X.25
   TStr10      Broker_;    
   TStr4       Passwd_;
   TFreeStr26  PortName_;  //TSE PORT NAME��GATEWAY
   TStr4       ApplySeqNo_;//�ӽЦ���Ǹ�
   TStr15      BrokerIP_;  //���IP ADDRESS
   TStr1       Type_;      //�u���O�]�w, F=FIX, K=Kway-TMP
   TStr2       TmpPVCID_;  //TMP�u��PVCID
   TStr1       APCode_;    //TMP�@�~�O, 0=�e�U�U��, 3=����^��
   TStr2       FixPVCID_;  //TMP�ҹ�����FIX PVCID
   TStr20      Filler2_;

   K_mf(ESetType)  WhichType() { return ((Type_[0]=='F' || Type_[0]=='K') ? (Type_[0]=='F'?est_Appia:est_Tmp) : est_None);}
};

//���սu���]�w���(Len=100 + 100)
struct  SK01 : public SK01Data1, public SK01Data2 
{
   K_mf(int)  GetDataSize(int i=0) { return i==1 ? (sizeof(SK01Data1)) 
                                                 : (sizeof(SK01)-sizeof(SK01Data1)); }
};
//---------------------------------------------------------------------------
//FIX���յ��G�^��(Len=120)
struct  SK02
{
   TStr8       TestNo_;    //��Ӻݤ����սu�� #K01#
   TStr5       PVCNo_;     //��Ӻݤ�PVC NO��SOCKET PORT NO #K01#
   TStr1       Market_;    //#K01#
   TStr1       IP_X25_;    //#K01#
   TStr8       ApplyDate_; //�ӽд��դ��
   TStr10      Broker_;    //#K01#
   TStr4       ApplySeqNo_;//�ӽЦ���Ǹ� #K01#
   TStr4       Passwd_;    //#K01#
   TFreeStr26  PortName_;  //TSE PORT NAME��GATEWAY #K01#
   TStr8       OKDate_;    //���է������
   TStr8       OKTime_;    //���է����ɶ�
   TStr3       Result_;    //���յ��G
   TStr15      BrokerIP_;  //���IP ADDRESS #K01#
   TStr1       Type_;      //�u���O�]�w, F=FIX, K=Kway-TMP #K01#
   TStr2       FixPVCID_;  //TMP�ҹ�����FIX PVCID #K01#
   TStr16      Filler_;
   
   K_mf(int)  GetDataSize(int i=0) { return sizeof(SK02); }
};
//---------------------------------------------------------------------------
//FIX�t�β��ʳq��Data1(Len=60)
struct  SK03Data1
{
   TStr8       Date_;      //�褸���
   TStr52      Filler1_;
};

//FIX�t�β��ʳq��Data2(Len=60)
struct  SK03Data2
{
   TStr1       WorkCode_;  //���ʧO, I=�s�W, D=�R��
   TStr1       Type_;      //�u���O�]�w, F=FIX, K=Kway-TMP
   TStr7       PVCName_;   //PVC�W��
   TStr10      Broker_;
   TStr1       Market_;
   TStr2       FixPVCID_;  //TMP�ҹ�����FIX PVCID
   TStr2       TmpPVCID_;  //TMP�u��PVCID
   TStr1       APCode_;    //TMP�@�~�O, 0=�e�U�U��, 3=����^��
   TStr8       EffDate_;   //�ͮĤ��
   TStr11      LineNo_;    //�u�����X
   TStr16      Filler2_;
  
  K_mf(ESetType)  WhichType() { return ((Type_[0]=='F' || Type_[0]=='K') ? (Type_[0]=='F'?est_Appia:est_Tmp) : est_None);}
};

//FIX�t�β��ʳq��(Len=60 + 60)
struct  SK03 : public SK03Data1, public SK03Data2
{   
   K_mf(int)  GetDataSize(int i=0) { return i==1 ? (sizeof(SK03Data1)) 
                                                 : (sizeof(SK03)-sizeof(SK03Data1)); }
};
//----------------------------------------------------------------------------
//�չ�K03���T�{��Data1(Len=80),18:00��
struct  SK05Data1
{
   TStr8       Date_;      //�褸���
   TStr72      Filler1_;
};

//�չ�K03���T�{��Data2(Len=80),18:00��
struct  SK05Data2
{
   TStr7       PVCName_;   //PVC�W��
   TStr4       Broker_;
   TStr1       APCode_;    //���ε{���N�X
   TStr2       TmpPVCID_;  //TMP�u��PVCID
   TStr4       Passwd_;
   TStr8       ProgramID_; //���ε{���W��
   TFreeStr34  PhysicalName_; //��ڦ�m
   TStr5       Node_;      //Pathway�t��
   TStr5       Pathway_;   //Pathway�W��
   TStr1       Code_;      //�s�u�˶�
   TStr2       FixPVCID_;  //TMP�ҹ�����FIX PVCID
   TStr2       FTGruop_;   //���O�s��
   TStr1       Type_;      //�u���O�]�w, F=FIX, K=Kway-TMP
   TStr4       Filler2_;

   K_mf(ESetType)  WhichType() { return ((Type_[0]=='F' || Type_[0]=='K') ? (Type_[0]=='F'?est_Appia:est_Tmp) : est_None);}
};

//�չ�K03���T�{��(Len=80 + 80),18:00��
struct  SK05 : public SK05Data1, public SK05Data2
{
   K_mf(int)  GetDataSize(int i=0) { return i==1 ? (sizeof(SK05Data1)) 
                                                 : (sizeof(SK05)-sizeof(SK05Data1)); }
};
//---------------------------------------------------------------------------
//�L�O��TData1(Len=50),17:00~17:30�|�i��
struct  SK04Data1
{
   TStr8       Date_;      //�褸���
   TStr8       NextDate_;  //���@�����
   TStr34      Filler1_;
};

//�L�O��TData2(Len=50),17:00~17:30�|�i��
struct  SK04Data2
{
   TStr1       APCode_;    //���ε{���N�X
   TStr10      Name_;      //�~�ȦW��
   TStr8       APProgram_; //���ε{���W��
   TStr6       BeginTime_; //�@�~�_�l�ɶ�
   TStr6       EndTime_;   //�@�~�����ɶ�
   TStr6       SysBeginTime_; //�t�ΰ���_�l�ɶ�
   TStr13      Filler2_;
};

//�L�O��T(Len=50 + 50),17:00~17:30�|�i��
struct  SK04 : public SK04Data1, public SK04Data2
{
   K_mf(int)  GetDataSize(int i=0) { return i==1 ? (sizeof(SK04Data1)) 
                                                 : (sizeof(SK04)-sizeof(SK04Data1)); }
};
//----------------------------------------------------------------------------
//�}�l/�����@�~Data1(Len=50)
struct  SK06Data1
{
   TStr8       Date_;      //�褸���
   TStr42      Filler1_;
};

//�}�l/�����@�~Data2(Len=50)
struct  SK06Data2
{
   TStr2       WorkCode_;  //�@�~�N�X, 01=���}�L�}�l, 02=���}�L����, 03=��ѧ@�~�_�l, 04=�鵲�M�ɧ@�~
   TStr8       WorkTime_;  //LOG�@�~�ɶ�
   TStr40      Filler2_;
};

//�}�l/�����@�~(Len=50 + 50)
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
