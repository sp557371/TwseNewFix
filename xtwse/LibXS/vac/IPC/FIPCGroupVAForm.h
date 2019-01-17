//---------------------------------------------------------------------------
#ifndef FIPCGroupVAFormH
#define FIPCGroupVAFormH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
/////////////////////////////////////////////////////////////////////////////
#include "FevcViewer.h"
#include "ViewAreaMgr.h"
#include "IPCGroupProcess.h"

using namespace Kway;
class TIPCGroupMgr;
/////////////////////////////////////////////////////////////////////////////
struct TIPCProcessFieldMaker
{
   typedef TIPCGroupProcesses       TEvContainer;
   typedef TEvContainer::iterator   iterator;
   typedef size_t                   field_n_type;
   TIPCGroupMgr*     Mgr_;
   field_n_type      FieldCount_;
   const TFieldDesp* FieldDesps_;

   K_ctor TIPCProcessFieldMaker (TIPCGroupMgr* mgr, field_n_type fieldCount, const TFieldDesp* fieldDesps)
      : Mgr_(mgr)
      , FieldCount_(fieldCount)
      , FieldDesps_(fieldDesps)
      {
      }
   inline K_mf(field_n_type) GetFieldCount () const {
      return FieldCount_;
   }
   K_mf(const TFieldDesp&) GetFieldDesp (field_n_type n) const {
      return FieldDesps_[n];
   }

   K_mf(std::string) Get (const iterator& i, field_n_type fieldNo) const;
   K_mf(void)        Put (TEvContainer& evc, iterator& i, field_n_type fieldNo, const std::string& str);
};
typedef TTViewArea<TIPCGroupProcesses,TIPCProcessFieldMaker>   TIPCProcessVA;
//---------------------------------------------------------------------------
struct TIPCGroupsFieldMaker
{
   typedef TIPCGroupRecs            TEvContainer;
   typedef TEvContainer::iterator   iterator;
   typedef size_t                   field_n_type;

   const TFieldDesp* FieldDesps_;
   field_n_type      FieldCount_;
   TIPCGroupMgr*     Mgr_;

   K_ctor TIPCGroupsFieldMaker (const TFieldDesp* flds, field_n_type fldsCount, TIPCGroupMgr* mgr = NULL)
      : FieldDesps_(flds)
      , FieldCount_(fldsCount)
      , Mgr_(mgr)
      {
      }

   K_mf(field_n_type) GetFieldCount () const {
      return FieldCount_;
   }
   K_mf(const TFieldDesp&) GetFieldDesp (field_n_type n) const {
      return FieldDesps_[n];
   }

   K_mf(std::string) Get  (const iterator& i, field_n_type fieldNo) const;
   K_mf(void)        Put  (TEvContainer& evc, iterator& i, field_n_type fieldNo, const std::string& str);
};

typedef TTViewArea<TIPCGroupRecs,TIPCGroupsFieldMaker>   TIPCGroupsViewArea;

extern TFieldDesp fdespGroupsView[5];
extern TFieldDesp fdespGroupsSetting[4];
extern TFieldDesp fdespGroupsHold[6];
/////////////////////////////////////////////////////////////////////////////
class TIPCGroupVAForm : public TevcViewer
{
   typedef TevcViewer   inherited;
__published:	// IDE-managed Components
   TPanel *ProcessesPanel;
   TSplitter *SplitterDetail;
private:	// User declarations
   TevcViewer* ProcessesViewer_;
   class TGroupVAEvHandler : public TVAEvHandler {
      TIPCProcessVA* ProcessVA_;

      K_mf(void) ResetProcessContainer(const TIPCGroupsViewArea::cursor_type* cur) {
         TIPCGroupProcesses* processes = (cur==0 || cur->IsInvalid() ? 0 : &(cur->GetIterator()->second.Processes_));
         IsContainerChanging = true;
         ProcessVA_->SetContainer(processes);
         IsContainerChanging = false;
      }
      K_mf(void) OnViewAfEndUpdate2 (TViewArea& va, const TViewCursor& updrow) {
         if(updrow.DistanceBegin() == va.GetCurRow()->DistanceBegin())
            OnViewRowMoved(va, NULL, NULL);
      }
      K_mf(void) OnViewRowMoved (TViewArea& va, const TViewCursor*, const TViewCursor*) {
         TIPCGroupsViewArea* gva = dynamic_cast<TIPCGroupsViewArea*>(&va);
         ResetProcessContainer(gva ? gva->GetCurRow() : NULL);
      }
   public:
      K_ctor TGroupVAEvHandler(const TIPCProcessFieldMaker& fieldMaker)
         : ProcessVA_(new TIPCProcessVA(NULL, TIPCProcessFieldMaker(fieldMaker)))
         , IsContainerChanging(false)
         { }
      inline K_mf(TIPCProcessVA*) GetProcessVA() {
         return ProcessVA_;
      }
      bool IsContainerChanging;
   };
   TGroupVAEvHandler GroupVAEvHandler_;

   void __fastcall OnAfRecChanged (TObject* sender) {
      if(!GroupVAEvHandler_.IsContainerChanging)
         inherited::OnAfRecChanged(sender);
   }
public:		// User declarations
   __fastcall  TIPCGroupVAForm(TIPCGroupMgr* Owner, TIPCGroupsViewArea*, TevcViewerOptions);
   __fastcall ~TIPCGroupVAForm();
};
//---------------------------------------------------------------------------
#endif

