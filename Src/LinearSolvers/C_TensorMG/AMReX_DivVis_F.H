#ifndef _DivVis_F_H_
#define _DivVis_F_H_

#include <AMReX_REAL.H>

#if   defined(BL_LANG_FORT)
#   if  (BL_SPACEDIM==2)
#     define FORT_DVAPPLY          dvapply2d
#     define FORT_DVFLUX           dvflux2d
#     define FORT_GSRB             gsrbvisc2d
#     define FORT_APPLYBC          mcapplybc2dgen
#   elif(BL_SPACEDIM==3)
#     define FORT_DVAPPLY          dvapply3d
#     define FORT_DVFLUX           dvflux3d
#     define FORT_GSRB             gsrbvisc3d
#     define FORT_APPLYBC          mcapplybc3dgen
#   endif
#else

#if (BL_SPACEDIM == 2)

# if defined(BL_FORT_USE_UPPERCASE)
#  define FORT_DVAPPLY          DVAPPLY2D
#  define FORT_DVFLUX           DVFLUX2D
#  define FORT_GSRB             GSRBVISC2D
#  define FORT_APPLYBC          MCAPPLYBC2DGEN
# elif defined(BL_FORT_USE_LOWERCASE)
#  define FORT_DVAPPLY          dvapply2d
#  define FORT_DVFLUX           dvflux2d
#  define FORT_GSRB             gsrbvisc2d
#  define FORT_APPLYBC          mcapplybc2dgen
# elif defined(BL_FORT_USE_UNDERSCORE)
#  define FORT_DVAPPLY          dvapply2d_
#  define FORT_DVFLUX           dvflux2d_
#  define FORT_GSRB             gsrbvisc2d_
#  define FORT_APPLYBC          mcapplybc2dgen_
# endif

#else

# if defined(BL_FORT_USE_UPPERCASE)
#  define FORT_DVAPPLY          DVAPPLY3D
#  define FORT_DVFLUX           DVFLUX3D
#  define FORT_GSRB             GSRBVISC3D
#  define FORT_APPLYBC          MCAPPLYBC3DGEN
# elif defined(BL_FORT_USE_LOWERCASE)
#  define FORT_DVAPPLY          dvapply3d
#  define FORT_DVFLUX           dvflux3d
#  define FORT_GSRB             gsrbvisc3d
#  define FORT_APPLYBC          mcapplybc3dgen
# elif defined(BL_FORT_USE_UNDERSCORE)
#  define FORT_DVAPPLY          dvapply3d_
#  define FORT_DVFLUX           dvflux3d_
#  define FORT_GSRB             gsrbvisc3d_
#  define FORT_APPLYBC          mcapplybc3dgen_
# endif

#endif

#include <AMReX_ArrayLim.H>

#ifdef __cplusplus
extern "C"
{
#endif
  void FORT_DVAPPLY(
		    const amrex_real*u, ARLIM_P(ulo), ARLIM_P(uhi),
		    const amrex_real*alpha, const amrex_real*beta,
		    const amrex_real*a, ARLIM_P(alo),   ARLIM_P(ahi),
		    const amrex_real*muX, ARLIM_P(muXlo), ARLIM_P(muXhi),
		    const amrex_real*muY, ARLIM_P(muYlo), ARLIM_P(muYhi),
#if (BL_SPACEDIM > 2)
		    const amrex_real*muZ, ARLIM_P(muZlo), ARLIM_P(muZhi),
#endif
		    amrex_real*out, ARLIM_P(outlo), ARLIM_P(outhi),
		    const int*maskn, ARLIM_P(masknlo), ARLIM_P(masknhi),
		    const int*maske, ARLIM_P(maskelo), ARLIM_P(maskehi),
		    const int*maskw, ARLIM_P(maskwlo), ARLIM_P(maskwhi),
		    const int*masks, ARLIM_P(maskslo), ARLIM_P(maskshi),
#if (BL_SPACEDIM > 2)
		    const int*maskt, ARLIM_P(masktlo), ARLIM_P(maskthi),
		    const int*maskb, ARLIM_P(maskblo), ARLIM_P(maskbhi),
#endif
		    const amrex_real*trandern,
		    ARLIM_P(trandernlo),ARLIM_P(trandernhi),
		    const amrex_real*trandere,
		    ARLIM_P(tranderelo),ARLIM_P(tranderehi),
		    const amrex_real*tranderw,
		    ARLIM_P(tranderwlo),ARLIM_P(tranderwhi),
		    const amrex_real*tranders,
		    ARLIM_P(tranderslo),ARLIM_P(trandershi),
#if (BL_SPACEDIM > 2)
		    const amrex_real*trandert,
		    ARLIM_P(trandertlo),ARLIM_P(tranderthi),
		    const amrex_real*tranderb,
		    ARLIM_P(tranderblo),ARLIM_P(tranderbhi),
#endif
		    const int* lo, const int* hi, const amrex_real* h);

  void FORT_DVFLUX(
		   const amrex_real*u, ARLIM_P(ulo), ARLIM_P(uhi),
		   const amrex_real*alpha, const amrex_real*beta,
		   const amrex_real*a, ARLIM_P(alo),   ARLIM_P(ahi),
		   const amrex_real*muX, ARLIM_P(muXlo), ARLIM_P(muXhi),
		   const amrex_real*muY, ARLIM_P(muYlo), ARLIM_P(muYhi),
#if BL_SPACEDIM>2
		   const amrex_real*muZ, ARLIM_P(muZlo), ARLIM_P(muZhi),
#endif
		   amrex_real*xflux, ARLIM_P(xfluxlo), ARLIM_P(xfluxhi),
		   amrex_real*yflux, ARLIM_P(yfluxlo), ARLIM_P(yfluxhi),
#if BL_SPACEDIM>2
		   amrex_real*zflux, ARLIM_P(zfluxlo), ARLIM_P(zfluxhi),
#endif
		   const int*maskn, ARLIM_P(masknlo), ARLIM_P(masknhi),
		   const int*maske, ARLIM_P(maskelo), ARLIM_P(maskehi),
		   const int*maskw, ARLIM_P(maskwlo), ARLIM_P(maskwhi),
		   const int*masks, ARLIM_P(maskslo), ARLIM_P(maskshi),
#if BL_SPACEDIM>2
		   const int*maskt, ARLIM_P(masktlo), ARLIM_P(maskthi),
		   const int*maskb, ARLIM_P(maskblo), ARLIM_P(maskbhi),
#endif
		   const amrex_real*trandern,
		   ARLIM_P(trandernlo),ARLIM_P(trandernhi),
		   const amrex_real*trandere,
		   ARLIM_P(tranderelo),ARLIM_P(tranderehi),
		   const amrex_real*tranderw,
		   ARLIM_P(tranderwlo),ARLIM_P(tranderwhi),
		   const amrex_real*tranders,
		   ARLIM_P(tranderslo),ARLIM_P(trandershi),
#if BL_SPACEDIM>2
		   const amrex_real*trandert,
		   ARLIM_P(trandertlo),ARLIM_P(tranderthi),
		   const amrex_real*tranderb,
		   ARLIM_P(tranderblo),ARLIM_P(tranderbhi),
#endif
		   const int* lo, const int* hi, const amrex_real* h);

  void FORT_GSRB(
		 amrex_real* u, ARLIM_P(ulo), ARLIM_P(uhi),
		 const amrex_real* rhs, ARLIM_P(rhslo), ARLIM_P(rhshi),
		 amrex_real* alpha, amrex_real* beta,
		 const amrex_real* a, ARLIM_P(alo), ARLIM_P(ahi),
		 const amrex_real* muX, ARLIM_P(muXlo), ARLIM_P(muXhi),
		 const amrex_real* muY, ARLIM_P(muYlo), ARLIM_P(muYhi),
#if BL_SPACEDIM>2
		 const amrex_real* muZ, ARLIM_P(muZlo), ARLIM_P(muZhi),
#endif
		 const int* maskn, ARLIM_P(masknlo), ARLIM_P(masknhi),
		 const amrex_real* fn, ARLIM_P(fnlo), ARLIM_P(fnhi),
		 const int* maske, ARLIM_P(maskelo), ARLIM_P(maskehi),
		 const amrex_real* fe, ARLIM_P(felo), ARLIM_P(fehi),
		 const int* maskw, ARLIM_P(maskwlo), ARLIM_P(maskwhi),
		 const amrex_real* fw, ARLIM_P(fwlo), ARLIM_P(fwhi),
		 const int* masks, ARLIM_P(maskslo), ARLIM_P(maskshi),
		 const amrex_real* fs, ARLIM_P(fslo), ARLIM_P(fshi),
#if BL_SPACEDIM>2
		 const int* maskt, ARLIM_P(masktlo), ARLIM_P(maskthi),
		 const amrex_real* ft, ARLIM_P(ftlo), ARLIM_P(fthi),
		 const int* maskb, ARLIM_P(maskblo), ARLIM_P(maskbhi),
		 const amrex_real* fb, ARLIM_P(fblo), ARLIM_P(fbhi),
#endif
		 const amrex_real* trandern, 
		 ARLIM_P(trandernlo), ARLIM_P(trandernhi),
		 const amrex_real* trandere, 
		 ARLIM_P(tranderelo), ARLIM_P(tranderehi),
		 const amrex_real* tranderw, 
		 ARLIM_P(tranderwlo), ARLIM_P(tranderwhi),
		 const amrex_real* tranders, 
		 ARLIM_P(tranderslo), ARLIM_P(trandershi),
#if BL_SPACEDIM>2
		 const amrex_real* trandert, 
		 ARLIM_P(trandertlo), ARLIM_P(tranderthi),
		 const amrex_real* tranderb, 
		 ARLIM_P(tranderblo), ARLIM_P(tranderbhi),
#endif
		 const int* lo, const int* hi,
		 const amrex_real* h, int &nc, int &phaseflag);
		 
  void FORT_APPLYBC(
		    const int *flagden,  // 1 if want values in den
		    const int *flagbc,   // 1 for inhomogeneous
		    const int *maxorder, // maxorder of interpolant
		    amrex_real *phi, ARLIM_P(phi_lo), ARLIM_P(phi_hi),
		    const int *cdir,
		    const int *bct,
		    const amrex_real *bcl,
		    const amrex_real *bcval, ARLIM_P(bcval_lo), ARLIM_P(bcval_hi),
#if BL_SPACEDIM==2
		    const int *mask,   ARLIM_P(mask_lo),  ARLIM_P(mask_hi), 
		    const int *maskp,   ARLIM_P(maskp_lo),  ARLIM_P(maskp_hi), 
		    const int *maskm,   ARLIM_P(maskm_lo),  ARLIM_P(maskm_hi), 
#elif BL_SPACEDIM==3
		    const int *maskn,   ARLIM_P(maskn_lo),  ARLIM_P(maskn_hi), 
		    const int *maske,   ARLIM_P(maske_lo),  ARLIM_P(maske_hi), 
		    const int *maskw,   ARLIM_P(maskw_lo),  ARLIM_P(maskw_hi), 
		    const int *masks,   ARLIM_P(masks_lo),  ARLIM_P(masks_hi), 
		    const int *maskt,   ARLIM_P(maskt_lo),  ARLIM_P(maskt_hi), 
		    const int *maskb,   ARLIM_P(maskb_lo),  ARLIM_P(maskb_hi), 
#endif
		    amrex_real *den,         ARLIM_P(den_lo),   ARLIM_P(den_hi),
		    const amrex_real *exttd, ARLIM_P(exttd_lo), ARLIM_P(exttd_hi),
		    amrex_real *tander,      ARLIM_P(tander_lo),ARLIM_P(tander_hi),
		    const int *lo, const int *hi, 
		    const int *nc,
		    const amrex_real *h
		    );


#ifdef __cplusplus
}
#endif

#endif

#endif
