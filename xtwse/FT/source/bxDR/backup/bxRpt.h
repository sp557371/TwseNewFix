//---------------------------------------------------------------------------
#ifndef bxRptH
#define bxRptH
//---------------------------------------------------------------------------
#include <map>
#include "bxHeadDef.h"
#include "bxLink.h"
//---------------------------------------------------------------------------
namespace Kway  {
namespace Tw    {
namespace Bx    {
namespace Rpt   {
#pragma pack(1)
/////////////////////////////////////////////////////////////////////////////
static const TSubSys  snRPTTse   = 50;  // TSE ����^�� Sub-System Code
static const TSubSys  snRPTOtc   = 95;  // OTC ����^�� Sub-System Code
static const TFunCode ckRPTLink  = 00;  // ����^���_�l�@�~ R1/R2/R4/R5
static const TFunCode ckRPTData  = 10;  // ����^�����     R3
static const TFunCode ckRPTEnd   = 20;  // ����^�������@�~ R6

static const TBxMsg mgR1 = 00;    // �_�l�T��
static const TBxMsg mgR2 = 01;    // �_�l�^�аT��
static const TBxMsg mgR3 = 00;    // ����^����ưT��
static const TBxMsg mgR4 = 04;    // �T�w�s�u�@�~�T��
static const TBxMsg mgR5 = 05;    // �T�w�s�u�@�~�^�аT��
static const TBxMsg mgR6 = 00;    // ��������^���T��

typedef Kstl::char_ary<char,6,'0'>  TSeqNo;
typedef Kstl::char_ary<char,4,'0'>  TLength;
typedef Kstl::char_ary<char,2,'0'>  TCount;
typedef Kstl::char_ary<char,6,'0'>  TRecs;
//---------------------------------------------------------------------------
struct TRptBody
{
   typedef Kstl::char_ary<char,7,'0'>   TIvacNo;
   typedef Kstl::char_ary<char,6,' '>   TStkNo;
   typedef Kstl::char_ary<char,6,'0'>   TPri;
   typedef Kstl::char_ary<char,1,' '>   TBSCode;
   typedef Kstl::char_ary<char,5,'0'>   TOrdNo;
   typedef Kstl::char_ary<char,8,'0'>   TRecNo;
   typedef Kstl::char_ary<char,1,' '>   TMark;
   typedef Kstl::char_ary<char,1,' '>   TOrdType;
   typedef Kstl::char_ary<char,1,' '>   TExcCode;
   typedef Kstl::char_ary<char,6,'0'>   TTime;
   typedef Kstl::char_ary<char,2,'0'>   TTimems;
   typedef Kstl::char_ary<char,8,'0'>   TQty;


   TStkNo   StkNo_;     // [�Ҩ�N��]�A�����Ʀr
   TQty     MthQty_;    // [����ƶq]�A�K���Ʀr�A��ܦ���i��(���q��/�~��Ѳ�)�Φ���Ѽ�(�s��)
   TPri     MthPri_;    // [�������]�A�|���ơA�G��p�ơA��쬰���C
   TTime    MthTime_;   // [����ɶ�]�A�K��Ʀr�A��ɡA���A��A�L��C
   TTimems  MthTimems_;
   TExcCode ExcCode_;   // [�������]�A�@��Ʀr�A��ܥ������ 0�J���q�ѥ���Υ~��Ѳ���� 1�J�d�B��� 2�J�s�ѥ��
   TBSCode  BSCode_;    // [�R��O]�A�@���Ʀr B�J�R�AS�J��
   TOrdNo   OrdNo_;     // [�e�U�ѽs��]�A�t�U�C�G��줺�e�G�׺ݾ��N���G�@���Ʀr   �e�U�ѽs���G�|��Ʀr
   TIvacNo  IvacNo_;    // [���H�b��]�A�C��Ʀr�C
   TOrdType OrdType_;   // [�e�U����]�A�@��Ʀr�C
   TSeqNo   SeqNo_;     // [�y����]�A����Ʀr�A��ܦ���^�������ǡC
   TBrkID   BrkID_;     // [�Ҩ�ӥN��]�A�|���Ʀr�C
   TRecNo   RecNo_;     // [�����`�ɽs��]�A�K��Ʀr�A��ܦ��������Ʀb�����`�ɤ��Ǹ��C
   TMark    MarkS_;     // [�ɰe���O]�A�@���r�A�ɰe�������ơA����줺�e��"*"�C
};
//---------------------------------------------------------------------------
K_class TR1R2 : public TControlHead     //����^���_�l�@�~�T��(�^��) R1/R2
{
   typedef TControlHead inherited;
   typedef TR1R2        this_type;
public:
   TBrkID   BrkID_;
   TSeqNo   StartSeq_;
};
//---------------------------------------------------------------------------
K_class TR3 : public TControlHead       //����^����� R3
{
   typedef TControlHead inherited;
   typedef TR3          this_type;
public:
   TLength  BodyLength_;
   TCount   BodyCount_;
   TRptBody Body_[4];   //�̦h�|���w�d�T�w�Ŷ�
   #define szTR3_Head   (sizeof(TControlHead) + sizeof(TLength) + sizeof(TCount))
};
//---------------------------------------------------------------------------
K_class TR4R5 : public TControlHead {}; //����^���T�w�s�u�@�~�^�аT�� R4/R5
//---------------------------------------------------------------------------
K_class TR6 : public TControlHead       //��������^����ƶǰe�T��  R6
{
   typedef TControlHead inherited;
   typedef TR6          this_type;
public:
   TRecs    TotalRec_;  //�������`����
};
//---------------------------------------------------------------------------
#pragma pack()
}; // Rpt
}; // Bx
}; // Tw
}; // Kway
//---------------------------------------------------------------------------
#endif
