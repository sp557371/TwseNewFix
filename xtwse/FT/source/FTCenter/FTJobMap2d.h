//---------------------------------------------------------------------------
#ifndef JobMap2dH
#define JobMap2dH
//---------------------------------------------------------------------------
#include "ev_map.h"
#include "ev_map2d.h"
#include "KwayBase.h"
#include "FTJobDef.h"
//---------------------------------------------------------------------------
namespace Kway {
namespace Tw   {
namespace Bx   {
namespace FT   {
//---------------------------------------------------------------------------
//typedef fixed_num<dword>    TSysID;
K_class TSysID : public fixed_num<dword>
{
private:
   typedef fixed_num<dword> inherited;
   typedef TSysID           this_type;
public:
   K_ctor     TSysID():inherited(){};
   K_ctor     TSysID(dword n):inherited(n){};
   K_ctor     TSysID(const std::string& s):inherited(s){};
   K_mf(bool) operator < (const TSysID& a) const
      {return *this > a;}
};
//---------------------------------------------------------------------------
K_class TFtJobs : public Kstl::ev_map <TSysID, TFTJob>
{
  TBrkMkt    BrkMkt_;
public:
  K_mf(TBrkMkt) get_master_key() const                   { return BrkMkt_;        }
  K_mf(void)   set_master_key(const TBrkMkt& masterkey)  { BrkMkt_ = masterkey;   }
};
//---------------------------------------------------------------------------
struct TBrokJob
{
   TFtJobs  Jobs_;
   K_mf(TFtJobs*) GetContainer(void) {return &Jobs_;}
};
//---------------------------------------------------------------------------
typedef Kstl::ev_map<TBrkMkt, TBrokJob> TBrkJobs;
//---------------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////
struct TFtJobMapMaker : public Kstl::map2d_maker_base<TBrkJobs, TFtJobs> //
{
   typedef Kstl::map2d_maker_base<TBrkJobs, TFtJobs> inherited;
   struct value_type
   {
      key_type             key_;
      detail_value_type    detail_value_;
      K_ctor  value_type  (const key_type& k, const mapped_type& m)
                              : key_(k), detail_value_(k.detail_key_, m)
                              {}
      K_ctor  value_type  ()  {}
   };
   inline K_mf(const master_key_type)    get_master_key   (const value_type& v)
                                const   { return v.key_.master_key_;        }
   inline K_mf(const detail_map_type*)   get_detail_map   (const master_value_type& v)
                                const   { return &(v.second.Jobs_);         }
   inline K_mf(detail_map_type*)         get_detail_map   (      master_value_type& v)
                                const   { return &(v.second.Jobs_);         }
   inline K_mf(detail_map_type*)         fetch_detail_map (const master_iterator&   i)
                                const   { return &(i->second.Jobs_);        }
   inline K_mf(const detail_value_type&) get_detail_value (const value_type& v)
                                const   { return v.detail_value_;           }
   static inline K_mf(void)  detail_set_master_key  (detail_map_type& dmap, const inherited::master_key_type& key)
                                        { dmap.set_master_key(key);         }

   static K_mf(bool) find_master_iterator(inherited::master_map_type& mmap
                                          ,const inherited::detail_map_type& dmap
                                          ,inherited::master_iterator& imaster)
   {
      master_iterator ifind = mmap.find(dmap.get_master_key());
      if(ifind == mmap.end())
         return false;
      imaster = ifind;
      return true;
   }
};
//---------------------------------------------------------------------------
};  // FT
};  // Bx
};  // Tw
};  // Kway
#endif
