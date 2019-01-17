/////////////////////////////////////////////////////////////////////////////
#if !defined(_Nsa_EvMap_0Setting_h_)
#define _Nsa_EvMap_0Setting_h_
//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
namespace Kway {
namespace Nsa  {

/////////////////////////////////////////////////////////////////////////////
// class TEvMapSettingApply
/////////////////////////////////////////////////////////////////////////////
template <class ExecObjT, class MapT>
class TEvMapSettingApply
{
   typedef ExecObjT  TExecObj;
   typedef MapT      TMap;

   TExecObj*    ExecObj_;
   TMap*        OrigMap_;
   // 如果是兩層的設定，則可以記錄上一層 map 的 key
   std::string  PrevKey_;

public:
   inline K_ctor TEvMapSettingApply (TExecObj* ExecObj) : ExecObj_(ExecObj), OrigMap_(NULL)  {}
      
   inline K_mf(void) SetOrigMap (TMap* Map)               { OrigMap_ = Map; }
   inline K_mf(void) SetPrevKey (const std::string& str)  { PrevKey_ = str; }
      
   inline K_mf(TExecObj*)          GetExecObj ()  { return ExecObj_; }
   inline K_mf(TMap*)              GetOrigMap ()  { return OrigMap_; }
   inline K_mf(const std::string&) GetPrevKey ()  { return PrevKey_; }
};


/////////////////////////////////////////////////////////////////////////////
// class TEvMapSettingViewer
/////////////////////////////////////////////////////////////////////////////
template <class ExecObjT, class MapT, class FieldMakerT>
class TEvMapSettingViewer : public TTViewArea<MapT, FieldMakerT>
{
public:
   typedef TTViewArea<MapT, FieldMakerT>   inherited;
   typedef ExecObjT                              TExecObj;
   typedef MapT                                  TMap;
   typedef FieldMakerT                           TFieldMaker;
   typedef TEvMapSettingApply<TExecObj, TMap>    TApplyObj;

private:
   TApplyObj    ApplyObj_;
   TMap         Shadow_;
   TMap*        Origin_;
   TFieldMaker  FieldMaker_;

public:
   K_ctor TEvMapSettingViewer (TExecObj*, TViewControls, TMap*);

   K_mf(void) SetContainer (TMap*);

protected:
   K_mf(bool) SettingApply  ();
   K_mf(bool) SettingCancel ();
};
/////////////////////////////////////////////////////////////////////////////
// class TEvMapSettingViewer impl
/////////////////////////////////////////////////////////////////////////////
#define Nsa_TEvMapSettingViewer_Impl(return_type)                          \
   template <class ExecObjT, class MapT, class FieldMakerT>                \
   K_mf(return_type) TEvMapSettingViewer<ExecObjT, MapT, FieldMakerT>::    \
//---------------------------------------------------------------------------

template <class ExecObjT, class MapT, class FieldMakerT>
K_ctor TEvMapSettingViewer<ExecObjT, MapT, FieldMakerT>::TEvMapSettingViewer (TExecObj* execObj, TViewControls vcs, TMap* map)
   : inherited(map, FieldMaker_, vcs)
   , ApplyObj_(execObj)
   , Origin_  (map)
{
   if (Origin_) {
      Shadow_ = *Origin_;
      ApplyObj_.SetOrigMap(Origin_);

      inherited::SetContainer(&Shadow_);
   }
}
//---------------------------------------------------------------------------

Nsa_TEvMapSettingViewer_Impl(void) SetContainer (TMap* map)
{
   if (Origin_ == map)
      return;

   if (map) {
      Shadow_ = *map;
      inherited::SetContainer(&Shadow_);
   }
   else {
      Shadow_.clear();
      inherited::SetContainer(NULL);
   }

   Origin_ = map;
   ApplyObj_.SetOrigMap(Origin_);
}
//---------------------------------------------------------------------------

Nsa_TEvMapSettingViewer_Impl(bool) SettingApply ()
{
   if (Origin_ == NULL) {
      Shadow_.clear();
      return true;
   }

   TExecObj* ExecObj = ApplyObj_.GetExecObj();
   if (ExecObj) {
      bool Result = ExecObj->SettingApply(&Shadow_);
      *Origin_ = Shadow_;

      ExecObj->AfSettingApply(Origin_);

      return Result;
   }
   *Origin_ = Shadow_;
   return true;
}
//---------------------------------------------------------------------------

Nsa_TEvMapSettingViewer_Impl(bool) SettingCancel ()
{
   if (Origin_) {
      Shadow_ = *Origin_;
   }
   else {
      Shadow_.clear();
   }

   return true;
}
//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
}; // namespace Nsa
}; // namespace Kway

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_Nsa_EvMap_0Setting_h_)
