
#include <CNS.H>
#include <CNS_F.H>

#include <AMReX_EBMultiFabUtil.H>
#include <AMReX_ParmParse.H>

#include <climits>

using namespace amrex;

constexpr int CNS::NUM_GROW;

ErrorList CNS::err_list;
BCRec     CNS::phys_bc;

int       CNS::verbose = 0;
IntVect   CNS::hydro_tile_size {1024,16,16};
Real      CNS::cfl = 0.3;
int       CNS::do_load_balance = 1;

CNS::CNS ()
{}

CNS::CNS (Amr&            papa,
          int             lev,
          const Geometry& level_geom,
          const BoxArray& bl,
          const DistributionMapping& dm,
          Real            time)
    : AmrLevel(papa,lev,level_geom,bl,dm,time)
{
    // xxxxx need to build flux register

    buildMetrics();

    // xxxxx initialize EB
}

CNS::~CNS ()
{}

void
CNS::init (AmrLevel& old)
{
    auto& oldlev = dynamic_cast<CNS&>(old);

    Real dt_new    = parent->dtLevel(level);
    Real cur_time  = oldlev.state[State_Type].curTime();
    Real prev_time = oldlev.state[State_Type].prevTime();
    Real dt_old    = cur_time - prev_time;
    setTimeLevel(cur_time,dt_old,dt_new);

    MultiFab& S_new = get_new_data(State_Type);
    FillPatch(old,S_new,0,cur_time,State_Type,0,NUM_STATE);

    if (CNS::do_load_balance) {
        MultiFab& C_new = get_new_data(Cost_Type);
        FillPatch(old,C_new,0,cur_time,Cost_Type,0,1);
    }
}

void
CNS::init ()
{
    Real dt        = parent->dtLevel(level);
    Real cur_time  = getLevel(level-1).state[State_Type].curTime();
    Real prev_time = getLevel(level-1).state[State_Type].prevTime();
    Real dt_old = (cur_time - prev_time)/static_cast<Real>(parent->MaxRefRatio(level-1));
    setTimeLevel(cur_time,dt_old,dt);

    MultiFab& S_new = get_new_data(State_Type);
    FillCoarsePatch(S_new, 0, cur_time, State_Type, 0, NUM_STATE);

    if (CNS::do_load_balance) {
        MultiFab& C_new = get_new_data(Cost_Type);
        FillCoarsePatch(C_new, 0, cur_time, Cost_Type, 0, 1);
    }
}

void
CNS::initData ()
{
    BL_PROFILE("CNS::initData()");

    const Real* dx  = geom.CellSize();
    const Real* prob_lo = geom.ProbLo();
    MultiFab& S_new = get_new_data(State_Type);
    Real cur_time   = state[State_Type].curTime();

#ifdef _OPENMP
#pragma omp parallel
#endif
    for (MFIter mfi(S_new); mfi.isValid(); ++mfi)
    {
        const Box& box = mfi.validbox();
        cns_initdata(&level, &cur_time,
                     BL_TO_FORTRAN_BOX(box),
                     BL_TO_FORTRAN_ANYD(S_new[mfi]),
                     dx, prob_lo);
    }

    if (CNS::do_load_balance)
    {
        MultiFab& C_new = get_new_data(Cost_Type);
        C_new.setVal(1.0);
        EB_set_covered(C_new, 0, 1, 0.2);
        EB_set_single_valued_cells(C_new, 0, 1, 5.0);
    }
}

void
CNS::computeInitialDt (int                   finest_level,
                       int                   sub_cycle,
                       Array<int>&           n_cycle,
                       const Array<IntVect>& ref_ratio,
                       Array<Real>&          dt_level,
                       Real                  stop_time)
{
    //
    // Grids have been constructed, compute dt for all levels.
    //
    if (level > 0) {
        return;
    }

    Real dt_0 = std::numeric_limits<Real>::max();
    int n_factor = 1;
    for (int i = 0; i <= finest_level; i++)
    {
        dt_level[i] = getLevel(i).initialTimeStep();
        n_factor   *= n_cycle[i];
        dt_0 = std::min(dt_0,n_factor*dt_level[i]);
    }

    //
    // Limit dt's by the value of stop_time.
    //
    const Real eps = 0.001*dt_0;
    Real cur_time  = state[State_Type].curTime();
    if (stop_time >= 0.0) {
        if ((cur_time + dt_0) > (stop_time - eps))
            dt_0 = stop_time - cur_time;
    }

    n_factor = 1;
    for (int i = 0; i <= finest_level; i++)
    {
        n_factor *= n_cycle[i];
        dt_level[i] = dt_0/n_factor;
    }
}

void
CNS::computeNewDt (int                   finest_level,
                   int                   sub_cycle,
                   Array<int>&           n_cycle,
                   const Array<IntVect>& ref_ratio,
                   Array<Real>&          dt_min,
                   Array<Real>&          dt_level,
                   Real                  stop_time,
                   int                   post_regrid_flag)
{
    //
    // We are at the end of a coarse grid timecycle.
    // Compute the timesteps for the next iteration.
    //
    if (level > 0) {
        return;
    }

    for (int i = 0; i <= finest_level; i++)
    {
        dt_min[i] = getLevel(i).estTimeStep();
    }

    if (post_regrid_flag == 1) 
    {
	//
	// Limit dt's by pre-regrid dt
	//
	for (int i = 0; i <= finest_level; i++)
	{
	    dt_min[i] = std::min(dt_min[i],dt_level[i]);
	}
    }
    else 
    {
	//
	// Limit dt's by change_max * old dt
	//
	static Real change_max = 1.1;
	for (int i = 0; i <= finest_level; i++)
	{
	    dt_min[i] = std::min(dt_min[i],change_max*dt_level[i]);
	}
    }
    
    //
    // Find the minimum over all levels
    //
    Real dt_0 = std::numeric_limits<Real>::max();
    int n_factor = 1;
    for (int i = 0; i <= finest_level; i++)
    {
        n_factor *= n_cycle[i];
        dt_0 = std::min(dt_0,n_factor*dt_min[i]);
    }

    //
    // Limit dt's by the value of stop_time.
    //
    const Real eps = 0.001*dt_0;
    Real cur_time  = state[State_Type].curTime();
    if (stop_time >= 0.0) {
        if ((cur_time + dt_0) > (stop_time - eps)) {
            dt_0 = stop_time - cur_time;
        }
    }

    n_factor = 1;
    for (int i = 0; i <= finest_level; i++)
    {
        n_factor *= n_cycle[i];
        dt_level[i] = dt_0/n_factor;
    }
}

void
CNS::post_regrid (int lbase, int new_finest)
{
    fixUpGeometry();
}

void
CNS::post_timestep (int iteration)
{
    // xxxxx some reflux stuff

    if (level < parent->finestLevel()) {
        avgDown();
    }
}

void
CNS::postCoarseTimeStep (Real time)
{
    // This only computes sum on level 0
    if (verbose >= 2) {
        const MultiFab& S_new = get_new_data(State_Type);
        MultiFab mf(grids, dmap, 1, 0);
        std::array<Real,5> tot;
        for (int comp = 0; comp < 5; ++comp) {
            MultiFab::Copy(mf, S_new, comp, 0, 1, 0);
            MultiFab::Multiply(mf, volfrac, 0, 0, 1, 0);
            tot[comp] = mf.sum(0,true) * geom.ProbSize();
        }
#ifdef BL_LAZY
        Lazy::QueueReduction( [=] () mutable {
#endif
        ParallelDescriptor::ReduceRealSum(tot.data(), 5, ParallelDescriptor::IOProcessorNumber());
        amrex::Print().SetPrecision(17) << "\n[CNS] Total mass       is " << tot[0] << "\n"
                                        <<   "      Total x-momentum is " << tot[1] << "\n"
                                        <<   "      Total y-momentum is " << tot[2] << "\n"
                                        <<   "      Total z-momentum is " << tot[3] << "\n"
                                        <<   "      Total energy     is " << tot[4] << "\n";
#ifdef BL_LAZY
        });
#endif
    }
}

void
CNS::post_init (Real)
{
    fixUpGeometry();

    if (level > 0) return;
    for (int k = parent->finestLevel()-1; k >= 0; --k) {
        getLevel(k).avgDown();
    }
}

void
CNS::post_restart ()
{
    fixUpGeometry();
}

void
CNS::errorEst (TagBoxArray& tags, int clearval, int tagval, Real time, int n_error_buf, int ngrow)
{
    // xxxxx tagging
}

void
CNS::read_params ()
{
    ParmParse pp("cns");

    pp.query("v", verbose);
 
    Array<int> tilesize(AMREX_SPACEDIM);
    if (pp.queryarr("hydro_tile_size", tilesize, 0, AMREX_SPACEDIM))
    {
	for (int i=0; i<AMREX_SPACEDIM; i++) hydro_tile_size[i] = tilesize[i];
    }
   
    pp.query("cfl", cfl);

    Array<int> lo_bc(AMREX_SPACEDIM), hi_bc(AMREX_SPACEDIM);
    pp.getarr("lo_bc", lo_bc, 0, AMREX_SPACEDIM);
    pp.getarr("hi_bc", hi_bc, 0, AMREX_SPACEDIM);
    for (int i = 0; i < AMREX_SPACEDIM; ++i) {
        phys_bc.setLo(i,lo_bc[i]);
        phys_bc.setHi(i,hi_bc[i]);
    }

    pp.query("do_load_balance", do_load_balance);
}

void
CNS::avgDown ()
{
    BL_PROFILE("CNS::avgDown()");

    if (level == parent->finestLevel()) return;

    auto& fine_lev = getLevel(level+1);

    MultiFab& S_crse =          get_new_data(State_Type);
    MultiFab& S_fine = fine_lev.get_new_data(State_Type);

    MultiFab volume(S_fine.boxArray(), S_fine.DistributionMap(), 1, 0);
    volume.setVal(1.0);
    amrex::EB_average_down(S_fine, S_crse, volume, fine_lev.volFrac(),
                           0, S_fine.nComp(), fine_ratio);

    const int nghost = 0;
    computeTemp (S_crse, nghost);
}

void
CNS::buildMetrics ()
{
    BL_PROFILE("CNS::buildMetrics()");

    // make sure dx == dy == dz
    const Real* dx = geom.CellSize();
    if (std::abs(dx[0]-dx[1]) > 1.e-12*dx[0] || std::abs(dx[0]-dx[2]) > 1.e-12*dx[0]) {
        amrex::Abort("CNS: must have dx == dy == dz\n");
    }

    volfrac.clear();
    volfrac.define(grids,dmap,1,NUM_GROW,MFInfo(),Factory());
    amrex::EB_set_volume_fraction(volfrac);

    bndrycent.clear();
    bndrycent.define(grids,dmap,AMREX_SPACEDIM,NUM_GROW,MFInfo(),Factory());
    amrex::EB_set_bndry_centroid(bndrycent);

    for (int idim = 0; idim < AMREX_SPACEDIM; ++idim) {
        const BoxArray& ba = amrex::convert(grids,IntVect::TheDimensionVector(idim));
        areafrac[idim].clear();
        areafrac[idim].define(ba,dmap,1,NUM_GROW,MFInfo(),Factory());
        facecent[idim].clear();
        facecent[idim].define(ba,dmap,AMREX_SPACEDIM-1,NUM_GROW,MFInfo(),Factory());
    }
    amrex::EB_set_area_fraction_face_centroid(areafrac, facecent);
}

Real
CNS::estTimeStep ()
{
    BL_PROFILE("CNS::estTimeStep()");

    Real estdt = std::numeric_limits<Real>::max();

    const Real* dx = geom.CellSize();
    const MultiFab& S = get_new_data(State_Type);

#ifdef _OPENMP
#pragma omp parallel reduction(min:estdt)
#endif
    {
        Real dt = std::numeric_limits<Real>::max();
        for (MFIter mfi(S,true); mfi.isValid(); ++mfi)
        {
            const Box& box = mfi.tilebox();

            const auto& sfab = dynamic_cast<EBFArrayBox const&>(S[mfi]);
            const auto& flag = sfab.getEBCellFlagFab();

            if (flag.getType(box) != FabType::covered) {
                cns_estdt(BL_TO_FORTRAN_BOX(box),
                          BL_TO_FORTRAN_ANYD(S[mfi]),
                          dx, &dt);
            }
        }
        estdt = std::min(estdt,dt);
    }

    estdt *= cfl;
    ParallelDescriptor::ReduceRealMin(estdt);
    return estdt;
}

Real
CNS::initialTimeStep ()
{
    return estTimeStep();
}

void
CNS::computeTemp (MultiFab& State, int ng)
{
    BL_PROFILE("CNS::computeTemp()");

    // This will reset Eint and compute Temperature 
#ifdef _OPENMP
#pragma omp parallel
#endif
    for (MFIter mfi(State,true); mfi.isValid(); ++mfi)
    {
        const Box& bx = mfi.growntilebox(ng);

        const auto& sfab = dynamic_cast<EBFArrayBox const&>(State[mfi]);
        const auto& flag = sfab.getEBCellFlagFab();

        if (flag.getType(bx) != FabType::covered) {
            cns_compute_temperature(BL_TO_FORTRAN_BOX(bx),
                                    BL_TO_FORTRAN_ANYD(State[mfi]));
        }
    }
}

void
CNS::fixUpGeometry ()
{
    const auto& S = get_new_data(State_Type);

    const int ng = numGrow()-1;

#ifdef _OPENMP
#pragma omp parallel
#endif
    for (MFIter mfi(S, true); mfi.isValid(); ++mfi)
    {
        EBCellFlagFab& flag = const_cast<EBCellFlagFab&>(static_cast<EBFArrayBox const&>
                                                         (S[mfi]).getEBCellFlagFab());
        const Box& bx = mfi.growntilebox(ng);
        if (flag.getType(bx) == FabType::singlevalued)
        {
            cns_eb_fixup_geom(BL_TO_FORTRAN_BOX(bx),
                              BL_TO_FORTRAN_ANYD(flag),
                              BL_TO_FORTRAN_ANYD(volfrac[mfi]));
        }
    }

}

void
CNS::LoadBalance (Amr& amr)
{
    if (amr.levelSteps(0) == 1)
    {
        amrex::Print() << "Load balance at Step " << amr.levelSteps(0) << "\n";

        for (int lev = 0; lev <= amr.finestLevel(); ++lev)
        {
            MultiFab& C_new = amr.getLevel(lev).get_new_data(Cost_Type);

            const DistributionMapping& newdm = DistributionMapping::makeKnapSack(C_new);

            amr.InstallNewDistributionMap(lev, newdm);

            dynamic_cast<CNS&>(amr.getLevel(lev)).fixUpGeometry();
        }
    }
}
