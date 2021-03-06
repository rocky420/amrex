module cns_module

  use amrex_fort_module, only : rt=>amrex_real
  implicit none
  private

  integer, parameter, public :: URHO  = 1
  integer, parameter, public :: UMX   = 2
  integer, parameter, public :: UMY   = 3
  integer, parameter, public :: UMZ   = 4
  integer, parameter, public :: UEDEN = 5
  integer, parameter, public :: UEINT = 6
  integer, parameter, public :: UTEMP = 7
  integer, parameter, public :: NVAR  = 7

  integer, parameter, public :: QRHO   = 1
  integer, parameter, public :: QU     = 2
  integer, parameter, public :: QV     = 3
  integer, parameter, public :: QW     = 4
  integer, parameter, public :: QP     = 5
  integer, parameter, public :: QC     = 6
  integer, parameter, public :: QEINT  = 7
  integer, parameter, public :: QTEMP  = 8
  integer, parameter, public :: QVAR   = 8
  

  real(rt), parameter, public :: smallp = 1.d-10
  real(rt), parameter, public :: smallr = 1.d-19

  ! boundary condition information
  integer, save, public :: physbc_lo(3)
  integer, save, public :: physbc_hi(3)
  integer, save, public :: Interior, Inflow, Outflow, Symmetry, SlipWall, NoSlipWall

  ! problem domain
  real(rt), save, public :: problo(3), probhi(3), center(3)

  logical, save, public :: actually_2D = .false.
  logical, save, public :: use_total_energy_as_eb_weights = .false.

  public :: cns_init_fort

contains

  subroutine cns_init_fort (physbc_lo_in, physbc_hi_in, &
       Interior_in, Inflow_in, Outflow_in, Symmetry_in, SlipWall_in, NoSlipWall_in, &
       problo_in, probhi_in) &
       bind(c,name='cns_init_fort')
    use cns_physics_module, only : physics_init
    use amrex_parmparse_module
    integer, intent(in) :: physbc_lo_in(3), physbc_hi_in(3)
    integer, value, intent(in) :: Interior_in, Inflow_in, Outflow_in, Symmetry_in, SlipWall_in, NoSlipWall_in
    real(rt), intent(in) :: problo_in(3), probhi_in(3)

    type(amrex_parmparse) :: pp
    integer :: dim, i_use_total_energy_as_eb_weights

    physbc_lo = physbc_lo_in
    physbc_hi = physbc_hi_in

    Interior   = Interior_in
    Inflow     = Inflow_in
    Outflow    = Outflow_in
    Symmetry   = Symmetry_in
    SlipWall   = SlipWall_in
    NoSlipWall = NoSlipWall_in

    problo = problo_in
    probhi = probhi_in
    center = 0.5d0*(problo+probhi)

    call physics_init()

    call amrex_parmparse_build(pp,"cns")

    dim = 3
    call pp%query("dim", dim)
    actually_2D = dim.eq.2

    i_use_total_energy_as_eb_weights = 0
    call pp%query("use_total_energy_as_eb_weights", i_use_total_energy_as_eb_weights)
    use_total_energy_as_eb_weights = i_use_total_energy_as_eb_weights .ne. 0

    call amrex_parmparse_destroy(pp)

  end subroutine cns_init_fort

end module cns_module
