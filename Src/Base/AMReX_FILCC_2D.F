
#undef  BL_LANG_CC
#ifndef BL_LANG_FORT
#define BL_LANG_FORT
#endif

#include "AMReX_REAL.H"
#include "AMReX_CONSTANTS.H"
#include "AMReX_BC_TYPES.H"
#include "AMReX_ArrayLim.H"

#define SDIM 2

c ::: -----------------------------------------------------------
c ::: This routine is intended to be a generic fill function
c ::: for cell-centered data.  It knows how to extrapolate
c ::: and reflect data and is used to supplement the problem-specific
c ::: fill functions which call it.
c ::: 
c ::: INPUTS/OUTPUTS:
c ::: q           <=  array to fill
c ::: lo,hi        => index extent of q array
c ::: domlo,domhi  => index extent of problem domain
c ::: dx           => cell spacing
c ::: xlo          => physical location of lower left hand
c :::	              corner of q array
c ::: bc	   => array of boundary flags bc(SPACEDIM,lo:hi)
c ::: 
c ::: NOTE: all corner as well as edge data is filled if not EXT_DIR
c ::: -----------------------------------------------------------

      subroutine filcc(q,DIMS(q),domlo,domhi,dx,xlo,bc)

      implicit none

      integer    DIMDEC(q)
      integer    domlo(SDIM), domhi(SDIM)
      integer    bc(SDIM,2)
      REAL_T     xlo(SDIM), dx(SDIM)
      REAL_T     q(DIMV(q))

      integer    nlft, nrgt, nbot, ntop
      integer    ilo, ihi, jlo, jhi
      integer    i, j
      integer    is, ie, js, je

      nlft = max(0,domlo(1)-ARG_L1(q))
      nrgt = max(0,ARG_H1(q)-domhi(1))
      nbot = max(0,domlo(2)-ARG_L2(q))
      ntop = max(0,ARG_H2(q)-domhi(2))

      is = max(ARG_L1(q),domlo(1))
      ie = min(ARG_H1(q),domhi(1))
      js = max(ARG_L2(q),domlo(2))
      je = min(ARG_H2(q),domhi(2))

c     ::::: first fill sides
      if (nlft .gt. 0) then
         ilo = domlo(1)

         if (bc(1,1) .eq. EXT_DIR) then
c     set all ghost cell values to a prescribed dirichlet
c     value; in this example, we have chosen 1
c	    do i = 1, nlft
c	    do j = ARG_L2(q), ARG_H2(q)
c	       q(ilo-i,j) = 1.d0
c	    end do
c	    end do
         else if (bc(1,1) .eq. FOEXTRAP) then
	    do i = 1, nlft
	    do j = ARG_L2(q), ARG_H2(q)
	       q(ilo-i,j) = q(ilo,j)
	    end do
	    end do
	 else if (bc(1,1) .eq. HOEXTRAP) then
	    do i = 2, nlft
	    do j = ARG_L2(q), ARG_H2(q)
	       q(ilo-i,j) = q(ilo,j) 
	    end do 
	    end do 
            if (ilo+2 .le. ie) then 
	     do j = ARG_L2(q), ARG_H2(q)
		q(ilo-1,j) = (15*q(ilo,j) - 10*q(ilo+1,j) + 
     $                        3*q(ilo+2,j)) * eighth
	     end do 
            else 
	     do j = ARG_L2(q), ARG_H2(q)
	       q(ilo-1,j) = half*(3*q(ilo,j) - q(ilo+1,j))
	     end do
            end if
	 else if (bc(1,1) .eq. REFLECT_EVEN) then
	    do i = 1, nlft
	     do j = ARG_L2(q), ARG_H2(q)
	       q(ilo-i,j) = q(ilo+i-1,j)
	    end do
	    end do
	 else if (bc(1,1) .eq. REFLECT_ODD) then
	    do i = 1, nlft
	    do j = ARG_L2(q), ARG_H2(q)
	       q(ilo-i,j) = -q(ilo+i-1,j)
	    end do
	    end do
	 end if
      end if

      if (nrgt .gt. 0) then
         ihi = domhi(1)

	 if (bc(1,2) .eq. EXT_DIR) then
c	    do i = 1, nrgt
c	    do j = ARG_L2(q), ARG_H2(q)
c	       q(ihi+i,j) = 1.d0
c	    end do
c	    end do
	 else if (bc(1,2) .eq. FOEXTRAP) then
	    do i = 1, nrgt
	    do j = ARG_L2(q), ARG_H2(q)
	       q(ihi+i,j) = q(ihi,j)
	    end do
	    end do
         else if (bc(1,2) .eq. HOEXTRAP) then
            do i = 2, nrgt
	    do j = ARG_L2(q), ARG_H2(q)
               q(ihi+i,j) = q(ihi,j)
            end do
            end do
            if (ihi-2 .ge. is) then
	     do j = ARG_L2(q), ARG_H2(q)
	       q(ihi+1,j) = (15*q(ihi,j) - 10*q(ihi-1,j) + 
     $                        3*q(ihi-2,j)) * eighth
             end do
            else
	     do j = ARG_L2(q), ARG_H2(q)
	       q(ihi+1,j) = half*(3*q(ihi,j) - q(ihi-1,j))
             end do
            end if
	 else if (bc(1,2) .eq. REFLECT_EVEN) then
	    do i = 1, nrgt
            do j = ARG_L2(q), ARG_H2(q)
	       q(ihi+i,j) = q(ihi-i+1,j)
	    end do
	    end do
	 else if (bc(1,2) .eq. REFLECT_ODD) then
	    do i = 1, nrgt
            do j = ARG_L2(q), ARG_H2(q)
	       q(ihi+i,j) = -q(ihi-i+1,j)
	    end do
	    end do
	 end if
      end if

      if (nbot .gt. 0) then
         jlo = domlo(2)

	 if (bc(2,1) .eq. EXT_DIR) then
c	    do j = 1, nbot
c	    do i = ARG_L1(q), ARG_H1(q)
c	       q(i,jlo-j) = 1.d0
c	    end do
c	    end do
	 else if (bc(2,1) .eq. FOEXTRAP) then
	    do j = 1, nbot
	    do i = ARG_L1(q), ARG_H1(q)
	       q(i,jlo-j) = q(i,jlo)
	    end do
	    end do
         else if (bc(2,1) .eq. HOEXTRAP) then
            do j = 2, nbot
	    do i = ARG_L1(q), ARG_H1(q)
               q(i,jlo-j) = q(i,jlo)
            end do
            end do
            if (jlo+2 .le. je) then
 	     do i = ARG_L1(q), ARG_H1(q)
               q(i,jlo-1) = (15*q(i,jlo) - 10*q(i,jlo+1) + 
     $                        3*q(i,jlo+2)) * eighth
             end do
            else
 	     do i = ARG_L1(q), ARG_H1(q)
               q(i,jlo-1) = half*(3*q(i,jlo) - q(i,jlo+1))
             end do
            end if
	 else if (bc(2,1) .eq. REFLECT_EVEN) then
	    do j = 1, nbot
 	    do i = ARG_L1(q), ARG_H1(q)
	       q(i,jlo-j) = q(i,jlo+j-1)
	    end do
	    end do
	 else if (bc(2,1) .eq. REFLECT_ODD) then
	    do j = 1, nbot
 	    do i = ARG_L1(q), ARG_H1(q)
	       q(i,jlo-j) = -q(i,jlo+j-1)
	    end do
	    end do
	 end if
      end if

      if (ntop .gt. 0) then
         jhi = domhi(2)

	 if (bc(2,2) .eq. EXT_DIR) then
c	    do j = 1, ntop
c 	    do i = ARG_L1(q), ARG_H1(q)
c	       q(i,jhi+j) = 1.d0
c	    end do
c	    end do
	 else if (bc(2,2) .eq. FOEXTRAP) then
	    do j = 1, ntop
 	    do i = ARG_L1(q), ARG_H1(q)
	       q(i,jhi+j) = q(i,jhi)
	    end do
	    end do
         else if (bc(2,2) .eq. HOEXTRAP) then
            do j = 2, ntop
 	    do i = ARG_L1(q), ARG_H1(q)
	       q(i,jhi+j) = q(i,jhi)
            end do
            end do
            if (jhi-2 .ge. js) then
 	    do i = ARG_L1(q), ARG_H1(q)
               q(i,jhi+1) = (15*q(i,jhi) - 10*q(i,jhi-1) + 
     $                         3*q(i,jhi-2)) * eighth
             end do
            else
 	     do i = ARG_L1(q), ARG_H1(q)
               q(i,jhi+1) = half*(3*q(i,jhi) - q(i,jhi-1))
             end do
            end if
	 else if (bc(2,2) .eq. REFLECT_EVEN) then
	    do j = 1, ntop
 	    do i = ARG_L1(q), ARG_H1(q)
	       q(i,jhi+j) = q(i,jhi-j+1)
	    end do
	    end do
	 else if (bc(2,2) .eq. REFLECT_ODD) then
	    do j = 1, ntop
 	    do i = ARG_L1(q), ARG_H1(q)
	       q(i,jhi+j) = -q(i,jhi-j+1)
	    end do
	    end do
	 end if
      end if

      if ((nlft .gt. 0 .and. bc(1,1) .eq. HOEXTRAP) .and.
     $    (nbot .gt. 0 .and. bc(2,1) .eq. HOEXTRAP) ) then
      
        if (jlo+2 .le. je) then 
          q(ilo-1,jlo-1) = half * eighth * 
     $    (15*q(ilo-1,jlo) - 10*q(ilo-1,jlo+1) + 3*q(ilo-1,jlo+2))
        else
          q(ilo-1,jlo-1) = half * half * 
     $    (3*q(ilo-1,jlo) - q(ilo-1,jlo+1))
        end if

        if (ilo+2 .le. ie) then 
          q(ilo-1,jlo-1) =  q(ilo-1,jlo-1) + half * eighth * 
     $    (15*q(ilo,jlo-1) - 10*q(ilo+1,jlo-1) + 3*q(ilo+2,jlo-1)) 
        else
          q(ilo-1,jlo-1) =  q(ilo-1,jlo-1) + half * half * 
     $    (3*q(ilo,jlo-1) - q(ilo+1,jlo-1))
        end if


      end if

      if ((nlft .gt. 0 .and. bc(1,1) .eq. HOEXTRAP) .and.
     $    (ntop .gt. 0 .and. bc(2,2) .eq. HOEXTRAP) ) then

        if (jhi-2 .ge. js) then 
          q(ilo-1,jhi+1) = half * eighth * 
     $    (15*q(ilo-1,jhi) - 10*q(ilo-1,jhi-1) + 3*q(ilo-1,jhi-2))
        else
          q(ilo-1,jhi+1) = half * half * 
     $    (3*q(ilo-1,jhi) - q(ilo-1,jhi-1))
        end if

        if (ilo+2 .le. ie) then 
          q(ilo-1,jhi+1) = q(ilo-1,jhi+1) + half * eighth * 
     $    (15*q(ilo,jhi+1) - 10*q(ilo+1,jhi+1) + 3*q(ilo+2,jhi+1))
        else
          q(ilo-1,jhi+1) = q(ilo-1,jhi+1) + half * half * 
     $    (3*q(ilo,jhi+1) - q(ilo+1,jhi+1))
        end if
      end if

      if ((nrgt .gt. 0 .and. bc(1,2) .eq. HOEXTRAP) .and.
     $    (nbot .gt. 0 .and. bc(2,1) .eq. HOEXTRAP) ) then
        if (jlo+2 .le. je) then 
          q(ihi+1,jlo-1) = half * eighth * 
     $    (15*q(ihi+1,jlo) - 10*q(ihi+1,jlo+1) + 3*q(ihi+1,jlo+2))
        else
          q(ihi+1,jlo-1) = half * half * 
     $    (3*q(ihi+1,jlo) - q(ihi+1,jlo+1))
        end if

        if (ihi-2 .ge. is) then 
          q(ihi+1,jlo-1) = q(ihi+1,jlo-1) + half * eighth * 
     $    (15*q(ihi,jlo-1) - 10*q(ihi-1,jlo-1) + 3*q(ihi-2,jlo-1))
        else
          q(ihi+1,jlo-1) = q(ihi+1,jlo-1) + half * half * 
     $    (3*q(ihi,jlo-1) - q(ihi-1,jlo-1))
        end if
      end if

      if ((nrgt .gt. 0 .and. bc(1,2) .eq. HOEXTRAP) .and.
     $    (ntop .gt. 0 .and. bc(2,2) .eq. HOEXTRAP) ) then

        if (jhi-2 .ge. js) then 
          q(ihi+1,jhi+1) = half * eighth * 
     $    (15*q(ihi+1,jhi) - 10*q(ihi+1,jhi-1) + 3*q(ihi+1,jhi-2))
        else
          q(ihi+1,jhi+1) = half * half * 
     $    (3*q(ihi+1,jhi) - q(ihi+1,jhi-1))
        end if

        if (ihi-2 .ge. is) then 
          q(ihi+1,jhi+1) = q(ihi+1,jhi+1) + half * eighth * 
     $    (15*q(ihi,jhi+1) - 10*q(ihi-1,jhi+1) + 3*q(ihi-2,jhi+1))
        else
          q(ihi+1,jhi+1) = q(ihi+1,jhi+1) + half * half * 
     $    (3*q(ihi,jhi+1) - q(ihi-1,jhi+1))
        end if

      end if

      return
      end

      subroutine hoextraptocc(q,DIMS(q),domlo,domhi,dx,xlo)

      implicit none

      integer    DIMDEC(q)
      integer    domlo(SDIM), domhi(SDIM)
      REAL_T     xlo(SDIM), dx(SDIM)
      REAL_T     q(DIMV(q))

      integer    nlft, nrgt, nbot, ntop
      integer    ilo, ihi, jlo, jhi
      integer    i, j
      integer    is, ie, js, je

      nlft = max(0,domlo(1)-ARG_L1(q))
      nrgt = max(0,ARG_H1(q)-domhi(1))
      nbot = max(0,domlo(2)-ARG_L2(q))
      ntop = max(0,ARG_H2(q)-domhi(2))

      is = max(ARG_L1(q),domlo(1))
      ie = min(ARG_H1(q),domhi(1))
      js = max(ARG_L2(q),domlo(2))
      je = min(ARG_H2(q),domhi(2))
      
c
c     Set these to invalid values, they shouldn't be used if not reset
c
      ilo = -10
      jlo = -10
      ihi = 100000000
      jhi = 100000000

c
c     First fill sides.
c
      if (nlft .gt. 0) then
         ilo = domlo(1)
         do i = 2, nlft
	    do j = ARG_L2(q), ARG_H2(q)
	       q(ilo-i,j) = q(ilo,j) 
	    end do 
         end do 
         if (ilo+2 .le. ie) then 
            do j = ARG_L2(q), ARG_H2(q)
               q(ilo-1,j) = 3*q(ilo,j) - 3*q(ilo+1,j) + q(ilo+2,j)
            end do 
         else 
            do j = ARG_L2(q), ARG_H2(q)
	       q(ilo-1,j) = 2*q(ilo,j) - q(ilo+1,j)
            end do
         end if
      end if

      if (nrgt .gt. 0) then
         ihi = domhi(1)
         do i = 2, nrgt
	    do j = ARG_L2(q), ARG_H2(q)
               q(ihi+i,j) = q(ihi,j)
            end do
         end do
         if (ihi-2 .ge. is) then
            do j = ARG_L2(q), ARG_H2(q)
               q(ihi+1,j) = 3*q(ihi,j) - 3*q(ihi-1,j) + q(ihi-2,j)
            end do
         else
            do j = ARG_L2(q), ARG_H2(q)
               q(ihi+1,j) = 2*q(ihi,j) - q(ihi-1,j)
            end do
         end if
      end if

      if (nbot .gt. 0) then
         jlo = domlo(2)
         do j = 2, nbot
	    do i = ARG_L1(q), ARG_H1(q)
               q(i,jlo-j) = q(i,jlo)
            end do
         end do
         if (jlo+2 .le. je) then
            do i = ARG_L1(q), ARG_H1(q)
               q(i,jlo-1) = 3*q(i,jlo) - 3*q(i,jlo+1) + q(i,jlo+2)
            end do
         else
            do i = ARG_L1(q), ARG_H1(q)
               q(i,jlo-1) = 2*q(i,jlo) - q(i,jlo+1)
            end do
         end if
      end if

      if (ntop .gt. 0) then
         jhi = domhi(2)
         do j = 2, ntop
 	    do i = ARG_L1(q), ARG_H1(q)
	       q(i,jhi+j) = q(i,jhi)
            end do
         end do
         if (jhi-2 .ge. js) then
 	    do i = ARG_L1(q), ARG_H1(q)
               q(i,jhi+1) = 3*q(i,jhi) - 3*q(i,jhi-1) + q(i,jhi-2)
            end do
         else
            do i = ARG_L1(q), ARG_H1(q)
               q(i,jhi+1) = 2*q(i,jhi) - q(i,jhi-1)
            end do
         end if
      end if

      if ((nlft .gt. 0) .and. (nbot .gt. 0)) then
          if (jlo+2 .le. je) then
             q(ilo-1,jlo-1) = half *
     $      (3*q(ilo-1,jlo) - 3*q(ilo-1,jlo+1) + q(ilo-1,jlo+2))
          else
             q(ilo-1,jlo-1) = half * (2*q(ilo-1,jlo) - q(ilo-1,jlo+1))
          end if
          
          if (ilo+2 .le. ie) then 
             q(ilo-1,jlo-1) =  q(ilo-1,jlo-1) + half *
     $      (3*q(ilo,jlo-1) - 3*q(ilo+1,jlo-1) + q(ilo+2,jlo-1)) 
          else
             q(ilo-1,jlo-1) =  q(ilo-1,jlo-1) + half *
     $        (2*q(ilo,jlo-1) - q(ilo+1,jlo-1))
          end if
      end if

      if ((nlft .gt. 0) .and. (ntop .gt. 0)) then 
          if (jhi-2 .ge. js) then 
             q(ilo-1,jhi+1) = half *
     $      (3*q(ilo-1,jhi) - 3*q(ilo-1,jhi-1) + q(ilo-1,jhi-2))
          else
             q(ilo-1,jhi+1) = half * (2*q(ilo-1,jhi) - q(ilo-1,jhi-1))
          end if
          
          if (ilo+2 .le. ie) then 
             q(ilo-1,jhi+1) = q(ilo-1,jhi+1) + half *
     $      (3*q(ilo,jhi+1) - 3*q(ilo+1,jhi+1) + q(ilo+2,jhi+1))
          else
             q(ilo-1,jhi+1) = q(ilo-1,jhi+1) + half *
     $        (2*q(ilo,jhi+1) - q(ilo+1,jhi+1))
          end if
      end if

      if ((nrgt .gt. 0) .and. (nbot .gt. 0)) then 
          if (jlo+2 .le. je) then 
             q(ihi+1,jlo-1) = half *
     $      (3*q(ihi+1,jlo) - 3*q(ihi+1,jlo+1) + q(ihi+1,jlo+2))
          else
             q(ihi+1,jlo-1) = half * (2*q(ihi+1,jlo) - q(ihi+1,jlo+1))
          end if
          
          if (ihi-2 .ge. is) then 
             q(ihi+1,jlo-1) = q(ihi+1,jlo-1) + half *
     $      (3*q(ihi,jlo-1) - 3*q(ihi-1,jlo-1) + q(ihi-2,jlo-1))
          else
             q(ihi+1,jlo-1) = q(ihi+1,jlo-1) + half *
     $        (2*q(ihi,jlo-1) - q(ihi-1,jlo-1))
          end if
      end if

      if ((nrgt .gt. 0) .and. (ntop .gt. 0)) then 
          if (jhi-2 .ge. js) then 
             q(ihi+1,jhi+1) = half *
     $      (3*q(ihi+1,jhi) - 3*q(ihi+1,jhi-1) + q(ihi+1,jhi-2))
          else
             q(ihi+1,jhi+1) = half * (2*q(ihi+1,jhi) - q(ihi+1,jhi-1))
          end if
          
          if (ihi-2 .ge. is) then 
             q(ihi+1,jhi+1) = q(ihi+1,jhi+1) + half *
     $      (3*q(ihi,jhi+1) - 3*q(ihi-1,jhi+1) + q(ihi-2,jhi+1))
          else
             q(ihi+1,jhi+1) = q(ihi+1,jhi+1) + half *
     $        (2*q(ihi,jhi+1) - q(ihi-1,jhi+1))
          end if
      end if
      
      end
