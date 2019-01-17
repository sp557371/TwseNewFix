//---------------------------------------------------------------------------
#ifndef PkFTH
#define PkFTH
//---------------------------------------------------------------------------
#include "SesPk.h"
#include "KFile.h"
//---------------------------------------------------------------------------
// 檔案傳輸協定 (PkFT)
//---------------------------------------------------------------------------
namespace Kway {
namespace Pk {
/////////////////////////////////////////////////////////////////////////////
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(push,1)
#else
#pragma pack(1)
#endif
//---------------------------------------------------------------------------
static const TMsgCode   mcSesFT        = 0x020;
static const TMsgCode   mcSesFT_Cnt    = 0x021;
static const TMsgCode   mcSesFT_ID0    = 0xe00;
static const size_t     mcSesFT_IDLast = 0xfff;
static const size_t     MaxFT_Num      = (mcSesFT_IDLast - mcSesFT_ID0) + 1;
//---------------------------------------------------------------------------
typedef dword           TFileSizeFT;
//---------------------------------------------------------------------------
#ifdef K_EnumPacked_Bug
   typedef byte   TSesFTFunc;
   enum TSesFTFunc_t
#else
   enum TSesFTFunc
#endif
{
      ftf_ReqGetStatic, //取得一個[靜態]檔 (檔案傳輸時不考慮檔案異動的問題)
      ftf_ReqGetAppend, //取得一個[大小仍持續增加]的檔案 (如:log檔)
      ftf_ReqPutStatic,
      ftf_ReqPutAppend,

      ftf_ReqCancel,
} K_EnumPacked;
//---------------------------------------------------------------------------
#ifdef K_EnumPacked_Bug
   typedef byte   TSesFTAckCode;
   enum TSesFTAckCode_t
#else
   enum TSesFTAckCode
#endif
{
      ftac_ResetFile,
      ftac_Resume,
      ftac_Finish,
} K_EnumPacked;
/////////////////////////////////////////////////////////////////////////////
class TSesFTReq : public THead
{
   typedef TSesFTReq   this_type;
   typedef TPkRec      TFiller;

   TSesFTFunc     FnCode_;
   word           ReqID_;
   TKDateTime     ModTime_;
   TFileSizeFT    FileSize_;
   TSize          FileNameLen_;
   union {
      char        Dat_[];
      TFiller     Filler_;
   };
public:
   //----- for client -----
   // client要求檔案時,必須將client端的檔案狀態傳給server
   // 由server決定client是否可以續傳
   inline K_ctor  TSesFTReq  (TSesFTFunc fn, word reqID,
                              TKDateTime modTime, TFileSizeFT fsize,
                              const std::string& remoteFileName,
                              THiLoTransRef ref)
            : FnCode_(fn)
            , ReqID_(reqID)
            , ModTime_(HiLoTransT(modTime.GetDate(),ref), modTime.GetTime())
            , FileSize_(HiLoTransT(fsize,ref))
            , FileNameLen_(static_cast<TSize>(remoteFileName.length()))
            { memcpy(Dat_, remoteFileName.data(), FileNameLen_);
              SetMsgCode(mcSesFT);
              SetPkSize(GetExpectSize());
              FileNameLen_ = HiLoTransT(FileNameLen_,ref); }

   //----- for server -----
   static inline K_mf(TSize) GetMinSize    ()        { return sizeof(this_type) - sizeof(TFiller);  }
   inline K_mf(TSize)        GetExpectSize () const  { return static_cast<TSize>(sizeof(*this) - sizeof(Filler_) + FileNameLen_); }
   inline K_mf(TSesFTFunc)   GetFuncCode   () const  { return FnCode_;   }
   inline K_mf(word)         GetReqID      () const  { return ReqID_;    }
   inline K_mf(TKDateTime)   GetModTime    () const  { return ModTime_;  }
   inline K_mf(TFileSizeFT)  GetFileSize   () const  { return FileSize_; }
   inline K_mf(std::string)  GetFileName   () const  { return std::string(Dat_, Dat_+FileNameLen_); }
};
//---------------------------------------------------------------------------
class TSesFTReqCnt : public THead
{
   typedef TSesFTReqCnt    this_type;
   TSesFTFunc     FnCode_;
   word           FTID_;
public:
   //----- for client -----
   // cancel request
   inline K_ctor  TSesFTReqCnt  (word ftID, THiLoTransRef ref)
            : FnCode_(ftf_ReqCancel)
            , FTID_(HiLoTransT(ftID,ref))
            { SetMsgCode(mcSesFT_Cnt);
              SetPkSize(GetExpectSize()); }

   //----- for server -----
   static inline K_mf(TSize) GetMinSize    ()        { return sizeof(this_type); }
   inline K_mf(TSize)        GetExpectSize () const  { return sizeof(this_type); }
   inline K_mf(TSesFTFunc)   GetFuncCode   () const  { return FnCode_;           }
   inline K_mf(word)         GetFTID       () const  { return FTID_;             }
};
//---------------------------------------------------------------------------
class TSesFTAck : public THead
{
   typedef TSesFTAck    this_type;
   typedef TPkRec       TFiller;

   word     ReqID_;
   word     FTID_;
   word     DatLen_;
   union {
      char     Dat_[];
      struct {
         TSesFTAckCode  AckCode_;
         TFileSizeFT    FileSize_;
         char           FileName_[];
      }        Result_;
      TFiller  Filler_;
   };
public:
   static const word    InvalidFTID = static_cast<word>(-1);
   //----- for server -----
   inline K_ctor  TSesFTAck  (word reqID, word ftID, TSesFTAckCode ac, TFileSizeFT fsize,
                              const std::string& fileName)
            : ReqID_(reqID)
            , FTID_(ftID)
            , DatLen_(static_cast<word>(sizeof(Result_)+fileName.length()+1))
            { Result_.AckCode_  = ac;
              Result_.FileSize_ = fsize;
              strcpy(Result_.FileName_, fileName.c_str());
              SetMsgCode(mcSesFT);
              SetPkSize(static_cast<TSize>(sizeof(*this) - sizeof(TFiller) + DatLen_)); }
   inline K_ctor  TSesFTAck  (word reqID, const std::string& errmsg)
            : ReqID_(reqID)
            , FTID_(InvalidFTID)
            , DatLen_(static_cast<word>(errmsg.length()))
            { memcpy(Dat_, errmsg.data(), DatLen_);
              SetMsgCode(mcSesFT);
              SetPkSize(static_cast<TSize>(sizeof(*this) - sizeof(TFiller) + DatLen_)); }
   //----- for client -----
   inline K_mf(word)          GetReqID   ()                  const  { return ReqID_; }
   inline K_mf(word)          GetFTID    (THiLoTransRef ref) const  { return HiLoTransT(FTID_,ref); }
   inline K_mf(TSesFTAckCode) GetAckCode ()                  const  { return Result_.AckCode_;      }
   inline K_mf(TFileSizeFT)   GetFileSize(THiLoTransRef ref) const  { return HiLoTransT(Result_.FileSize_,ref); }
   inline K_mf(std::string)   GetErrMsg  (THiLoTransRef ref) const  { return std::string(Dat_, Dat_+HiLoTransT(DatLen_,ref)); }
   inline K_mf(const char*)   GetFileName() const                   { return Result_.FileName_; }

   static inline K_mf(TSize) GetMinSize    ()                         { return sizeof(this_type) - sizeof(TFiller); }
   inline K_mf(TSize)        GetExpectSize (THiLoTransRef ref) const  { return static_cast<TSize>(sizeof(*this) - sizeof(Filler_) + HiLoTransT(DatLen_,ref)); }
};
/////////////////////////////////////////////////////////////////////////////
class TSesFT_FileBlock : public THead
{
   typedef TSesFT_FileBlock   this_type;
   typedef TPkRec             TFiller;

   TFileSizeFT FilePos_;  //也許可以考慮省略此欄位,接收端直接 append 即可?
   word        DatSize_;
   union {
      char     Dat_[];
      TFiller  Filler_;
   };

   K_mf(word)  ReadFile  (TFile& file, TFileSizeFT pos)
            { if(file.Seek(pos,fsBegin) != pos)
                 return 0;
              TFile::size_type   sz = file.Read(Dat_, 1024);//每個block n bytes
              return static_cast<word>(file.IsResultOK(sz) ? sz : 0); }
public:
   //----- for server(send) / client(recv) -----
   inline K_ctor  TSesFT_FileBlock  (word ftID, TFile& file, TFileSizeFT pos)
            : FilePos_(pos)
            , DatSize_(ReadFile(file,pos))
            { SetMsgCode(static_cast<TMsgCode>(mcSesFT_ID0 + ftID));
              SetPkSize(GetExpectSize()); }
   //----- for client(send) / server(recv) -----
   inline K_ctor  TSesFT_FileBlock  (word ftID, TFile& file, TFileSizeFT pos, THiLoTransRef ref)
            : FilePos_(HiLoTransT(pos,ref))
            , DatSize_(ReadFile(file,pos))
            { SetMsgCode(static_cast<TMsgCode>(mcSesFT_ID0 + ftID));
              SetPkSize(GetExpectSize()); 
              DatSize_ = HiLoTransT(DatSize_,ref); }

   //----- for server -----
   static inline K_mf(TSize) GetMinSize    ()        { return sizeof(this_type) - sizeof(TFiller);  }
   inline K_mf(TSize)        GetExpectSize () const  { return static_cast<TSize>(sizeof(*this) - sizeof(Filler_) + DatSize_); }
   inline K_mf(TFileSizeFT)  GetFilePos    () const  { return FilePos_; }
   inline K_mf(word)         GetDatSize    () const  { return DatSize_; }
   inline K_mf(const void*)  GetFileDat    () const  { return Dat_;     }

   inline K_mf(TSize)        GetExpectSize (THiLoTransRef ref) const  { return static_cast<TSize>(sizeof(*this) - sizeof(Filler_) + HiLoTransT(DatSize_,ref)); }
   inline K_mf(TFileSizeFT)  GetFilePos    (THiLoTransRef ref) const  { return HiLoTransT(FilePos_,ref); }
   inline K_mf(word)         GetDatSize    (THiLoTransRef ref) const  { return HiLoTransT(DatSize_,ref); }
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(pop)
#else
#pragma pack()
#endif
/////////////////////////////////////////////////////////////////////////////
} // namespace Pk
} // namespace Kway
//---------------------------------------------------------------------------
#endif

