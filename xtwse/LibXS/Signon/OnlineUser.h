//
// 線上使用者管理用的基本 class
//
// 版本: $Id: OnlineUser.h,v 1.3 2006/04/04 03:10:13 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef OnlineUserH
#define OnlineUserH
//---------------------------------------------------------------------------
#include "Signon/Policy/PolicyBase.h"
//---------------------------------------------------------------------------
namespace Kway {
namespace Signon {
/////////////////////////////////////////////////////////////////////////////
using Kway::Signon::Policy::TOnlineID;
//---------------------------------------------------------------------------
LIBNS_class TOnlineUser
{
   std::string    User_;
   TOnlineID      OnlineID_;
protected:
   inline K_mf(void)  ClearOnlineID  ()
            { OnlineID_ = 0; }

   inline K_mf(void)  Set  (const std::string& user, TOnlineID onlineID)
            { User_ = user;
              OnlineID_ = onlineID; }

public:
   inline K_ctor  TOnlineUser  ()
            : User_(),
              OnlineID_()
            { }
   virtual K_dtor ~TOnlineUser  ()
            { }

   inline K_mf(const std::string&) GetUser     () const  { return User_;     }
   inline K_mf(TOnlineID)          GetOnlineID () const  { return OnlineID_; }
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Signon
} // namespace Kway
//---------------------------------------------------------------------------
#endif
