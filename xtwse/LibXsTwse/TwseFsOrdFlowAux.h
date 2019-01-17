/////////////////////////////////////////////////////////////////////////////
#if !defined(_TwseFsOrdFlowAux_h_)
#define _TwseFsOrdFlowAux_h_
//---------------------------------------------------------------------------
#include "TwseFsOrdFlow.h"
#include "tplns/Nsa_EvMap_2k.h"

/////////////////////////////////////////////////////////////////////////////
namespace Kway {
   using namespace Nsa;

/////////////////////////////////////////////////////////////////////////////
// class TTwseFsOrdFlow::TRsOrdID
/////////////////////////////////////////////////////////////////////////////
// ���ϥ� recorder, �b�C���Ұʮɱq FixOrdCenter ���J�Y�i
class TTwseFsOrdFlow::TRsOrdID
{
public:
   typedef TEvMap2k<TBrkOrdID, TFixSysOrdID>  TRecsType;
   typedef TRecsType::TMap1      TRsBrkOrdID;
   typedef TRecsType::TDat1      TRsBrkOrdIDDat;
   typedef TRecsType::TMap2      TRsFixSoid;
   typedef TRsFixSoid::iterator  TRsFixSoidI;

private:
   class TFieldMaker1;
   class TFieldMaker2;
   class TRsVac;

   TRsBrkOrdID   RsFull_;
   TRsBrkOrdID   RsOdd_;
   TRsBrkOrdID   RsFixed_;
   TViewAreaMgr  VaMgr_;
   TRsVac*       RsVacFull_;
   TRsVac*       RsVacOdd_;
   TRsVac*       RsVacFixed_;

public:
   K_ctor  TRsOrdID (const char*);
   K_dtor ~TRsOrdID ();

   // �����s�W���
   K_mf(void) Insert (const TTrxSegment&, const TBrkOrdID&, const TFixSysOrdID&);

   // �ˬd�O�_�����Ъ�[���ӥN��+�e�U�Ѹ�],�p�G�����Ъ��h�����^�ǥ���
   K_mf(bool) CheckInsert (const TTrxSegment&, const TBrkOrdID&, const TFixSysOrdID&);

   K_mf(TRsFixSoid*) FindRsFixSoid (const TTrxSegment&, const TBrkOrdID&);
   
   K_mf(TVACreater&) GetVaMgr ();
};

/////////////////////////////////////////////////////////////////////////////
// class TTwseFsOrdFlow::TOrdFdChk
/////////////////////////////////////////////////////////////////////////////
// > �e�U���e���
class TTwseFsOrdFlow::TOrdFdChk
{
   struct SConf
   {
      bool  ChkPrice_;
      bool  ChkAccount_;
      
      inline K_ctor SConf ()
         : ChkPrice_  (true)
         , ChkAccount_(true)
      {}
   };

   ETrxSegment            TrxSeg_;
   const TFsStkOrder&     FixOrd_;
   const SFixOfOrdField&  FixOrdField_;
   bool   IsChg_;
   bool   IsDel_;
   bool   IsQry_;
   SConf  Conf_;
   char   ErrField_[50];
   char   ErrDesc_ [128];

public:
   K_ctor TOrdFdChk (const TTrxSegment&, EFixOrdCmdID, const TFsStkOrder&, const SFixOfOrdField&);

   // �}�l�ˬd
   K_mf(bool) DoChk ();
   
   // ���o���~�T��
   K_mf(const char*) GetErrDesc (const TFixSysOrdID&, const TBrkOrdID&);

private:
   K_mf(bool) ChkField ();
};

/////////////////////////////////////////////////////////////////////////////
} // namespace Kway

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_TwseFsOrdFlowAux_h_)
