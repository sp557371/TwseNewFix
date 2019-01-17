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
//���սu���]�w���Data1(Len=100)
struct  SK01Data1
{
   TStr8   Date_;      //�褸���
   TStr92  Filler1_;      
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
   //TStr20      Filler2_;
   TStr1       FixVersion_;//FIX�����O, 1=4.2, 2=4.4, (Type_=F,�~�ݦ���)
   TStr19      Filler2_;

   K_mf(ESetType)  WhichType() { 
      return ((Type_[0]=='F' || Type_[0]=='K') ? (Type_[0]=='F'?est_Appia:est_Tmp) : est_None);
   }
};

//���սu���]�w���(Len=100 + 100)
struct  SK01 : public SK01Data1, public SK01Data2 
{
   K_mf(int)  GetDataSize(int i=0) { return (i==1 ? sizeof(SK01Data1) : sizeof(SK01Data2)); }
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
   //TStr16      Filler_;
   TStr1       FixVersion_;//FIX�����O, 1=4.2, 2=4.4, (Type_=F,�~�ݦ���)
   TStr15      Filler_;
   
   
   K_mf(int)  GetDataSize(int i=0) { return sizeof(SK02); }
};
//---------------------------------------------------------------------------
K_fn(void)  FillChar(std::string& str, int size, char c);
//---------------------------------------------------------------------------
//FIX�t�β��ʳq��Data1(Len=100)
struct  SK03Data1
{
   TStr8   Date_;      //�褸���
   TStr92  Filler1_;
   
   K_mf(std::string)  ToString();
};

//FIX�t�β��ʳq��Data2(Len=100)
struct  SK03Data2
{
   TStr1   WorkCode_;  //���ʧO, I=�s�W, D=�R��
   TStr1   Type_;      //�u���O�]�w, F=FIX, K=Kway-TMP
   TStr7   PVCName_;   //PVC�W��
   TStr10  Broker_;
   TStr1   Market_;
   TStr2   FixPVCID_;  //TMP�ҹ�����FIX PVCID
   TStr2   TmpPVCID_;  //TMP�u��PVCID
   TStr1   APCode_;    //TMP�@�~�O, 0=�e�U�U��, 3=����^��
   TStr8   EffDate_;   //�ͮĤ��
   TStr11  LineNo_;    //�u�����X
   TStr4   Passwd_;    //�K�X
   TStr5   SrcPort_;   //Socket Port No
   TStr15  IP_;        //IP ADDRESS
   //TStr32  Filler2_;
   TStr1   FixVersion_;//FIX�����O, 1=4.2, 2=4.4, (Type_=F,�~�ݦ���)
   TStr31  Filler2_;
  
   K_mf(ESetType)  WhichType() { return ((Type_[0]=='F' || Type_[0]=='K') ? (Type_[0]=='F'?est_Appia:est_Tmp) : est_None);}
   K_mf(std::string)  ToString();
   K_mf(void)  Clear();
};

//FIX�t�β��ʳq��(Len=60 + 100)
struct  SK03 : public SK03Data1, public SK03Data2
{   
   K_mf(int)  GetDataSize(int i=0) { return (i==1 ? sizeof(SK03Data1) : sizeof(SK03Data2)); }
};
//----------------------------------------------------------------------------
//�չ�K03���T�{��Data1(Len=80),18:00��
struct  SK05Data1
{
   TStr8   Date_;      //�褸���
   TStr72  Filler1_;
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
   K_mf(int)  GetDataSize(int i=0) { return i==1 ? sizeof(SK05Data1) : sizeof(SK05Data2); } 
};
//---------------------------------------------------------------------------
//�L�O��TData1(Len=50),17:00~17:30�|�i��
struct  SK04Data1
{
   TStr8   Date_;      //�褸���
   TStr8   NextDate_;  //���@�����
   TStr34  Filler1_;
};

//�L�O��TData2(Len=50),17:00~17:30�|�i��
struct  SK04Data2
{
   TStr1   APCode_;       //���ε{���N�X
   TStr10  Name_;         //�~�ȦW��
   TStr8   APProgram_;    //���ε{���W��
   TStr6   BeginTime_;    //�@�~�_�l�ɶ�
   TStr6   EndTime_;      //�@�~�����ɶ�
   TStr6   SysBeginTime_; //�t�ΰ���_�l�ɶ�
   TStr13  Filler2_;
};

//�L�O��T(Len=50 + 50),17:00~17:30�|�i��
struct  SK04 : public SK04Data1, public SK04Data2
{
   K_mf(int)  GetDataSize(int i=0) { return (i==1 ? sizeof(SK04Data1) : sizeof(SK04Data2)); }
};
//----------------------------------------------------------------------------
//�}�l/�����@�~Data1(Len=50)
struct  SK06Data1
{
   TStr8   Date_;      //�褸���
   TStr42  Filler1_;
};

//�}�l/�����@�~Data2(Len=50)
struct  SK06Data2
{
   TStr2   WorkCode_;  //�@�~�N�X, 01=���}�L�}�l, 02=���}�L����, 03=��ѧ@�~�_�l, 04=�鵲�M�ɧ@�~
   TStr8   WorkTime_;  //LOG�@�~�ɶ�
   TStr40  Filler2_;
};

//�}�l/�����@�~(Len=50 + 50)
struct  SK06 : public SK06Data1, public SK06Data2
{
   K_mf(int)  GetDataSize(int i=0) { return (i==1 ? sizeof(SK06Data1) : sizeof(SK06Data2)); }
};
//----------------------------------------------------------------------------
//���}�L�Ѳ���Data1(Len=50)
struct  SK07Data1
{
   TStr8   Date_;      //�褸���
   TStr42  Filler1_;
};

//���}�L�Ѳ���Data2(Len=50)
struct  SK07Data2
{
   TStr8   Date_;      //�褸���
   TStr6   StkNo_;     //�Ҩ�N��
   TStr6   StkName_;   //�Ҩ�W��
   TStr6   PriUp_;     //������ 9(04)V99
   TStr6   PriRef_;    //�Ѧһ� 9(04)V99
   TStr6   PriDown_;   //�^���� 9(04)V99
//#ifdef __20100825__
   TStr1   TradeOn_;   //�s�ѵ��O, Y��ܥi�e�U
   TStr11  Filler2_;
//#else //__20100825__
//   TStr12      Filler2_;
//#endif //__20100825__
   K_mf(std::string)  as_price(TStr6& p) { std::string a = p.as_string(); return std::string(a.substr(0,4)+"."+a.substr(4,2)); }
};

//���}�L�Ѳ���(Len=50 + 50)
struct  SK07 : public SK07Data1, public SK07Data2
{
   K_mf(int)  GetDataSize(int i=0) { return (i==1 ? sizeof(SK07Data1) : sizeof(SK07Data2)); }
};
//----------------------------------------------------------------------------
//�ק�FIX�K�X��Data1(Len=30)
struct  SK08Data1
{
   TStr8   Date_;      //�褸���
   TStr22  Filler1_;
};

//�ק�FIX�K�X��Data2(Len=30)
struct  SK08Data2
{
   TStr4   Broker_;
   TStr2   FixPVCID_;  //FIX Socket ID
   TStr4   OldPasswd_; //�±K�X
   TStr4   Passwd_;    //�s�K�X
   TStr16  Filler2_;
};

struct  SK08 : public SK08Data1, public SK08Data2
{
   K_mf(int)  GetDataSize(int i=0) { return i==1 ? sizeof(SK08Data1) : sizeof(SK08Data2); }
};
//----------------------------------------------------------------------------
//FIX�M���Y�L�O���~������
struct  SK09  //(Len=50)
{
   TStr4   Broker_;
   TStr2   FixPVCID_;  //FIX Socket ID
   TStr1   APCode_;    //���ε{���N�X
   TStr8   TransTime_; //LOG�@�~�ɶ�
   TStr35  Filler_;
  
   K_mf(int)  GetDataSize(int i=0) { return sizeof(SK09); }
   K_mf(std::string)  ToString();
};
//----------------------------------------------------------------------------
//FIX�Ҩ�ӦX�֨����@�~��
struct  SK11Data1  //(Len=50)
{
   TStr8   Date_;      //�褸���
   TStr42  Filler1_;
};

struct  SK11Data2  //(Len=50)
{
   TStr4   BfBroker_;   //��l�Ҩ�ӥN��
   TStr4   AfBroker_;   //�s���Ҩ�ӥN��
   TStr42  Filler2_;
};

struct  SK11 : public SK11Data1, public SK11Data2
{
   K_mf(int)  GetDataSize(int i=0) { return i==1 ? sizeof(SK11Data1) : sizeof(SK11Data2); }
};
//----------------------------------------------------------------------------
//FIX�Ҩ�Ӭy�q��
struct  SK12Data1  //(Len=30)
{
   TStr8   Date_;      //�褸���
   TStr4   BaseFlow_;  //�򥻬y�q����
   TStr18  Filler1_;
};

struct  SK12Data2  //(Len=30)
{
   TStr4   Broker_;    //�Ҩ�ӥN��
   TStr2   FixPVCID_;  //FIX Socket ID
   TStr8   TotalFlow_; //�`�y�q��
   TStr16  Filler2_;
};

struct  SK12 : public SK12Data1, public SK12Data2
{
   K_mf(int)  GetDataSize(int i=0) { return i==1 ? sizeof(SK12Data1) : sizeof(SK12Data2); }
};
//----------------------------------------------------------------------------
//FIX�������ʳq����
struct  SK13Data1  //(Len=50)
{
   TStr8   Date_;      //�褸���
   TStr42  Filler1_;
};

struct  SK13Data2  //(Len=50)
{
   TStr10  Broker_;    //�Ҩ�ӥN��
   TStr2   FixPVCID_;  //FIX Socket ID
   TStr1   BfFixVer_;  //FIX�����O, 1=4.2, 2=4.4
   TStr1   AfFixVer_;  //FIX�����O, 1=4.2, 2=4.4
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
   //ft_K10, //K10�PK03�ۦP
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
