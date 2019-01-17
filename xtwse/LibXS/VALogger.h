//
// ViewArea - ModifyLogger
//
// 版本: $Id: VALogger.h,v 1.2 2004/07/14 04:33:51 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef VALoggerH
#define VALoggerH
//---------------------------------------------------------------------------
#include "ViewAreaMgr.h"
//---------------------------------------------------------------------------
namespace Kway {
/////////////////////////////////////////////////////////////////////////////
LIBNS_class TTableLog
{
public:
   virtual K_dtor     ~TTableLog ()  { }
   virtual K_mf(void)  WriteMod  (const std::string& key,
                                  size_t             fieldNo,
                                  const std::string& bf,
                                  const std::string& af,
                                  TViewArea*) = 0;
   virtual K_mf(void)  WriteAdd  (const std::string& key, TViewArea* va)= 0;
   virtual K_mf(void)  WriteDel  (const std::string& key, TViewArea* va)= 0;
};
//---------------------------------------------------------------------------
LIBNS_class TTableLog2d
{
public:
   virtual K_dtor     ~TTableLog2d ()  { }
   virtual K_mf(void)  WriteMod  (const std::string& masterKey,
                                  const std::string& detailKey,
                                  size_t             fieldNo,
                                  const std::string& bf,
                                  const std::string& af,
                                  TViewArea* va) = 0;
   virtual K_mf(void)  WriteAdd  (const std::string& masterKey, const std::string& detailKey, TViewArea* va) = 0;
   virtual K_mf(void)  WriteDel  (const std::string& masterKey, const std::string& detailKey, TViewArea* va) = 0;
};
/////////////////////////////////////////////////////////////////////////////
template <class TFieldMaker>
class TTVALogger2d
{  //TFieldMaker::TEvContainer 必須提供 K_mf(std::string) GetMasterKeyStr() const;
   typedef TTVALogger2d<TFieldMaker>   this_type;
   TTableLog2d*   Log_;
   TFieldMaker    FieldMaker_;
public:
   typedef typename TFieldMaker::TEvContainer   TRecs;
   typedef typename TRecs::iterator             Iterator;
   //Types.
   typedef TTViewArea<TRecs,TFieldMaker,TVAMkKey,this_type>   TViewArea;
   K_mf(TaViewArea)  CreateNewVA  (TRecs* recs, const TFieldMaker&   fmaker = TFieldMaker(),
                                                const TViewControls& vcs = TViewArea::DefaultViewControls())
      { return TaViewArea(new TViewArea(recs, fmaker, vcs, this)); }

   //Ctor.
   K_ctor  TTVALogger2d  (TTableLog2d* log, TFieldMaker fmaker = TFieldMaker())
            : Log_(log)
            , FieldMaker_(fmaker)
            { }

   //mf.
   K_mf(bool) WriteMod  (const TRecs& evc, const Iterator& i, size_t fieldNo, const std::string& str, Kway::TViewArea* va)
      { if(Log_ == 0)
          return false;
        std::string bf = FieldMaker_.Get(i,fieldNo);
        if(bf == str)
          return false;
        Log_->WriteMod(evc.GetMasterKeyStr(), GetKey(i), FieldNo2Log(fieldNo), bf, str, va);
        return true;
      }
   K_mf(void) WriteAdd  (const TRecs& evc, const Iterator& i, Kway::TViewArea* va)
      { if(Log_)
           Log_->WriteAdd(evc.GetMasterKeyStr(), GetKey(i), va);
      }
   K_mf(void) WriteDel  (const TRecs& evc, const Iterator& i, Kway::TViewArea* va)
      { if(Log_)
           Log_->WriteDel(evc.GetMasterKeyStr(), GetKey(i), va);
      }
   K_mf(void) WriteCopy (const TRecs& evc, const Iterator&, const Iterator& to, Kway::TViewArea* va)
      { WriteRec(evc,to,va); }
   K_mf(void) WriteSwap (const TRecs& evc, const Iterator& i1, const Iterator& i2, Kway::TViewArea* va)
      { WriteRec(evc,i1,va);
        WriteRec(evc,i2,va); }
protected:
   K_mf(void) WriteRec  (const TRecs& evc, const Iterator& i, Kway::TViewArea* va)
      {  if(Log_ == 0)
            return;
         std::string masterkey = evc.GetMasterKeyStr();
         std::string detailkey = GetKey(i);
         for(int L = (int)(FieldMaker_.GetFieldCount()-1);  L >= 0;  --L)
            if(FieldMaker_.GetFieldDesp(L).FieldStyle_ & tfs_CanEdit)
               Log_->WriteMod(masterkey, detailkey, FieldNo2Log(L), std::string(), FieldMaker_.Get(i,L), va);
      }

   inline  K_mf(std::string) GetKey  (const Iterator& i) const   { return FieldMaker_.Get(i,0); }
   virtual K_mf(size_t)      FieldNo2Log  (size_t n) const       { return n;        }
};
//---------------------------------------------------------------------------
template <class TFieldMaker>
class TTVALogger
{
   typedef TTVALogger<TFieldMaker>    this_type;
   TTableLog*  Log_;
   TFieldMaker FieldMaker_;
public:
   typedef typename TFieldMaker::TEvContainer   TRecs;
   typedef typename TRecs::iterator             Iterator;
   //Types.
   typedef TTViewArea<TRecs,TFieldMaker,TVAMkKey,this_type> TViewArea;

   K_mf(TaViewArea)  CreateNewVA  (TRecs* recs, const TFieldMaker&   fmaker = TFieldMaker()
                                              , const TViewControls& vcs = TViewArea::DefaultViewControls()) {
      return TaViewArea(new TViewArea(recs, fmaker, vcs, this));
   }
   K_mf(TaViewArea)  CreateSettingVA  (TRecs* recs, const TFieldMaker& fmaker = TFieldMaker()
                                                  , TViewControls      vcs = TViewArea::DefaultViewControls()) {
      vcs.set(vco_Setting);
      return TaViewArea(new TSettingVA<TViewArea>(recs, fmaker, vcs, this));
   }

   //Ctor
   K_ctor  TTVALogger  (TTableLog* log, TFieldMaker fmaker = TFieldMaker())
            : Log_(log)
            , FieldMaker_(fmaker)
            { }

   //mf
   K_mf(bool) WriteMod  (const TRecs& evc, const Iterator& i, size_t fieldNo, const std::string& str, Kway::TViewArea* va)
      { if(Log_ == 0)
           return false;
        std::string bf = FieldMaker_.Get(i,fieldNo);
        if(bf == str)
           return false;
        Log_->WriteMod(GetKey(i), FieldNo2Log(fieldNo), bf, str, va);
        return true;
      }
   K_mf(void) WriteAdd  (const TRecs& evc, const Iterator& i, Kway::TViewArea* va)
      { if(Log_)
           Log_->WriteAdd(GetKey(i), va);
      }
   K_mf(void) WriteDel  (const TRecs& evc, const Iterator& i, Kway::TViewArea* va)
      { if(Log_)
           Log_->WriteDel(GetKey(i), va);
      }
   K_mf(void) WriteCopy (const TRecs& evc, const Iterator&, const Iterator& to, Kway::TViewArea* va)
      { WriteRec(evc, to, va); }
   K_mf(void) WriteSwap (const TRecs& evc, const Iterator& i1, const Iterator& i2, Kway::TViewArea* va)
      { WriteRec(evc, i1, va);
        WriteRec(evc, i2, va); }
protected:
   K_mf(void) WriteRec  (const TRecs& evc, const Iterator& i, Kway::TViewArea* va)
      {  if(Log_ == 0)
            return;
         std::string key = GetKey(i);
         for(int L = (int)(FieldMaker_.GetFieldCount()-1);  L >= 0;  --L)
            if(FieldMaker_.GetFieldDesp(L).FieldStyle_ & tfs_CanEdit)
               Log_->WriteMod(key, FieldNo2Log(L), std::string(), FieldMaker_.Get(i,L), va);
      }
   inline  K_mf(std::string) GetKey  (const Iterator& i) const   { return FieldMaker_.Get(i,0); }
   virtual K_mf(size_t)      FieldNo2Log  (size_t n) const       { return n;        }
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Kway
//---------------------------------------------------------------------------
#endif
