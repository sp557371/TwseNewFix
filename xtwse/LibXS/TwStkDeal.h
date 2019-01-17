//
// TwStk �� [�������]Record �� [��ƪ�] �w�q
//
// ����: $Id: TwStkDeal.h,v 1.2 2004/09/14 02:53:42 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef TwStkDealH
#define TwStkDealH
//---------------------------------------------------------------------------
#include "ev_map.h"
#include "KTime.h"
//---------------------------------------------------------------------------
#include "TwStk.h"
#include "TwStkTseOrdKey.h"
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
struct TStkDeal//����^������
{
   typedef Kstl::fixed_num<dword>   TMSeqNo; //��������Ǹ�
   typedef Kstl::fixed_num<dword>   TBSeqNo; //��Ӧ^���Ǹ�
   typedef TBSeqNo                  TKey;

   TKTime   Time_;   //����Ҽ��X�ɶ�
   TStkPri  Pri_;    //�����
   TStkQty  Qty_;    //����Ѽ�(�̤p������)
   TKey     BSeqNo_; //��Ӧ���^���Ǹ�
// TMSeqNo  MSeqNo_; //��������Ǹ�

#ifdef _MSC_VER
   K_mf(bool) operator==(const TStkDeal& r) const {
	   return memcmp(this, &r, sizeof(r)) == 0;
   }
#endif
};
//---------------------------------------------------------------------------
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(pop)
#else
#pragma pack()
#endif
//---------------------------------------------------------------------------
//����^�����Ӫ�,�C���e�U�@��    ���O�_�i�H��� ev_vector<>
TWSTK_class TStkDeals : public Kstl::ev_map<TStkDeal::TKey,TStkDeal>
{
   TTseOrdKey  TseOrdKey_;
public:
   inline K_mf(TTseOrdKey)  get_master_key  () const                 { return TseOrdKey_; }
   inline K_mf(void)        set_master_key  (const TTseOrdKey& k)    { TseOrdKey_ = k;    }

   //�Ǧ^true��ܧ��result���V���w�����, �Ǧ^false��ܨS���result���V�i�w������m
// K_mf(bool)  FindBSeqNo  (TStkDeal::TBSeqNo, const_iterator& result) const; //��ӧǸ�
   K_mf(bool)  FindMSeqNo  (TStkDeal::TMSeqNo, const_iterator& result) const; //�����Ǹ�
   //�Ǧ^true��ܷs�Wresult���V�s���, �Ǧ^false��ܭ���result���V�¸��
   K_mf(bool)  AddDeal     (TStkDeal::TMSeqNo,  //�����Ǹ�
                            TStkDeal::TBSeqNo,  //��ӧǸ�
                            TKTime,             //���X�ɶ�
                            TStkPri,            //�����
                            TStkQty,            //����Ѽ�(�̤p������)
                            const_iterator& result);
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Stk
} // namespace Tw
} // namespace Kway
//---------------------------------------------------------------------------
#endif
