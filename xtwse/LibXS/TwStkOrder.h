//
// TwStk �� [��@�e�U��]Record �� ������ types�Bclasses �w�q
//
// ����: $Id: TwStkOrder.h,v 1.13 2006/04/04 03:46:03 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef TwStkOrderH
#define TwStkOrderH
//---------------------------------------------------------------------------
#include "wise_ptr.h"
//---------------------------------------------------------------------------
#include "KTime.h"
#include "TwStk.h"
#include "TwStkIvacNo.h"
#include "TwStkSaleNo.h"
#include "TwStkDeal.h"
#include "Signon/Policy/UPTwStkScItems.h"
//---------------------------------------------------------------------------
namespace Kway {
namespace Tw {
namespace Stk {
using namespace Kstl;
//---------------------------------------------------------------------------
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(push,1)
#else
#pragma pack(1)
#endif
/////////////////////////////////////////////////////////////////////////////
#ifdef K_EnumPacked_Bug
   typedef byte   EOrdCmdID;
   enum EOrdCmdID_t
#else
   enum EOrdCmdID
#endif
{
      cid_New,
      cid_Qry,
      cid_Del,
      cid_ChgQty,
      cid_ChgPri,
      cid_TriMul,
      cid_TriDiv,
      cid_End,//TOrdCmdID���w�q����
      cid_Bulletin = cid_End  //���i
} K_EnumPacked;
TWSTK_class TOrdCmdID : public EnumT<EOrdCmdID>
{
   typedef EnumT<EOrdCmdID>   inherited;
public:
   inline K_ctor      TOrdCmdID  (EOrdCmdID a = cid_New)  : inherited(a) {}
   K_mf(std::string)  as_string  (bool longStr=false) const;
   K_mf(void)         assign     (const std::string&);
};
//-----------------------------------------------------------------------------
#ifdef K_EnumPacked_Bug
   typedef byte   EBulletinItem;
   enum EBulletinItem_t
#else
   enum EBulletinItem
#endif
{  // ���i����
   bi_TrafficStock,   // ���Ѳ�
   bi_General,        // �@�뤽�i
   bi_TrxSegment,     // ����ɬq�ܧ󤽧i

//����ɬq�ܧ󤽧i,���t�X FlowID_:
// of_P_ing = �ҰʻP����Ҫ��s�u����,�Y�Ұʳs�u����,�h��������ջP����ҳs�u
// of_Q_ing = �Y�Ұʳs�u���\,�h�i�J�����A,�i�}�l�����e�U,�� <�����> �e�U�ɶ��w��
//            of_P_ing,of_Q_ing,����b�P�@���e�U���i
// of_S_ing = �w���������Ҫ��e�U���\�T��,��ܽT�w����ɶ��w��
//            ���ﶵ,�����A�i�H�����i,�� [�e�U�ǰe�y�{�Ҳ�] ��@�өw
// of_Finish  ���L���i
// of_LineBreakBulletin   �_�u���i
//
// �H�W�� [���i�ɾ�] ����,�Ȭ���ĳ,���� [�e�U�ǰe�y�{�Ҳ�] ��@�өw
// ��l�w��U�ؤ��i�����ͮɾ�,�Φ��줽�i�ɪ��B�z,���浹�U�ӼҲզۦ�B�z
};
//-----------------------------------------------------------------------------
#ifdef K_EnumPacked_Bug
   typedef byte   ETrxSegment;
   enum ETrxSegment_t
#else
   enum ETrxSegment
#endif
{  // ����ɬq
   ts_Early,   //����|���}�l.

   ts_Full,    //���.
   ts_FullLate,//��Ѧ��L.

   ts_Odd,     //�s��.     94/12/19:�s�ѩe�U�ɶ��אּ: pm1:40-pm2:30, ��馨��.
   ts_Fix,     //�L��w��.

   ts_OddLate, //�s�Ѧ��L.
   ts_FixLate, //�w�����L.

   ts_Late,    //����������.
};
TWSTK_class TTrxSegment : public EnumT<ETrxSegment>
{
   typedef EnumT<ETrxSegment>   inherited;
public:
   inline K_ctor      TTrxSegment (ETrxSegment a = ts_Full)  : inherited(a) {}
   K_mf(std::string)  as_string   (bool longStr=false) const;
   K_mf(void)         assign      (const std::string&);
};
//---------------------------------------------------------------------------
#ifdef K_EnumPacked_Bug
   typedef char   EBSCode;
   enum EBSCode_t
#else
   enum EBSCode
#endif
{
   bsc_Buy    = 'B',
   bsc_Sell   = 'S',
} K_EnumPacked;
struct TBSCode : public EnumT<EBSCode>
{
 //K_mf(std::string)  as_string  () const;                 �ϥ� EnumT<> ���w�]
 //K_mf(void)         assign     (const std::string&);     �ϥ� EnumT<> ���w�]
};
//---------------------------------------------------------------------------
#ifdef K_EnumPacked_Bug
   typedef char   EOType;
   enum EOType_t
#else
   enum EOType
#endif
{
   ot_Unknow = 0,
   ot_Gn   = '0',  //�{��/���O
   ot_CrA  = '1',  //�N��ĸ�
   ot_DbA  = '2',  //�N��Ĩ�
   ot_CrS  = '3',  //�ۿ�ĸ�
   ot_DbS  = '4',  //�ۿ�Ĩ�
   ot_Tse5 = '5',  //�����-�ɨ��X        (�e����Ү�,������O��'5')
   ot_Tse6 = '6',  //�����-���I�M�Q�ɨ��X(�e����Ү�,������O��'6')

   //����ҨS���o�����w�q,�ҥH�ϥίS��Ÿ�
   ot_CrC  = '%',  //�R��
   ot_DbC  = '^',  //�R��
   ot_GnC  = '&',  //�{�R

   ot_Auct = 'A',  //���
   ot_Lend = 'L',  //�Э�
   ot_Tend = 'T',  //����
} K_EnumPacked;
TWSTK_class TOType : public EnumT<EOType>
{
   typedef EnumT<EOType>   inherited;
public:
   inline K_ctor  TOType  (EOType a = ot_Unknow)   : inherited(a)  {}

   K_mf(std::string)  as_string  () const;  // "0","1","2","3","4","C5","C6","C7","T5","T6"
   K_mf(void)         assign     (const std::string&);
   K_mf(TOType)       GetLegal   () const;

   K_mf(bool)  IsGn  () const  { return(Data_==ot_Gn   ||  Data_==ot_GnC); }
   K_mf(bool)  IsCr  () const  { return(Data_==ot_CrA  ||  Data_==ot_CrS  ||  Data_==ot_CrC); }
   K_mf(bool)  IsDb  () const  { return(Data_==ot_DbA  ||  Data_==ot_DbS  ||  Data_==ot_DbC); }
};
//---------------------------------------------------------------------------
#ifdef K_EnumPacked_Bug
   typedef byte   EOrdFlowID;
   enum EOrdFlowID_t
#else
   enum EOrdFlowID
#endif
{
   of_P_ing,            //�B�z��.
   of_Q_ing,            //�ƶ���.
   of_S_ing,            //�ǰe��.
   of_Sent,             //�A�Ω�[�e�U�n�D]�P[�e�U���\]�q���P�u���i����.
   of_Finish,           //���`����.
   of_AbortOrdClear,    //�e�U�y�{���_-[�s�W]�e�U�L��.
   of_AbortOrdUnknow,   //�e�U�y�{���_-[�s�W/�R��/��q]���p����.
   of_AbortOrdNoChange, //�e�U�y�{���_-[�R��/��q]�e�U���p����.
   of_LineBreakBulletin,//����_�u���i.
} K_EnumPacked;
TWSTK_class TOrdFlowID
{
   union {
      struct {
         byte  Flow_ : 4,  // of_P_ing, of_Q_ing, of_S_ing, of_Finish...
               Step_ : 4;  // 0 = #Finish#, 1-15
      };
      byte     UData_;
   };
public:
   inline explicit K_ctor  TOrdFlowID  (int stepn=0, EOrdFlowID of=of_Finish)
                              : Flow_(of), Step_(static_cast<byte>(stepn))
                              { }

   K_mf(std::string)  as_string  () const;   //1P,1Q,1S,1F,1A...2P,2Q,2S...
   K_mf(void)         assign     (const std::string&);

   inline K_mf(byte)       GetStep    ()             const   { return Step_;                 }
   inline K_mf(EOrdFlowID) GetFlow    ()             const   { return EOrdFlowID(Flow_);     }
   inline K_mf(void)       SetStep    (int n)                { Step_ = static_cast<byte>(n); }
   inline K_mf(void)       SetFlow    (EOrdFlowID a)         { Flow_ = a;                    }
   /** �e�U�y�{����:�i���������,�Φb�Y�B�J���_.
   */
   inline K_mf(bool)       IsAllFinish () const              { return( (Step_ == 0  &&  Flow_ >= of_Finish)
                                                                     || Flow_ > of_Finish); }
   /** �e�U�y�{�������`���槹��.
   */
   inline K_mf(bool)       IsNormalFinish () const           { return(Step_ == 0  &&  Flow_ >= of_Finish); }

   inline K_mf(bool)       operator== (TOrdFlowID a) const   { return UData_==a.UData_;  }
   inline K_mf(bool)       operator!= (TOrdFlowID a) const   { return UData_!=a.UData_;  }
   inline K_mf(bool)       operator<  (TOrdFlowID a) const   { return Step_ == a.Step_
                                                                      ? Flow_     < a.Flow_
                                                                      : (Step_-1) < (a.Step_-1); }
   inline K_mf(bool)       operator>  (TOrdFlowID a) const   { return(a < *this); }
};
//---------------------------------------------------------------------------
#ifdef K_EnumPacked_Bug
   typedef byte   EScCode;
   enum EScCode_t
#else
   enum EScCode
#endif
{  //�����ި�إN�X

   sc_None         =  0,
   sc_NoT30        =  1,  // T30�L���         �z sc_NoT30 �� sc_PriceLimit
   sc_PriceLimit   =  2,  // ����W�L���^��    �| ���F�bT30��Ʋ��`��,���i�j����J
   sc_FullT        =  3,  // ���B���

   sc_Dep1BQty     =  4,  // �浧B/S�i�ƶW�L _Dep1BQty  �h�w���ڨ�
   sc_Dep1SQty     =  sc_Dep1BQty,
   sc_DepNBQty     =  5,  // �h��B/S�i�ƶW�L _DepNBQty  �h�w���ڨ�
   sc_DepNSQty     =  sc_DepNBQty,
   sc_DepNBSQty    =  6,  // �h��B+S�i�ƶW�L _DepNBSQty �h�w���ڨ�
   sc_DepAutoQty   =  7,  // KPTC �� KPTS �i�ƶW�L�w����,�۰ʽվ㦨���n�w�����ƶq
   sc_EchoNone     =  7,  // KPTS �� KPTC

   sc_IvacQtyB     =  8,            // �C���@�b��[�{�R�i][��R�i]�X�p�i�ƭ��B,(���t��R)
   sc_IvacAmtB     =  9,            // �C���@�b��[�{�R�i][��R�i]�X�p�������B,(���t��R)
   sc_Qty1B        = 10,            // �浧        [�{�R�i][��R�i]�i�ƭ��B,(���t��R)
   sc_Amt1B        = 11,            // �浧        [�{�R�i][��R�i]�������B,(���t��R)
   sc_IvacQtyS     = sc_IvacQtyB,   // �C���@�b��[�{��X][���X]�X�p�i�ƭ��B,(���t���)
   sc_IvacAmtS     = sc_IvacAmtB,   // �C���@�b��[�{��X][���X]�X�p�������B,(���t���)
   sc_Qty1S        = sc_Qty1B,      // �浧        [�{��X][���X]�i�ƭ��B,(���t���)
   sc_Amt1S        = sc_Amt1B,      // �浧        [�{��X][���X]�������B,(���t���)
   sc_IvacQtyBS    = 12,            // �C���@�b��[�{B/S][��S][��B]�X�p�i�ƭ��B
   sc_IvacAmtBS    = 13,            // �C���@�b��[�{B/S][��S][��B]�X�p�������B
   sc_CtrlBS       = 14,            // BS�ި�
   sc_CtrlCD       = 15,            // �H�κި�
   sc_CpAmtB       = 16,            // ���q(��@��~���I),�W�L�R�i���B
   sc_CpAmtS       = 17,            // ���q(��@��~���I),�W�L��X���B
   sc_CpQtyB       = 18,            // ���q(��@��~���I),�W�L�R�i���i
   sc_CpQtyS       = 19,            // ���q(��@��~���I),�W�L��X���i
   sc_CpAmtBS      = 20,            // ���q(��@��~���I),�W�LBS���B
   sc_CpQtyBS      = 21,            // ���q(��@��~���I),�W�LBS���i

   sc_CrDbStop     = 22, // �����j���X��
   sc_CrDbQty      = 23, // ���t�B����
   sc_RvDbStop     = 24, // ��R�e�U,���O���鰱��
   sc_RvDbOver     = 25, // ��R�e�U,�W�L����i����

   sc_BalQty       = 26, // �w�s�ѼƤ���
   sc_BalAmt       = 27, // �W�L�H�ίżƤW��
   sc_CrDb1StkAmt  = 28, // �W�L�H�γ�ѤW��
   sc_TotBSAmt     = 29, // ���BS���W��(�«H�B��)
   sc_NoFinaStk    = 30, // �Ҫ��t�B������,�Ч令[�{��]��j��

   sc_CrDbQtyDeal     = 31, // ���t�B����    (���e�U+�w���� > �t�B)
   sc_BalQtyDeal      = 32, // �w�s�ѼƤ���    (���e�U+�w���� > �w�s)
   sc_BalAmtDeal      = 33, // �W�L�H�ίżƤW��(���e�U+�w���� > �H�ίż�)
   sc_CrDb1StkAmtDeal = 34, // �W�L�H�γ�ѤW��(���e�U+�w���� > ��ѤW��)
   sc_TotBSAmtDeal    = 35, // ���BS���W��  (���e�U+�w���� > �«H�B��)

   sc_BalAmtSum       = 36, // �W�L�H�ίżƤW��(���e�U+�`�k��e�U > �H�ίż�)
   sc_CrDb1StkAmtSum  = 37, // �W�L�H�γ�ѤW��(���e�U+�`�k��e�U > ��ѤW��)
   sc_TotBSAmtSum     = 38, // �`�k����W��  (���e�U+�`�k��e�U > �«H�B��)

   sc_AmtBOtc2        = 39, // OTC 2���� �R�i�W�B
   sc_AmtLevel1       = 40, // �e�U���B���޲Ĥ@��
   sc_AmtLevel2       = 41, // �e�U���B���޲ĤG��
   sc_AmtLevel3       = 42, // �e�U���B���޲ĤT��
   sc_Dormant         = 43, // �R���
   sc_GroupAmt        = 44, // 20070820�s�է���B�ױ���
   sc_NoKYC           = 45, // 20080220�ˬdKYC���A
   sc_IvacStk         = 46, // 20090324�ӤH�ӪѦw��

   sc_End     = 8*8,//�w�d0-63,�@64�غި��
   sc_AllPass = 0x7f,
} K_EnumPacked;

//typedef Kway::Signon::Policy::TScLevel    TScLevel;
using ::Kway::Signon::Policy::TScLevel;

TWSTK_class TScCode
{
   byte     ScCode_;
   TScLevel ScLevel_;
public:
   inline K_ctor  TScCode  ()  : ScCode_(sc_AllPass) { }

   inline K_mf(void)      Set        (EScCode scCode, TScLevel scLevel)   { ScCode_ = scCode;  ScLevel_ = scLevel; }
   inline K_mf(EScCode)   Get        () const  { return (EScCode)ScCode_;      }
   inline K_mf(bool)      IsPass     () const  { return ScCode_ == sc_AllPass; }
   inline K_mf(TScLevel)  GetScLevel () const  { return ScLevel_;              }
   /** �����ˬd�q�L.
   */
   inline K_mf(void)      SetPass    ()        { ScCode_ = sc_AllPass;         }
};
typedef std::bitset<sc_End>  TScForces;   //���w�j���X��: ���w�j���Y��[�����ި�إN�X]

TWSTK_gd(TScForces)     SC_CanChgOType;    // �����ި�,��,KPTC�i�����O
TWSTK_gd(TScForces)     SC_CannotTri;      // �պ��,�@�������ި�,�h����պ�
TWSTK_gd(TScForces)     SC_Deposit;        // �w��
TWSTK_gd(TScForces)     SC_TriErrMsg;      // �պ⥢��,�����Ǧ^�����T��
TWSTK_gd(TScForces)     SC_QtyAmt_All;     // ��ѭ�������(sc_IvacQtyB��sc_CpQtyBS)
TWSTK_gd(TScForces)     SC_CpAll;          // ��ѭ�������(sc_CpXXXX)��@��~���I
TWSTK_gd(TScForces)     SC_CrDb;           //sc_NoFinaStk|sc_CrDbStop|sc_CrDbQty
TWSTK_gd(TScForces)     SC_TotBSAmt_All;   //sc_TotBSAmt|sc_TotBSAmtDeal|sc_TotBSAmtSum
TWSTK_gd(TScForces)     SC_CrDbQty_All;    //sc_CrDbQty|sc_CrDbQtyDeal
TWSTK_gd(TScForces)     SC_BalQty_All;     //sc_BalQty|sc_BalQtyDeal
TWSTK_gd(TScForces)     SC_BalAmt_All;     //sc_BalAmt|sc_BalAmtDeal|sc_BalAmtSum
TWSTK_gd(TScForces)     SC_CrDb1StkAmt_All;//sc_CrDb1StkAmt|sc_CrDb1StkAmtDeal|sc_CrDb1StkAmtSum
//---------------------------------------------------------------------------
struct TOrdSource //�e�U�ӷ�
{
   typedef char_ary<char,12,0>   TUserID;
   typedef char_ary<char,40,0>   TClOrdID;
   typedef char_ary<char,1,' '>  TOrderLine;
   TUserID     UserID_;
   TKTime      Time_;      //�D������e�U�n�D���ɶ�
   TClOrdID    ClOrdID_;   //�n�D�ݦۦ�s��,�D���ݤ��@���󪺭ק�,�z�n��J�r���binary��Ƭҥi!
   TOrderLine  OrderLine_;
};
//---------------------------------------------------------------------------
#ifdef K_EnumPacked_Bug
   typedef byte   EOrdEmergency;
   enum EOrdEmergency_t
#else
   enum EOrdEmergency //�e�U���{��
#endif
{
   oe_Lowest  =   0,
   oe_Lower   =  50,
   oe_Normal  = 100,
   oe_Higher  = 150,
   oe_Highest = 255,
} K_EnumPacked;
TWSTK_class TOrdEmergency : public EnumT<EOrdEmergency>
{
   typedef EnumT<EOrdEmergency>   inherited;
public:
   inline K_ctor      TOrdEmergency  (EOrdEmergency a = oe_Normal)  : inherited(a) {}
   inline K_mf(std::string) as_string () const                 { char buf[128]; ltoa(Data_,buf,10); return std::string(buf); }
   inline K_mf(void)        assign    (const std::string& str) { Data_ = static_cast<EOrdEmergency>(atol(str.c_str())); }
};
//---------------------------------------------------------------------------
#ifdef K_EnumPacked_Bug
   typedef byte   ETseOrderLine;
   enum ETseOrderLine_t
#else
   enum ETseOrderLine           //����ҨϥΪ��e�U�޹D�O20080901 eric: 20081006�_�e�U�ʥ]�ݥ[���e�U�ӷ�
#endif
{
   to_Normal    = 0,             //�@��Υ��]�w
   to_Auto      = 1,             //�۰ʤƪA�ȳ]�Ʃe�U�U��
   to_DMA       = 2,             //�q�l���M�ݽu��(DMA)�e�U�U��
   to_Internet  = 3,             //���ں����e�U�U��
   to_Voice     = 4,             //�y���e�U�U��
   to_F_Normal  = 5,             //FIX�@��Υ��]�w
   to_F_Auto    = 6,             //FIX�۰ʤƪA�ȳ]�Ʃe�U�U��
   to_F_DMA     = 7,             //FIX�q�l���M�ݽu��(DMA)�e�U�U��
   to_F_Internet= 8,             //FIX���ں����e�U�U��
   to_F_Voice   = 9,             //FIX�y���e�U�U��
};
//---------------------------------------------------------------------------
TWSTK_class TTseOrderLine : public EnumT<ETseOrderLine>
{
   typedef EnumT<ETseOrderLine>   inherited;
public:
   inline K_ctor      TTseOrderLine   (ETseOrderLine a = to_Normal)  : inherited(a) {}
   K_mf(std::string)  as_string  (bool longStr = true) const;
   K_mf(void)         assign     (const std::string&);
};
//---------------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////
typedef fixed_num<dword>   TSysOrdID;   //�C�ӥ�����x,�̦h4����(1-0xffffffff)�e�U!
typedef fixed_num<byte>    THostID;     //�C�ӥ�����x,�̦h255�x(1-0xff)����D��.
struct TOrigSysOrdID {
   THostID     HostID_;
   TSysOrdID   SysOrdID_;
};
//---------------------------------------------------------------------------
struct TOrdFields_ForNew
{  //�e�U�����-�s�W�e�U��
   TIvacKey       IvacKey_;   //���H�b��(��ӥN��+�Ȥ�b��)
   TOType         OType_;
   TStkNo         StkNo_;
   TStkQty        Qty_;       //��l�e�U(�̤p������ = �@�묰�Ѽ�),�Y==0,��ܩ|������s�W�e�U���^��
   TStkPri        Pri_;       //�e�U��,�Y==0,��ܥu���즨��^��,�|������e�U�^��
   TBSCode        BSCode_;
   TSfGroup       SfGroup_;      //�H�ΰt�B�էO
   TSaleNo        SaleNo_;
   TOrdSource     OrigSource_;   //��l�e�U�ӷ�
   TTrxSegment    TrxSegment_;   //����ɬq
   Kway::byte     Attr_;         //�t�XOMS��: �e�U�ݩ�(TOrderAttr): OmsOrder.h
   Kstl::char_ary<char,1,' '>  TseOrderLine_; //�t�X20081006�e�U�ʥ]�[���ӷ��O, 20100712 �קאּ�����ϥΤ�r���O

   typedef Kstl::char_ary<char,8,' '>  TSubAcc;
   TSubAcc  SubAcc_; ///< ���ݳs���ӥD����,���Ǩ�ӷ|�n�D[�l�b��].

   TTseOrdID      TseOrdID_;     //�e�U�Ѹ�(�d��+�Ǹ�)
   TKDateTimeMS   SendingTime_;

};
//---------------------------------------------------------------------------
struct TStkOrder_Rec : public TOrdFields_ForNew
{  //�����c���]�t���Щʸ��(�p:���Ӫ�),�i�����x�s
   //TSysOrdID    SysOrdID_;  Key��,��iterator���o,�b���B�����O�d�����

   inline K_ctor  TStkOrder_Rec  ()  : LastTseCode_(0)
                                     , Flags_(0)
                                     , LastOkOCmd_(cid_End)
                                     { }

   //�o������ data members �@���T�w��(����Ҧ^��OK��), [���|]�H�۩e�U���A���ܤ�
   TOType         TseOType_;     //�u���b�����[�^�Ц��\��]�~��J���T��OType,����N�A�]���|����!
   TKTime         TseOrigTime_;  //����Ҧ^�Ъ���l�e�U�ɶ�
   TBrokerID      DstBrkID_;     //���d�e�U:�e�U�ت��a���˨��, �W��/�W�d:�e�U�u������ӥN��
   TBrokerID      SrcBrkID_;     //�Y�����b�e�U, IvacKey_ ��[�`���q���b�b��], ���� SrcBrkID_ ���ӳ����b��[�����q�N��]
   TMarket        Market_;
   //���U�Ӫ� data members �|�H�� ����B�R��B��q... �Ӧ��ҧ���
   TKTime         TseLastTime_;  //�̫�@������Ҧ^�Ъ��ɶ�(�R��.��q.�d��...���t����)
   TOrdCmdID      LastOCmd_;     //�̫�@���n�D����Ұ��檺�ާ@
   TOrdCmdID      LastOkOCmd_;   //�̫�@���n�D�����[���\��]���ާ@,�u�i��]�t: cid_End, cid_New, cid_Del, cid_ChgQty
                                 //�R�異��,�_�u,Timeout,<��>�ܰʦ����; �d�ߦ��\��<��>�ܰʦ����.
                                 //������[����ҳs�u�{��]or[�e�U�^���s�u�����{��]���@.

   //93.05.12 ���� TseBfQty_, TseAfQty_ �令�P KPTS �ۦP���w�q
   static inline K_mf(TStkQty)  DiffQty  (TStkQty q1, TStkQty q2)  { return q1 < q2 ? TStkQty(0) : TStkQty(q1-q2); }
   //����ҧ�e�q:
//   inline K_mf(TStkQty)  TseBfQty () const   { return DiffQty(BfQty_, MatchQty_); }
   inline K_mf(TStkQty)  TseBfQty () const   { return TMPBfQty_; }
   //����ҧ��q:
//   inline K_mf(TStkQty)  TseAfQty () const   { return DiffQty(AfQty_, MatchQty_); }
   inline K_mf(TStkQty)  TseAfQty () const   { return TMPAfQty_; }
   //����ҳѾl�q:   or = AfQty_ - max(DealQty_,MatchQty_);
   inline K_mf(TStkQty)  LeftQty  () const   { return DiffQty(AfQty_, DealQty_); }
   //�̫�@���R��q:
   inline K_mf(TStkQty)  LastDecQty () const { return DiffQty(BfQty_, AfQty_); }
   //�֭p�R��q:
   inline K_mf(TStkQty)  TotDecQty () const  { return DiffQty(Qty_, AfQty_); }
   // �p���@�ӹ�Qty���ާ@�N�i�H�Ѧ�KPTS�����X: bxSesApTx.cpp RptDef.cpp

   //�����ҳ̫�@�����q(�s�W�e�U,�d��,�R��,��q)�����G(�tMatchQty_)
   TStkQty        BfQty_;
   TStkQty        AfQty_;
   //MatchQty_: 1.�ھ�[�R��.��q.�d��]���G,�o�쪺����ƶq(�tDealQty_)
   //           2.�����L�D����[�R��^��]��: MatchQty_ = DealQty_
   TStkQty        MatchQty_;
   TStkQty        LostQty_;      //�R���q���_�u,�L�k�P�_�O�_���\���ƶq(93.05.12)
   TStkQty        DealQty_;      //�w���쪺����^��[����ƶq]
   TStkAmt        DealAmt_;      //        ����^��[�֭p����]
   inline K_mf(TStkPri)  GetDealPri () const  { return DealQty_.is_0() ? TStkPri() : TStkPri(DealAmt_ / DealQty_); }

   TOrdFlowID     FlowID_;
   TScCode        ScCode_;       //�̫�@��[�����ި�]���N�X
   TScForces      CfmForces_;    //�w�j���L����������
   TScForces      HisForces_;    //���g�Q�ި�L����������
   union {
      byte           LastTseCode_;  //����Ҧ^�� StatusCode
      EBulletinItem  BuItem_;       //���i����
   };
   TOrdSource     LastSource_;      //�̫�@���e�U�n�D���ӷ�
   TOrdEmergency  Emergency_;       //���{��.�e�U�y�{�i�椤(TFlowStep),���P�_�����M�w�e�U���u���v.
   TOrigSysOrdID  OrigSysOrdID_;    //��l[�t�ΧǸ�]: HostID+SysOrdID. ��J�ɾ�: (1)OrdFlow-NewOrd, (2)�e�U�ѦP�B��.

   typedef char_ary<char,80,0>   TMsg;
   TMsg           LastMsg_;
   TIvacNo        TMPIvacNo_;       //20110407eric Tandem �ݦ^�Ъ��Ȥ�b����������e�Ȥ�b��, FIX �^�Щe�^�����H����쬰��
   TStkQty        TMPBfQty_;        //20110408eric Tandem �ݦ^�Ъ���e�q&���q
   TStkQty        TMPAfQty_;

   //�W�[flag��,�O�o�b�غc�ɳ]�w��l��.
   union {
      byte        Flags_;
      struct {
         byte     IsErOrder_ : 1,   // ���b�ӳ�
                  UnknowNew_ : 1,   // �s�W�e�U���_�u,���p����.
                  ChgPriNew_ : 1,   // �]����Ӳ��ͪ��s�e�U.
                  NewOrdFlow_: 1,   // [�s�e�U�n�D]�y�{�i�椤,UpdateFrom==uf_OrdFlow��,���X�Ф~����.
                  IsBooked_  : 1,   // �۰ʰ�s���e�U�v������s�ʧ@(��/��)
                  Component_ : 1,   // �����ѺX��, �b�s�e�U�ɩβĤ@������ɶ�J
                  ExceedBnd_ : 1,   // �~��Ѳ�����W�L�W�U��
                  WrntMktMkr_: 1;   // 20100623 �v�Ҭy�ʶq���Ѫ̷s���R���q
      };
   };

   //IsTseOK()�ä����iord�������s�W���e�U
   //         (�Ҧp:���i��������L�D�����e�U�^��,�M��b�����@�d��,�M�����Ҧ^��OK!)
   //         �ȯ�T�w�����e�U���g�P����ҷ��q�L,�B����Ҧ^��OK
   inline K_mf(bool)  IsTseOK  () const   { return(TseOType_ != ot_Unknow); }
};
//---------------------------------------------------------------------------
struct TStkOrder : public TStkOrder_Rec
{
   //���Ӫ�,���F�קK�ƻsOrd��,�|�N���Ӫ�ƻs�@��,�ҥH�ϥ� wise_ptr<>, ���� wise_container<>
   // 1.�������,�ϥ�mutable,��ܱz�ק�Deals_,���|���� DealQty_ �P DealAmt_
   //            �z�����ϥ� TOrdCenter ���Ѫ� [����^��]���� �ӳB�z Deals_
   mutable wise_ptr<TStkDeals>  Deals_;
   // 2.�ݳB�z���e�U�n�D
   //   PendingReq_;
};
/////////////////////////////////////////////////////////////////////////////
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(pop)
#else
#pragma pack()
#endif
//---------------------------------------------------------------------------
} // namespace Stk
} // namespace Tw
} // namespace Kway
//---------------------------------------------------------------------------
#endif

