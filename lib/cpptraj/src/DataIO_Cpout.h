#ifndef INC_DATAIO_CPOUT_H
#define INC_DATAIO_CPOUT_H
#include "DataIO.h"
#include "Cph.h"
#include "BufferedLine.h"
/// Read Amber cpout file 
class DataIO_Cpout : public DataIO {
  public:
    DataIO_Cpout();
    static void ReadHelp();
    static void WriteHelp();
    static BaseIOtype* Alloc() { return (BaseIOtype*)new DataIO_Cpout(); }
    int processReadArgs(ArgList&);
    int ReadData(FileName const&, DataSetList&, std::string const&);
    int processWriteArgs(ArgList&);
    int WriteData(FileName const&, DataSetList const&);
    bool ID_DataFormat(CpptrajFile&);
  private:
    static const char* FMT_REDOX_;
    static const char* FMT_PH_;
    enum FileType { PH = 0, REDOX, NONE };

    typedef std::vector<double> Darray;
    typedef std::vector<int> Iarray;
    typedef std::vector<Cph::CpRes> Rarray;

    struct StateInfo {
      int num_states_;
      int first_atom_;
      int num_atoms_;
      int first_state_;
      int first_charge_;
    };
    typedef std::vector<StateInfo> StateArray;

    int ReadCpin(FileName const&);
    int ReadRecord(BufferedLine&, const char*, const char*);
    double CalcTimeStep() const;
    int ReadSorted(BufferedLine&, DataSetList&, std::string const&, const char*, const char*);
    int ReadUnsortedExplicit(BufferedLine&, DataSetList&, std::string const&, const char*, const char*);
    int ReadUnsortedImplicit(BufferedLine&, DataSetList&, std::string const&, const char*, const char*);

    void WriteHeader(CpptrajFile&, double, double, float, int) const;

    FileName cpin_file_;
    FileType type_;
    float original_pH_;
    Rarray Residues_;    ///< Hold all residues from CPIN file

    int maxRes_;       ///< Total number of residues

    unsigned int nframes_; ///< Total number of frames
    int recType_;      ///< Current record type
    int mc_stepsize_;  ///< Monte Carlo step size
    int step_;         ///< Current step
    int s0_;           ///< Initial step
    int nRes_;         ///< Number of residues in current record.
    float solvent_pH_; ///< Current solvent pH
    float pHval_;      ///< Current pH value
    double time_;      ///< Current time
    double t0_;        ///< Initial time
    Iarray resStates_; ///< Current residue states

};
#endif
