//
// TwStk �� [�H�ΰt�B���] �z�L TViewArea ��ܮ�, ��쪺��ܤ覡
//
// ����: $Id: TwStkCreditVA.h,v 1.1.1.1 2003/10/24 15:51:17 cvsroot Exp $
//
//---------------------------------------------------------------------------
#ifndef TwStkCreditVAH
#define TwStkCreditVAH
//---------------------------------------------------------------------------
#include "ViewArea.h"
//---------------------------------------------------------------------------
#include "libnsTwStk.h"
#include "TwStkCredit.h"
//---------------------------------------------------------------------------
namespace Kway {
namespace Tw {
namespace Stk {
/////////////////////////////////////////////////////////////////////////////
enum TStkCreditFiledNo {//�����W�H�Υ������T,�P�Ҫ����q�L��
         scfn_CrStop,   //����
         scfn_CrRate,   //����

         scfn_DbStop,   //����
         scfn_DbRate,   //����

         scfn_End,
};
//---------------------------------------------------------------------------
TWSTK_gd(TFieldDesp)  StkCreditFieldDesp[scfn_End];
//---------------------------------------------------------------------------
TWSTK_class  TStkCreditFieldMakerBase
{
public:
   typedef size_t    field_n_type;

inline K_mf(field_n_type)      GetFieldCount () const               { return numofele(StkCreditFieldDesp); }
inline K_mf(const TFieldDesp&) GetFieldDesp  (field_n_type n) const { return StkCreditFieldDesp[n];        }

       K_mf(std::string)  Get  (const TStkCredit&, field_n_type) const;
       K_mf(void)         Put  (      TStkCredit&, field_n_type, const std::string&);
};
//---------------------------------------------------------------------------
/*
TWSTK_class  TStkCreditFieldMaker : public TStkCreditFieldMakerBase
{
   typedef TStkCreditFieldMakerBase   inherited;
public:
   //���ѵ� TStkCredits �ϥ�
   typedef TStkCredits                TEvContainer;
   typedef TEvContainer::iterator   iterator;

inline K_mf(field_n_type)      GetFieldCount () const   { return numofele(StkCreditFieldDesp)+1; }
       K_mf(const TFieldDesp&) GetFieldDesp  (field_n_type n) const;

   K_mf(std::string)  Get  (           const iterator& iter, field_n_type fieldNo) const;
   K_mf(void)         Put  (TEvContainer& c, iterator& iter, field_n_type fieldNo, const std::string&);
};
*/
/////////////////////////////////////////////////////////////////////////////
} // namespace Stk
} // namespace Tw
} // namespace Kway
//---------------------------------------------------------------------------
#endif
