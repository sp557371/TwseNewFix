//
// 模組管理員:
//    template TModuleMgr<>
//
// 版本: $Id: ModuleLoader.h,v 1.6 2006/04/04 03:06:52 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef ModuleLoaderH
#define ModuleLoaderH
//---------------------------------------------------------------------------
#include "wise_ptr.h"
#include "fixed_num.h"
#include "inn_file.h"
#include "char_ary.h"
#include "SimpleRecorder.h"
#include "ViewAreaMgr.h"
//---------------------------------------------------------------------------
#if defined(K_UNIXLIKE) //use dlopen
#  include <limits.h> // for _POSIX_PATH_MAX
   #define MAXPATH   _POSIX_PATH_MAX
   typedef void*     HINSTANCE;
#elif defined(_MSC_VER)
#  define MAXPATH  MAX_PATH
#else
#  include <dir.h> // for MAXPATH
#endif
//---------------------------------------------------------------------------
namespace Kway {
/////////////////////////////////////////////////////////////////////////////
//您的擴充模組一定要完成底下的Functions
typedef void*          TnsmStartObj; 
typedef TnsmStartObj (*TModuleStartFn)  ();              // _nsModuleStart
typedef const char*  (*TModuleDespFn)   ();              // _nsModuleDesp

///> nsModuleFinish() 傳回true, 表示可以 FreeLibrary(),
//   否則表示此 lib 還需要保留(可能有些交給其他人代管的obj無法在卸載lib時刪除)
typedef bool         (*TModuleFinishFn) (TnsmStartObj);  // _nsModuleFinish
/////////////////////////////////////////////////////////////////////////////
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(push,1)
#else
#pragma pack(1)
#endif
//---------------------------------------------------------------------------
LIBNS_class TModuleRecBase   //設定要儲存的部分
{
   typedef Kstl::char_ary<char,MAXPATH,0> TFileName;
   byte        Enabled_;
   TFileName   LibName_;

public:
   inline K_ctor  TModuleRecBase  ()
                     : Enabled_(true),
                       LibName_()
                     { }

   inline K_mf(std::string)  GetLibName () const                  { return LibName_.as_string(); }
   inline K_mf(bool)         IsEnabled  () const                  { return Enabled_!=0;          }
   inline K_mf(void)         SetLibName (const std::string& name) { LibName_.assign(name);       }
   inline K_mf(void)         SetEnabled (bool isEnabled)          { Enabled_ = isEnabled;        }
   inline K_mf(void)         swap       (TModuleRecBase& a)       { std::swap(*this, a);         }
};
//---------------------------------------------------------------------------
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(pop)
#else
#pragma pack()
#endif
//---------------------------------------------------------------------------
LIBNS_class TModuleLoader : public TModuleRecBase
{
   typedef TModuleRecBase     inherited;
   typedef TModuleLoader      this_type;

protected:
   LIBNS_class TModuleHandle {
      K_mf(void) ShowVrboseLibName ();
      K_mf(void) UnloadLib   (const char* unLoadMsg);
   protected:
      HINSTANCE      LibInst_;
      TnsmStartObj   nsmStartObj_;
      #ifdef __BORLANDC__
         bool        IsPackage_;
      #endif
      std::string    LastError_;
      std::string    LibDesp_;

      virtual K_mf(bool)      CheckLoadLib    (const std::string& libName, bool isStart);
   public:
              K_ctor  TModuleHandle  ();
      virtual K_dtor ~TModuleHandle  ();

      inline  K_mf(bool)         IsLoaded     () const  { return(LibInst_ != 0); }
      inline  K_mf(std::string)  GetLibDesp   () const  { return LibDesp_;       }
      inline  K_mf(std::string)  GetLastError () const  { return LastError_;     }

      typedef void  (*TFuncAds)  ();
      K_mf(TFuncAds)  GetFuncAds (const char* procName, bool mustExist);
      K_mf(void)      UnloadLib  ();
      K_mf(bool)      LoadLib    (const std::string& libName, bool isStart);

      inline K_mf(TnsmStartObj) GetStartObj () const   { return nsmStartObj_; }
   };
private:
   std::string    LibDespCatch_;
   //Kstl::wise_ptr<TModuleHandle>    ModuleHandle_;
   virtual K_mf(const TModuleHandle*)  GetModuleHandle    () const = 0;
   virtual K_mf(TModuleHandle&)        FetchModuleHandle  () = 0;

public:
           K_ctor  TModuleLoader  ();
   virtual K_dtor ~TModuleLoader  ();

           K_mf(void)  SetLibName         (const std::string&);
           K_mf(bool)  LoadLib            ();
   virtual K_mf(void)  ResetModuleHandle  () = 0;

   inline K_mf(std::string)  GetLibDesp   () const  { return LibDespCatch_; }
   inline K_mf(std::string)  GetErrorMsg  () const  { const TModuleHandle* mh = GetModuleHandle(); return mh ? mh->GetLastError() : std::string(); }
   inline K_mf(bool)         IsLoaded     () const  { const TModuleHandle* mh = GetModuleHandle(); return(mh && mh->IsLoaded()); }
   inline K_mf(void)         swap         (this_type& a)   { inherited::swap(a);
                                                             LibDespCatch_.swap(a.LibDespCatch_); }
};
/////////////////////////////////////////////////////////////////////////////
typedef Kstl::rr_inn_file<dword,word>     TModuleRecInn;
//---------------------------------------------------------------------------
namespace for_bcb_bug {//不加上這段無用的定義,會造成 BCB compiler 說 TModuleRecorder 的定義有問題!!
struct TBCB_bug_filler {};
typedef TSimpleMap_AddRoomKey<int,TBCB_bug_filler,TModuleRecInn::rr_key>::TEvRecs  TBugRecs;
}
//---------------------------------------------------------------------------
//typedef Kstl::fixed_num<dword>            TModuleRecKey;
struct TModuleRecKey : public Kstl::fixed_num<dword> {};   //不用這種方式定義 Key,會造成BCB產生的程式碼有問題
/////////////////////////////////////////////////////////////////////////////
LIBNS_class TModuleRecFieldMakerBase {
public:
   typedef size_t            field_n_type;
   static K_mf(field_n_type)      GetFieldCount ();
   static K_mf(const TFieldDesp&) GetFieldDesp  (field_n_type);

   K_mf(std::string) Get (const TModuleRecKey&, const TModuleLoader&, field_n_type) const;
   K_mf(void)        Put (TModuleLoader&, field_n_type, const std::string&);
};
//---------------------------------------------------------------------------
//資料顯示方法
template <class TRecs>
class TTModuleRecFieldMaker : public TModuleRecFieldMakerBase
{
   typedef TModuleRecFieldMakerBase             inherited;
public:
   typedef TRecs                                TEvContainer;
   typedef typename TEvContainer::iterator      iterator;

   K_mf(std::string)  Get  (const iterator& i, field_n_type fieldNo) const
                                 { return inherited::Get(i->first, i->second, fieldNo); }
   K_mf(void)         Put  (TEvContainer& evc, iterator& i, field_n_type fieldNo, const std::string& str)
                                 { typename TEvContainer::updater u = evc.begin_update(i);
                                   inherited::Put(*u, fieldNo, str); }
}; // class TTModuleRecFieldMaker
//---------------------------------------------------------------------------
//typedef TSimpleRecorder_AddRoomKey<TModuleRecKey, TModuleLoader, TModuleRecInn, sizeof(TModuleRecBase)>
//        TModuleRecorder;
//---------------------------------------------------------------------------
template <class TModuleRecorder, class TFieldMaker = TTModuleRecFieldMaker<typename TModuleRecorder::TRecs> >
class TModuleMgr : protected TModuleRecorder,
                   public    TVACreater
{
   typedef TModuleRecorder                      inheritedRecorder;
protected:
   typedef typename inheritedRecorder::TRecs    TRecs;

public:
   K_ctor  TModuleMgr  (const std::string& settingFileName, typename inheritedRecorder::TInn::rr_size roomSize = MAXPATH+16)
            : inheritedRecorder(settingFileName, roomSize, true)
            {  //#if sizeof(TRecs::key_type)+inheritedRecorder::rec_size > roomSize
               //#  error "客房住不下囉!!"
               //#endif
               //BindInn(); 您必須在衍生出的class在建構完成後,呼叫 BindInn()
            }
protected:
   #ifndef __BORLANDC__
   using inheritedRecorder::Recs_;
   #endif

   typedef typename TModuleRecorder::iterator   iterator;
   K_mf(void)  BindInn  ()
               { inheritedRecorder::BindInn();
                 iterator  iend = Recs_.end();
                 for(iterator i = Recs_.begin();  i != iend;  ++i)
                    LoadLib(*Recs_.begin_update(i));
               }

private:
   virtual K_mf(void)        LoadLib           (typename TRecs::updater::reference u)  { u.LoadLib(); }
   virtual K_mf(TFieldMaker) CreateFieldMaker  () = 0;
   //給 ViewAreaMgr 管理用的 members
   K_mf(TaViewArea)  CreateNewVA  ()
            {
               TViewControls  vcs;
               vcs.set(vco_InputKey);
               vcs.set(vco_Erase);
               vcs.set(vco_Insert);
               vcs.set(vco_CopyCurrent);
             //vcs.set(vco_MoveCurrentUpDn); 調整啟動順序? 需要重新啟動,所以暫時不提供!
             #ifdef __alpha//在 alpha 上, 其實 Recs_ 的型態不是 TRecs, 因為 K_typedef() 使用繼承的方式定義 TRecs
               // 使用 static_cast<> 轉型是安全的嗎?
               return TaViewArea(new TTViewArea<TRecs,TFieldMaker>(static_cast<TRecs*>(&Recs_),CreateFieldMaker(),vcs));
             #else
               return TaViewArea(new TTViewArea<TRecs,TFieldMaker>(&Recs_,CreateFieldMaker(),vcs));
             #endif
            }
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Kway
//---------------------------------------------------------------------------
#endif

