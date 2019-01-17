/////////////////////////////////////////////////////////////////////////////
#if !defined(_QFixFileStore_h_)
#define _QFixFileStore_h_
//---------------------------------------------------------------------------
#include "KFile.h"
#include "TimerThread.h"
#include "../src/Code/MessageStore.h"
#include "QFixType.h"

/////////////////////////////////////////////////////////////////////////////
namespace Kway {
namespace FIX  {

/////////////////////////////////////////////////////////////////////////////
// class TFixSeqNumsStore
/////////////////////////////////////////////////////////////////////////////
K_class TFixSeqNumsStore : public TFixSnsBase
{
   typedef TFixSnsBase  inherited;

private:
   class TStoreFile;
   typedef std::auto_ptr<TStoreFile>  TStoreFilePtr;

   TStoreFilePtr  File_;
   std::string    FileName_;

public:
   K_ctor  TFixSeqNumsStore (TFixSsuBase*);
   K_dtor ~TFixSeqNumsStore ();

   //==============================================
   // overwrite TFixSnsBase virtual function
   K_mf(EFixWFileSts) SetNextSenderMsgSeqNum (int value);
   K_mf(EFixWFileSts) SetNextTargetMsgSeqNum (int value);

   //==============================================
   // TFixSeqNumsStore function
   K_mf(void) InitFromFile (const std::string& fileName);
   K_mf(void) Reset        ();
   K_mf(bool) OpenFile     (const std::string&);
   K_mf(void) CloseFile    ();

   K_mf(EFixWFileSts) IncNextSenderMsgSeqNum ();
   K_mf(EFixWFileSts) IncNextTargetMsgSeqNum ();

protected:
   K_mf(EFixWFileSts) SetSeqNums (int sender, int target);
};
//---------------------------------------------------------------------------
typedef Kstl::wise_ref<TFixSeqNumsStore>  TFixSeqNumsStorePtr;


/////////////////////////////////////////////////////////////////////////////
// class TFixFileStore
/////////////////////////////////////////////////////////////////////////////
K_class TFixFileStore : public MessageStore
{
public:
    TFixFileStore (TFixSesInfo*);
   ~TFixFileStore ();

   //==============================================
   // overwrite MessageStore virtual function
   bool set (int, const std::string&)
      throw (IOException);
   
   void get (int, int, std::vector<std::string>&) const
      throw ( IOException );

   int  getNextSenderMsgSeqNum  ()    const throw (IOException);
   int  getNextTargetMsgSeqNum  ()    const throw (IOException);
   void setNextSenderMsgSeqNum  (int)       throw (IOException);
   void setNextTargetMsgSeqNum  (int)       throw (IOException);
   void incrNextSenderMsgSeqNum ()          throw (IOException);
   void incrNextTargetMsgSeqNum ()          throw (IOException);

   UtcTimeStamp getCreationTime () const throw (IOException);

   void reset   () throw (IOException);
   void refresh () throw (IOException);
   
private:
   typedef std::pair < int, int > OffsetSize;
   typedef std::map < int, OffsetSize > NumToOffset;

   void open( bool deleteFile );
   void populateCache();
   bool readFromFile( int offset, int size, std::string& msg );
   //void setSeqNum();
   void setSession();

   bool get( int, std::string& ) const throw ( IOException );

   TFixSesInfo*  FixSesInfo_;
   
   MemoryStore m_cache;
   NumToOffset m_offsets;

   std::string  m_msgFileName;
   std::string  m_headerFileName;
   std::string  m_seqNumsFileName;
   std::string  m_sessionFileName;
//   std::string  SessionID_;
//   Kway::TKDate TradeDate_;

   FILE* m_msgFile;
   FILE* m_headerFile;
   FILE* m_seqNumsFile;
   FILE* m_sessionFile;

   TFixSeqNumsStorePtr  SeqNumsStore_;
};


/////////////////////////////////////////////////////////////////////////////
}; // namespace FIX
}; // namespace Kway

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_QFixFileStore_h_)

