//
// Policy Manager - UserFrom (�u�@���ӷ��޲z)
//
// ����: $Id: PmUFrom.h,v 1.8 2006/04/04 04:04:19 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef PmUFromH
#define PmUFromH
//---------------------------------------------------------------------------
//
// �u�@���ӷ�(PIv4,IPv6,Modem,RS232...)
// �F���޲z��ƪ�,��¦�w�q
//
//   �Ҧp: �q (IPv4) 172.16.100.170 �Ӫ��u�@��,���\�n�J :8770 ���s�� (��@�s�� 8770)
//         �q (IPv4) 127.0.0.1      �Ӫ��u�@��,���\�n�J *     ���s�� (���N�s��)
//         �q (IPv4) 172.16.100.169 �Ӫ��u�@��,���\�n�J 877   ���s�� (GroupKind=877,�d�߸s�զC��)
//         �q (TEL)  0225071823     �Ӫ��u�@��,���\�n�J :kway ���s�� (��@�s�� kway)
//
//---------------------------------------------------------------------------
#include "Signon/Policy/UFGroups.h"
#include "PmBase.h"
#include "inn_file.h"
#include "SimpleRecorder.h"
#include "ViewArea.h"
//---------------------------------------------------------------------------
namespace Kway {
namespace Signon {
/////////////////////////////////////////////////////////////////////////////
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(push,1)
#else
#pragma pack(1)
#endif
//---------------------------------------------------------------------------
using Kway::Signon::Policy::TUFGroup;
typedef TPolicyKey          TUFGroupKind;
class TPmUFromGroup;
//---------------------------------------------------------------------------
struct TUFGroupPolicy         //�ϥΪ̨ӷ�(ipv4�Bipv6�Bmodem...),���\ñ�J���ϥΪ̸s��
{
   TUFGroupKind   GroupKind_; // *     = �����s��(�i�n�J���N�s��),
                              // :xxxx = �ȥi�n�Jxxxx�s��
                              // gkID  = �� gkID �d�� GroupKind ��Ӫ�ҦC���s��

   TPmGroupID     PmGroupID_; // ��L�޲z�F�����]�w,�i�Φ�ID�n�DPmGroup�d�ߤ@��[PolicyName-PolicyKey]


   //�]�������c�|�Φb�\�h�a��(IPv4,IPv6...),�ҥH�N�@�ǥi��|�@�Ϊ�method,
   //�\�b�o��,���Ѥj�a��K�ϥ�:
   //  ���X���y�z,�w�]����3�����:
   //    (0)�F���s�N�X�B(1)�s�դ����B(2)���\ñ�J���s��(�q TPmUFromGroup* ���o)
   static        K_mf(const TFieldDesp&)  GetFieldDesp   (TPmUFromGroup*, size_t fieldNo);
   static inline K_mf(size_t)             GetFieldCount  ()               { return 3; }
                 K_mf(std::string)        Get            (TPmUFromGroup*, size_t fieldNo) const;
                 K_mf(void)               Put            (size_t fieldNo, const std::string&);
};
//---------------------------------------------------------------------------
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(pop)
#else
#pragma pack()
#endif
/////////////////////////////////////////////////////////////////////////////
class TPmUFromGroup : public TPmGroup
{  // TPmUFromGroup���N�O�@�� TPmGroup
   //    1.�h���ѤF�@�Ӥ��ݭn���U�� Policy: GetPmGroupKinds()
   //    2.����GroupKind���ƥ���U����
   //    3.GetPolicy()���ѼƱN�쥻�� TPmGroupID �令 TUFGroupPolicy
   //      ��ϥ� TUFGroupPolicy.PmGroupID_ �䤣������� Policy ��,
   //      �B PolicyName == Signon::TUFGroups::PolicyName, �h��d�� GroupKind
   typedef TPmGroup  inherited;
   class TImpl;
   TImpl*   Impl_;
public:
   K_ctor  TPmUFromGroup  (const std::string& defaultPath, TPmMgr*,
                           const std::string& mGroupName, const std::string& mTableName,
                           const std::string& dGroupName, const std::string& dTableName);
   K_dtor ~TPmUFromGroup  ();

   //�� policyName==TUFGroups::PolicyName ��, �h���ϥ� gp.GroupKind_ �d�� GroupKinds
   //�Y�䤣��,�~�ϥ� gp.PmGroupID_ �d�� [�F���s]���]�w
   K_mf(TPolicyBaseRef)  GetPolicy  (const TPmMgr&, const TUFGroupPolicy*, const std::string& policyName, bool forLocalUse) const;
   K_mf(std::string)     GetGroups  (const TUFGroupKind&) const;  //�C�ӥi�n�J�s�եΡu,�v���}

   //���X�w�]�� GroupKind - Policy
   K_mf(TPmBaseVA*) GetPmGroupKinds  ();
   //���U�ƥ�(��GroupKinds��Ʀ����ʮ�)
   K_mf(void)  GroupKinds_add_ev_handler  (Kstl::container_ev_handler*);
   K_mf(void)  GroupKinds_del_ev_handler  (Kstl::container_ev_handler*);

   //���X������ GroupKind ��@��J�ɪ� [�C�|]
   K_mf(TVAGetEditProp&)  GetVAEditProp_ForKinds  ();

   #ifdef __TransLnDB__ //�p�G�ݭn�q KPTS��LnDB ��J�n�J��Ʈw��,�~�ݭn�}�񩳤U�� members, ���z�����ۤv�����L
   K_mf(std::string)  LoadLnDB  (const std::string& pathLnDB);
   #endif
};
/////////////////////////////////////////////////////////////////////////////
template <class TUFKey, class TUFRec = TUFGroupPolicy, size_t RecSz = sizeof(TUFRec)>
              //TUFKey = TUFIPv4Range (IPv4 ���@�� ip �d��)
              //         TUFIPv6Range (IPv6 ���@�� ip �d��)
              //         TUFTel       (�q�ܸ��X)
              //         ...
class TPmUFromBase : public TVACreater
{
   typedef TVACreater                           inherited;
   typedef TPmUFromBase<TUFKey,TUFRec,RecSz>    this_type;
   typedef this_type                            TUFromBase;
   typedef word                                 rr_size; //�p�G�b���U�� rr_size �ϥ� TUFInn::rr_size, bcb4 compile�|�����D? ��...
#ifdef __TransLnDB__ //�p�G�ݭn�q KPTS��LnDB ��J�n�J��Ʈw��,�~�ݭn�}�񩳤U�� members
protected:
#endif
   typedef Kstl::rr_inn_file<dword,rr_size>     TUFInn;
   //---------------------------------------------------------------------------
   class TUFRecorder : public TSimpleRecorder_AddRoomKey<TUFKey,TUFRec,TUFInn,RecSz>
   {
      friend class TPmUFromBase; //TPmUFromBase<>
      typedef TSimpleRecorder_AddRoomKey<TUFKey,TUFRec,TUFInn,sizeof(TUFRec)> inherited;
      #ifndef __BORLANDC__
         using inherited::BindInn;
         using inherited::rec_size;
         using inherited::Recs_;
      #endif
   public:
      K_ctor  TUFRecorder  (const std::string& fileName, typename TUFromBase::rr_size roomSize)
               : inherited(fileName, roomSize)
               {
               if(sizeof(typename TRecs::key_type) + rec_size > roomSize)
                  throw "TPmUFromBase::TUFRecorder �ȩЦ��U�o!!";
               BindInn();
               }
      typedef typename inherited::TRecs  TRecs_t;
      K_typedef(TRecs_t,                 TRecs);
      K_mf(TRecs*)  get_recs ()  { return static_cast<TRecs*>(&Recs_); }
      #ifdef __TransLnDB__ //�p�G�ݭn�q KPTS��LnDB ��J�n�J��Ʈw��,�~�ݭn�}�񩳤U�� members
      typedef TRecs::iterator    iterator;
      typedef inherited::TInn    TInn;
      inline K_mf(void)  peek_rooms  (TRecs& recs, TInn& inn)  { inherited::peek_rooms(recs,inn); }
      inline K_mf(void)  insert      (iterator s, iterator e)  { Recs_.insert(s,e); }
      #endif
   };
   //---------------------------------------------------------------------------
   TUFRecorder    Recorder_;
   TPmUFromGroup* UFromGroup_;

protected:
   typedef typename TUFRecorder::TRecs    TRecs;
   inline K_mf(TRecs&)                    recs  ()    { return *Recorder_.get_recs(); }
   inline K_mf(typename TRecs::iterator)  begin ()    { return recs().begin();        }
   inline K_mf(typename TRecs::iterator)  end   ()    { return recs().end();          }
   K_mf(const TUFRec*)  find  (const TUFKey& key, typename TRecs::iterator* iresult)
      { typename TRecs::iterator i = recs().find(key);
        if(iresult)  *iresult = i;
        return(i==recs().end() ? 0 : &(i->second)); }

public:
   K_ctor  TPmUFromBase  (const std::string& fileName, rr_size roomSize)
      : Recorder_(fileName,roomSize),
        UFromGroup_(0)
      {
      }

// K_mf(const TUFRec*)  GetPolicy  (const TUFKey&, bool forLocalUse) const;

   inline K_mf(void)            SetUFromGroup  (TPmUFromGroup& a)   { UFromGroup_ = &a;   }
   inline K_mf(TPmUFromGroup*)  GetUFromGroup  () const             { return UFromGroup_; }

protected:
   //�� ViewAreaMgr �޲z�Ϊ� members
   template <class TFieldMaker, class TVALogger>
   K_mf(TaViewArea)  CreateNewVA_T  (const TFieldMaker& fieldMaker, TVALogger* log)
   {
      typedef TVAMkKey   TKeyMaker;
      class TVA : public TTViewArea<TRecs,TFieldMaker,TKeyMaker,TVALogger> {
         //�Y TPmUFromGroup->GroupKinds ������,�h���ViewArea��s�e��!
         //��M,�z�]�i�H�ھڲ��ʪ���ƴM��n��s������,���L�ӳ·ФF!
         //�p�B�e���W����Ƥ]���|�Ӧh,�ҥH��ӧ�s,���ܳy���Ӥj��Loading!
         typedef TTViewArea<TRecs,TFieldMaker,TKeyMaker,TVALogger>  inherited;
         class THdr//TGroupKindsHandler
                  : public Kstl::container_ev_handler {
            TVA*           va_;
            TPmUFromGroup* UFromGroup_;
            K_mf(void)  OnContainerBfDestroy    (const ev_container&)     { UFromGroup_ = 0; }
            K_mf(void)  OnContainerAfClear      (const ev_container&)     { va_->Repaint();  }
            K_mf(void)  OnContainerAfAssign     (const ev_container&)     { va_->Repaint();  }
            K_mf(void)  OnContainerAfInsert     (const ev_container&, const ev_iterator&, size_type)  { va_->Repaint(); }
            K_mf(void)  OnContainerAfErase      (const ev_container&, const ev_iterator&, size_type)  { va_->Repaint(); }
            K_mf(void)  OnContainerAfEndUpdate  (const ev_updater_impl&)  { va_->Repaint(); }
         public:
            K_ctor  THdr//TGroupKindsHandler
                    (TVA* va, TPmUFromGroup* UFromGroup)
                        : va_(va), UFromGroup_(UFromGroup)
                        { if(UFromGroup)       UFromGroup->GroupKinds_add_ev_handler(this);  }
            virtual K_dtor ~THdr()//TGroupKindsHandler  ()
                        { if(UFromGroup_)      UFromGroup_->GroupKinds_del_ev_handler(this); }
         };
         friend class THdr;//TGroupKindsHandler;
           THdr                GroupPolicyHandler_;
         //TGroupKindsHandler  GroupPolicyHandler_;
      public:
         K_ctor  TVA  (TRecs* recs, const TFieldMaker& fieldMaker, TPmUFromGroup* PmUFromGroup, TVALogger* log)
                  : inherited(recs,fieldMaker,inherited::DefaultViewControls(),log),
                    GroupPolicyHandler_(this, PmUFromGroup)
                  { }
      };
      return TaViewArea(new TVA(Recorder_.get_recs(), fieldMaker, UFromGroup_, log));
   }
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Signon
} // namespace Kway
//---------------------------------------------------------------------------
#endif

