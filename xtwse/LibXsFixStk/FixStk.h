/////////////////////////////////////////////////////////////////////////////
#if !defined(_FixStk_h_)
#define _FixStk_h_
//---------------------------------------------------------------------------
#include "kutility.h"
#include "fixed_num.h"
#include "FixStkVer.h"

/////////////////////////////////////////////////////////////////////////////
namespace Kway {
   using namespace Kstl;
namespace FIX {
namespace Stk {

/////////////////////////////////////////////////////////////////////////////
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(push,1)
#else
#pragma pack(1)
#endif

/////////////////////////////////////////////////////////////////////////////
// �򥻫��A
/////////////////////////////////////////////////////////////////////////////
typedef fixed_num<Kway::dword>  TFixSysOrdID;
typedef fixed_num<Kway::dword>  TFixSysRptID;
typedef fixed_num<Kway::dword>  TFixSysSeqNo;
typedef fixed_num<Kway::dword>  TFixRefOrdID;  // fix �������e�U�N��

/////////////////////////////////////////////////////////////////////////////
// TUpdFrom
/////////////////////////////////////////////////////////////////////////////
#ifdef K_EnumPacked_Bug
   typedef Kway::byte  TFixUpdFrom;
   enum TFixUpdFrom_t
#else
   enum TFixUpdFrom
#endif
{
   fuf_Unknown,
      //uf_RptOrd,
      //uf_RptDeal,
      //uf_Client,
      fuf_OrdFlow,
      //uf_ViewArea,
} K_EnumPacked;

   
/////////////////////////////////////////////////////////////////////////////
// class TFixOrdCmdID
/////////////////////////////////////////////////////////////////////////////
#ifdef K_EnumPacked_Bug
   typedef Kway::byte  EFixOrdCmdID;
   enum EFixOrdCmdID_t
#else
   enum EFixOrdCmdID
#endif
{
   fcid_Unknown,
      //=================================
      // fix �w�q
      fcid_New,
      fcid_Qry,
      fcid_Del,
      fcid_ChgQty,
      fcid_ChgPri,
      fcid_ChgPriQty,
      
      //=================================
      // �B�~�w�q
      // �̾� Fix �q��, �۰ʲ��ͪ��s�e�U
      fcid_AutoNew,
      // �Q�Ψ�l�޹D�^��, �۰ʲ��ͪ��s�e�U
      fcid_RptNew,

   fcid_Total,
   
   fcid_Start = (fcid_Unknown + 1),
} K_EnumPacked;
//---------------------------------------------------------------------------
K_class TFixOrdCmdID : public EnumT<EFixOrdCmdID>
{
   typedef EnumT<EFixOrdCmdID>  inherited;

public:
   inline K_ctor TFixOrdCmdID (EFixOrdCmdID a = fcid_Unknown) : inherited(a) {}

   K_mf(void) assign (const char*);

   K_mf(std::string) as_string () const;
   K_mf(std::string) as_desc   () const;
      
   inline K_mf(void) assign (const std::string& str)  { assign(str.c_str()); }

   inline K_mf(EFixOrdCmdID) get_orig () const  { return inherited::Data_; }
};


/////////////////////////////////////////////////////////////////////////////
// class TFixOrdFlowID
/////////////////////////////////////////////////////////////////////////////
#ifdef K_EnumPacked_Bug
   typedef Kway::byte  EFixOrdFlowID;
   enum EFixOrdFlowID_t
#else
   enum EFixOrdFlowID
#endif
{
   fof_None,
      fof_P_ing,            //�B�z��.
      //of_Q_ing,            //�ƶ���.
      //of_S_ing,            //�ǰe��.
      fof_Sent,             //�A�Ω�[�e�U�n�D]�P[�e�U���\]�q���P�u���i����.
      fof_Finish,           //���`����.
      fof_AbortOrdClear,    //�e�U�y�{���_-[�s�W]�e�U�L��.
      //of_AbortOrdUnknow,   //�e�U�y�{���_-[�s�W/�R��/��q]���p����.
      fof_AbortOrdNoChange, //�e�U�y�{���_-[�R��/��q]�e�U���p����.
   fof_Total
} K_EnumPacked;
//---------------------------------------------------------------------------
K_class TFixOrdFlowID : public EnumT<EFixOrdFlowID>
{
   typedef EnumT<EFixOrdFlowID>  inherited;
   typedef TFixOrdFlowID         this_type;

public:
   inline K_ctor TFixOrdFlowID (EFixOrdFlowID e = fof_None) : inherited(e)  {}

   K_mf(void) assign (const char*);
   
   K_mf(std::string) as_string () const;

   inline K_mf(void) assign (const std::string& str)  { assign(str.c_str()); }
   
   inline K_mf(EFixOrdFlowID) get_orig () const  { return Data_; }
   inline K_mf(bool)          IsFinish () const  { return (Data_ >= fof_Finish); }
};


/////////////////////////////////////////////////////////////////////////////
// class TFixRptCmdID
/////////////////////////////////////////////////////////////////////////////
#ifdef K_EnumPacked_Bug
   typedef Kway::byte  EFixRptCmdID;
   enum EFixRptCmdID_t
#else
   enum EFixRptCmdID
#endif
{
   frid_Unknown,
      frid_ErOrd,    // Execution Report (order)
      frid_ErReject, // Execution Report (reject)
      frid_ErDeal,   // Execution Report (deal)
      frid_Ocr,      // Order Cancel Reject
   frid_Total,
   
   frid_Start = (frid_Unknown + 1),
} K_EnumPacked;
//---------------------------------------------------------------------------
K_class TFixRptCmdID : public EnumT<EFixRptCmdID>
{
   typedef EnumT<EFixRptCmdID>  inherited;

public:
   inline K_ctor TFixRptCmdID (EFixRptCmdID a = frid_Unknown) : inherited(a) {}

   K_mf(void) assign (const char*);

   K_mf(std::string) as_string () const;

   inline K_mf(void) assign (const std::string& str)  { assign(str.c_str()); }

   inline K_mf(EFixRptCmdID) get_orig () const  { return inherited::Data_; }
   inline K_mf(bool)         is_Er    () const  { return (Data_ == frid_ErOrd || Data_ == frid_ErReject || Data_ == frid_ErDeal); }
   inline K_mf(bool)         is_Ocr   () const  { return (Data_ == frid_Ocr); }
};


/////////////////////////////////////////////////////////////////////////////
// class TBaseFunc
/////////////////////////////////////////////////////////////////////////////
K_class TBaseFunc
{
public:
   static K_mf(std::string) GetVersion    ();
   static K_mf(std::string) GetModifyDate ();
};


/////////////////////////////////////////////////////////////////////////////
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(pop)
#else
#pragma pack()
#endif

/////////////////////////////////////////////////////////////////////////////
}; // namespace Stk
}; // namespace FIX
}; // namespace Kway

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_FixStk_h_)
