/////////////////////////////////////////////////////////////////////////////
#if !defined(_FixStkOrdCenter_Aux_h_)
#define _FixStkOrdCenter_Aux_h_
//---------------------------------------------------------------------------
#include <deque>
#include "SimpleRecorder.h"
#include "Nsa_UtilComm.h"
#include "tplns/Nsa_EvCtn.h"
#include "FixStkOrdCenter.h"

/////////////////////////////////////////////////////////////////////////////
namespace Kway {
   using namespace Nsa;

namespace FIX  {
namespace Stk  {

/////////////////////////////////////////////////////////////////////////////
// class TFsOrdCenter::TSysOrdsRecorder
/////////////////////////////////////////////////////////////////////////////
class TFsOrdCenter::TSysOrdsRecorder : public Kstl::recorder<TFsSysOrds, inn_type>
{
   typedef Kstl::recorder<TFsSysOrds, inn_type>  inherited;

   TFsSysOrds*  SysOrds_;
   inn_type*    SysOrdsInn_;

public:
   K_ctor  TSysOrdsRecorder (TFsSysOrds*, inn_type* inn);
   K_dtor ~TSysOrdsRecorder ();

private:
   K_mf(bool)   into          (inn_type&, TFixSysOrdID, const TFsStkOrder*);
   K_mf(bool)   into_new_room (inn_type&, const TFsSysOrds::const_iterator&);
   K_mf(bool)   into_room     (inn_type&, const TFsSysOrds::iterator&);
   K_mf(rr_key) get_room_key  (inn_type&, const TFsSysOrds::iterator&);
};

/*
class TFsOrdCenter::TSysOrdsRecorder : public  Kstl::recorder        <TFsSysOrds, TEvCtnInn>,
                                       private Kstl::inn_rooms_peeker<TFsSysOrds, TEvCtnInn>
{
   typedef Kstl::recorder<TFsSysOrds, TEvCtnInn>  inher_recorder;

   typedef TFsSysOrds                  TContainer;
   typedef TFsSysOrd                   TRec;

   typedef inher_recorder::TInn        TInn;
   typedef inher_recorder::rr_key      rr_key;
   typedef inher_recorder::rr_room     rr_room;
   typedef TInn::rr_size               rr_size;

   typedef TContainer::key_type        TCtnKey;
   typedef TContainer::value_type      TCtnValType;
   typedef TContainer::iterator        TCtnI;
   typedef TContainer::const_iterator  TCtnCi;

   //typedef std::vector<rr_key>         TRoomKeys;
   typedef std::deque<rr_key>          TRoomKeys;

   TContainer*  Container_;
   TInn         Inn_;
   TRoomKeys    RoomKeys_;

public:
   K_ctor  TSysOrdsRecorder (TContainer*, const std::string& fileName);
   K_dtor ~TSysOrdsRecorder ();
   
private:
   //==============================================
   // overwrite Kstl::recorder virtual function
   K_mf(bool)   into_new_room (TInn&, const TCtnCi&);
   K_mf(bool)   into_room     (TInn&, const TCtnI&);
   K_mf(rr_key) get_room_key  (TInn&, const TCtnI&);

   //==============================================
   // overwrite Kstl::inn_rooms_peeker virtual function
   K_mf(bool) peek_room (TContainer&, rr_room);
   
   //==============================================
   // TSysOrdsRecorder function
   K_mf(bool)    into_room      (rr_room, rr_key&, const TCtnKey&, const TRec&);
   K_mf(rr_key&) fetch_room_key (const TCtnKey&);
};
*/

/////////////////////////////////////////////////////////////////////////////
}; // namespace Stk
}; // namespace FIX
}; // namespace Kway

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_FixStkOrdCenter_Aux_h_)
