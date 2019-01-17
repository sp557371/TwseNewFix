//---------------------------------------------------------------------------
#ifndef bxRptH
#define bxRptH
//---------------------------------------------------------------------------
#include <map>
#include "char_ary.h"
#include "bxDef.h"
//---------------------------------------------------------------------------
namespace Kway {
namespace Tw {
namespace bx {
/////////////////////////////////////////////////////////////////////////////
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
   #pragma pack(push,1)
#else
   #pragma pack(1)
#endif
//---------------------------------------------------------------------------
K_class TR1 : public TTseControlHead            //����^���_�l�@�~�T��
{
   typedef TTseControlHead      inherited;
   typedef TR1                  this_type;
public:
   Kstl::char_ary<char,4,' '>   BrokerID_;
   Kstl::char_ary<char,6,'0'>   StartSeq_;
   K_ctor TR1(TSubSysName);
};
//---------------------------------------------------------------------------
K_class TR2 : public TTseControlHead            //����^���_�l�@�~�^�аT��
{
   typedef TTseControlHead      inherited;
   typedef TR2                  this_type;
public:
   Kstl::char_ary<char,4,' '>   BrokerID_;
   Kstl::char_ary<char,6,'0'>   StartSeq_;
};
//---------------------------------------------------------------------------
struct TRptBody
{
   Kstl::char_ary<char,6,' '>   StkNo_;
   Kstl::char_ary<char,8,'0'>   MthQty_;
   Kstl::char_ary<char,6,'0'>   MthPri_;
   Kstl::char_ary<char,6,' '>   MthTime_;
   Kstl::char_ary<char,2,' '>   MthTimems_;
   Kstl::char_ary<char,1,' '>   ExchageCode_;
   Kstl::char_ary<char,1,' '>   BSCode_;
   Kstl::char_ary<char,5,' '>   OrderNo_;
   Kstl::char_ary<char,7,' '>   IvacNo_;
   Kstl::char_ary<char,1,' '>   OrderType_;
   Kstl::char_ary<char,6,' '>   SeqNo_;
   Kstl::char_ary<char,4,' '>   BrokerID_;
   Kstl::char_ary<char,8,' '>   RecNo_;                 //�����`�ɽs��
   Kstl::char_ary<char,1,' '>   MarkS_;                 //�ɰe���O,�Y���ɰe��"*"
};
//---------------------------------------------------------------------------
K_class TR3 : public TTseControlHead                    //����^�����
{
   typedef TTseControlHead      inherited;
   typedef TR3                  this_type;
public:
   Kstl::char_ary<char,4,'0'>   BodyLength_;
   Kstl::char_ary<char,2,'0'>   BodyCount_;
   TRptBody                     Body_[48];               //�̦h48���w�d�T�w�Ŷ�
};
//---------------------------------------------------------------------------
K_class TR5 : public TTseControlHead    //����^���T�w�s�u�@�~�^�аT��
{
   typedef TTseControlHead      inherited;
   typedef TR5                  this_type;
public:
   K_ctor TR5(TSubSysName);
};
//---------------------------------------------------------------------------
K_class TR6 : public TTseControlHead    //��������^����ƶǰe�T��
{
   typedef TTseControlHead      inherited;
   typedef TR6                  this_type;
public:
   Kstl::char_ary<char,6,'0'>   TotalRec_;              //�������`����
};
//---------------------------------------------------------------------------
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
  #pragma pack(pop)
#else
  #pragma pack()
#endif
/////////////////////////////////////////////////////////////////////////////
} // namespace bx
} // namespace Tw
} // namespace Kway
//---------------------------------------------------------------------------
#endif
