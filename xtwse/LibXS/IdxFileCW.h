//---------------------------------------------------------------------------
#ifndef IdxFileCWH
#define IdxFileCWH
//---------------------------------------------------------------------------
#include "IdxFile.h"
#include "cc++/thread.h"
//---------------------------------------------------------------------------
namespace Kway {
/////////////////////////////////////////////////////////////////////////////
/** ����-�����.
    Catch Write.
    �ϥ�Thread�g�J���! ������ TIdxFileCW �@��n��Thread.

*/
LIBNS_class TIdxFileCW : public TIdxFile
{
   typedef TIdxFile  inherited;
private:
   unsigned             ReserveCount_;
   ost::Mutex mutable   Mutex_;

   class LockerCW {
      ost::Mutex* Mutex_;
   public:
      inline K_ctor LockerCW(const TIdxFileCW& v)
         : Mutex_(&v.Mutex_)
         { Mutex_->enterMutex();
         }
      inline K_dtor ~LockerCW() {
         if(Mutex_)
            Mutex_->leaveMutex();
      }
   };
   friend class LockerCW;

   class ThreadCW;
   friend class ThreadCW;

   struct PendingRec : public TRecSaver {
      EIdxSaveFlag            SaveFlag_;
      Kstl::persistent_auto   mutable per;
      unsigned                MaxSize_;
      inline K_mf(void) Clear (EIdxSaveFlag flag) {
         per.rewind();
         SaveFlag_ = flag;
      }
      inline K_ctor PendingRec ()
         : SaveFlag_(isf_Append)
         , per(1024)
         { }
      inline K_ctor PendingRec (const PendingRec& r)
         : SaveFlag_(isf_Append)
         , per(1024)
         { }
   private:
      inline void   operator=  (const PendingRec&);//�T��ƻs!

      virtual K_mf(unsigned) GetStoreSize () const {
         return per.get_saved_size();
      }
      //�Y�z���ۤv�� Persistent, �h�i�H���z�|���B�� per, ���ݭn�A GetPersistent() �ɶǦ^�z�ۤv�� per!
      virtual K_mf(bool) SaveTo (TIdxFile&, Kstl::persistent&) const {
         return true;
      }
      //�b SaveTo() ����I�s, �Y�Ǧ^�DNULL, �h�ϥζǦ^�� persistent �x�s!
      virtual K_mf(Kstl::persistent*) GetPersistent () const {
         return &per;
      }
   };
   typedef std::map<TIdxKey,PendingRec>   TPendings;
   TPendings            Pendings_;
   TPendings::iterator  ThrIPending_;

   inline K_mf(bool) FindPending (const TIdxKey& key, TPendings::iterator& i) {
      i = Pendings_.find(key);
      return i != Pendings_.end();
   }
   inline K_mf(bool) FlushPending (const TIdxKey& key) {
      TPendings::iterator i;
      if(FindPending(key, i))
         return FlushPending(*i);
      return true;
   }
   inline K_mf(bool) FlushPending (TPendings::value_type& v) {
      return FlushPending(v.first, v.second);
   }
   K_mf(bool) FlushPending (const TIdxKey&, PendingRec&);
   K_mf(void) ThrFlush     (ThreadCW* thr);

public:
   /** �غc. ������ = fname + ".idx"
             ����� = fname + ".dat"
       reserveCount = �O�d��Ƶ���. �O�d��Ƥj�p = (�Ĥ@����Ƥj�p * reserveCount);
   */
   inline K_ctor TIdxFileCW (const std::string& fname, size_t reserveCount, TFileMode fileMode = DefaultFileMode)
      : inherited(fname, 1, fileMode)
      , ReserveCount_(reserveCount <= 0 ? 1 : (unsigned)reserveCount)
      { }
   /** �Ѻc.
   */
   virtual K_dtor ~TIdxFileCW () {
      Close();
   }
   /** �N���Լg�J�����,�����g�J��~��^!
   */
   virtual K_mf(bool) Flush ();

   /** ���������.
   */
   virtual K_mf(void) Close ();

   /** �N��Ƽg�J�ɮ�.
   */
   virtual K_mf(bool) Save (const TIdxKey&, const TRecSaver&, EIdxSaveFlag);

   /** ���J. �Y key==0 �������J.
                key!=0 ���J���wkey�����.
       isLastBlk = �ȸ��J�̫�@��Blk.
   */
   virtual K_mf(bool) Load (const TIdxKey*, TRecLoader&, bool isLastBlk);

   /** ����, ������i�H�ɦA���J.
       �Y�䤣����w��key,�h�Ǧ^false.
   */
   virtual K_mf(bool) Unload (const TIdxKey&);

   /** ���ʼg�J����: �q�{�b��m���e���� n bytes.(�ȯ�b�̫�@��blk����).
   */
   virtual K_mf(bool) SeekBack (const TIdxKey&, unsigned, bool cutTail);

   /** �R���YKey���������.
   */
   virtual K_mf(bool) DelKey (const TIdxKey&);

   /** ����Key��, �Y oldKey ���s�b, �� newKey �w�s�b, �h�Ǧ^ false
   */
   virtual K_mf(bool) ReKey (const TIdxKey& oldKey, const TIdxKey& newKey);

   /** �P�_�YKey�O�_�s�b.
   */
   virtual K_mf(bool) IsKeyExists (const TIdxKey& key) const {
      LockerCW locker(*this);
      return inherited::IsKeyExists(key);
   }
   /** �����ɪ����~�T��.
   */
   virtual K_mf(std::string) GetLastErr() const {
      LockerCW locker(*this);
      return inherited::GetLastErr();
   }

   static K_mf(void) InitThread();
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Kway
//---------------------------------------------------------------------------
#endif

