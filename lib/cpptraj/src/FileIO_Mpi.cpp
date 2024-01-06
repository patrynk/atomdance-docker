#include "FileIO_Mpi.h"
#ifdef MPI
// Needed for some older Intel MPI and newer OpenMPI versions
# include <cstdio>
// FileIO_Mpi::Open()
int FileIO_Mpi::Open(const char *filename, const char *mode) {
  if (comm_.IsNull()) return 1;
  if (filename == 0) return 1;
  return pfile_.OpenFile(filename, mode, comm_);
}

// FileIO_Mpi::Close()
int FileIO_Mpi::Close() { return (pfile_.CloseFile()); }

// FileIO_Mpi::Read()
int FileIO_Mpi::Read(void *buffer, size_t num_bytes) {
  return (pfile_.Fread(buffer, num_bytes, MPI_CHAR));
}

// FileIO_Mpi::Write()
int FileIO_Mpi::Write(const void *buffer, size_t num_bytes) {
  if (pfile_.Fwrite(buffer, num_bytes, MPI_CHAR)) return 1;
  // NOTE: Check for errors here.
  return 0;
}

int FileIO_Mpi::Flush() { return pfile_.Flush(); }

// FileIO_Mpi::Seek()
int FileIO_Mpi::Seek(off_t offset) {
  if (pfile_.Fseek(offset, SEEK_SET)) return 1;
  return 0;
}

// FileIO_Mpi::Rewind()
int FileIO_Mpi::Rewind() {
  if (pfile_.Fseek(0L, SEEK_SET)) return 1;
  return 0;
}

// FileIO_Mpi::Tell()
off_t FileIO_Mpi::Tell() { return ( pfile_.Position() ); }

// FileIO_Mpi::Gets()
int FileIO_Mpi::Gets(char *str, int num) {
  if ( pfile_.Fgets(str, num) == 0 ) return 1;
  return 0;
}

// FileIO_Mpi::SetSize()
/** Set size of mpi file, required when splitting up writes.
  */
int FileIO_Mpi::SetSize(long int offset) {
  if ( pfile_.SetSize(offset) ) return 1;
  return 0;
}
#endif
