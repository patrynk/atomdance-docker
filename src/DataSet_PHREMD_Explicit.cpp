#include "DataSet_PHREMD_Explicit.h"
#include "CpptrajStdio.h"

DataSet_PHREMD_Explicit::DataSet_PHREMD_Explicit() :
  DataSet_PHREMD(PH_EXPL, TextFormat(TextFormat::DOUBLE, 10, 4))
{}

int DataSet_PHREMD_Explicit::Allocate(SizeArray const& sizeIn) {
  if (!sizeIn.empty()) {
    resStates_.reserve( sizeIn[0] * residues_.size() );
    solvent_pH_.reserve( sizeIn[0] );
    recType_.reserve( sizeIn[0] );
  }
  return 0;
}
/*
void DataSet_PHREMD_Explicit::Resize(size_t n) {
  resStates_.resize(n * residues_.size(), 0);
  solvent_pH_.resize(n, 0.0);
  recType_.resize(n, false);
}
*/
void DataSet_PHREMD_Explicit::Info() const {
  mprintf(" (%zu residues, %zu frames)", residues_.size(), solvent_pH_.size());
}
/*
# ifdef MPI
void DataSet_PHREMD_Explicit::Reduce(Parallel::Comm const& commIn, int rank)
{
  if (commIn.Rank() == rank) {
    //commIn.Barrier(); // DEBUG
    //rprintf("MASTER:\n");
    //for (std::vector<int>::const_iterator it = residues_.front().begin(); it != residues_.front().end(); ++it)
    //  rprintf("%4u %1i\n", it-residues_.front().begin()+1, *it);
    //mflush();
    //commIn.Barrier(); // DEBUG
    std::vector<float> ftmp = solvent_pH_;
    commIn.Reduce(rank, &solvent_pH_[0], &ftmp[0], solvent_pH_.size(), MPI_FLOAT, MPI_SUM);
    ftmp.clear();
    std::vector<int> itmp = recType_;
    commIn.Reduce(rank, &recType_[0], &itmp[0], recType_.size(), MPI_INT, MPI_SUM);
    if (!residues_.empty()) {
      itmp = resStates_;
      commIn.Reduce(rank, &resStates_[0], &itmp[0], resStates_.size(), MPI_INT, MPI_SUM);
    }
  } else {
    //commIn.Barrier(); // DEBUG
    //commIn.Barrier();
    //rprintf("CHILD_:\n");
    //for (std::vector<int>::const_iterator it = residues_.front().begin(); it != residues_.front().end(); ++it)
    //  rprintf("%4u %1i\n", it-residues_.front().begin()+1, *it);
    commIn.Reduce(rank, 0, &solvent_pH_[0], solvent_pH_.size(), MPI_FLOAT, MPI_SUM);
    commIn.Reduce(rank, 0, &recType_[0], recType_.size(), MPI_INT, MPI_SUM);
    if (!residues_.empty()) {
      commIn.Reduce(rank, 0, &resStates_[0], resStates_.size(), MPI_INT, MPI_SUM);
    }
  }
}
#endif*/
