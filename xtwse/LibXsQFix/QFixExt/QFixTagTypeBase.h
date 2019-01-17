/////////////////////////////////////////////////////////////////////////////
#if !defined(_QFixTagTypeBase_h_)
#define _QFixTagTypeBase_h_
//---------------------------------------------------------------------------
#include <string>
#include "char_ary.h"
#include "tplns/Nsa_FixedNum.h"

/////////////////////////////////////////////////////////////////////////////
namespace Kway {
   using namespace Nsa;

namespace FIX  {
//---------------------------------------------------------------------------
class FieldBase;
   
/////////////////////////////////////////////////////////////////////////////
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(push,1)
#else
#pragma pack(1)
#endif

/////////////////////////////////////////////////////////////////////////////
// class TFixStdString1
/////////////////////////////////////////////////////////////////////////////
K_class TFixStdString1 : public std::string
{
   typedef std::string           inherited;
   typedef inherited::size_type  size_type;
   typedef TFixStdString1        this_type;

public:
   using inherited::assign;

   inline          K_ctor TFixStdString1 ()                                         {}
   inline explicit K_ctor TFixStdString1 (const char*        str) : inherited(str)  {}
   inline explicit K_ctor TFixStdString1 (const std::string& str) : inherited(str)  {}

   //==============================================
   // TFixStdString function
   K_mf(void) assign (const FieldBase&);
   
   //==============================================
   // TFixStdString inline function
   inline K_mf(void) assign (char ch)  { inherited::assign(1, ch); }
   
   inline K_mf(const std::string&) as_string () const
      {
         const std::string* str = static_cast<const std::string*>(this);
         return *str;
      }

   inline K_mf(const std::string&) as_fix  () const  { return as_string();        }
   inline K_mf(bool)               is_null () const  { return inherited::empty(); }
};


/////////////////////////////////////////////////////////////////////////////
// class TFixCaString
/////////////////////////////////////////////////////////////////////////////
template <unsigned N>
class TFixCaString : public Kstl::char_ary<char, N, 0>
{
   typedef Kstl::char_ary<char, N, 0>  inherited;

public:
   using inherited::assign;

   inline          K_ctor TFixCaString ()                       : inherited()     {}
   inline explicit K_ctor TFixCaString (const char*        str) : inherited(str)  {}
   inline explicit K_ctor TFixCaString (const std::string& str) : inherited(str)  {}

   inline K_mf(void) assign (char ch)  { inherited::set(0, ch); }

   inline K_mf(std::string) as_fix  () const  { return inherited::as_string(); }
   inline K_mf(bool)        is_null () const  { return inherited::empty();     }

   K_mf(void) append (const char* str)
   {
      char buf[512];
      sprintf(buf, "%s%s", inherited::as_string().c_str(), str);
      inherited::assign(buf);
   }
};


/////////////////////////////////////////////////////////////////////////////
// class TFixBool
/////////////////////////////////////////////////////////////////////////////
K_class TFixBool
{
public:
   typedef char  orig_type;
   
   enum {
      Bool_Null  = ' ',
      Bool_True  = 'Y',
      Bool_False = 'N',
   };

protected:
   orig_type  Data_;

public:
   inline          K_ctor TFixBool (orig_type          ch = Bool_Null) : Data_(ch)  {}
   inline explicit K_ctor TFixBool (const char*        str)                         { assign(str); }
   inline explicit K_ctor TFixBool (const std::string& str)                         { assign(str); }

   K_mf(void) assign (char);
   K_mf(void) assign (const char*);
   K_mf(void) assign (const FieldBase&);

   inline K_mf(bool) is_null () const
      {
         if (Data_ == Bool_True || Data_ == Bool_False)
            return false;
         return true;
      }

   inline K_mf(std::string) as_string () const
      {
         switch (Data_) {
            case Bool_True:   return std::string(1, (char)'Y');
            case Bool_False:  return std::string(1, (char)'N');
         }
         return std::string();
      }

   inline K_mf(void) assign (const std::string& str)  { assign(str.c_str()); }
   inline K_mf(void) clear  ()                        { Data_ = Bool_Null;   }

   inline K_mf(bool) is_true  () const  { return (Data_ == Bool_True); }
   inline K_mf(bool) is_false () const  { return (!is_true());         }
   inline K_mf(bool) as_fix   () const  { return (Data_ == Bool_True); }
};

/////////////////////////////////////////////////////////////////////////////
// class TFixChar
/////////////////////////////////////////////////////////////////////////////
class TFixChar
{
public:
   typedef char  orig_type;
   
   enum {
      Null_Val = 0,
   };

protected:
   orig_type  Data_;

public:
   inline K_ctor TFixChar (orig_type          ch = Null_Val) : Data_(ch)  {}
   inline K_ctor TFixChar (const std::string& str)                        { assign(str); }

   K_mf(void) assign (const FieldBase&);

   K_mf(void) assign (const char* str)
      {
         Data_ = Null_Val;
         if (str && strlen(str) > 0)
            Data_ = str[0];
      }

   K_mf(std::string) as_string () const
      {
         if (Data_ != Null_Val)  return std::string(1, Data_);
         return std::string();
      }

   inline K_mf(void) assign  (const std::string& str)  { assign(str.c_str()); }
   inline K_mf(void) assignT (orig_type          ch)   { Data_ = ch;          }
   inline K_mf(void) clear   ()                        { Data_ = Null_Val;    }

   inline K_mf(orig_type) as_fix   () const  { return Data_;               }
   inline K_mf(orig_type) get_orig () const  { return Data_;               }
   //inline K_mf(bool)      empty    () const  { return (Data_ == Null_Val); }
   inline K_mf(bool)      is_null  () const  { return (Data_ == Null_Val); }

   inline K_mf(bool) operator ==        (const orig_type& a) const  { return Data_ == a; }
   inline K_mf(bool) operator !=        (const orig_type& a) const  { return Data_ != a; }
   inline            operator orig_type ()                   const  { return Data_;      }
};
/////////////////////////////////////////////////////////////////////////////
// class TFixChar
/////////////////////////////////////////////////////////////////////////////
/*
class TFixChar : public Kstl::char_ary_base<char, 1>
{
   typedef Kstl::char_ary_base<char, 1>  inherited;

public:
   typedef char  orig_type;
   
   enum {
      Null_Val = 0,
   };

//protected:
//   orig_type  Data_;

public:
   inline K_ctor TFixChar (orig_type          ch = Null_Val)  { inherited::data_[0] = ch; }
   inline K_ctor TFixChar (const std::string& str)            { assign(str);           }

   K_mf(void) assign (const FieldBase&);

   K_mf(void) assign (const char* str)
      {
         inherited::data_[0] = Null_Val;
         if (str && strlen(str) > 0)
            inherited::data_[0] = str[0];
      }

   K_mf(std::string) as_string () const
      {
         if (inherited::data_[0] != Null_Val)  return std::string(1, inherited::data_[0]);
         return std::string();
      }

   inline K_mf(void) assign  (const std::string& str)  { assign(str.c_str()); }
   inline K_mf(void) assignT (orig_type          ch)   { inherited::data_[0] = ch;          }
   inline K_mf(void) clear   ()                        { inherited::data_[0] = Null_Val;    }

   inline K_mf(orig_type) as_fix   () const  { return inherited::data_[0];               }
   inline K_mf(orig_type) get_orig () const  { return inherited::data_[0];               }
   //inline K_mf(bool)      empty    () const  { return (Data_ == Null_Val); }
   inline K_mf(bool)      is_null  () const  { return (inherited::data_[0] == Null_Val); }

   inline K_mf(bool) operator ==        (const orig_type& a) const  { return inherited::data_[0] == a; }
   inline K_mf(bool) operator !=        (const orig_type& a) const  { return inherited::data_[0] != a; }
   inline            operator orig_type ()                   const  { return inherited::data_[0];      }
};
*/

/////////////////////////////////////////////////////////////////////////////
// class TFixInt
/////////////////////////////////////////////////////////////////////////////
class TFixInt
{
public:
   typedef int  orig_type;
   
   enum {
      Null_Val = -1,
   };

protected:
   orig_type  Data_;

public:
   inline K_ctor TFixInt (orig_type          val = Null_Val) : Data_(val)  {}
   inline K_ctor TFixInt (const std::string& str)                          { assign(str); }

   K_mf(void) assign (const FieldBase&);
   
   K_mf(void) assign (const char* str)
      {
         Data_ = Null_Val;
         if (str && strlen(str) > 0)
            Data_ = atoi(str);
      }

   K_mf(std::string) as_string () const
      {
         if (Data_ == Null_Val)  return std::string();
            
         char buf[20];
         sprintf(buf, "%d", Data_);
         return std::string(buf);
      }

   inline K_mf(void) assignT (orig_type          ch)   { Data_ = ch;          }
   inline K_mf(void) assign  (const std::string& str)  { assign(str.c_str()); }

   inline K_mf(orig_type) as_fix   () const  { return Data_;               }
   inline K_mf(orig_type) get_orig () const  { return Data_;               }
   //inline K_mf(bool)      empty    () const  { return (Data_ == Null_Val); }
   inline K_mf(bool)      is_null  () const  { return (Data_ == Null_Val); }

   inline operator orig_type () const  { return Data_; }
};


/////////////////////////////////////////////////////////////////////////////
// class TFixQty2
/////////////////////////////////////////////////////////////////////////////
K_class TFixQty2 : public Kstl::fixed_num<int>
{
   typedef Kstl::fixed_num<int>  inherited;

public:
   typedef inherited::orig_type  orig_type;

   static const orig_type  Null_Qty;

   using inherited::assign;

public:
   inline          K_ctor TFixQty2 (orig_type          a = Null_Qty) : inherited(a)    {}
   inline explicit K_ctor TFixQty2 (const char*        str)          : inherited(str)  {}
   inline explicit K_ctor TFixQty2 (const std::string& str)          : inherited(str)  {}

   inline K_mf(void) assign (double d)  { inherited::assign((orig_type)d); }
   inline K_mf(void) clear  ()          { inherited::assign(Null_Qty);     }

   inline K_mf(bool) is_null () const  { return (inherited::get_orig() == Null_Qty); }
   
   inline K_mf(std::string) as_string () const
      {
              if (is_null())          return std::string();
         else if (inherited::is_0())  return std::string("0");
         return inherited::as_string();
      }

   inline K_mf(double) as_fix () const
      {
         if (is_null())
            return (double)0;
         return (double)(inherited::get_orig());
      }
};


/////////////////////////////////////////////////////////////////////////////
// class TFixPri2
/////////////////////////////////////////////////////////////////////////////
K_class TFixPri2 : public TFixedDec<int64, 6>
{
   typedef TFixedDec<int64, 6>  inherited;
   typedef TFixPri2             this_type;
      
public:
   typedef inherited::orig_type  orig_type;

   static const orig_type  Null_Pri;

   using inherited::assign;

public:
   inline          K_ctor TFixPri2 (orig_type          a = Null_Pri) : inherited(a, 6)  {}
   inline explicit K_ctor TFixPri2 (const char*        str)          : inherited(str)   {}
   inline explicit K_ctor TFixPri2 (const std::string& str)          : inherited(str)   {}

   inline K_mf(void) clear ()  { *this = this_type(Null_Pri); }

   inline K_mf(bool) is_null () const  { return (inherited::get_orig() == Null_Pri); }
   
   inline K_mf(std::string) as_string () const
      {
              if (is_null())          return std::string();
         else if (inherited::is_0())  return std::string("0");
         return inherited::as_str();
      }

   inline K_mf(double) as_fix () const
      {
         if (is_null())
            return (double)0;
         return inherited::as_double();
      }
};


/////////////////////////////////////////////////////////////////////////////
// class TFixQty1
/////////////////////////////////////////////////////////////////////////////
class TFixQty1 : public Kstl::fixed_num<int>
{
   typedef Kstl::fixed_num<int>  inherited;
   typedef inherited::orig_type  orig_type;

public:
   using inherited::assign;

   inline K_mf(void) assign (double d)  { inherited::assign((orig_type)d); }
   //inline K_mf(void) assign (orig_type          n)    { inherited::assign(n);   }
   //inline K_mf(void) assign (const char*        str)  { inherited::assign(str); }
   //inline K_mf(void) assign (const std::string& str)  { inherited::assign(str); }

   inline K_mf(double) as_fix () const  { return (double)(inherited::get_orig()); }
   inline K_mf(bool)   empty  () const  { return inherited::is_0();               }
};


/////////////////////////////////////////////////////////////////////////////
// class TFixPri1
/////////////////////////////////////////////////////////////////////////////
class TFixPri1 : public TFixedDec<Kway::int64, 6>
{
   typedef TFixedDec<Kway::int64, 6>  inherited;
      
public:
   inline          K_ctor TFixPri1 ()                       : inherited()     {}
   inline explicit K_ctor TFixPri1 (const char*        str) : inherited(str)  {}
   inline explicit K_ctor TFixPri1 (const std::string& str) : inherited(str)  {}

   inline K_mf(double) as_fix () const  { return inherited::as_double(); }
   inline K_mf(bool)   empty  () const  { return inherited::is_0();      }
};


/////////////////////////////////////////////////////////////////////////////
// class TFixChar
/////////////////////////////////////////////////////////////////////////////
template <class EnumT, class ObjInfoT>
class TFixFmtChar
{
public:
   typedef EnumT     TEnum;
   typedef ObjInfoT  TObjInfo;

protected:
   TEnum  Data_;

public:
   inline explicit K_ctor TFixFmtChar (TEnum e = TObjInfo::GetEUnknown()) : Data_(e) {}
   inline explicit K_ctor TFixFmtChar (char  ch)                                     { assign(ch); }

   K_mf(void) assign (char);
   K_mf(void) assign (const char*);

   K_mf(std::string) as_string () const;
   K_mf(char)        as_char   () const;
   K_mf(std::string) as_desc   () const;

   inline K_mf(void) assign  (const std::string& str)  { assign(str.c_str()); }
   inline K_mf(void) assignT (TEnum e)      { Data_ = e;           }

   inline K_mf(char)  as_fix   () const  { return as_char(); }
   inline K_mf(TEnum) get_orig () const  { return Data_;     }
   
   inline operator TEnum () const  { return Data_; }
};


/////////////////////////////////////////////////////////////////////////////
// class TFixFmtStr
/////////////////////////////////////////////////////////////////////////////
template <class EnumT, class ObjInfoT>
class TFixFmtStr
{
public:
   typedef EnumT     TEnum;
   typedef ObjInfoT  TObjInfo;

protected:
   TEnum  Data_;

public:
   inline explicit K_ctor TFixFmtStr (TEnum e = TObjInfo::GetEUnknown()) : Data_(e) {}
   inline explicit K_ctor TFixFmtStr (const char* str)                              { assign(str); }

   K_mf(void) assign (const char*);

   K_mf(std::string) as_string () const;
   K_mf(std::string) as_desc   () const;

   inline K_mf(void) assign  (const std::string& str)  { assign(str.c_str()); }
   inline K_mf(void) assignT (TEnum e)                 { Data_ = e;           }

   inline K_mf(std::string) as_fix   () const  { return as_string(); }
   inline K_mf(TEnum)       get_orig () const  { return Data_;       }
};

/////////////////////////////////////////////////////////////////////////////
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(pop)
#else
#pragma pack()
#endif


/////////////////////////////////////////////////////////////////////////////
// class TFixFmtChar impl
/////////////////////////////////////////////////////////////////////////////
#define TFixFmtChar_Impl(return_type)                                      \
   template <class EnumT, class ObjInfoT>                                  \
   K_mf(return_type) TFixFmtChar<EnumT, ObjInfoT>::                        \
//---------------------------------------------------------------------------

TFixFmtChar_Impl(void) assign (char ch)
{
   Data_ = TObjInfo::GetEUnknown();

   int         nCur    = (int)TObjInfo::GetEStart  ();
   int         nTol    = (int)TObjInfo::GetETotal  ();
   const char* ccpList =      TObjInfo::GetItemList();

   for (; nCur<nTol; ++nCur) {
      if (ch == ccpList[nCur]) {
         Data_ = (TEnum)nCur;
         break;
      }
   }
}
//---------------------------------------------------------------------------

TFixFmtChar_Impl(void) assign (const char* str)
{
   if (str == NULL || strlen(str) == 0) {
      Data_ = TObjInfo::GetEUnknown();
      return;
   }

   char ch = str[0];

   assign(ch);
}
//---------------------------------------------------------------------------

TFixFmtChar_Impl(char) as_char () const
{
   char ch = 0;

   if (Data_ > TObjInfo::GetEUnknown() && Data_ < TObjInfo::GetETotal()) {
      const char* ccpList = TObjInfo::GetItemList();

      ch = ccpList[Data_];
   }

   return ch;
}
//---------------------------------------------------------------------------

TFixFmtChar_Impl(std::string) as_string () const
{
   char ch = as_char();
   
   if (ch)  return std::string(1, ch);

   return std::string();
}
//---------------------------------------------------------------------------

TFixFmtChar_Impl(std::string) as_desc () const
{
   if (Data_ > TObjInfo::GetEUnknown() && Data_ < TObjInfo::GetETotal()) {
      const char** ccpList = TObjInfo::GetDescList();

      return std::string(ccpList[Data_]);
   }

   return std::string();
}


/////////////////////////////////////////////////////////////////////////////
// class TFixFmtStr impl
/////////////////////////////////////////////////////////////////////////////
#define TFixFmtStr_Impl(return_type)                                       \
   template <class EnumT, class ObjInfoT>                                  \
   K_mf(return_type) TFixFmtStr<EnumT, ObjInfoT>::                         \
//---------------------------------------------------------------------------

TFixFmtStr_Impl(void) assign (const char* str)
{
   Data_ = TObjInfo::GetEUnknown();

   int         nCur    = (int)TObjInfo::GetEStart  ();
   int         nTol    = (int)TObjInfo::GetETotal  ();
   const char* ccpList =      TObjInfo::GetItemList();

   for (; nCur<nTol; ++nCur) {
      if (strcmp(str, ccpList[nCur]) == 0) {
         Data_ = (TEnum)nCur;
         break;
      }
   }
}
//---------------------------------------------------------------------------

TFixFmtStr_Impl(std::string) as_string () const
{
   if (Data_ > TObjInfo::GetEUnknown() && Data_ < TObjInfo::GetETotal()) {
      const char** ccpList = TObjInfo::GetItemList();
      return std::string(ccpList[Data_]);
   }

   return std::string();
}
//---------------------------------------------------------------------------

TFixFmtStr_Impl(std::string) as_desc () const
{
   if (Data_ > TObjInfo::GetEUnknown() && Data_ < TObjInfo::GetETotal()) {
      const char** ccpList = TObjInfo::GetDescList();

      return std::string(ccpList[Data_]);
   }

   return std::string();
}

/////////////////////////////////////////////////////////////////////////////
}; // namespace FIX
}; // namespace Kway

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_QFixTagTypeBase_h_)
