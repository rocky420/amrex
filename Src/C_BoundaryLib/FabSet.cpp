//BL_COPYRIGHT_NOTICE

//
// $Id: FabSet.cpp,v 1.17 1998-05-28 21:33:45 lijewski Exp $
//

#include <FabSet.H>
#include <Looping.H>

FabSet::FabSet () {}

FabSet::~FabSet () {}

FabSet::FabSet (int _len)
{
    fabparray.resize(_len);
}

void
FabSet::setFab (int        boxno,
                FArrayBox* fab)
{
    if (n_comp == 0)
        n_comp = fab->nComp();

    assert(n_comp == fab->nComp());
    assert(boxarray.ready());
    assert(!fabparray.defined(boxno));
    assert(distributionMap.ProcessorMap()[boxno] == ParallelDescriptor::MyProc());

    fabparray.set(boxno, fab);
    fabboxarray.set(boxno, fab->box());
}

FabSet&
FabSet::copyFrom (const FArrayBox& src)
{
    for (FabSetIterator fsi(*this); fsi.isValid(false); ++fsi)
    {
        fsi().copy(src);
    }
    return *this;
}

FabSet&
FabSet::copyFrom (const FArrayBox& src,
                  int              src_comp,
                  int              dest_comp,
                  int              num_comp)
{
    for (FabSetIterator fsi(*this); fsi.isValid(false); ++fsi)
    {
        fsi().copy(src,src_comp,dest_comp,num_comp);
    }
    return *this;
}

FabSet&
FabSet::copyFrom (const FArrayBox& src,
                  const Box&       subbox,
                  int              src_comp,
                  int              dest_comp,
                  int              num_comp)
{
    assert(src.box().contains(subbox));

    for (FabSetIterator fsi(*this); fsi.isValid(false); ++fsi)
    {
        if (subbox.intersects(fsi().box()))
        {
            Box dbox = fsi().box() & subbox;

            fsi().copy(src,dbox,src_comp,dbox,dest_comp,num_comp);
        }
    }
    return *this;
}

void
FabSet::copyTo (MultiFab& dest) const
{
    BoxArray tmp = boxarray;
    boxarray = fabboxarray;
    dest.copy(*this);
    boxarray = tmp;
}

void
FabSet::copyTo (MultiFab& dest,
                int       nghost,
                int       src_comp,
                int       dest_comp,
                int       num_comp) const
{
    BoxArray tmp = boxarray;
    boxarray = fabboxarray;
    dest.copy(*this, src_comp, dest_comp, num_comp, nghost);
    boxarray = tmp;
}

FabSet&
FabSet::copyFrom (const MultiFab& src,
                  int             nghost,
                  int             src_comp,
                  int             dest_comp,
                  int             num_comp)
{
    assert(nghost <= src.nGrow());
    BoxArray tmp = boxarray;
    boxarray = fabboxarray;
    this->copy(src,src_comp,dest_comp,num_comp,nghost);
    boxarray = tmp;

    return *this;
}

FabSet&
FabSet::plusFrom (const MultiFab& src,
                  int             nghost,
                  int             src_comp,
                  int             dest_comp,
                  int             num_comp)
{
    assert(nghost <= src.nGrow());

    MultiFabCopyDescriptor mfcd;
    MultiFabId             mfidsrc = mfcd.RegisterFabArray((MultiFab*) &src);
    vector<FillBoxId>      fillBoxIdList;
    FArrayBox              tmpfab;

    for (FabSetIterator thismfi(*this); thismfi.isValid(false); ++thismfi)
    {
        for (int isrc = 0; isrc < src.length(); ++isrc)
        {
            Box ovlp = thismfi().box() & ::grow(src.boxArray()[isrc], nghost);

            if (ovlp.ok())
            {
                fillBoxIdList.push_back(mfcd.AddBox(mfidsrc,
                                                    ovlp,
                                                    0,
                                                    isrc,
                                                    src_comp,
                                                    dest_comp,
                                                    num_comp,
                                                    false));
            }
        }
    }

    mfcd.CollectData();

    vector<FillBoxId>::const_iterator fbidli = fillBoxIdList.begin();

    for (FabSetIterator thismfi(*this); thismfi.isValid(false); ++thismfi)
    {
        for (int isrc = 0; isrc < src.length(); ++isrc)
        {
            Box ovlp = thismfi().box() & ::grow(src.boxArray()[isrc], nghost);

            if (ovlp.ok())
            {
                assert(!(fbidli == fillBoxIdList.end()));
                const FillBoxId& fbidsrc = *fbidli++;
                assert(fbidsrc.box() == ovlp);
                tmpfab.resize(fbidsrc.box(), num_comp);
                mfcd.FillFab(mfidsrc, fbidsrc, tmpfab);
                thismfi().plus(tmpfab,ovlp,src_comp,dest_comp,num_comp);
            }
        }
    }

    return *this;
}

//
// Linear combination this := a*this + b*src
// Note: corresponding fabsets must be commensurate.
//
FabSet&
FabSet::linComb (Real          a,
                 Real          b,
                 const FabSet& src,
                 int           src_comp,
                 int           dest_comp,
                 int           num_comp)
{
    assert(length() == src.length());

    for (FabSetIterator fsi(*this); fsi.isValid(false); ++fsi)
    {
        DependentFabSetIterator dfsi(fsi, src);

        assert(fsi().box() == dfsi().box());
        //
        // WARNING: same fab used as src and dest here.
        //
        fsi().linComb(fsi(),
                      fsi().box(),
                      dest_comp,
                      dfsi(),
                      dfsi().box(),
                      src_comp,
                      a,
                      b,
                      fsi().box(),
                      dest_comp,
                      num_comp);
    }
    return *this;
}

FabSet&
FabSet::linComb (Real            a,
                 const MultiFab& mfa,
                 int             a_comp,
                 Real            b,
                 const MultiFab& mfb,
                 int             b_comp,
                 int             dest_comp,
                 int             num_comp,
                 int             n_ghost)
{
    const BoxArray& bxa = mfa.boxArray();
    const BoxArray& bxb = mfb.boxArray();

    assert(bxa == bxb);
    assert(n_ghost <= mfa.nGrow());
    assert(n_ghost <= mfb.nGrow());

    MultiFabCopyDescriptor mfcd;

    MultiFabId mfid_mfa = mfcd.RegisterFabArray((MultiFab*) &mfa);
    MultiFabId mfid_mfb = mfcd.RegisterFabArray((MultiFab*) &mfb);

    vector<FillBoxId> fillBoxIdList_mfa, fillBoxIdList_mfb;

    for (FabSetIterator thismfi(*this); thismfi.isValid(false); ++thismfi)
    {
        for (int grd = 0; grd < bxa.length(); grd++)
        {
            Box ovlp = thismfi().box() & ::grow(bxa[grd],n_ghost);

            if (ovlp.ok())
            {
                fillBoxIdList_mfa.push_back(mfcd.AddBox(mfid_mfa,
                                                        ovlp,
                                                        0,
                                                        grd,
                                                        a_comp,
                                                        0,
                                                        num_comp,
                                                        false));
                fillBoxIdList_mfb.push_back(mfcd.AddBox(mfid_mfb,
                                                        ovlp,
                                                        0,
                                                        grd,
                                                        b_comp,
                                                        0,
                                                        num_comp,
                                                        false));
            }
        }
    }

    mfcd.CollectData();

    FArrayBox a_fab, b_fab;

    vector<FillBoxId>::const_iterator fbidli_mfa = fillBoxIdList_mfa.begin();
    vector<FillBoxId>::const_iterator fbidli_mfb = fillBoxIdList_mfb.begin();

    for (FabSetIterator thismfi(*this); thismfi.isValid(false); ++thismfi)
    {
        for (int grd = 0; grd < bxa.length(); grd++)
        {
            Box ovlp = thismfi().box() & ::grow(bxa[grd],n_ghost);

            if (ovlp.ok())
            {
                assert(!(fbidli_mfa == fillBoxIdList_mfa.end()));
                const FillBoxId& fbid_mfa = *fbidli_mfa++;
                assert(fbid_mfa.box() == ovlp);
                a_fab.resize(fbid_mfa.box(), num_comp);
                mfcd.FillFab(mfid_mfa, fbid_mfa, a_fab);

                assert(!(fbidli_mfb == fillBoxIdList_mfb.end()));
                const FillBoxId& fbid_mfb = *fbidli_mfb++;
                assert(fbid_mfb.box() == ovlp);
                b_fab.resize(fbid_mfb.box(), num_comp);
                mfcd.FillFab(mfid_mfb, fbid_mfb, b_fab);

                thismfi().linComb(a_fab,
                                  ovlp,
                                  0,
                                  b_fab,
                                  ovlp,
                                  0,
                                  a,
                                  b,
                                  ovlp,
                                  dest_comp,
                                  num_comp);
            }
        }
    }

    return *this;
}
