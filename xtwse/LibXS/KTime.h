//
// class TKTime :�ɶ� hh:mm:ss
// class TKDate :���
//
// ����: $Id: KTime.h,v 1.9 2005/08/08 08:50:36 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef KTimeH
#define KTimeH
//---------------------------------------------------------------------------
#include <string>
//---------------------------------------------------------------------------
#include "KwayBase.h"
//---------------------------------------------------------------------------
namespace Kway {
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(push,1)
#else
#pragma pack(1)
#endif
/////////////////////////////////////////////////////////////////////////////
LIBNS_class  TKTime
{
   byte  Hour_;
   byte  Min_;
   byte  Sec_;
public:
   inline K_ctor             TKTime     ()                       :Hour_(99),Min_(0),Sec_(0) {}
   inline K_ctor             TKTime     (byte h, byte m, byte s) :Hour_(h),Min_(m),Sec_(s)  {}
   inline K_ctor             TKTime     (dword hhmmss)           { assign(hhmmss); }
          K_mf(std::string)  as_string  (bool longStr=true) const;
   inline K_mf(const char*)  assign     (const std::string& str)   { return assign(str.c_str()); }
          K_mf(const char*)  assign     (const char*); //�Ǧ^�ɶ��r�ꪺ������m
          K_mf(bool)         assign     (dword hhmmss);

   inline K_mf(byte)  GetHour    () const    { return Hour_;        }
   inline K_mf(byte)  GetMin     () const    { return Min_;         }
   inline K_mf(byte)  GetSec     () const    { return Sec_;         }
   inline K_mf(dword) GetHHMM    () const    { return Hour_*100 + Min_; }
   inline K_mf(dword) GetHHMMSS  () const    { return GetHHMM()*100 + Sec_; }
   inline K_mf(bool)  IsInvalid  () const    { return(Hour_ >= 24); }
   inline K_mf(void)  Clear      ()          { Hour_ = 99;          }
   inline K_mf(void)  SetHour    (byte h)    { Hour_ = h; }
   inline K_mf(void)  SetMin     (byte m)    { Min_  = m; }
   inline K_mf(void)  SetSec     (byte s)    { Sec_  = s; }
   inline K_mf(void)  Encode     (byte h, byte m, byte s)   { SetHour(h);  SetMin(m);  SetSec(s); }
   inline K_mf(void)  IncHour    (int hours= 1) { IncMin(60 * hours); }
   inline K_mf(void)  IncMin     (int mins = 1) { IncSec(60 * mins);  }
          K_mf(void)  IncSec     (int secs = 1);
         K_mf(size_t) GetSecOfDay () const;

          K_mf(long)  operator-  (const TKTime& a) const;//�Ǧ^�ۮt����
   inline K_mf(bool)  operator ==(const TKTime& a) const  { return(Hour_==a.Hour_  &&  Min_==a.Min_  &&  Sec_==a.Sec_); }
   inline K_mf(bool)  operator !=(const TKTime& a) const  { return !(*this==a); }
   inline K_mf(bool)  operator < (const TKTime& a) const  { return(Hour_==a.Hour_ ? (Min_==a.Min_ ? Sec_ < a.Sec_
                                                                                                  : Min_ < a.Min_)
                                                                                  :  Hour_ < a.Hour_); }
   inline K_mf(bool)  operator <= (const TKTime& a) const { return !(a < *this); }
   inline K_mf(bool)  operator >  (const TKTime& a) const { return a < *this;    }
   inline K_mf(bool)  operator >= (const TKTime& a) const { return !(*this < a); }

          K_mf(bool)    SetSystem  () const;
   static K_mf(TKTime)  Now        ();
};
//---------------------------------------------------------------------------
LIBNS_class  TKTimeMS //�d�����@��(ms).
{
   word  MS_;
public:
   inline K_ctor             TKTimeMS   (word ms=0) :MS_(ms) {}
   inline K_mf(const char*)  assign     (const std::string& str)   { return assign(str.c_str()); }
          K_mf(const char*)  assign     (const char*); //�Ǧ^�ɶ��r�ꪺ������m
   /** ��X�榡: sprintf(buf, "%0*d", len, MS_);
       �Y len <= 0  ||  len > 3, �h len = 3,
       �Y len == 1, �h ��X�Q�����@��: sprintf(buf, "%0*d", len, MS_/100);
       �Y len == 2, �h ��X�ʤ����@��: sprintf(buf, "%0*d", len, MS_/10);
   */
   K_mf(std::string)  as_string  (int len=3) const;

   inline K_mf(word)  GetMS  () const    { return MS_; }
   inline K_mf(void)  SetMS  (word ms)   { MS_ = ms;   }

   inline K_mf(bool)  operator ==(const TKTimeMS& a) const  { return(MS_ == a.MS_); }
   inline K_mf(bool)  operator < (const TKTimeMS& a) const  { return(MS_  < a.MS_); }

   static K_mf(TKTimeMS)  Now  ();
};
//---------------------------------------------------------------------------
LIBNS_class  TKDate
{
   typedef word   DataType;
   DataType    Data_;   // ����~ =   Data_ / (12*31)  �i�B�z����0�~-176�~
                        //     �� = ((Data_ / 31) % 12) + 1
                        //     �� =  (Data_ % 31) + 1
public:
   inline K_ctor  TKDate   ()                        : Data_(0) {}
   inline K_ctor  TKDate   (word y, byte m, byte d)  { Encode(y,m,d);  }
   inline K_ctor  TKDate   (dword yymmdd)            { assign(yymmdd); }

   enum TStrFmt {
      sf_E = 0x1000,    //�褸
      sf_C = 0x0000,    //����
      sf_YSelMask = 0x1000,
      sf_YLenMask = 0x0f00,
      sf_MLenMask = 0x00f0,
      sf_DLenMask = 0x000f,

      sf_ELong  = sf_E|0x0422,  //�褸:yyyy/mm/dd:2003/10/31  2003/08/02
      sf_EShort = sf_E|0x0411,  //     yyyy/m/d  :2003/10/31  2003/8/2
      sf_CLong  = sf_C|0x0222,  //����:yy/mm/dd  :92/10/31    92/08/02  <=�w�]��
      sf_CShort = sf_C|0x0211,  //����:yy/mm/dd  :92/10/31    92/8/2
   };
   static inline K_mf(int)  GetYLen (TStrFmt sf)   { return(sf & sf_YLenMask) >> 8; }
   static inline K_mf(int)  GetMLen (TStrFmt sf)   { return(sf & sf_MLenMask) >> 4; }
   static inline K_mf(int)  GetDLen (TStrFmt sf)   { return(sf & sf_DLenMask);      }

          K_mf(std::string)  as_string (TStrFmt = sf_CLong, char spl='/') const;
   inline K_mf(const char*)  assign     (const std::string& str)   { return assign(str.c_str()); }
          K_mf(const char*)  assign     (const char*); //�Ǧ^����r�ꪺ������m
          K_mf(bool)         assign     (dword yymmdd);//if(yy>=1911)�h���褸�~,�_�h������~.

   inline K_mf(byte)  GetCYear   () const    { return static_cast<byte>( Data_ / (12*31) );        }
   inline K_mf(word)  GetEYear   () const    { return static_cast<word>( Data_ / (12*31) + 1911 ); }
   inline K_mf(byte)  GetMon     () const    { return static_cast<byte>((Data_ / 31)%12  + 1);     }
   inline K_mf(byte)  GetDay     () const    { return static_cast<byte>((Data_ % 31)     + 1);     }
   inline K_mf(dword) GetYYYYMMDD() const    { return GetEYear()*10000 + GetMon()*100 + GetDay();  }
   inline K_mf(bool)  IsInvalid  () const    { return(Data_==0);               }
   inline K_mf(void)  Clear      ()          { Data_ = 0;                      }
   inline K_mf(void)  SetCYear   (word cy)   { Encode(cy, GetMon(), GetDay()); }
   inline K_mf(void)  SetEYear   (word ey)   { Encode(ey, GetMon(), GetDay()); }
   inline K_mf(void)  SetMon     (byte m)    { Encode(GetCYear(), m, GetDay());}
   inline K_mf(void)  SetDay     (byte d)    { Encode(GetCYear(), GetMon(), d);}
          K_mf(void)  Encode     (word y, byte m, byte d);//if(y>=1911) {y=�褸} else {y=����}, m=1--12, d=1--31
          K_mf(void)  IncDay     (int days = 1);

   inline K_mf(bool)  operator== (const TKDate& a) const  { return(Data_ ==a.Data_); }
   inline K_mf(bool)  operator!= (const TKDate& a) const  { return(Data_ !=a.Data_); }
   inline K_mf(bool)  operator<  (const TKDate& a) const  { return(Data_ < a.Data_); }
   inline K_mf(bool)  operator>  (const TKDate& a) const  { return(Data_ > a.Data_); }
   inline K_mf(bool)  operator>= (const TKDate& a) const  { return(Data_ >=a.Data_); }
   inline K_mf(bool)  operator<= (const TKDate& a) const  { return(Data_ <=a.Data_); }
          K_mf(long)  operator-  (const TKDate&) const;//�Ǧ^�ۮt���Ѽ�.
          K_mf(bool)  InRange    (const TKDate& b, const TKDate& e) const;

   //��L������U functions
   inline K_mf(bool)  IsLeapYear () const   { return IsLeapYear(GetEYear()); }
          K_mf(word)  GetYDay    () const;//this�����~���ĴX��,�q1�}�l�p��
          K_mf(byte)  GetWDay    () const;//this�����g���ĴX��,0-Sunday,1-6(�P���@��P����)

          K_mf(bool)    SetSystem  () const;
   static K_mf(TKDate)  Now        ();
   static K_mf(bool)    IsLeapYear (int year);  //�O�_���|�~
};
//---------------------------------------------------------------------------
LIBNS_class  TKDateTime
{
protected:
   TKDate   Date_;
   TKTime   Time_;
public:
   inline K_ctor           TKDateTime  ()                            : Date_(),  Time_()  {}
   inline explicit K_ctor  TKDateTime  (TKDate d, TKTime t=TKTime()) : Date_(d), Time_(t) {}

   inline K_mf(bool) IsInvalid () const      { return(Date_.IsInvalid() || Time_.IsInvalid()); }

   inline K_mf(void) SetDate     (TKDate d)              { Date_=d; }
   inline K_mf(void) SetTime     (TKTime t)              { Time_=t; }
   inline K_mf(void) SetDateTime (TKDate d, TKTime t)    { Date_=d;  Time_=t; }
   inline K_mf(void) SetDateTime (const TKDateTime& dt)  { *this = dt; }

   inline K_mf(TKDate)  GetDate  () const    { return Date_; }
   inline K_mf(TKTime)  GetTime  () const    { return Time_; }

   inline K_mf(void)  IncHour  (int hours= 1) { IncMin(60 * hours); }
   inline K_mf(void)  IncMin   (int mins = 1) { IncSec(60 * mins);  }
          K_mf(void)  IncSec   (int secs = 1);
   inline K_mf(void)  IncDay   (int days = 1) { Date_.IncDay(days); }

          K_mf(std::string)  as_string  (TKDate::TStrFmt = TKDate::sf_CLong, char spl='/', bool timeLongStr=true) const;
   inline K_mf(const char*)  assign     (const std::string& str)   { return assign(str.c_str()); }
          K_mf(const char*)  assign     (const char*); //�Ǧ^[���+�ɶ�]�r�ꪺ������m

   inline K_mf(bool)  operator ==(const TKDateTime& a) const  { return(Date_ ==a.Date_  &&  Time_ ==a.Time_); }
   inline K_mf(bool)  operator !=(const TKDateTime& a) const  { return !(this->operator==(a)); }
   inline K_mf(bool)  operator < (const TKDateTime& a) const  { return(Date_ ==a.Date_  ?   Time_ < a.Time_   :  Date_ < a.Date_); }
   inline K_mf(bool)  operator > (const TKDateTime& a) const  { return  (a < *this); }
   inline K_mf(bool)  operator>= (const TKDateTime& a) const  { return !(*this < a); }
   inline K_mf(bool)  operator<= (const TKDateTime& a) const  { return !(a < *this); }
          K_mf(long)  operator - (const TKDateTime&)  const;//�Ǧ^�ۮt�����.

          K_mf(bool)        SetSystem  () const;
   static K_mf(TKDateTime)  Now        ();
};
//---------------------------------------------------------------------------
LIBNS_class  TKDateTimeMS : public TKDateTime  
{
   TKTimeMS TimeMS_;
public:
   inline K_ctor  TKDateTimeMS  (TKDate d=TKDate(), TKTime t=TKTime(), TKTimeMS ms=TKTimeMS())
                     : TKDateTime(d,t)
                     , TimeMS_(ms)
                     { }

   inline K_mf(TKTimeMS)  GetMS  () const       { return TimeMS_; }
   inline K_mf(void)      SetMS  (TKTimeMS ms)  { TimeMS_ = ms;   }

          K_mf(void)  IncMS  (long ms);

   /** msLen==0��ܤ���Xms, >0��ܥ[�W ".ms", <0��ܥ[�W "ms"
   */
   K_mf(std::string)  as_string  (TKDate::TStrFmt = TKDate::sf_CLong, char spl='/', bool timeLongStr=true,
                                  int msLen = 3) const;

          K_mf(const char*)  assign     (const char*); //�Ǧ^[���+�ɶ�]�r�ꪺ������m
   inline K_mf(const char*)  assign     (const std::string& str)   { return assign(str.c_str()); }

   inline K_mf(bool)  operator ==(const TKDateTimeMS& a) const  { return(TKDateTime::operator==(a)  &&  TimeMS_==a.TimeMS_); }
   inline K_mf(bool)  operator < (const TKDateTimeMS& a) const  { return( Date_ == a.Date_
                                                                        ? Time_ == a.Time_ ? TimeMS_ < a.TimeMS_
                                                                                           : Time_   < a.Time_
                                                                        : Date_ <  a.Date_); }
   static K_mf(TKDateTimeMS)  Now  ();
          K_mf(int)   operator - (const TKDateTimeMS&)  const;//�Ǧ^�ۮt�������.
};
/////////////////////////////////////////////////////////////////////////////
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(pop)
#else
#pragma pack()
#endif
//---------------------------------------------------------------------------
} // namespace Kway
//---------------------------------------------------------------------------
#endif

