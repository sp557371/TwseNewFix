//
// ViewArea Creater �������,��U�B�z Master-Detail �� container
//
// ����: $Id: VACrControl.h,v 1.2 2005/08/09 05:26:33 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef VACrControlH
#define VACrControlH
//---------------------------------------------------------------------------
#include "ViewAreaMgr.h"
//---------------------------------------------------------------------------
namespace Kway {
/////////////////////////////////////////////////////////////////////////////
LIBNS_class TVACrControl : public TVAEvHandler
{
   typedef TVACrControl this_type;
public:
           K_ctor  TVACrControl  ();
   virtual K_dtor ~TVACrControl  ();

   virtual K_mf(void) SetVACreater (Kway::TVACreater*, const std::string& vaIdName, TViewArea* parent);
   virtual K_mf(void) SetViewArea  (Kway::TViewArea*);

   //����O�� Detail Tab ��, �n���� detail VA
   virtual K_mf(void) DetailSelect (size_t detailID);

   inline K_mf(TViewArea*)  GetViewArea  () const  { return VA_.get();     }
   inline K_mf(TVACreater*) GetVACreater () const  { return VACr_;         }
   inline K_mf(this_type*)  GetDetail    () const  { return Detail_.get(); }
   inline K_mf(std::string) GetVAIdName  () const  { return VA_.get() ? VA_->GetIdName() : std::string(); }
   inline K_mf(size_t)      GetDetailID  () const  { return DetailID_;     }

protected:
   //override TVAEvHandler:: virtual functions
   // �� va_ �� Cursor ���ʮ�, �n�q�� detail
   virtual K_mf(void) OnViewRowMoved (Kway::TViewArea&, const Kway::TViewCursor*, const Kway::TViewCursor*);

private:
   TaViewArea     VA_;
   TVACreater*    VACr_;

   typedef std::auto_ptr<this_type>   TaDetail;
   TaDetail       Detail_;
   size_t         DetailID_;

   virtual K_mf(this_type*)  CreateDetail  (int detailCount, const char** details);
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Kway
//---------------------------------------------------------------------------
#endif
