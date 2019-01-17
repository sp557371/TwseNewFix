//
// �z�L TVACreater ���� TViewArea
// �å浹 TViewAreaMgr �޲z
// �P�� TViewAreaMgr ���� TVACreater �����U
//
// ����: $Id: ViewAreaMgr.h,v 1.7 2005/08/09 05:25:22 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef ViewAreaMgrH
#define ViewAreaMgrH
//---------------------------------------------------------------------------
#include "ViewArea.h"
#include "ev_map.h"
//---------------------------------------------------------------------------
namespace Kway {
/*/////////////////////////////////////////////////////////////////////////*/
LIBNS_class  TViewAreaMgr;
/*/////////////////////////////////////////////////////////////////////////*/
// �t�d�إ� ViewArea, �ι����� details ViewArea
LIBNS_class  TVACreater
{
public:
   virtual K_dtor  ~TVACreater  ()  {}

   //�� TViewAreaMgr �Ѻc�e, �|�ϥ� OnBfVAMgrDestroy() �q���j�a
   //�Y�ϱz�ϥΤ��P�W�ٸ� TViewAreaMgr::Reg() ���U�h��,�]�u�|����@���q��
   virtual K_mf(void)  OnBfVAMgrDestroy  (const TViewAreaMgr&)  {}

   //�I�sCreateNewVA()���H,�����޲z�ѳo�ಣ�ͥX�Ӫ�VA
   virtual K_mf(TaViewArea)  CreateNewVA  () = 0;
   inline  K_mf(TaViewArea)  CreateNewVA  (const std::string& vaIdName, TViewArea* parent)
                                             { TaViewArea   va(CreateNewVA().release());
                                               if(va.get()) {
                                                  if(!vaIdName.empty() && va->GetIdName().empty())
                                                     va->SetIdName(vaIdName);
                                                  va->SetParentVA(parent);
                                               }
                                               return va; }

//�����U�� virtual functions, �O���F�䴩 detail ViewArea �Ϊ�, �w�]���S�� detail
   //��GetDetails()���o�� VACreater �@���h�� detail ��ƪ�
   virtual K_mf(size_t)      GetDetails   (const char**&)   { return 0; }
                         //�d��: { static const char* desp[] = { "�w�s����", "�e�U����", "�������" };
                         //        a = desp;
                         //        return numofele(desp); }
   //��GetDetailVACreater()���X�� n(detailID) �� detail �� VACreater,
   //  �z�Ǧ^�� VACreater �����ѱz�ۤv�޲z, �B���i���N�߱�!
   virtual K_mf(TVACreater*) GetDetailVACreater (size_t detailID)  { return 0; }

   //���� MasterVA(��this CreateNewVA()���ͪ�VA) �o�X OnViewRowMoved() �ƥ��, �n�p��B�z detail:
   //  (1) ���� detail �� VACreater : �Y�Ǧ^�Ȥ��O DetailVACreaterNoChange,
   //                                 �h��ܭn���� detail �� VACreater
   //                                 [�Ѽ�:detailVA]�����ܦ� don't care
   //  (2) ���ͷs�� VA              : �ק� [�Ѽ�:detailVA]
   //  (3) ���� VA ������ Container : �]�w detailVA->SetContainer() �Y�i
   //      (2)�P(3) �ҶǦ^ DetailVACreaterNoChange
   //  ���z���i�H�R�� detailVA
   static TVACreater* const   DetailVACreaterNoChange;
   virtual K_mf(TVACreater*)  OnMasterViewRowMoved  (TViewArea& masterVA, TViewArea*& detailVA, size_t detailID)
                                                      { return DetailVACreaterNoChange; }
};
/*/////////////////////////////////////////////////////////////////////////*/
template <class TRecs>
struct TVAKeyFieldMaker
{
   typedef TRecs                             TEvContainer;
   typedef typename TEvContainer::iterator   iterator;
   typedef size_t                            field_n_type;

// K_mf(const TFieldDesp&)  GetFieldDesp   (field_n_type n) const;  �٬O�����ѱz�ۤv�ӧ���
   K_mf(field_n_type)       GetFieldCount  () const                 { return 1; }

   K_mf(std::string) Get  (const iterator& i, field_n_type) const   { return TEvContainer::get_key(i).as_string(); }
   K_mf(void)        Put  (TEvContainer&, iterator&, field_n_type, const std::string&) {}
}; // class TVAKeyFieldMaker
/*/////////////////////////////////////////////////////////////////////////*/
// �t�d�޲z�@�s TVACreater
// TViewAreaMgr �����]�O�@�� TVACreater
LIBNS_class  TViewAreaMgr : public TVACreater
{
   typedef Kstl::ev_map<std::string,TVACreater*>   TVACreaters_t;
   K_typedef(TVACreaters_t,                        TVACreaters);
   typedef TVACreaters::iterator                   TVACreateri;
   TVACreaters    VACreaters_;
   struct TFieldMaker : public TVAKeyFieldMaker<TVACreaters>
   {
      TViewAreaMgr*   VAMgr_;
      K_ctor TFieldMaker(TViewAreaMgr& mgr)  : VAMgr_(&mgr) {
      }
      K_mf(const TFieldDesp&)  GetFieldDesp  (field_n_type) const;
      K_mf(std::string)        Get           (const iterator& i, field_n_type) const;
   }; // struct TFieldMaker
   friend struct TFieldMaker;
   std::string DetailName_;
   const char* DetailNames_[1];
   std::string FieldTitle_;
   TFieldDesp  FieldDesp_;
public:
   K_ctor  TViewAreaMgr (const std::string& fieldTitle = std::string(), const std::string& detailName = std::string());
   K_dtor ~TViewAreaMgr ();  //�q���Ҧ����U��VACreater

   K_mf(void) SetFieldTitle (const std::string& fieldTitle);

   //���U/�R�� TVACreater
   K_mf(bool)  Add  (const std::string& evcName, TVACreater&);
   K_mf(bool)  Del  (const std::string& evcName, TVACreater&);
   K_mf(bool)  Del  (TVACreater&);
   K_mf(bool)  Find (TVACreater&);
   //���X���U��T
   //  ���o���������U�W��
   K_mf(bool)  GetRegNames   (Kway::TStrings&) const;
   //  ���o�Y���U�W��,������ƪ�Keys
   K_mf(bool)  GetDetailKeys (const std::string& evcName, Kway::TStrings&) const;
   //  ���o�Y���U�W��,������ƪ�Keys
   K_mf(TVACreater*)  GetReg (const std::string& evcName) const;

   //�~�Ӧ� TVACreater �� methods
   K_mf(TaViewArea)   CreateNewVA           ();
   K_mf(size_t)       GetDetails            (const char**&);
   K_mf(TVACreater*)  GetDetailVACreater    (size_t);
   K_mf(TVACreater*)  OnMasterViewRowMoved  (Kway::TViewArea& masterVA, Kway::TViewArea*& detailVA, size_t detailID);

   typedef TTViewArea<TVACreaters,TFieldMaker>    TMgrVA;
   K_mf(TVACreater*)  GetDetailVACreater    (TMgrVA&);

   //public types
   //--------------------------------------------------
   class TVAGetEditProp_GetNames : public TVAGetEditProp
   {  //���o VAMgr ������[���U�W��], ��@��J�ɪ�[�C�|]
      TViewAreaMgr&  VAMgr_;
   public:
      inline K_ctor  TVAGetEditProp_GetNames  (TViewAreaMgr& vaMgr)  : VAMgr_(vaMgr)  { }
      K_mf(TEditStyle)  GetEnums  (const TViewArea&, const TViewCursor*, size_t, TStrings* result, bool&)
                                    { if(result)   VAMgr_.GetRegNames(*result);
                                      return es_AllRecsEqual; }
   };
   /*--------------------------------------------------*/
   class TVAGetEditProp_GetKeys : public TVAGetEditProp
   {  //���o VAMgr �Y[���U�W��] [������Keys], ��@��J�ɪ�[�C�|]
      TViewAreaMgr&  VAMgr_;
   public:
      inline K_ctor  TVAGetEditProp_GetKeys  (TViewAreaMgr& vaMgr)  : VAMgr_(vaMgr)  { }
      K_mf(TEditStyle)  GetEnums  (const TViewArea&, const TViewCursor* cur, size_t, TStrings* result, bool&)
                                    { if(result  &&  cur)
                                         VAMgr_.GetDetailKeys(cur->GetStr(0), *result);
                                      return es_EnumsByKey; }
   };

   static const char       DefaultDetailName[];  // = "��ƪ�";
   static const TFieldDesp DefaultDetailField;
};
/*/////////////////////////////////////////////////////////////////////////*/
} // namespace Kway
//---------------------------------------------------------------------------
#endif

