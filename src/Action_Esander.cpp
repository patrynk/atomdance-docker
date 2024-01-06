#include "Action_Esander.h"
#include "CpptrajStdio.h"

#if defined(USE_SANDERLIB) && !defined(LIBCPPTRAJ)
// CONSTRUCTOR
Action_Esander::Action_Esander() : currentParm_(0) {}

int Action_Esander::AddSet(Energy_Sander::Etype typeIn, DataSetList& DslIn, DataFile* outfile,
                           std::string const& setname)
{
  Esets_[typeIn] = DslIn.AddSet(DataSet::DOUBLE, MetaData(setname, Energy_Sander::Easpect(typeIn)));
  if (Esets_[typeIn] == 0) return 1;
  if (outfile != 0) outfile->AddDataSet( Esets_[typeIn] );
  return 0;
}

void Action_Esander::Help() const {
  mprintf("\t[<name>] [out <filename>] [saveforces] [parmname <file>] [keepfiles]\n"
          "\t[<namelist vars>]\n"
          "    Supported namelist vars: %s\n", Energy_Sander::SupportedNamelist());
  mprintf("  Calculate energy for atoms in mask using Sander energy routines. This\n"
          "  currently requires writing a temporary Amber topology, the name of which\n"
          "  can be set by 'parmname' (default: 'CpptrajEsander.parm7').\n");
}

// Action_Esander::Init()
Action::RetType Action_Esander::Init(ArgList& actionArgs, ActionInit& init, int debugIn)
{
# ifdef MPI
  trajComm_ = init.TrajComm();
# endif
  SANDER_.SetDebug( debugIn );
  Init_ = init;
  // Get keywords
  outfile_ = init.DFL().AddDataFile( actionArgs.GetStringKey("out"), actionArgs );
  save_forces_ = actionArgs.hasKey("saveforces");
  ReferenceFrame REF = init.DSL().GetReferenceFrame( actionArgs );
  if (REF.error()) return Action::ERR;
  if (!REF.empty()) {
    refFrame_ = REF.Coord();
    currentParm_ = REF.ParmPtr();
  }
  if (SANDER_.SetInput( actionArgs )) return Action::ERR;
  // DataSet name and array
  setname_ = actionArgs.GetStringNext();
  if (setname_.empty())
    setname_ = init.DSL().GenerateDefaultName("ENE");
  Esets_.clear();
  Esets_.resize( (int)Energy_Sander::N_ENERGYTYPES, 0 );

  mprintf("    ESANDER: Calculating energy using Sander.\n");
  mprintf("\tTemporary topology file name is '%s'\n", SANDER_.TopFilename().full());
  if (save_forces_) mprintf("\tSaving force information to frame.\n");
  mprintf("\tReference for initialization");
  if (!REF.empty())
    mprintf(" is '%s'\n", REF.refName());
  else
    mprintf(" will be first frame.\n");
  return Action::OK;
}

/** Initialize sander energy for current top/reference, set up data sets. */
int Action_Esander::InitForRef() {
# ifdef MPI
  if ( SANDER_.Initialize( *currentParm_, refFrame_, trajComm_ ) ) return 1;
# else
  if ( SANDER_.Initialize( *currentParm_, refFrame_ ) ) return 1;
# endif
  // Set up DataSets
  int Nsets = (int)Energy_Sander::N_ENERGYTYPES;
  for (int ie = 0; ie != Nsets; ie++) {
    Energy_Sander::Etype etype = (Energy_Sander::Etype)ie;
    if (SANDER_.IsActive( etype )) {
      if (AddSet( etype, Init_.DSL(), outfile_, setname_ )) return 1;
    }
  }
  mprintf("\tSaving the following energy terms:\n");
  for (int ie = 0; ie != Nsets; ie++)
    if (Esets_[ie] != 0)
      mprintf("\t  %s in '%s'\n", Energy_Sander::Elabel((Energy_Sander::Etype)ie),
              Esets_[ie]->legend());

  return 0;
}

// Action_Esander::Setup()
/** Set angle up for this parmtop. Get masks etc.
  */
Action::RetType Action_Esander::Setup(ActionSetup& setup) {
  if (currentParm_ != 0 && currentParm_->Pindex() != setup.Top().Pindex())
  {
    mprintf("Warning: Current topology is %i:%s but reference is %i:%s. Skipping.\n",
            setup.Top().Pindex(), setup.Top().c_str(),
            currentParm_->Pindex(), currentParm_->c_str());
    return Action::SKIP;
  }
  // Check for LJ terms
  if (!setup.Top().Nonbond().HasNonbond())
  {
    mprinterr("Error: Topology '%s' does not have non-bonded parameters.\n", setup.Top().c_str());
    return Action::ERR;
  }
  // If reference specified, init now. Otherwise using first frame.
  if (currentParm_ != 0 ) {
    if ( InitForRef() ) return Action::ERR;
  } else
    currentParm_ = setup.TopAddress();
  // If saving of forces is requested, make sure CoordinateInfo has force.
  if (save_forces_) {
    cInfo_ = setup.CoordInfo();
    cInfo_.SetForce( true );
    newFrame_.SetupFrameV( setup.Top().Atoms(), cInfo_ );
    setup.SetCoordInfo( &cInfo_ );
    ret_ = Action::MODIFY_COORDS;
    return Action::MODIFY_TOPOLOGY;
  }
  ret_ = Action::OK;
  return Action::OK;
}

// Action_Esander::DoAction()
Action::RetType Action_Esander::DoAction(int frameNum, ActionFrame& frm) {
  if (refFrame_.empty()) {
    refFrame_ = frm.Frm();
    if ( InitForRef() ) return Action::ERR;
  }
  if (save_forces_) {
    newFrame_.SetCoordAndBox( frm.Frm() );
    SANDER_.CalcEnergyForces( newFrame_ );
    frm.SetFrame( &newFrame_ );
  } else
    // FIXME: Passing in ModifyFrm() to give CalcEnergy access to non-const pointers
    SANDER_.CalcEnergy( frm.ModifyFrm() );

  for (int ie = 0; ie != (int)Energy_Sander::N_ENERGYTYPES; ie++) {
    if (Esets_[ie] != 0)
      Esets_[ie]->Add(frameNum, SANDER_.Eptr((Energy_Sander::Etype)ie));
  }

  return ret_;
}

#else /* ----- Not using SANDERLIB or LIBCPPTRAJ is defined ----------------- */
Action_Esander::Action_Esander() {}

void Action_Esander::Help() const {
  mprintf("Warning: CPPTRAJ was compiled without libsander. This Action is disabled.\n");
}

Action::RetType Action_Esander::Init(ArgList& actionArgs, ActionInit& init, int debugIn)
{
  mprinterr("Error: CPPTRAJ was compiled without libsander. This Action is disabled.\n");
  return Action::ERR;
}

Action::RetType Action_Esander::Setup(ActionSetup& setup) { return Action::ERR; }

Action::RetType Action_Esander::DoAction(int frameNum, ActionFrame& frm) { return Action::ERR; }
#endif
