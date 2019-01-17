//
// ���F��ְO������t���t��, �i�ϥ� TMemPool ���O����޲z
//
// ����: $Id: MemPool.h,v 1.4 2006/04/04 03:00:03 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef MemPoolH
#define MemPoolH
//---------------------------------------------------------------------------
#ifdef _MSC_VER
#pragma warning ( disable : 4200 )
#endif
//---------------------------------------------------------------------------
#include "KwayBase.h"
#include <cc++/thread.h>
//---------------------------------------------------------------------------
namespace Kway {
/////////////////////////////////////////////////////////////////////////////
LIBNS_class TMemPool
{  //���F��ְO������t���t��
   //�ϥ� TMemPool �w�����t�O����
protected:
   static const size_t   BlkSize_  = 256;       //�����O������t�̤p���
   static const size_t   MaxSize_  = 64 * 1024; //�W�L���j�p,�������t,����MemPool�޲z
   static const size_t   BlkCount_ = MaxSize_ / BlkSize_;

   struct SMemList {
      union {
         SMemList*   Next_;
         dword       Idx_; // Index in MemList_[]
      };
      char           Mem_[];
   };
   SMemList*   MemList_[BlkCount_];
   ost::Mutex  Critical_;

public:
   K_ctor  TMemPool ();
   K_dtor ~TMemPool ();

   K_mf(void*)   Alloc    (size_t);
   K_mf(void)    Free     (void*);

   static inline K_mf(size_t)  MemSize  (void* mem)  { return ( ((SMemList*)(((byte*)mem)-sizeof(SMemList)))
                                                              ->Idx_ + 1 ) * BlkSize_ - sizeof(SMemList); }
};
//---------------------------------------------------------------------------
LIBNS_class TMemQueue
{
   struct SMemList {
      SMemList*   Next_;
      size_t      UsedSize_;
      char        Mem_[];
   };
   SMemList*   First_;
   SMemList*   Last_;
   size_t      FirstUsed_;
   size_t      MaxMergeSize_;
   size_t      SizeCatch_;

   virtual K_mf(void)  OnMemQueueEmpty  ()  {}
   virtual K_mf(void)  OnMemQueueDataIn ()  {}

   static K_mf(TMemPool&)  MemPool ();
   static K_mf(SMemList*)  Alloc   (const void* mem, size_t memSize, size_t maxSize);
protected:
   ost::Mutex  Critical_;
public:
           K_ctor  TMemQueue  (size_t maxMergeSize);
   #ifdef __BORLANDC__
           K_dtor ~TMemQueue  ();
   #else//���קKwarning��: ��virtual functions, �S��virtual dtor!
   virtual K_dtor ~TMemQueue  ();
   #endif

   K_mf(void)    PutMemBlock  (const void*, size_t);
   K_mf(size_t)  GetMemBlock  (const void*&);
   K_mf(void)    Clear        ();

   enum TSetMemUsedResult {
      smur_NoMore,
      smur_Continue, //�������
   };
   K_mf(TSetMemUsedResult)  SetMemUsed   (size_t);

   inline K_mf(bool)    IsEmpty  () const  { return First_==0;  }
   inline K_mf(size_t)  Size     () const  { return SizeCatch_; }
   inline K_mf(size_t)  GetMaxMergeSize () const { return MaxMergeSize_; }
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Kway
//---------------------------------------------------------------------------
#endif

