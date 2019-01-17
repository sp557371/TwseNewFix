//
// Policy Manager Base - Impl
//
// �קK�b alpha �� DEC UNIX �W, g++ compile �ɰO���餣��:
//    �uvirtual memory exhausted: Not enough space�v
// �ҥH�N TPmBase::TImpl ��X��
//
// ����: $Id: PmBase_Impl.h,v 1.3 2004/07/15 15:38:20 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef PmBase_ImplH
#define PmBase_ImplH
/////////////////////////////////////////////////////////////////////////////
#include "PmBase.h"
#include "inn_file.h"
#include "SimpleRecorder.h"
//---------------------------------------------------------------------------
#include "SignonSvrDB.h"   //K_gd(TExtabUpdaterMgr*) SignonDB_ExtabMgr;
#include "VALogger.h"
#include "SesExtab.h"
//---------------------------------------------------------------------------
using namespace Kway;
using namespace Kway::Signon;
using namespace Kway::Extab;
/////////////////////////////////////////////////////////////////////////////
namespace Kpmb {  // Kway PmBase
//�쥻�o�@�q���w�q�O��b struct TPmGroup::TImpl ����,
//���O,���F��� Symbol ������, �ҥH�W�ߩ�b�@�� namespace
   //----- ��ƫ��O�w�q
   typedef Kstl::rr_inn_file<dword,word>  TInn;
   //------------------------------------------------------------------------
   // Detail: PolicyName-PolicyKey�����X
   //------------------------------------------------------------------------
   #ifdef HANDLE_PRAGMA_PACK_PUSH_POP
   #pragma pack(push,1)
   #else
   #pragma pack(1)
   #endif
   struct TDetailRec
   {
      TPolicyKey  PolicyKey_;
   };
   #ifdef HANDLE_PRAGMA_PACK_PUSH_POP
   #pragma pack(pop)
   #else
   #pragma pack()
   #endif
   //------------------------------------------------------------------------
   struct TDetailRec_RoomKey : public TDetailRec
   {
      TInn::rr_key mutable RoomKey_;
   };
   struct TDetailRecs : public Kstl::ev_map<TPolicyName, const TDetailRec_RoomKey>
   {
      TPmGroupID  MasterKey_;
      inline K_mf(void)               set_master_key  (const TPmGroupID& key)  { MasterKey_ = key;  }
      inline K_mf(const TPmGroupID&)  get_master_key  () const                 { return MasterKey_; }
      inline K_mf(std::string)        GetMasterKeyStr () const                 { return MasterKey_.as_string(); }
   };
   //------------------------------------------------------------------------
   //Detail-�����ܤ�k
   struct TvacDetail : public TVACreater {
      typedef TDetailRecs  TRecs;
      struct  TFieldMaker
      {  //Detail-���:PolicyName,PolicyKey
         typedef TRecs                    TEvContainer;
         typedef TEvContainer::iterator   iterator;
         typedef size_t                   field_n_type;
         TPmMgr*  PmMgr_;

         inline K_ctor  TFieldMaker  (TPmMgr* pmMgr)   : PmMgr_(pmMgr) {}
         inline K_ctor  TFieldMaker  ()                : PmMgr_(0)     {}

         static K_mf(field_n_type)  GetFieldCount  ()    { return 2; }
         K_mf(const TFieldDesp&)    GetFieldDesp   (field_n_type fieldNo) const
            {
               static TFieldDesp fdesp[] = {
                     { "�F���W��", 20, ta_xLeft_yCenter, ta_xLeft_yCenter, tfs_Fixed,   },
                     { "�F���]�w", 16, ta_xLeft_yCenter, ta_xLeft_yCenter, tfs_CanEdit, }
                     };
               switch(fieldNo) {
               case 0: fdesp[fieldNo].GetEditProp_ = (PmMgr_ ? &(PmMgr_->GetEditProp_PolicyNames()) : 0);   break;
               case 1: fdesp[fieldNo].GetEditProp_ = (PmMgr_ ? &(PmMgr_->GetEditProp_PolicyKeys())  : 0);   break;
               }
               return fdesp[fieldNo];
            }

         K_mf(std::string) Get  (const iterator& i, field_n_type fieldNo) const
            {
               switch(fieldNo) {
               case 0:  return i->first.as_string();
               case 1:  return i->second.PolicyKey_.as_string();
               }
               return std::string();
            }
         K_mf(void)  Put  (TEvContainer& evc, iterator& i, field_n_type fieldNo, const std::string& str)
            {
               if(fieldNo==1) {
                  TEvContainer::updater   u = evc.begin_update(i);
                  u->PolicyKey_.assign(str);
               }
            }
      }; // class TFieldMaker
      TPmMgr*    PmMgr_;
      typedef TTVALogger2d<TFieldMaker>   TVALogger;
      TVALogger  VALog_;

      K_ctor  TvacDetail   (TPmMgr* pmMgr, TTableLog2d* log)
                : PmMgr_(pmMgr)
                , VALog_(log, TFieldMaker(pmMgr))
                { }
      K_mf(TaViewArea)  CreateNewVA  ()   { return VALog_.CreateNewVA(0,TFieldMaker(PmMgr_)); }
   };
   typedef TvacDetail::TFieldMaker           TDetailFieldMaker;
   typedef TvacDetail::TVALogger::TViewArea  TDetailVA;
   //========================================================================
   struct TMasterRec : public TDetailRecs
   {
      //TDetailRecs mutable  PolicyGroup_;
      K_mf(const TDetailRecs&)  GetDetail ()  const  { return *this; }
      K_mf(TDetailRecs&)        GetDetail ()         { return *this; }
   };
   typedef ev_map<TPmGroupID,TMasterRec>  TMasterRecs_t;
   K_typedef(TMasterRecs_t,               TMasterRecs);
   typedef TMasterRecs                    TRecs;
   //------------------------------------------------------------------------
   //MasterRecs�����ܤ�k
   struct TFieldMaker
   {
      typedef TRecs                    TEvContainer;
      typedef TEvContainer::iterator   iterator;
      typedef size_t                   field_n_type;

      static K_mf(field_n_type)  GetFieldCount  ()    { return 1; }
      K_mf(const TFieldDesp&)    GetFieldDesp   (field_n_type fieldNo) const
         {
            static TFieldDesp fdesp[] = {
               { "�F���s�N��", 12, Kway::ta_xLeft_yCenter, Kway::ta_xLeft_yCenter, Kway::tfs_Fixed, },
               };
            return fdesp[fieldNo];
         }
      K_mf(std::string) Get  (const iterator& i, field_n_type fieldNo) const
         {
            switch(fieldNo) {
            case 0:  return i->first.as_string();
            }
            return std::string();
         }
      K_mf(void)  Put  (TEvContainer&, iterator&, field_n_type, const std::string&)
         {
         }
   };
   //---------------------------------------------------------------------------
   typedef TTVALogger<TFieldMaker>  TVALogger;
   typedef TVALogger::TViewArea     TMasterVA;
   //========================================================================
   //�w�q Detail ���x�s��k
   struct detail_map2d_maker : public map2d_maker_base<TMasterRecs,TDetailRecs>
   {
             inline K_mf(const detail_map_type*)  get_detail_map   (const master_value_type& v) const { return &(v.second.GetDetail());  }
             inline K_mf(detail_map_type*)        get_detail_map   (      master_value_type& v) const { return &(v.second.GetDetail());  }
      static inline K_mf(detail_map_type*)        fetch_detail_map (const master_iterator&   i)       { return &(i->second.GetDetail()); }
   };
   //typedef TSimpleRecorder_Detail<map2d_maker_key_find<detail_map2d_maker>,TInn,sizeof(TDetailRec)>    TDetailRecorder;
   typedef map2d_maker_key_find<detail_map2d_maker>  KeyFinder_t;
   K_typedef(KeyFinder_t, KeyFinder);
   K_typedef_TSimpleRecorder_Detail(TDetailRecorder, KeyFinder, TInn, sizeof(TDetailRec));
}  // namespace Kpmb
using namespace Kpmb;
/////////////////////////////////////////////////////////////////////////////
class TPmGroup::TImpl
{
public:
   //========================================================================
   //----- �غc
   K_ctor  TImpl  (const std::string& fileName, TPmMgr* pmMgr,
                   const std::string& mGroupName, const std::string& mTableName,
                   const std::string& dGroupName, const std::string& dTableName);
   //---------------------------------------------------------------------------
   K_mf(const TPolicyKey*)  GetPolicy  (const TPmGroupID& gid, const std::string& policyName) const;
   //---------------------------------------------------------------------------
   K_mf(TaViewArea)  CreateNewVA  ()      { return VALog_.CreateNewVA(&Recs_); }
   //---------------------------------------------------------------------------
   #ifdef __TransLnDB__ //�p�G�ݭn�q KPTS��LnDB ��J�n�J��Ʈw��,�~�ݭn�}�񩳤U�� members, ���z�����ۤv�����L
   K_mf(std::string)  LoadLnDB  (const std::string& pathLnDB);
   #endif
   //========================================================================
   //----- data members
   #ifndef __TransLnDB__
   TExtabLogUpdater<TExtabLog,  TTExtabUpdater<TFieldMaker> >           MasterLog_;
   TExtabLogUpdater<TExtabLog2d,TTExtabUpdater2d<TMasterRecs,
                                                 TDetailFieldMaker> >   DetailLog_;
   #endif
   TVALogger                     VALog_;
   TRecs                         Recs_;
   TVAEditProp_GetKeys<TRecs>    VAEditProp_ForKeys_;  //���o������ Master-Keys, ����J[�F���s�N��]�ɧ@[�C�|]��
   TDetailRecorder               DetailRecorder_;
   TvacDetail                    DetailVACreater_;
};
/////////////////////////////////////////////////////////////////////////////
#endif
