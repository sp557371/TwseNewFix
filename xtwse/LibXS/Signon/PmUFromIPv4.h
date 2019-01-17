//
// Policy Manager - UserFrom (工作站來源 IPv4 管理)
//
// 版本: $Id: PmUFromIPv4.h,v 1.3 2005/01/14 07:31:26 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef PmUFromIPv4H
#define PmUFromIPv4H
//---------------------------------------------------------------------------
#include "PmUFrom.h"
#include "VALogger.h"
#include "SesExtab.h"
//---------------------------------------------------------------------------
//
// User From IPv4          當工作站使用 IPv4 進入系統時
//  Group Policy           可允許登入的使用者群組
//
//---------------------------------------------------------------------------
namespace Kway {
namespace Signon {
/////////////////////////////////////////////////////////////////////////////
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(push,1)
#else
#pragma pack(1)
#endif
struct TUFIPv4
{
   // ip = a.b.c.d = _ip[3]._ip[2]._ip[1]._ip[0]
   union {
      byte     IP_[4];
      dword    IPN_;
   };
   inline K_ctor             TUFIPv4 ()                        { }
   inline K_ctor             TUFIPv4 (const char*        str)  { assign(str); }
   inline K_ctor             TUFIPv4 (const std::string& str)  { assign(str.c_str()); }
   inline K_mf(const char*)  assign  (const std::string& str)  { return assign(str.c_str()); }
          K_mf(const char*)  assign  (const char*);
          K_mf(std::string)  as_string () const;
};
//---------------------------------------------------------------------------
struct TUFIPv4Range
{
   // ip 範圍的登入管制
   TUFIPv4   To_;
   TUFIPv4   From_;

   K_mf(bool)  operator<  (const TUFIPv4Range& a) const  { return (From_.IPN_ < a.From_.IPN_) ? true
                                                                : (From_.IPN_ > a.From_.IPN_) ? false
                                                                : (To_.IPN_ < a.To_.IPN_);      }
   K_mf(bool)  operator== (const TUFIPv4Range& a) const  { return (From_.IPN_ == a.From_.IPN_
                                                                && To_.IPN_   == a.To_.IPN_);   }
   K_mf(std::string)  as_string  () const;
   K_mf(void)         assign     (const std::string&);
};
//---------------------------------------------------------------------------
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(pop)
#else
#pragma pack()
#endif
/////////////////////////////////////////////////////////////////////////////
class  TPmUFromIPv4 : public TPmUFromBase<TUFIPv4Range,TUFGroupPolicy,sizeof(TUFGroupPolicy)>
{
   typedef TPmUFromBase<TUFIPv4Range,TUFGroupPolicy,sizeof(TUFGroupPolicy)>  inherited;
public:
   K_ctor  TPmUFromIPv4  (const std::string& defaultPath);

   K_mf(const TUFGroupPolicy*)  GetPolicy  (const TUFIPv4&);

   #ifdef __TransLnDB__ //如果需要從 KPTS的LnDB 轉入登入資料庫時,才需要開放底下的 members, 但您必須自己完成他
   K_mf(std::string)  LoadLnDB  (const std::string& pathLnDB);
   #endif
   //---------------------------------------------------------------------------
private:
   struct TFieldMaker {
      typedef TRecs                    TEvContainer;
      typedef TEvContainer::iterator   iterator;
      typedef size_t                   field_n_type;

      K_mf(const TFieldDesp&)    GetFieldDesp   (field_n_type n) const;
      static K_mf(field_n_type)  GetFieldCount  ();

      K_mf(std::string) Get  (const iterator&, field_n_type) const;
      K_mf(void)        Put  (TEvContainer&, iterator&, field_n_type, const std::string&);

      inline K_ctor  TFieldMaker  (TPmUFromIPv4& mgr)  : Mgr_(&mgr) {}
      inline K_ctor  TFieldMaker  ()  : Mgr_(0) {}
      TPmUFromIPv4*  Mgr_;
   }; // struct TFieldMaker
   //---------------------------------------------------------------------------
   K_mf(TaViewArea)  CreateNewVA  ();

   #ifndef __TransLnDB__
   typedef TExtabLogUpdater<TExtabLog,TTExtabUpdater<TFieldMaker> >  TLoggerBase;
   class TLogger : public TLoggerBase {
      K_mf(size_t)  FieldNoFromLog  (size_t n) const;
   public:
      K_ctor  TLogger  ();
   };
   TLogger     Logger_;
   #endif

   class TVALog : public TTVALogger<TFieldMaker> {
      typedef TTVALogger<TFieldMaker>  inherited;
      K_mf(size_t)  FieldNo2Log     (size_t n) const;
   public:
      K_ctor  TVALog  (TExtabLog*, TFieldMaker);
   } VALog_;
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Signon
} // namespace Kway
//---------------------------------------------------------------------------
#endif

