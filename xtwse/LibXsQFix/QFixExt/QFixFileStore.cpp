/////////////////////////////////////////////////////////////////////////////
#if defined(__BORLANDC__)
#pragma hdrstop
#pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "QFixFileStore.h"
#include "Nsa_TextSimpReco.h"
#include "../src/Code/UtilityQ.h"
#include "Nse2_CommType.h"
#include "QFixStatusViewer.h"
//---------------------------------------------------------------------------
using namespace Kway;
using namespace Kway::Nsa;
using namespace Kway::FIX;

/////////////////////////////////////////////////////////////////////////////
// class TFixSeqNumsStore::TStoreFile
/////////////////////////////////////////////////////////////////////////////
class TFixSeqNumsStore::TStoreFile
{
   Kway::TFile  File_;

   static const size_t  OneRecLen_;
   static const char*   NewLineSplit_;

public:
   K_mf(bool)         GetSeqNums (const std::string&, int&, int&);
   K_mf(bool)         Open       (const std::string&);
   K_mf(EFixWFileSts) SetSeqNums (int, int);
};
/////////////////////////////////////////////////////////////////////////////
// class TFixSeqNumsStore::TStoreFile impl
/////////////////////////////////////////////////////////////////////////////
// [%10d : %10d] + [0x0d 0x0a]
const size_t  TFixSeqNumsStore::TStoreFile::OneRecLen_    = 25;
const char*   TFixSeqNumsStore::TStoreFile::NewLineSplit_ = { "\r\n" };
//---------------------------------------------------------------------------

K_mf(bool) TFixSeqNumsStore::TStoreFile::GetSeqNums (const std::string& strFileName, int& sender, int& target)
{
   TTextSimpReco_Reader reader(strFileName.c_str(), NewLineSplit_);
   std::string          strVal = reader.GetLastRec(OneRecLen_);

   if (strVal.empty())  return false;

   std::string::size_type pos = strVal.find(':');
   if (pos == std::string::npos)
      return false;

   sender = atoi(strVal.c_str());
   target = atoi(strVal.begin() + pos + 1);

   return true;
}
//---------------------------------------------------------------------------

K_mf(bool) TFixSeqNumsStore::TStoreFile::Open (const std::string& strFileName)
{
   Kway::TFileMode fileMode = (Kway::TFileMode)(Kway::fmAppend|Kway::fmCreatePath);

   return File_.Open(strFileName.c_str(), fileMode);
}
//---------------------------------------------------------------------------

K_mf(EFixWFileSts) TFixSeqNumsStore::TStoreFile::SetSeqNums (int sender, int target)
{
   char                   caBuf[50];
   Kway::TFile::size_type resu;

   //sprintf(caBuf, "%10d : %10d%s", sender, target, NewLineSplit_);
   sprintf(caBuf, "%d : %d%s", sender, target, NewLineSplit_);
   resu = File_.Write(caBuf);
   if (resu == KFILE_INVALID_SIZE)
      return wfs_WriteError;

   if (File_.Flush() == false)
      return wfs_FlushError;

   return wfs_Success;
}


/////////////////////////////////////////////////////////////////////////////
// class TFixSeqNumsStore impl
/////////////////////////////////////////////////////////////////////////////
K_ctor TFixSeqNumsStore::TFixSeqNumsStore (TFixSsuBase* ssuBase)
   : inherited(ssuBase)
{
}
//---------------------------------------------------------------------------

K_dtor TFixSeqNumsStore::~TFixSeqNumsStore ()
{
}
//---------------------------------------------------------------------------

K_mf(void) TFixSeqNumsStore::InitFromFile (const std::string& fileName)
{
   TStoreFile file;
   int        sender = inherited::NextSenderMsgSeqNum_;
   int        target = inherited::NextTargetMsgSeqNum_;

   if (file.GetSeqNums(fileName, sender, target)) {
      inherited::SetNextSenderMsgSeqNum(sender);
      inherited::SetNextTargetMsgSeqNum(target);
   }
   else {
      inherited::Clear();
   }
}
//---------------------------------------------------------------------------

K_mf(void) TFixSeqNumsStore::Reset ()
{
   File_.reset();

   inherited::Clear();
}
//---------------------------------------------------------------------------

K_mf(bool) TFixSeqNumsStore::OpenFile (const std::string& strFileName)
{
   TStoreFile* file = File_.get();
   if (file == NULL) {
      file = new TStoreFile;
      File_.reset(file);
   }

   FileName_ = strFileName;

   return file->Open(strFileName);
}
//---------------------------------------------------------------------------

K_mf(void) TFixSeqNumsStore::CloseFile ()
{
   File_.reset();
}
//---------------------------------------------------------------------------

K_mf(EFixWFileSts) TFixSeqNumsStore::SetSeqNums (int sender, int target)
{
   TStoreFile* file    = File_.get();
   bool        autoNew = false;

   if (file == NULL) {
      autoNew = true;
      file    = new TStoreFile;

      file->Open(FileName_);
   }

   EFixWFileSts resu = file->SetSeqNums(NextSenderMsgSeqNum_, NextTargetMsgSeqNum_);

   if (autoNew)
      delete file;

   return resu;
}
//---------------------------------------------------------------------------

K_mf(EFixWFileSts) TFixSeqNumsStore::SetNextSenderMsgSeqNum (int value)
{
   inherited::SetNextSenderMsgSeqNum(value);

   return SetSeqNums(NextSenderMsgSeqNum_, NextTargetMsgSeqNum_);
}
//---------------------------------------------------------------------------

K_mf(EFixWFileSts) TFixSeqNumsStore::SetNextTargetMsgSeqNum (int value)
{
   inherited::SetNextTargetMsgSeqNum(value);

   return SetSeqNums(NextSenderMsgSeqNum_, NextTargetMsgSeqNum_);
}
//---------------------------------------------------------------------------

K_mf(EFixWFileSts) TFixSeqNumsStore::IncNextSenderMsgSeqNum ()
{
   inherited::SetNextSenderMsgSeqNum(NextSenderMsgSeqNum_ + 1);

   return SetSeqNums(NextSenderMsgSeqNum_, NextTargetMsgSeqNum_);
}
//---------------------------------------------------------------------------

K_mf(EFixWFileSts) TFixSeqNumsStore::IncNextTargetMsgSeqNum ()
{
   inherited::SetNextTargetMsgSeqNum(NextTargetMsgSeqNum_ + 1);

   return SetSeqNums(NextSenderMsgSeqNum_, NextTargetMsgSeqNum_);
}


/////////////////////////////////////////////////////////////////////////////
// class TFixFileStore impl
/////////////////////////////////////////////////////////////////////////////
TFixFileStore::TFixFileStore (TFixSesInfo* fsi)
   : m_msgFile    (0)
   , m_headerFile (0)
   , m_sessionFile(0)
   , FixSesInfo_  (fsi)
{
   std::string        strSessionID = fsi->GetFixSessionID().toFormat();
   const std::string& path         = fsi->GetFixLogPath  ();
   std::string        prefix       = file_appendpath(path, strSessionID + ".");

  m_msgFileName     = prefix + "body";
  m_headerFileName  = prefix + "header";
  m_seqNumsFileName = prefix + "seqnums";
  m_sessionFileName = prefix + "session";

  try
  {
    open( false );
  }
  catch ( IOException & e )
  {
    throw ConfigError( e.what() );
  }
}
//---------------------------------------------------------------------------

TFixFileStore::~TFixFileStore()
{
  fclose( m_msgFile );
  fclose( m_headerFile );
  fclose( m_sessionFile );

  SeqNumsStore_->CloseFile();
}
//---------------------------------------------------------------------------

void TFixFileStore::open( bool deleteFile )
{
  if ( m_msgFile )     fclose( m_msgFile );
  if ( m_headerFile )  fclose( m_headerFile );
  if ( m_sessionFile ) fclose( m_sessionFile );

  if ( deleteFile )
  {
    file_unlink( m_msgFileName.c_str() );
    file_unlink( m_headerFileName.c_str() );
    file_unlink( m_seqNumsFileName.c_str() );
    file_unlink( m_sessionFileName.c_str() );
  }

   //===============================================
   // 開啟 TFixSeqNumsStore
   if (SeqNumsStore_.get() == NULL) {
      // 向 FixSesInfo 查詢是否有建立過了
      TFixSnsBasePtr    snsPtr  = FixSesInfo_->GetFixSnsPtr();
      TFixSnsBase*      snsBase = snsPtr.get();
      TFixSeqNumsStore* store;

      if (snsBase == NULL) {
         TFixSsuBase* ssuBase = FixSesInfo_->GetFixSsuPtr().get();

         store = new TFixSeqNumsStore(ssuBase);

         FixSesInfo_->SetFixSnsPtr(TFixSnsBasePtr(store));
      }
      else {
         store = dynamic_cast<TFixSeqNumsStore*>(snsBase);
      }

      SeqNumsStore_.reset(store);
   }

  populateCache();
  m_msgFile = file_fopen( m_msgFileName.c_str(), "r+" );
  if ( !m_msgFile ) m_msgFile = file_fopen( m_msgFileName.c_str(), "w+" );
  if ( !m_msgFile ) throw ConfigError( "Could not open body file: " + m_msgFileName );

  m_headerFile = file_fopen( m_headerFileName.c_str(), "r+" );
  if ( !m_headerFile ) m_headerFile = file_fopen( m_headerFileName.c_str(), "w+" );
  if ( !m_headerFile ) throw ConfigError( "Could not open header file: " + m_headerFileName );

   //m_seqNumsFile = file_fopen( m_seqNumsFileName.c_str(), "r+" );
   //if ( !m_seqNumsFile ) m_seqNumsFile = file_fopen( m_seqNumsFileName.c_str(), "w+" );
   //if ( !m_seqNumsFile ) throw ConfigError( "Could not open seqnums file: " + m_seqNumsFileName );
   if (SeqNumsStore_->OpenFile(m_seqNumsFileName) == false)
      throw ConfigError( "Could not open seqnums file: " + m_seqNumsFileName );

  bool setCreationTime = false;
  m_sessionFile = file_fopen( m_sessionFileName.c_str(), "r" );
  if ( !m_sessionFile ) setCreationTime = true;
  else fclose( m_sessionFile );

  m_sessionFile = file_fopen( m_sessionFileName.c_str(), "r+" );
  if ( !m_sessionFile ) m_sessionFile = file_fopen( m_sessionFileName.c_str(), "w+" );
  if ( !m_sessionFile ) throw ConfigError( "Could not open session file" );
  if ( setCreationTime ) setSession();

  setNextSenderMsgSeqNum( getNextSenderMsgSeqNum() );
  setNextTargetMsgSeqNum( getNextTargetMsgSeqNum() );
}
//---------------------------------------------------------------------------

void TFixFileStore::populateCache()
{
  std::string msg;
  Message message;

  FILE* headerFile;
  headerFile = file_fopen( m_headerFileName.c_str(), "r+" );
  if ( headerFile )
  {
    int num, offset, size;
    while ( FILE_FSCANF( headerFile, "%d,%d,%d ", &num, &offset, &size ) == 3 )
      m_offsets[ num ] = std::make_pair( offset, size );
    fclose( headerFile );
  }

   //FILE* seqNumsFile;
   //seqNumsFile = file_fopen( m_seqNumsFileName.c_str(), "r+" );
   //if ( seqNumsFile )
   //{
   //   int sender, target;
   //   if ( FILE_FSCANF( seqNumsFile, "%d : %d", &sender, &target ) == 2 )
   //   {
   //      m_cache.setNextSenderMsgSeqNum( sender );
   //      m_cache.setNextTargetMsgSeqNum( target );
   //   }
   //   fclose( seqNumsFile );
   //}
   SeqNumsStore_->InitFromFile(m_seqNumsFileName);

  FILE* sessionFile;
  sessionFile = file_fopen( m_sessionFileName.c_str(), "r+" );
  if ( sessionFile )
  {
    char time[ 22 ];
#ifdef HAVE_FSCANF_S
    int result = FILE_FSCANF( sessionFile, "%s", time, 22 );
#else
    int result = FILE_FSCANF( sessionFile, "%s", time );
#endif
    if( result == 1 )
    {
      m_cache.setCreationTime( UtcTimeStampConvertor::convert( time, true ) );
    }
    fclose( sessionFile );
  }
}
//---------------------------------------------------------------------------

bool TFixFileStore::set( int msgSeqNum, const std::string& msg )
throw ( IOException )
{
  if ( fseek( m_msgFile, 0, SEEK_END ) ) 
    throw IOException( "Cannot seek to end of " + m_msgFileName );
  if ( fseek( m_headerFile, 0, SEEK_END ) ) 
    throw IOException( "Cannot seek to end of " + m_headerFileName );

  int offset = ftell( m_msgFile );
  if ( offset < 0 ) 
    throw IOException( "Unable to get file pointer position from " + m_msgFileName );
  int size = msg.size();

  if ( fprintf( m_headerFile, "%d,%d,%d ", msgSeqNum, offset, size ) < 0 )
    throw IOException( "Unable to write to file " + m_headerFileName );
  m_offsets[ msgSeqNum ] = std::make_pair( offset, size );
  fwrite( msg.c_str(), sizeof( char ), msg.size(), m_msgFile );
  if ( ferror( m_msgFile ) ) 
    throw IOException( "Unable to write to file " + m_msgFileName );
  if ( fflush( m_msgFile ) == EOF ) 
    throw IOException( "Unable to flush file " + m_msgFileName );
  if ( fflush( m_headerFile ) == EOF ) 
    throw IOException( "Unable to flush file " + m_headerFileName );
  return true;
}
//---------------------------------------------------------------------------

void TFixFileStore::get( int begin, int end,
                     std::vector < std::string > & result ) const
throw ( IOException )
{
  result.clear();
  std::string msg;
  for ( int i = begin; i <= end; ++i )
  {
    if ( get( i, msg ) )
      result.push_back( msg );
  }
}
//---------------------------------------------------------------------------

int TFixFileStore::getNextSenderMsgSeqNum() const throw ( IOException )
{
   //return m_cache.getNextSenderMsgSeqNum();
   return SeqNumsStore_->GetNextSenderMsgSeqNum();
}
//---------------------------------------------------------------------------

int TFixFileStore::getNextTargetMsgSeqNum() const throw ( IOException )
{
   //return m_cache.getNextTargetMsgSeqNum();
   return SeqNumsStore_->GetNextTargetMsgSeqNum();
}
//---------------------------------------------------------------------------

void TFixFileStore::setNextSenderMsgSeqNum( int value ) throw ( IOException )
{
   //m_cache.setNextSenderMsgSeqNum( value );
   //setSeqNum();
   EFixWFileSts resu = SeqNumsStore_->SetNextSenderMsgSeqNum(value);
        if (resu == wfs_WriteError)  throw IOException( "Unable to write to file " + m_seqNumsFileName );
   else if (resu == wfs_FlushError)  throw IOException( "Unable to flush file "    + m_seqNumsFileName );
}
//---------------------------------------------------------------------------

void TFixFileStore::setNextTargetMsgSeqNum( int value ) throw ( IOException )
{
   //m_cache.setNextTargetMsgSeqNum( value );
   //setSeqNum();
   EFixWFileSts resu = SeqNumsStore_->SetNextTargetMsgSeqNum(value);
        if (resu == wfs_WriteError)  throw IOException( "Unable to write to file " + m_seqNumsFileName );
   else if (resu == wfs_FlushError)  throw IOException( "Unable to flush file "    + m_seqNumsFileName );
}
//---------------------------------------------------------------------------

void TFixFileStore::incrNextSenderMsgSeqNum() throw ( IOException )
{
   //m_cache.incrNextSenderMsgSeqNum();
   //setSeqNum();
   EFixWFileSts resu = SeqNumsStore_->IncNextSenderMsgSeqNum();
        if (resu == wfs_WriteError)  throw IOException( "Unable to write to file " + m_seqNumsFileName );
   else if (resu == wfs_FlushError)  throw IOException( "Unable to flush file "    + m_seqNumsFileName );
}
//---------------------------------------------------------------------------

void TFixFileStore::incrNextTargetMsgSeqNum() throw ( IOException )
{
   //m_cache.incrNextTargetMsgSeqNum();
   //setSeqNum();
   EFixWFileSts resu = SeqNumsStore_->IncNextTargetMsgSeqNum();
        if (resu == wfs_WriteError)  throw IOException( "Unable to write to file " + m_seqNumsFileName );
   else if (resu == wfs_FlushError)  throw IOException( "Unable to flush file "    + m_seqNumsFileName );
}
//---------------------------------------------------------------------------

UtcTimeStamp TFixFileStore::getCreationTime() const throw ( IOException )
{
  return m_cache.getCreationTime();
}
//---------------------------------------------------------------------------

void TFixFileStore::reset() throw ( IOException )
{

  m_cache.reset();
  SeqNumsStore_.reset();
  open( true );
  setSession();
}
//---------------------------------------------------------------------------

void TFixFileStore::refresh() throw ( IOException )
{
  m_cache.reset();
  SeqNumsStore_.reset();
  open( false );
}
//---------------------------------------------------------------------------
//
//void TFixFileStore::setSeqNum()
//{
//  rewind( m_seqNumsFile );
//  fprintf( m_seqNumsFile, "%10.10d : %10.10d",
//           getNextSenderMsgSeqNum(), getNextTargetMsgSeqNum() );
//  if ( ferror( m_seqNumsFile ) )
//    throw IOException( "Unable to write to file " + m_seqNumsFileName );
//  if ( fflush( m_seqNumsFile ) )
//    throw IOException( "Unable to flush file " + m_seqNumsFileName );
//}
//---------------------------------------------------------------------------

void TFixFileStore::setSession()
{
  rewind( m_sessionFile );
  fprintf( m_sessionFile, "%s",
           UtcTimeStampConvertor::convert( m_cache.getCreationTime() ).c_str() );
  if ( ferror( m_sessionFile ) )
    throw IOException( "Unable to write to file " + m_sessionFileName );
  if ( fflush( m_sessionFile ) )
    throw IOException( "Unable to flush file " + m_sessionFileName );
}
//---------------------------------------------------------------------------

bool TFixFileStore::get( int msgSeqNum, std::string& msg ) const
throw ( IOException )
{
  NumToOffset::const_iterator find = m_offsets.find( msgSeqNum );
  if ( find == m_offsets.end() ) return false;
  const OffsetSize& offset = find->second;
  if ( fseek( m_msgFile, offset.first, SEEK_SET ) )
    throw IOException( "Unable to seek in file " + m_msgFileName );
  char* buffer = new char[ offset.second + 1 ];
  fread( buffer, sizeof( char ), offset.second, m_msgFile );
  if ( ferror( m_msgFile ) )
    throw IOException( "Unable to read from file " + m_msgFileName );
  buffer[ offset.second ] = 0;
  msg = buffer;
  delete [] buffer;
  return true;
}
//---------------------------------------------------------------------------

