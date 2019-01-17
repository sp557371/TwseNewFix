//
// TwStk ���� classes �� types
//
// ����: $Id: TwStk.h,v 1.4 2005/12/28 08:57:38 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef TwStkH
#define TwStkH
//---------------------------------------------------------------------------
#include "char_ary.h"
#include "fixed_num.h"
#include "kutility.h"
#include "AlphaSeq.h"
#include "libnsTwStk.h"
//---------------------------------------------------------------------------
namespace Kway {
namespace Tw {
namespace Stk {
/////////////////////////////////////////////////////////////////////////////
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(push,1)
#else
#pragma pack(1)
#endif
//---------------------------------------------------------------------------
typedef Kstl::char_ary<char,6,' '>  TStkNo;
typedef Kstl::char_ary<char,8,0>    TStkName;
typedef Kstl::fixed_dec<uint32,2>   TStkPri; //���椣�i�� < 0, �ҥH�ϥ� uint32
typedef Kstl::fixed_num<int32>      TStkQty; //�ƶq[�i��] < 0
typedef Kstl::fixed_dec<int64,2>    TStkAmt; //����[�i��] < 0
typedef Kstl::char_ary1<char,0>     TSfGroup;//[�H�ΰt�B�էO]:0-9,A-Z,a-z=�էO,
                                             //Space,Null=�����q������, '-'=BranchNo
//---------------------------------------------------------------------------
inline TStkAmt operator*  (const TStkQty& qty, const TStkPri& pri)  { return(TStkAmt(qty,0) *= pri); }
inline TStkAmt operator*  (const TStkPri& pri, const TStkQty& qty)  { return(TStkAmt(qty,0) *= pri); }
inline TStkQty operator/  (const TStkAmt& amt, const TStkPri& pri)  { return TStkQty(static_cast<TStkQty::orig_type>(amt.get_orig()/pri.get_orig())); }
inline TStkPri operator/  (const TStkAmt& amt, const TStkQty& qty)  { return TStkPri(static_cast<TStkPri::orig_type>(amt.get_orig()/static_cast<TStkQty::orig_type>(qty)), 2); }
//---------------------------------------------------------------------------
#ifdef K_EnumPacked_Bug
   typedef char   EMarket;
   enum EMarket_t
#else
   enum EMarket
#endif
{
      m_Tse    = 'T',  //�W��
      m_Otc    = 'O',  //�W�d
      m_OtcR   = 'R',  //���d
      m_Unknow = '?',  //�����������O.
} K_EnumPacked;
TWSTK_class TMarket : public Kstl::EnumT<EMarket>
{
   typedef Kstl::EnumT<EMarket>  inherited;
public:
   inline K_ctor      TMarket    (EMarket m = m_Tse)    : inherited(m)                      { }
          K_ctor      TMarket    (const TStkNo&);     //: inherited(GetMarket(stkNo).Data_) { }
   K_mf(std::string)  as_string  (bool longStr=false) const;
 //K_mf(void)         assign     (const std::string&); �ϥ� EnumT<> ���w�] assign()

   static K_mf(TMarket)  DefaultGetMarket  (const TStkNo&);//�w�]���o StkBase, �M��Ǧ^ Market_
   //�o�ˡ��w�q�b gcc ����! �]�� TMarket ���Ǧ^��!!  :(
   // typedef K_mf(TMarket)  (*TGetMarket)  (const TStkNo&);
   //�ҥH�u�n��b class TMarket ���~���w�q
};
typedef TMarket  (K_FASTCALL *TGetMarket)  (const TStkNo&);//�w�]���o StkBase, �M��Ǧ^ Market_
TWSTK_gd(TGetMarket)               GetMarket;
//---------------------------------------------------------------------------
#ifdef K_EnumPacked_Bug
   typedef char   EStkKind;
   enum EStkKind_t
#else
   enum EStkKind
#endif
{
      sk_Normal    = ' ',  //�@��Ѳ�
      sk_Special   = 'A',  //�@��S�O��
      sk_StkBond   = 'B',  //�ഫ���q��
      sk_CBond     = 'c',  //���q��
      sk_BeneCert  = 'C',  //���q����(Beneficiary Certificate)
      sk_ETF       = 'E',  //���ƪѲ������(ETF)
      sk_Foreign   = 'F',  //�~��Ѳ�
      sk_ForeBond  = 'f',  //�~��Ũ�
      sk_GovBond   = 'G',  //�����n������
      sk_ChgStk    = 'R',  //�����v�Q�Ү�
      sk_TDR       = 'T',  //�s�U����(TDR)
      sk_Wrnt      = 'W',  //�{��(��)�v��(Warrant)
      sk_Otc2      = '2',  //�G����
      sk_GBond     = 'b',  //���{���v(G)���q��
      sk_GWrnt     = 'w',  //�{���v����
      sk_GExecBond = 'e',  //���{���v(G)�i���ᤧ���q��
      sk_GSpecial  = 'g',  //���{���v(G)�S�O��
      sk_GToken    = 't',  //���{���v(G)�i����Ѵ�ú�Ǿ���
      sk_REITs     = 'r',  //���ʲ��Ҩ��
} K_EnumPacked;
TWSTK_class TStkKind : public Kstl::EnumT<EStkKind>
{
   typedef Kstl::EnumT<EStkKind>    inherited;
public:
          K_mf(std::string)  as_string  (bool longStr=false) const;
   inline K_mf(void)         assign     (const std::string& s)    { Data_ = (EStkKind)(*s.c_str()); }
};
//---------------------------------------------------------------------------
//���F²��Client�d�߮ɪ��B�z:�N���TStkBase�����ǰe
//�ҥH�ϥ� #pragma pack ���ﶵ
struct TStkBase
{
   TStkName    Name_;
   TMarket     Market_;    //�������:�W��/�W�d/���d...
   TStkKind    Kind_;
   TStkPri     PriRef_;
   TStkPri     PriUpLmt_;
   TStkPri     PriDnLmt_;
   TStkPri     PriClose_;  //���馬�L��
   TStkPri     PriOdd_;    //�s�ѰѦһ�

   //�Ǧ^ TStkBase* ���M���|�y�� gcc �����D, �����F�@�P��(�P TMarket �ۦP)���g�k
   //�ҥH�٬O�N���U���w�q��� struct TStkBase �~��
   //static K_mf(const TStkBase*)  (*GetStkBase)  (const TStkNo&);
};
typedef const TStkBase*  (K_FASTCALL *TGetStkBase)  (const TStkNo&);
TWSTK_gd(TGetStkBase)                  GetStkBase;
typedef K_mf(const dword)    (*TGetStkUnitQty)  (const TStkNo&);
TWSTK_gd(TGetStkUnitQty)             GetStkUnitQty;
//---------------------------------------------------------------------------
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(pop)
#else
#pragma pack()
#endif
//---------------------------------------------------------------------------
// typedef Kstl::ev_map<TStkNo, const TStkBase>  TStkBases;
/////////////////////////////////////////////////////////////////////////////
} // namespace Stk
} // namespace Tw
} // namespace Kway
//---------------------------------------------------------------------------
#endif

