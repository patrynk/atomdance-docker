#ifndef INC_EXEC_SORTENSEMBLEDATA_H
#define INC_EXEC_SORTENSEMBLEDATA_H
#include "Exec.h"
/// Sort unsorted data. Currently only for data from PH-REMD runs. 
class Exec_SortEnsembleData : public Exec {
  public:
    Exec_SortEnsembleData() : Exec(GENERAL), debug_(0) {}
    void Help() const;
    DispatchObject* Alloc() const { return (DispatchObject*)new Exec_SortEnsembleData(); }
    RetType Execute(CpptrajState&, ArgList&);
  private:
    inline bool CheckError(int err) const {
#     ifdef MPI
      if (comm_.CheckError( err )) return 1;
#     else
      if (err != 0) return 1;
#     endif
      return 0;
    }

    int SortData(DataSetList const&, DataSetList&) const;
    int Sort_pH_Data(DataSetList const&, DataSetList&, unsigned int) const;

    int debug_;
#   ifdef MPI
    Parallel::Comm comm_;
#   endif
};
#endif
