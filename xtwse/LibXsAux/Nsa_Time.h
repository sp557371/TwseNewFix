/////////////////////////////////////////////////////////////////////////////
#if !defined(_NsaTime_h_)
#define _NsaTime_h_
//---------------------------------------------------------------------------
#include "KTime.h"
#include "fixed_num.h"
#include "libnsAux.h"

/////////////////////////////////////////////////////////////////////////////
namespace Kway {
   using namespace Kstl;

namespace Nsa  {

/////////////////////////////////////////////////////////////////////////////
// class TTimeStamp
/////////////////////////////////////////////////////////////////////////////
LIBNSAUX_class TTimeStamp
{
   typedef TTimeStamp  this_type;

   TKTime    Time_;
   TKTimeMS  TimeMS_;

public:
   inline K_ctor TTimeStamp ()                                            : Time_(),        TimeMS_()        {}
   inline K_ctor TTimeStamp (const TKTime& time, const TKTimeMS& timeMS)  : Time_(time),    TimeMS_(timeMS)  {}
   inline K_ctor TTimeStamp (byte h, byte m, byte s, word ms)             : Time_(h, m, s), TimeMS_(ms)      {}
   
   //==============================================
   // assign
   //==============================================
   K_mf(void) assign (const char*);
   
   // 084500080 ==> 08:45:00.080
   // 如果上面字串轉成 int 來設定的話會有問題
   K_mf(void) assign_str_hhmmssms_9 (const char*);
   
   // 09441691 ==> 09:44:16.910
   // 使用: 台灣期交所時間設定
   K_mf(void) assign_hhmmssms_8 (int);
   K_mf(int)  as_int_hhmmssms_8 () const;

   // 084500080 ==> 08:45:00.080
   K_mf(void) assign_hhmmssms_9 (int);
   
   // 084500    ==> 08:45:00.000
   K_mf(void) assign_hhmmss (int);
   
   inline K_mf(void) assign_KDateTimeMS (const TKDateTimeMS& dtms)
      {
         Time_   = dtms.GetTime();
         TimeMS_ = dtms.GetMS  ();
      }

   //==============================================
   // as_xxx
   //==============================================
          K_mf(std::string) as_string (bool timeLongStr=true, int msLen=3) const;
   inline K_mf(std::string) as_str    (bool timeLongStr=true             ) const  { return Time_.as_string(timeLongStr); }

   //==============================================
   // Calc
   //==============================================
   // 傳回相差的豪秒 (TTimeStamp - TKDateTimeMS)
   inline K_mf(long) CalcFront (const TKDateTimeMS& dtms) const
      {
         long lhs = (long)(         Time_.GetSecOfDay() * 1000 +      TimeMS_.GetMS());
         long rhs = (long)(dtms.GetTime().GetSecOfDay() * 1000 + dtms.GetMS().GetMS());

         return (lhs - rhs);
      }

   // 傳回相差的豪秒 (TKDateTimeMS - TTimeStamp)
   inline K_mf(long) CalcBack (const TKDateTimeMS& dtms) const
      {
         long lhs = (long)(dtms.GetTime().GetSecOfDay() * 1000 + dtms.GetMS().GetMS());
         long rhs = (long)(         Time_.GetSecOfDay() * 1000 +      TimeMS_.GetMS());

         return (lhs - rhs);
      }

   //==============================================
   // inline
   //==============================================
   inline K_mf(void) Encode (byte h, byte m, byte s, word ms)
      {
         Time_  .Encode(h, m, s);
         TimeMS_.SetMS(ms);
      }

   inline K_mf(void) Clear ()
      {
         Time_  .Clear();
         TimeMS_.SetMS(0);
      }

   inline K_mf(void) assign                (const std::string& str)  { assign               (str.c_str()); }
   inline K_mf(void) assign_str_hhmmssms_9 (const std::string& str)  { assign_str_hhmmssms_9(str.c_str()); }
   
   inline K_mf(void) SetTime   (const TKTime&      a)    { Time_   = a;         }
   inline K_mf(void) SetTimeMs (const TKTimeMS&    a)    { TimeMS_ = a;         }
   inline K_mf(void) IncSec    (int secs = 1)            { Time_.IncSec(secs);  }

   inline K_mf(const TKTime&)   GetTime   () const  { return Time_;   }
   inline K_mf(const TKTimeMS&) GetTimeMS () const  { return TimeMS_; }
   //inline K_mf(TKTime)   GetTime   () const  { return Time_;   }
   //inline K_mf(TKTimeMS) GetTimeMS () const  { return TimeMS_; }

   inline K_mf(bool) IsInvalid () const  { return Time_.IsInvalid(); }

   //==============================================
   // operator
   //==============================================
   // 傳回相差的豪秒
   K_mf(long) operator - (const TTimeStamp&) const;

   inline K_mf(bool) operator == (const this_type& rhs) const
      { return (Time_ == rhs.Time_ && TimeMS_ == rhs.TimeMS_); }

   inline K_mf(bool) operator != (const this_type& rhs) const
      { return !(*this == rhs); }

   inline K_mf(bool) operator <  (const this_type& rhs) const
      {
         if (Time_ < rhs.Time_)
            return true;
         else if (Time_ == rhs.Time_ && TimeMS_ < rhs.TimeMS_)
            return true;
         
         return false;
      }
   
   //==============================================
   // static
   static K_mf(TTimeStamp) Now ();
};

/////////////////////////////////////////////////////////////////////////////
// class TTimeUs  microsecond
/////////////////////////////////////////////////////////////////////////////
LIBNSAUX_class TTimeUs
{
   typedef TTimeUs  this_type;

   TKTime  Time_;
   int     USec_; // 微秒(10E-6 second)

public:
   inline K_ctor TTimeUs ()                               : Time_(),        USec_(0)   {}
   inline K_ctor TTimeUs (byte h, byte m, byte s, int us) : Time_(h, m, s), USec_(us)  {}

   K_mf(void) assign (const char*);
   K_mf(void) IncUs  (int);

   K_mf(std::string) as_string (bool timeLongStr = true, int msLen = 6) const;

   //==============================================
   // inline
   inline K_mf(void) Encode (byte h, byte m, byte s, int us)
      {
         Time_.Encode(h, m, s);
         USec_ = us;
      }

   inline K_mf(void) Clear ()
      {
         Time_ .Clear();
         USec_ = 0;
      }

   inline K_mf(void) assign (const std::string& str)  { assign(str.c_str()); }
      
   inline K_mf(bool) IsInvalid () const  { return Time_.IsInvalid(); }

   //==============================================
   // operator
   K_mf(long) operator - (const this_type&) const; // 傳回相差的微秒

   inline K_mf(bool) operator == (const this_type& rhs) const
      { return (Time_ == rhs.Time_ && USec_ == rhs.USec_); }

   inline K_mf(bool) operator <  (const this_type& rhs) const
      {
         if (Time_ < rhs.Time_)
            return true;
         else if (Time_ == rhs.Time_ && USec_ < rhs.USec_)
            return true;
         
         return false;
      }
   
   //==============================================
   // static
   static K_mf(TTimeUs) Now ();

/*   
   
   
   // 084500080 ==> 08:45:00.080
   // 如果上面字串轉成 int 來設定的話會有問題
   K_mf(void) assign_str_hhmmssms_9 (const char*);
   
   K_mf(void) assign_hhmmssms_8 (int); // 09441691  ==> 09:44:16.910
   K_mf(void) assign_hhmmssms_9 (int); // 084500080 ==> 08:45:00.080
   K_mf(void) assign_hhmmss     (int); // 084500    ==> 08:45:00.000
   
   

   inline K_mf(void) assign_str_hhmmssms_9 (const std::string& str)  { assign_str_hhmmssms_9(str.c_str()); }
   

   //inline K_mf(const TKTime&)   GetTime   () const  { return Time_;             }
   //inline K_mf(const TKTimeMS&) GetTimeMS () const  { return TimeMS_;           }
   inline K_mf(TKTime)   GetTime   () const  { return Time_;   }
   inline K_mf(TKTimeMS) GetTimeMS () const  { return TimeMS_; }

*/
};


/////////////////////////////////////////////////////////////////////////////
// class TPerfTime
/////////////////////////////////////////////////////////////////////////////
/* AIX no gethrtime()....
LIBNSAUX_class TPerfTime
{
   typedef fixed_num<Kway::int64>  TInt64;

   Kway::int64  TimeMax_;
   Kway::int64  TimeMin_;
   Kway::int64  TimeTol_;
   Kway::int64  TimeCur_;
   Kway::int64  TimeGap_;
   Kway::int64  CntTol_;

public:
   inline K_ctor TPerfTime ()
      { Clear(); }

   K_mf(void) Start ();
   K_mf(void) Stop  ();

   K_mf(std::string) ShowState ();

   inline K_mf(void) Clear ()
      {
         TimeMax_ = 0;
         TimeMin_ = 999999999;
         TimeTol_ = TimeCur_ = TimeGap_ = CntTol_ = 0;
      }
      
   inline K_mf(Kway::int64) GetTolCnt () const  { return CntTol_; }
};

*/
/////////////////////////////////////////////////////////////////////////////
}; // namespace Nsa
}; // namespace Kway

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_NsaTime_h_)
