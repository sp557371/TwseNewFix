/////////////////////////////////////////////////////////////////////////////
#if defined(__BORLANDC__)
#pragma hdrstop
#pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "FixStkOrdCenter_Aux.h"
#include "Nsa_DbgInfo.h"
#include "FixStkPer.h"
//---------------------------------------------------------------------------
using namespace Kway;
using namespace Kway::FIX;
using namespace Kway::FIX::Stk;

/////////////////////////////////////////////////////////////////////////////
// class TFsOrdCenter::TSysOrdsRecorder impl
/////////////////////////////////////////////////////////////////////////////
K_ctor TFsOrdCenter::TSysOrdsRecorder::TSysOrdsRecorder (TFsSysOrds* evc, inn_type* inn)
   : inherited  (inn)
   , SysOrds_   (evc)
   , SysOrdsInn_(inn)
{
   //==============================================
   // 從房間取出委託書資料
   TFsSysOrd               sysOrdPtr;
   inn_type::rr_live_peek  peeker[] = { { 0, SysOrdRecSz },
                                        };
   for(TFixSysOrdID sysid = 0;;) {
      inn_type::rr_key     rkey = SysOrdsInn_->get_map_room_key(sysid);
      inn_type::rr_room    room = SysOrdsInn_->get_room(rkey);
      TFsStkOrderRec*      sysOrdRec = sysOrdPtr.renew();
      peeker[0].obj_ptr_ = sysOrdRec;
      if (!room.peek(peeker, peeker+numofele(peeker)))
         break;

      //資料取出OK,將取出的資料放到 SysOrds_ 之中
      ++sysid;
      SysOrds_->insert(TFsSysOrds::value_type(sysid, sysOrdPtr));            
   }
   
   inherited::reg(*SysOrds_);
}
//---------------------------------------------------------------------------

K_dtor TFsOrdCenter::TSysOrdsRecorder::~TSysOrdsRecorder ()
{
   inherited::unreg(*SysOrds_);
}
//---------------------------------------------------------------------------

K_mf(bool) TFsOrdCenter::TSysOrdsRecorder::into (inn_type& inn, TFixSysOrdID oid, const TFsStkOrder* rec)
{
   inn_type::rr_key        rkey(inn.get_map_room_key(oid-1));
   inn_type::rr_live_into  peeker[] = { { rec, SysOrdRecSz },
                                      };
   return inn.get_room(rkey).into(rkey, peeker, peeker+numofele(peeker));
}
//---------------------------------------------------------------------------

K_mf(bool)  TFsOrdCenter::TSysOrdsRecorder::into_new_room  (inn_type& inn, const TFsSysOrds::const_iterator& i)
{
   return into(inn, i.get_idx(), &(**i));
}
//---------------------------------------------------------------------------

K_mf(bool)  TFsOrdCenter::TSysOrdsRecorder::into_room  (inn_type& inn, const TFsSysOrds::iterator& i)
{
   return into(inn, i.get_idx(), &(**i));
}
//---------------------------------------------------------------------------

K_mf(TFsOrdCenter::inn_type::rr_key) TFsOrdCenter::TSysOrdsRecorder::get_room_key  (inn_type& inn, const TFsSysOrds::iterator& i)
{
   return inn_type::rr_key(inn.get_map_room_key(i.get_idx()-1));
}
//---------------------------------------------------------------------------












/*
/////////////////////////////////////////////////////////////////////////////
// class TFsOrdCenter::TSysOrdsRecorder impl
/////////////////////////////////////////////////////////////////////////////
K_ctor TFsOrdCenter::TSysOrdsRecorder::TSysOrdsRecorder (TContainer* evc, const std::string& fileName)
   : inher_recorder(&Inn_)
   , Container_    (evc)
   , Inn_          (fileName, 0, true)
   //, Inn_          (fileName, 0, false)
{
   peek_rooms(*evc, Inn_);
   inher_recorder::reg(*evc);
}
//---------------------------------------------------------------------------

K_dtor TFsOrdCenter::TSysOrdsRecorder::~TSysOrdsRecorder ()
{
   // 當結束時, 應 unreg event, 不然在觸發 event 時會 error
   inher_recorder::unreg(*Container_);
}
//---------------------------------------------------------------------------

K_mf(bool) TFsOrdCenter::TSysOrdsRecorder::into_new_room (TInn& inn, const TCtnCi& citer)
{
   //TUtilComm::ConsMsg("<TFsOrdCenter::TSysOrdsRecorder::into_new_room>");

   //
   //TCtnKey            ctnKey(citer.get_idx());
   //TCtnKey::orig_type origKey = ctnKey.get_orig();
   //
   //if (RoomKeys_.size() <= origKey)
   //   RoomKeys_.resize(origKey+1);
   //
   //rr_key&    rrkey   = (RoomKeys_[origKey] = inn.check_in(0));
   //const TRec ctnRec  = *citer;
   //

   TCtnKey    ctnKey(citer.get_idx());
   const TRec ctnRec = *citer;
   rr_key&    rrkey  = fetch_room_key(ctnKey);
   
   rrkey = inn.check_in(0);

   return into_room(inn.get_room(rrkey), rrkey, ctnKey, ctnRec);
}
//---------------------------------------------------------------------------

K_mf(bool) TFsOrdCenter::TSysOrdsRecorder::into_room (TInn& inn, const TCtnI& iter)
{
   //TUtilComm::ConsMsg("<TFsOrdCenter::TSysOrdsRecorder::into_room> 1");

   //
   //TCtnKey            ctnKey(iter.get_idx());
   //TCtnKey::orig_type origKey = ctnKey.get_orig();
   //rr_key&            rrkey   = RoomKeys_[origKey];
   //const TRec         ctnRec  = *iter;
   //

   TCtnKey    ctnKey(iter.get_idx());
   rr_key&    rrkey  = fetch_room_key(ctnKey);
   const TRec ctnRec = *iter;

   return into_room(inn.get_room(rrkey), rrkey, ctnKey, ctnRec);
}
//---------------------------------------------------------------------------

K_mf(TFsOrdCenter::TSysOrdsRecorder::rr_key) TFsOrdCenter::TSysOrdsRecorder::get_room_key (TInn& inn, const TCtnI& iter)
{
   //TUtilComm::ConsMsg("<TFsOrdCenter::TSysOrdsRecorder::get_room_key>");

   //
   //TCtnKey            ctnKey(iter.get_idx());
   //TCtnKey::orig_type origKey = ctnKey.get_orig();
   //
   //if (RoomKeys_.size() > origKey) {
   //   return RoomKeys_[origKey];
   //}
   //return rr_key();
   //

   TCtnKey ctnKey(iter.get_idx());
   return fetch_room_key(ctnKey);
}
//---------------------------------------------------------------------------

K_mf(TFsOrdCenter::TSysOrdsRecorder::rr_key&) TFsOrdCenter::TSysOrdsRecorder::fetch_room_key (const TCtnKey& ctnKey)
{
   TCtnKey::orig_type origKey = ctnKey.get_orig();

   if (RoomKeys_.size() <= origKey)
      RoomKeys_.resize(origKey+1);

   rr_key& rrkey = RoomKeys_[origKey];
   //TUtilComm::ConsMsg("<TFsOrdCenter::TSysOrdsRecorder::fetch_room_key> key[%d], RoomID[%d], RoomSize[%d]",
   //                        origKey, rrkey.get_room_id(), rrkey.get_room_size());

   return rrkey;
}
//---------------------------------------------------------------------------

K_mf(bool) TFsOrdCenter::TSysOrdsRecorder::peek_room (TContainer& evc, rr_room room)
{
   TCtnKey        ctnKey;
   TRec::ref_obj* refRec = new TRec::ref_obj;
   TRec           ctnRec(refRec);

   //**********************************************
   #if defined(_FixTag_Twse_Ver2_)
   TInn::rr_live_peek peeker[] = {
      { &ctnKey, sizeof(TCtnKey)     },
      { refRec,  sizeof(TFsStkOrder) },
   };
   
   #else //----------------------------------------

   char line[1024*10];
   
   TInn::rr_live_peek peeker[] = {
      { &ctnKey,                          sizeof(TCtnKey) },
      { K_GetLo(&peeker[2].obj_size_, 2), 2               },
      { line,                             0               },
   };
   #endif
   //**********************************************

   if (room.peek(peeker, peeker+numofele(peeker)) == true) {
      //**********************************************
      #if !defined(_FixTag_Twse_Ver2_)
      // 載入資料
      unsigned         useSize = peeker[2].obj_size_;
      Kstl::persistent per(line, useSize);

      per.rewind();

      Kstl::pload(per, *refRec);
      #endif
      //**********************************************

      // 新增到資料表
      std::pair<TCtnI, bool> ins = evc.insert(TCtnValType(ctnKey, ctnRec));
      if (!ins.second)
         const_cast<TRec&>(*ins.first) = ctnRec;
     
      //把[房間鑰匙]保留,待會兒存檔時,才找得到此人住的房間
      //RoomKeys_[key] = room.get_key();
      //TUtilComm::ConsMsg("<TFsOrdCenter::TSysOrdsRecorder::peek_room> CtnKey[%d], RoomSize[%d]",
      //                        ctnKey.get_orig(), RoomKeys_.size());
      rr_key& rrkey = fetch_room_key(ctnKey);
      rrkey = room.get_key();

      //TUtilComm::ConsMsg("<TFsOrdCenter::TSysOrdsRecorder::peek_room> key[%d], RoomID[%d], RoomSize[%d]",
      //                        ctnKey.get_orig(), rrkey.get_room_id(), rrkey.get_room_size());

      return true;
   }

   TUtilComm::ConsMsg("<TFsOrdCenter::TSysOrdsRecorder::peek_room> room.peek = false");
   
   return false;
}
//---------------------------------------------------------------------------

K_mf(bool) TFsOrdCenter::TSysOrdsRecorder::into_room (rr_room room, rr_key& rrkey, const TCtnKey& key, const TRec& rec)
{
   //TUtilComm::ConsMsg("<TFsOrdCenter::TSysOrdsRecorder::into_room> 2");
   const TRec::ref_obj* refRec = rec.get();
   bool                 valid  = true;

   if (refRec == NULL) {
      refRec = new TRec::ref_obj;
      valid  = false;
   }

   //**********************************************
   #if defined(_FixTag_Twse_Ver2_)
   TInn::rr_live_into peeker[] = {
      { &key,   sizeof(TCtnKey)     },
      { refRec, sizeof(TFsStkOrder) },
   };
   
   #else //----------------------------------------
   
   //Kstl::persistent_auto per(1024*10);
   static Kstl:: persistent_auto per(1024*10);
   //Kstl::psave(per, *refRec);

   TInn::rr_live_into peeker[] = {
      { &key,                             sizeof(TCtnKey)                                  },
      { K_GetLo(&peeker[2].obj_size_, 2), 2                                                },
      { per.getbuf(),                     static_cast<TInn::rr_size>(per.get_saved_size()) },
   };
   #endif
   //**********************************************
   
   bool resu = room.into(rrkey, peeker, peeker+numofele(peeker));
   
   if (valid == false)
      delete refRec;

   return resu;
}
//---------------------------------------------------------------------------
*/
