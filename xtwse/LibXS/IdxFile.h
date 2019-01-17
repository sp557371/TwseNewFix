//---------------------------------------------------------------------------
#ifndef IdxFileH
#define IdxFileH
//---------------------------------------------------------------------------
#include "kpersistent.h"
#include "KFile.h"
#include <map>
#include <vector>
//---------------------------------------------------------------------------
namespace Kway {
//---------------------------------------------------------------------------
enum EIdxSaveFlag
{
   isf_Append,    ///< ���[�b�̫�.
   isf_ClearOther,///< �x�s�e���M��,�A�g�J�s���.
};
/////////////////////////////////////////////////////////////////////////////
/** ����-�����.
*/
LIBNS_class TIdxFile
{
public:
   typedef std::string     TIdxKey;
   struct TRecSaver {
      virtual K_mf(unsigned) GetStoreSize () const = 0;
      //�Y�z���ۤv�� Persistent, �h�i�H���z�|���B�� per, ���ݭn�A GetPersistent() �ɶǦ^�z�ۤv�� per!
      virtual K_mf(bool)     SaveTo       (TIdxFile&, Kstl::persistent&) const = 0;
      //�b SaveTo() ����I�s, �Y�Ǧ^�DNULL, �h�ϥζǦ^�� persistent �x�s!
      virtual K_mf(Kstl::persistent*) GetPersistent () const {
         return NULL;
      }
   };
   struct TRecLoader {
      virtual K_mf(bool) LoadFrom (TIdxFile&, const TIdxKey&, const Kstl::persistent&) = 0;
   };

private:
   unsigned                ReserveCount_;
   TFile                   IdxFile_;
   TFile                   DatFile_;
   Kstl::persistent_auto   per;
   bool                    SaveKeysBfClose_;

   typedef Kway::word      TSize;
   struct TBlkHead {
      TFile::size_type  RoomPos_;
      TSize             RoomSize_;     ///< �ж��j�p(���t���ޥΪ�Header).
      TSize             DatSize_;      ///< ��Ƥj�p.
      TFile::size_type  NextRoomPos_;  ///< �YDest���T���o�өж��񤣤U,�h�ݭn�U�@�өж��Ӧs��.
   };
   struct TIdxRec {
      TFile::size_type  IdxPos_;
      TBlkHead          BlkFirst_;
      TBlkHead          BlkLast_;
      TFile::size_type  ReadEndPos_;///< �|��Ū�X���d��: [BlkFirst_..ReadEndPos_]

      inline K_ctor TIdxRec ()   : IdxPos_(0), ReadEndPos_(0), WaitErase_(false) {}

      K_mf(void) UpdateReadEndPos ();

      K_mf(bool) IsWaitErase() const {
         return(WaitErase_ && BlkFirst_.RoomPos_==0 && BlkLast_.RoomPos_==0);
      }
      inline K_mf(void) SetWaitErase(bool value) {
         WaitErase_ = value;
      }
   private:
      bool  WaitErase_;
   };
   typedef std::map<TIdxKey,TIdxRec>   TIdxRecs;
   TIdxRecs IdxRecs_;

   TFile::size_type  FreeList_[1024];

   enum EFreeRoomFlag {
      frf_All         = 0,
      frf_ExcludeFrom = 0x01,
      frf_ExcludeTo   = 0x02,
   };
   K_mf(bool) FreeRoom  (TBlkHead&);
   K_mf(bool) FreeRooms (TBlkHead& from, TBlkHead& to, EFreeRoomFlag = frf_All);
   K_mf(bool) AllocRoom (TBlkHead&, TSize datSize);

   typedef std::vector<TBlkHead> TBlkList;
   K_mf(bool) ReadRoomList (TBlkHead& from, TBlkHead& to, EFreeRoomFlag, TBlkList&);

   static K_mf(unsigned) CalcFreeListIdx (TSize roomSize);

   K_mf(bool) WriteIdx_All       (TIdxRec&, const TIdxKey*);
   K_mf(bool) WriteIdx_FirstRoom (const TIdxRec&);
   K_mf(bool) WriteIdx_LastRoom  (const TIdxRec&);

   K_mf(bool) WriteRoom_All          (const TBlkHead&, const TRecSaver&);
   K_mf(bool) WriteRoom_HeadNextRoom (TBlkHead&, TFile::size_type nextRoomPos);
   K_mf(bool) WriteRoom_AppendMsg    (TBlkHead&, const TRecSaver&);
   K_mf(bool) ReadRoom_Head          (TFile::size_type roomPos, TBlkHead&);

   K_mf(bool) LoadKeys ();
   K_mf(bool) SaveKeys ();
   K_mf(bool) LoadBlk  (TIdxRec&);
   K_mf(bool) Load     (TIdxRecs::iterator, TRecLoader&, bool isLastBlk);
   K_mf(bool) ClearRoom(const TIdxKey&, TIdxRecs::iterator* ifindKey);
   K_mf(bool) ClearIdxKey(TIdxRec&);
public:
   static const TFileMode DefaultFileMode = TFileMode(fmCreatePath|fmRead|fmWrite|fmDenyWrite|fmCheckFileNameCase);
   /** �غc. ������ = fname + ".idx"
             ����� = fname + ".dat"
       reserveCount = �O�d��Ƶ���. �O�d��Ƥj�p = (�Ĥ@����Ƥj�p * reserveCount);
   */
   K_ctor TIdxFile (const std::string& fname, size_t reserveCount, TFileMode fileMode = DefaultFileMode);

   /** �Ѻc.
   */
   virtual K_dtor ~TIdxFile ();

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

   /** ���ʼg�J����: �q�{�b��m���e���� n bytes.
       cutTail==false: �Y SeekBack() ����,�S������g�J�ʧ@,�h�U�����J��,������SeekBack()���e�����!
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
   virtual K_mf(bool) IsKeyExists (const TIdxKey&) const;

   /** �ɮ׶}�ҬO�_���\?
   */
   virtual K_mf(bool) IsOpened()  const   { return IdxFile_.IsOpened()  &&  DatFile_.IsOpened(); }
   inline  K_mf(bool) operator!() const   { return !IsOpened(); }
   /** �����ɪ����~�T��.
   */
   virtual K_mf(std::string) GetLastErr() const {
      std::string errMsg = IdxFile_.GetLastErr();
      if(!errMsg.empty())
         return errMsg;
      return DatFile_.GetLastErr();
   }
   inline K_mf(std::string) GetFileName() const {
      return DatFile_.GetFileName();
   }
   inline K_mf(TFileMode) GetFileMode() const {
      return IdxFile_.GetFileMode();
   }
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Kway
//---------------------------------------------------------------------------
#endif

