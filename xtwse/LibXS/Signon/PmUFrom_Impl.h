//
// Policy Manager - UserFrom (�u�@���ӷ��޲z) - Impl
//
// �קK�b alpha �� DEC UNIX �W, g++ compile �ɰO���餣��,
// �ҥH�N TPmUFromGroup::TImpl ��X��
//
// ����: $Id: PmUFrom_Impl.h,v 1.4 2005/04/26 06:55:29 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef PmUFrom_ImplH
#define PmUFrom_ImplH
/////////////////////////////////////////////////////////////////////////////
#include "PmUFrom.h"
#include "VALogger.h"
#include "SesExtab.h"
#include "SignonSvrDB.h"   //K_gd(TExtabUpdaterMgr*) SignonDB_ExtabMgr;
//---------------------------------------------------------------------------
using namespace Kway;
using namespace Kway::Signon;
/////////////////////////////////////////////////////////////////////////////
namespace Kpui {  // Kway PmUserFrom Impl
//---------------------------------------------------------------------------
#ifdef __alpha    // alpha�W,�Y�u�W�٥�
#define TUFGroupRec              Tufgr
#define TUFGroupsRec             Tufgs
#define TUFGroupKinds_KeyFinder  Tufgkf
#endif
//---------------------------------------------------------------------------
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(push,1)
#else
#pragma pack(1)
#endif
struct TUFGroupRec
{
   //�ثe�S��������,
   //  GroupKind - [TUFGroup-TUFGroupRec]*n��
};
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(pop)
#else
#pragma pack()
#endif
//---------------------------------------------------------------------------
// �w�q�s�ɤ覡(�ɮ׳̤j4G(dword),�C��Rec�̤j64K(word))
typedef Kstl::rr_inn_file<dword,word>                                      TInn;
typedef TSimpleMap_AddRoomKey<TUFGroup,TUFGroupRec,TInn::rr_key>::TEvRecs  TUFGroupsBase;
//---------------------------------------------------------------------------
class TUFGroupsRec : public TUFGroupsBase
{
   std::string mutable  PolicyResult_; //�Φb GetPolicy() ���Ǧ^��
public:
   K_mf(TPolicyBaseRef)  GetPolicy  () const
      {
         PolicyResult_ = GetGroups(0);
         return TPolicyBaseRef(PolicyResult_.data(),
                               static_cast<TPolicyBaseRef::second_type>(PolicyResult_.length()));
      }
   K_mf(std::string)  GetGroups  (char spl) const
      {
         std::string      result;
         bool             isFirst = true;
         const_iterator   iend = end();
         for(const_iterator  i = begin();  i != iend;  ++i) {
            const TUFGroup&   group = i->first;
            if(spl==0)
               result.insert(result.end(), group.begin(), group.end());
            else {
               if(isFirst)
                  isFirst = false;
               else
                  result += spl;
               result += group.as_string();
            }
         }
         return result;
      }

   typedef TUFGroupKind    TMasterKey;
   TMasterKey  MasterKey_;
   inline K_mf(const TMasterKey&)  get_master_key  () const                   { return MasterKey_; }
   inline K_mf(void)               set_master_key  (const TMasterKey& mkey)   { MasterKey_ = mkey; }
   inline K_mf(std::string)        GetMasterKeyStr () const                   { return MasterKey_.as_string(); }
};
typedef Kstl::ev_map<TUFGroupKind,TUFGroupsRec>    TUFGroupKinds_t; //master
K_typedef(TUFGroupKinds_t,                         TUFGroupKinds);
//---------------------------------------------------------------------------
//typedef TSimpleRecorder_Detail<map2d_maker_key_find<map2d_maker_base<TUFGroupKinds> >,
//                               TInn, sizeof(TUFGroupRec)//�n�s�JInn����Ƥj�p
//                               >      TUFGroupKinds_Recorder;
typedef map2d_maker_key_find<map2d_maker_base<TUFGroupKinds> >    TUFGroupKinds_KeyFinder_t;
K_typedef(TUFGroupKinds_KeyFinder_t, TUFGroupKinds_KeyFinder);

K_typedef_TSimpleRecorder_Detail(TUFGroupKinds_Recorder,
                                 TUFGroupKinds_KeyFinder,
                                 TInn,
                                 sizeof(TUFGroupRec));
//===========================================================================
//�����ܤ�k
struct TFieldMaker : public TVAKeyFieldMaker<TUFGroupKinds>
{  //Master-����ܸs�դ���
   K_mf(const TFieldDesp&)  GetFieldDesp   (field_n_type n) const
   { static TFieldDesp fdesp = { "�s�դ���", 12, Kway::ta_xLeft_yCenter, Kway::ta_xLeft_yCenter, Kway::tfs_Fixed, };
     return fdesp; }
}; // class TFieldMaker
//---------------------------------------------------------------------------
//Detail-�����ܤ�k
struct TvacDetail : public TVACreater {
   typedef TUFGroupsRec    TRecs;
   struct TFieldMaker : public TVAKeyFieldMaker<TRecs>
   {  //Detail-����ܸs�եN�X
      K_mf(const TFieldDesp&)  GetFieldDesp   (field_n_type n) const
      { static TFieldDesp fdesp = { "�s��", 4, Kway::ta_xLeft_yCenter, Kway::ta_xLeft_yCenter, Kway::tfs_Fixed, };
        return fdesp; }
   }; // class TFieldMaker
   typedef TTVALogger2d<TFieldMaker>   TVALogger;
   TVALogger  VALog_;
   K_ctor            TvacDetail   (TTableLog2d* log)  : VALog_(log) {}
   K_mf(TaViewArea)  CreateNewVA  ()   { return VALog_.CreateNewVA(0); }
};
typedef TvacDetail::TVALogger::TViewArea  TDetailVA;
typedef TvacDetail::TFieldMaker           TDetailFieldMaker;
}  // namespace Kpui
using namespace Kpui;
//---------------------------------------------------------------------------
class  TPmUFromGroup::TImpl : public  TUFGroupKinds,
                              private TUFGroupKinds_Recorder,
                              public  TPmBaseVA
{
   typedef TUFGroupKinds_Recorder   InheritedRecorder;
   typedef TUFGroupKinds            TMasterRecs;
   typedef TMasterRecs              TRecs;
   typedef TUFGroupsRec             TDetailRecs;

public:
   typedef TMasterRecs::iterator          iterator;
   typedef TMasterRecs::const_iterator    const_iterator;

   K_ctor  TImpl  (const std::string& defaultPath);

 //K_mf(TPolicyBaseRef)  GetPolicy  (const TUFGroupKind& gkind) const
   K_mf(TPolicyBaseRef)  GetPolicy  (const TPolicyKey&   gkind, bool forLocalUse) const
      {
         const_iterator   ifind = find(gkind);
         if(ifind==end())
            switch(gkind[0]) {
            case ':':   return TPolicyBaseRef(gkind.begin()+1,sizeof(TUFGroup));
            case '*':   return TPolicyBaseRef(gkind.begin(),  sizeof(TUFGroup));
            default:    return TPolicyBaseRef(0,0);
            }
         return ifind->second.GetPolicy();
      }

   //���o������ GroupKinds ��@��J�ɪ�[�C�|]
   TVAEditProp_GetKeys<TMasterRecs>    VAEditProp_ForKinds_;

   //���U��Ʋ��ʨƥ�
   inline K_mf(void)  GroupKinds_add_ev_handler  (Kstl::container_ev_handler* a)    { Recs_.add_ev_handler(a); }
   inline K_mf(void)  GroupKinds_del_ev_handler  (Kstl::container_ev_handler* a)    { Recs_.del_ev_handler(a); }

   #ifdef __TransLnDB__ //�p�G�ݭn�q KPTS��LnDB ��J�n�J��Ʈw��,�~�ݭn�}�񩳤U�� members, ���z�����ۤv�����L
   K_mf(std::string)  LoadLnDB  (const std::string& pathLnDB);
   #endif

private:
   K_mf(TMasterRecs*)  get_recs  ()  { return this; }
   //---------------------------------------------------------------------------
   #ifndef __TransLnDB__
   TExtabLogUpdater<TExtabLog,  TTExtabUpdater<TFieldMaker> >           MasterLog_;
   TExtabLogUpdater<TExtabLog2d,TTExtabUpdater2d<TMasterRecs,
                                                 TDetailFieldMaker> >   DetailLog_;
   #endif
   //---------------------------------------------------------------------------
   //----- data members
   Kpui::TvacDetail  DetailVACreater_;
   //---------------------------------------------------------------------------
   //�� ViewAreaMgr �޲z�Ϊ� members
   typedef TTVALogger<Kpui::TFieldMaker>  TVALogger;
   typedef TVALogger::TViewArea           TMasterVA;
   TVALogger  VALog_;
   K_mf(TaViewArea)  CreateNewVA  ()                  { return VALog_.CreateNewVA(this); }
   K_mf(size_t)      GetDetails   (const char**& details)
                                                      { static const char*  detDesp[] = { "�s�ժ�" };
                                                        details = detDesp;
                                                        return numofele(detDesp); }
   K_mf(TVACreater*)  GetDetailVACreater    (size_t n)  { return &DetailVACreater_; }
   K_mf(TVACreater*)  OnMasterViewRowMoved  (TViewArea& masterVA, TViewArea*& detailVA, size_t)
      {
         TMasterVA&  va = static_cast<TMasterVA&>(masterVA);
         const TMasterVA::cursor_type* cur = va.GetCurRow();
         static_cast<Kpui::TDetailVA*>(detailVA)->SetContainer(
                                 cur==0 || cur->IsInvalid() ? 0 : &(cur->GetIterator()->second) );
         return DetailVACreaterNoChange;
      }
   //---------------------------------------------------------------------------
};
/////////////////////////////////////////////////////////////////////////////
#endif
