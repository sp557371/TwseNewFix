//---------------------------------------------------------------------------
#ifndef FileLoaderH
#define FileLoaderH
//---------------------------------------------------------------------------
#include "KFile.h"
#include "ev_map.h"
#include "char_ary.h"
//---------------------------------------------------------------------------
namespace Kway{
//---------------------------------------------------------------------------
template<class TContainer, class TRec>           // 一維資料表
K_mf(bool) LoadMap(TContainer* c, char* FileName)
{
   Kway::TFile fd;

   fd.Open(FileName, fmReadSeq);
   if(!fd)
      return false;
   fd.Seek(0,fsBegin);
   typename TContainer::iterator iter_;
   typename TContainer::key_type Key_;

   size_t                   Recsz = sizeof(TRec);
   size_t                   RecRd = 65535 / Recsz;
   TRec*                    rec = new TRec[RecRd];
   while (!fd.IsEOF()) {
      int Ln_ = fd.Read(rec, Recsz, RecRd);
      for (int L = 0; L<Ln_; L++) {
         if (!(rec[L] >> &Key_))
            continue;
         iter_ = c->fetch(Key_);
         typename TContainer::updater  uptr_ = c->begin_update(iter_);
         rec[L] >> uptr_;
      }
   }
   fd.Close();
   delete rec;
   return true;
}
//---------------------------------------------------------------------------
template<class TContainer, class TRec>           // 一維資料表
K_mf(bool) UpdateMap(TContainer* c, char* FileName, Kway::dword& pos)
{
   Kway::TFile fd;

   fd.Open(FileName, fmReadSeq);
   if(!fd)
      return false;
   typename TContainer::iterator iter_;
   typename TContainer::key_type Key_;

   size_t                   Recsz = sizeof(TRec);
   size_t                   RecRd = 65535 / Recsz;
   TRec*                    rec = new TRec[RecRd];
   if(pos * Recsz > fd.GetFileSize())
      pos = 0;
   fd.Seek(pos*Recsz, fsBegin);
   while (!fd.IsEOF()) {
      int Ln_ = fd.Read(rec, Recsz, RecRd);
      for (int L = 0; L<Ln_; L++) {
         if (!(rec[L] >> &Key_))
            continue;
         iter_ = c->fetch(Key_);
         typename TContainer::updater  uptr_ = c->begin_update(iter_);
         rec[L] >> uptr_;
      }
      pos += Ln_;
   }
   fd.Close();
   delete rec;
   return true;
}
//---------------------------------------------------------------------------
template<class TContainer, class TRec>           // 一維資料表
K_mf(bool) LoadFMap(TContainer* c, char* FileName)
{
   Kway::TFile fd;
   fd.Open(FileName, fmReadSeq);
   if(!fd)
      return false;
   fd.Seek(0,fsBegin);
   typename TContainer::iterator iter_;
   typename TContainer::key_type Key_;

   size_t                   Recsz = sizeof(TRec);
   size_t                   RecRd = 65535 / Recsz;
   TRec*                    rec = new TRec[RecRd];
   while (!fd.IsEOF()) {
      int Ln_ = fd.Read(rec, Recsz, RecRd);
      for (int L = 0; L<Ln_; L++) {
         if (!(rec[L] >> &Key_))
            continue;
         iter_ = c->find(Key_);
         if (iter_ != c->end()){
           typename TContainer::updater  uptr_ = c->begin_update(iter_);
           rec[L] >> uptr_;
           }
      }
   }
   fd.Close();
   delete rec;
   return true;
}
//---------------------------------------------------------------------------
template<class TContainer, class TRec>           // 一維資料表
K_mf(bool) UpdateFMap(TContainer* c, char* FileName, Kway::dword& pos)
{
   Kway::TFile fd;
   fd.Open(FileName, fmReadSeq);
   if(!fd)
      return false;
   typename TContainer::iterator iter_;
   typename TContainer::key_type Key_;

   size_t                   Recsz = sizeof(TRec);
   size_t                   RecRd = 65535 / Recsz;
   TRec*                    rec = new TRec[RecRd];
   if(pos * Recsz > fd.GetFileSize())
      pos = 0;
   fd.Seek(pos*Recsz, fsBegin);
   while (!fd.IsEOF()) {
      int Ln_ = fd.Read(rec, Recsz, RecRd);
      for (int L = 0; L<Ln_; L++) {
         if (!(rec[L] >> &Key_))
            continue;
         iter_ = c->find(Key_);
         if (iter_ != c->end()){
           typename TContainer::updater  uptr_ = c->begin_update(iter_);
           rec[L] >> uptr_;
           }
      }
      pos += Ln_;
   }
   fd.Close();
   delete rec;
   return true;
}
//---------------------------------------------------------------------------
template<class TContainer1, class TContainer2, class TRec>           // 二維資料表
K_mf(bool) LoadMap2d(TContainer1* c, char* FileName)
{
   Kway::TFile fd;
   fd.Open(FileName, fmReadSeq);
   if(!fd)
      return false;
   fd.Seek(0,fsBegin);
   typename TContainer1::iterator iter_;
   typename TContainer1::updater  uptr_;
   typename TContainer1::key_type Key_;
   typename TContainer2::iterator iter2_;
   typename TContainer2::updater  uptr2_;
   typename TContainer2::key_type Key2_;

   size_t                          Recsz = sizeof(TRec);
   size_t                          RecRd = 65535 / Recsz;
   TRec*                           rec = new TRec[RecRd];
   TContainer2*                    c2;
   while (!fd.IsEOF()) {
      int Ln_ = fd.Read(rec, Recsz, RecRd);
      for (int L = 0; L<Ln_; L++) {
         if (!(rec[L] >> &Key_))
            continue;
         iter_ = c->fetch(Key_);
         uptr_ = c->begin_update(iter_);
         if (!(rec[L] >> &Key2_))
            continue;
         c2 =  (TContainer2*)(uptr_->GetContainer(c2));
         iter2_ = c2->fetch(Key2_);
         uptr2_ = c2->begin_update(iter2_);
         rec[L] >> uptr2_;
      }
   }
   fd.Close();
   delete rec;
   return true;
}
//---------------------------------------------------------------------------
template<class TContainer1, class TContainer2, class TRec>           // 二維資料表
K_mf(bool) UpdateMap2d(TContainer1* c, char* FileName, Kway::dword& pos)
{
   Kway::TFile fd;
   fd.Open(FileName, fmReadSeq);
   if(!fd)
      return false;
   typename TContainer1::iterator iter_;
   typename TContainer1::updater  uptr_;
   typename TContainer1::key_type Key_;
   typename TContainer2::iterator iter2_;
   typename TContainer2::updater  uptr2_;
   typename TContainer2::key_type Key2_;

   size_t                          Recsz = sizeof(TRec);
   size_t                          RecRd = 65535 / Recsz;
   TRec*                           rec = new TRec[RecRd];
   TContainer2*                    c2;
   if(pos * Recsz > fd.GetFileSize())
      pos = 0;
   fd.Seek(pos*Recsz, fsBegin);
   while (!fd.IsEOF()) {
      int Ln_ = fd.Read(rec, Recsz, RecRd);
      for (int L = 0; L<Ln_; L++) {
         if (!(rec[L] >> &Key_))
            continue;
         iter_ = c->fetch(Key_);
         uptr_ = c->begin_update(iter_);
         if (!(rec[L] >> &Key2_))
            continue;
         c2 =  (TContainer2*)(uptr_->GetContainer(c2));
         iter2_ = c2->fetch(Key2_);
         uptr2_ = c2->begin_update(iter2_);
         rec[L] >> uptr2_;
      }
      pos += Ln_;
   }
   fd.Close();
   delete rec;
   return true;
}
//---------------------------------------------------------------------------
template<class TContainer1, class TContainer2, class TContainer3, class TRec>           // 三維資料表
K_mf(bool) LoadMap3d(TContainer1* c, char* FileName)
{
   Kway::TFile fd;
   fd.Open(FileName, fmReadSeq);
   if(!fd)
      return false;
   fd.Seek(0,fsBegin);
   typename TContainer1::iterator  iter_;
   typename TContainer1::updater   uptr_;
   typename TContainer1::key_type  Key_;
   typename TContainer2::iterator  iter2_;
   typename TContainer2::updater   uptr2_;
   typename TContainer2::key_type  Key2_;
   typename TContainer3::iterator  iter3_;
   typename TContainer3::updater   uptr3_;
   typename TContainer3::key_type  Key3_;

   size_t                          Recsz = sizeof(TRec);
   size_t                          RecRd = 65535 / Recsz;
   TRec*                           rec = new TRec[RecRd];
   TContainer2*                    c2;
   TContainer3*                    c3;
   while (!fd.IsEOF()) {
      int Ln_ = fd.Read(rec, Recsz, RecRd);
      for (int L = 0; L<Ln_; L++) {
         if (!(rec[L] >> &Key_))
            continue;
         iter_ = c->fetch(Key_);
         uptr_ = c->begin_update(iter_);
         if (!(rec[L] >> &Key2_))
            continue;
         c2 =  (TContainer2*)(uptr_->GetContainer(c2));
         iter2_ = c2->fetch(Key2_);
         uptr2_ = c2->begin_update(iter2_);
         if (!(rec[L] >> &Key3_))
            continue;
         c3 =  (TContainer3*)(uptr2_->GetContainer(c3));
         iter3_ = c3->fetch(Key3_);
         uptr3_ = c3->begin_update(iter3_);
         rec[L] >> uptr3_;
      }
   }
   fd.Close();
   delete rec;
   return true;
}
//---------------------------------------------------------------------------
template<class TContainer1, class TContainer2, class TContainer3, class TRec>           // 三維資料表
K_mf(bool) UpdateMap3d(TContainer1* c, char* FileName, Kway::dword& pos)
{
   Kway::TFile fd;
   fd.Open(FileName, fmReadSeq);
   if(!fd)
      return false;
   typename TContainer1::iterator  iter_;
   typename TContainer1::updater   uptr_;
   typename TContainer1::key_type  Key_;
   typename TContainer2::iterator  iter2_;
   typename TContainer2::updater   uptr2_;
   typename TContainer2::key_type  Key2_;
   typename TContainer3::iterator  iter3_;
   typename TContainer3::updater   uptr3_;
   typename TContainer3::key_type  Key3_;

   size_t                          Recsz = sizeof(TRec);
   size_t                          RecRd = 65535 / Recsz;
   TRec*                           rec = new TRec[RecRd];
   TContainer2*                    c2;
   TContainer3*                    c3;
   if(pos * Recsz > fd.GetFileSize())
      pos = 0;
   fd.Seek(pos*Recsz, fsBegin);
   while (!fd.IsEOF()) {
      int Ln_ = fd.Read(rec, Recsz, RecRd);
      for (int L = 0; L<Ln_; L++) {
         if (!(rec[L] >> &Key_))
            continue;
         iter_ = c->fetch(Key_);
         uptr_ = c->begin_update(iter_);
         if (!(rec[L] >> &Key2_))
            continue;
         c2 =  (TContainer2*)(uptr_->GetContainer(c2));
         iter2_ = c2->fetch(Key2_);
         uptr2_ = c2->begin_update(iter2_);
         if (!(rec[L] >> &Key3_))
            continue;
         c3 =  (TContainer3*)(uptr2_->GetContainer(c3));
         iter3_ = c3->fetch(Key3_);
         uptr3_ = c3->begin_update(iter3_);
         rec[L] >> uptr3_;
      }
      pos += Ln_;
   }
   fd.Close();
   delete rec;
   return true;
}
//---------------------------------------------------------------------------
template<class TContainer, class TRec>           // CSV 一維資料表
K_mf(bool) LoadCSVMap(TContainer* c, char* FileName, char seperator = ',')
{
   Kway::TFile fd;
   fd.Open(FileName, fmReadSeq);
   if(!fd)
      return false;
   fd.Seek(0,fsBegin);

   typedef typename TRec::value_type  TValue;

   typename TContainer::iterator iter_;
   typename TContainer::key_type Key_;
   char  lnChar;
   char  buf[4096];
   char* Pos;
   char* StartPos;
   TRec recs;
   
   while (!fd.IsEOF()) {
      int Ln_ = fd.ReadLN(buf, sizeof(buf), lnChar, "\n");
      if(Ln_ == 0)
         continue;
      recs.clear();
      StartPos = buf;
      while((Pos = strchr(StartPos, seperator))){
      	*Pos = 0;
      	TValue v(StartPos);
      	recs.push_back(v);
      	StartPos = Pos+1;
      }
      TValue v(StartPos);
      recs.push_back(v);
      if (!(recs >> &Key_))
         continue;
      iter_ = c->fetch(Key_);
      typename TContainer::updater  uptr_ = c->begin_update(iter_);
      recs >> uptr_;
   }
   fd.Close();
   return true;
}
//---------------------------------------------------------------------------
template<class TContainer, class TRec>           // 一維資料表
K_mf(bool) UpdateCSVMap(TContainer* c, char* FileName, Kway::dword& lastpos, char seperator = ',')
{
   Kway::TFile fd;

   fd.Open(FileName, fmReadSeq);
   if(!fd)
      return false;
   typedef typename TRec::value_type  TValue;
   typename TContainer::iterator iter_;
   typename TContainer::key_type Key_;

   char  lnChar;
   char  buf[4096];
   char* Pos;
   char* StartPos;
   TRec recs;
   if(lastpos > fd.GetFileSize())
      lastpos = 0;
   fd.Seek(lastpos, fsBegin);
   while (!fd.IsEOF()) {
      int Ln_ = fd.ReadLN(buf, sizeof(buf), lnChar, "\n");
      if(Ln_ == 0)
         continue;
      lastpos += Ln_;
      recs.clear();
      StartPos = buf;
      while((Pos = strchr(StartPos, seperator))){
      	*Pos = 0;
      	TValue v(StartPos);
      	recs.push_back(v);
      	StartPos = Pos+1;
      }
      TValue v(StartPos);
      recs.push_back(v);
      if (!(recs >> &Key_))
         continue;
      iter_ = c->fetch(Key_);
      typename TContainer::updater  uptr_ = c->begin_update(iter_);
      recs >> uptr_;
   }
   fd.Close();
   return true;
}
//---------------------------------------------------------------------------
template<class TContainer1, class TContainer2, class TRec>           // CSV 二維資料表
K_mf(bool) LoadCSVMap2d(TContainer1* c, char* FileName, char seperator = ',')
{
   Kway::TFile fd;
   fd.Open(FileName, fmReadSeq);
   if(!fd)
      return false;
   fd.Seek(0,fsBegin);

   typedef typename TRec::value_type  TValue;

   typename TContainer1::iterator iter_;
   typename TContainer1::updater  uptr_;
   typename TContainer1::key_type Key_;
   typename TContainer2::iterator iter2_;
   typename TContainer2::updater  uptr2_;
   typename TContainer2::key_type Key2_;

   char  lnChar;
   char  buf[4096];
   char* Pos;
   char* StartPos;
   TRec recs;
   TContainer2*                    c2;
   
   while (!fd.IsEOF()) {
      int Ln_ = fd.ReadLN(buf, sizeof(buf), lnChar, "\n");
      if(Ln_ == 0)
         continue;
      recs.clear();
      StartPos = buf;
      while((Pos = strchr(StartPos, seperator))){
      	*Pos = 0;
      	TValue v(StartPos);
      	recs.push_back(v);
      	StartPos = Pos+1;
      }
      TValue v(StartPos);
      recs.push_back(v);

      if (!(recs >> &Key_))
         continue;
      iter_ = c->fetch(Key_);
      uptr_ = c->begin_update(iter_);
      if (!(recs >> &Key2_))
         continue;
      c2 =  (TContainer2*)(uptr_->GetContainer(c2));
      iter2_ = c2->fetch(Key2_);
      uptr2_ = c2->begin_update(iter2_);
      recs >> uptr2_;
   }
   fd.Close();
   return true;
}
//---------------------------------------------------------------------------
template<class TContainer1, class TContainer2, class TRec>           // 二維資料表
K_mf(bool) UpdateCSVMap2d(TContainer1* c, char* FileName, Kway::dword& lastpos, char seperator = ',')
{
   Kway::TFile fd;
   fd.Open(FileName, fmReadSeq);
   if(!fd)
      return false;
   typedef typename TRec::value_type  TValue;
   typename TContainer1::iterator iter_;
   typename TContainer1::updater  uptr_;
   typename TContainer1::key_type Key_;
   typename TContainer2::iterator iter2_;
   typename TContainer2::updater  uptr2_;
   typename TContainer2::key_type Key2_;

   char                           lnChar;
   char                           buf[4096];
   char*                          Pos;
   char*                          StartPos;
   TRec                           recs;
   TContainer2*                   c2;
   if(lastpos > fd.GetFileSize())
      lastpos = 0;
   fd.Seek(lastpos, fsBegin);

   while (!fd.IsEOF()) {
      int Ln_ = fd.ReadLN(buf, sizeof(buf), lnChar, "\n");
      if(Ln_ == 0)
         continue;
      lastpos += Ln_;
      recs.clear();
      StartPos = buf;
      while((Pos = strchr(StartPos, seperator))){
      	*Pos = 0;
      	TValue v(StartPos);
      	recs.push_back(v);
      	StartPos = Pos+1;
      }
      TValue v(StartPos);
      recs.push_back(v);

      if (!(recs >> &Key_))
         continue;
      iter_ = c->fetch(Key_);
      uptr_ = c->begin_update(iter_);
      if (!(recs >> &Key2_))
         continue;
      c2 =  (TContainer2*)(uptr_->GetContainer(c2));
      iter2_ = c2->fetch(Key2_);
      uptr2_ = c2->begin_update(iter2_);
      recs >> uptr2_;
   }
   fd.Close();
   return true;
}
//---------------------------------------------------------------------------
template<class TContainer1, class TContainer2, class TContainer3, class TRec>    // CSV 三維資料表
K_mf(bool) LoadCSVMap3d(TContainer1* c, char* FileName, char seperator = ',')
{
   Kway::TFile fd;
   fd.Open(FileName, fmReadSeq);
   if(!fd)
      return false;
   fd.Seek(0,fsBegin);

   typedef typename TRec::value_type  TValue;

   typename TContainer1::iterator  iter_;
   typename TContainer1::updater   uptr_;
   typename TContainer1::key_type  Key_;
   typename TContainer2::iterator  iter2_;
   typename TContainer2::updater   uptr2_;
   typename TContainer2::key_type  Key2_;
   typename TContainer3::iterator  iter3_;
   typename TContainer3::updater   uptr3_;
   typename TContainer3::key_type  Key3_;

   char  lnChar;
   char  buf[4096];
   char* Pos;
   char* StartPos;
   TRec recs;
   TContainer2*                    c2;
   TContainer3*                    c3;
   
   while (!fd.IsEOF()) {
      int Ln_ = fd.ReadLN(buf, sizeof(buf), lnChar, "\n");
      if(Ln_ == 0)
         continue;
      recs.clear();
      StartPos = buf;
      while((Pos = strchr(StartPos, seperator))){
      	*Pos = 0;
      	TValue v(StartPos);
      	recs.push_back(v);
      	StartPos = Pos+1;
      }
      TValue v(StartPos);
      recs.push_back(v);

      if (!(recs >> &Key_))
         continue;
      iter_ = c->fetch(Key_);
      uptr_ = c->begin_update(iter_);
      if (!(recs >> &Key2_))
         continue;
      c2 =  (TContainer2*)(uptr_->GetContainer(c2));
      iter2_ = c2->fetch(Key2_);
      uptr2_ = c2->begin_update(iter2_);
      if (!(recs >> &Key3_))
         continue;
      c3 =  (TContainer3*)(uptr2_->GetContainer(c3));
      iter3_ = c3->fetch(Key3_);
      uptr3_ = c3->begin_update(iter3_);
      recs >> uptr3_;
   }
   fd.Close();
   return true;
}
//---------------------------------------------------------------------------
template<class TContainer1, class TContainer2, class TContainer3, class TRec>           // 三維資料表
K_mf(bool) UpdateCSVMap3d(TContainer1* c, char* FileName, Kway::dword& lastpos, char seperator = ',')
{
   Kway::TFile fd;
   fd.Open(FileName, fmReadSeq);
   if(!fd)
      return false;
   typedef typename TRec::value_type  TValue;
   typename TContainer1::iterator  iter_;
   typename TContainer1::updater   uptr_;
   typename TContainer1::key_type  Key_;
   typename TContainer2::iterator  iter2_;
   typename TContainer2::updater   uptr2_;
   typename TContainer2::key_type  Key2_;
   typename TContainer3::iterator  iter3_;
   typename TContainer3::updater   uptr3_;
   typename TContainer3::key_type  Key3_;

   char                            lnChar;
   char                            buf[4096];
   char*                           Pos;
   char*                           StartPos;
   TRec                            recs;
   TContainer2*                    c2;
   TContainer3*                    c3;
   if(lastpos > fd.GetFileSize())
      lastpos = 0;
   fd.Seek(lastpos, fsBegin);

   while (!fd.IsEOF()) {
      int Ln_ = fd.ReadLN(buf, sizeof(buf), lnChar, "\n");
      if(Ln_ == 0)
         continue;
      lastpos += Ln_;
      recs.clear();
      StartPos = buf;
      while((Pos = strchr(StartPos, seperator))){
      	*Pos = 0;
      	TValue v(StartPos);
      	recs.push_back(v);
      	StartPos = Pos+1;
      }
      TValue v(StartPos);
      recs.push_back(v);

      if (!(recs >> &Key_))
         continue;
      iter_ = c->fetch(Key_);
      uptr_ = c->begin_update(iter_);
      if (!(recs >> &Key2_))
         continue;
      c2 =  (TContainer2*)(uptr_->GetContainer(c2));
      iter2_ = c2->fetch(Key2_);
      uptr2_ = c2->begin_update(iter2_);
      if (!(recs >> &Key3_))
         continue;
      c3 =  (TContainer3*)(uptr2_->GetContainer(c3));
      iter3_ = c3->fetch(Key3_);
      uptr3_ = c3->begin_update(iter3_);
      recs >> uptr3_;
   }
   fd.Close();
   return true;
}
//---------------------------------------------------------------------------
};
//---------------------------------------------------------------------------
#endif
