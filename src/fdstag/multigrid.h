//---------------------------------------------------------------------------
//..................   GALERKIN GEOMETRIC MULTIGRID   .......................
//---------------------------------------------------------------------------
#ifndef __multigrid_h__
#define __multigrid_h__
//---------------------------------------------------------------------------

// Galerkin multigrid level data structure

typedef struct
{
	// Constrained DOF stores parent DOF index in the boundary condition vector.
	// Parent DOF index is the only nonzero that is set in the row of R-matrix
	// and column of P-matrix to impose the constraints in a coarse grid operator
	// automatically. The finest grid uses standard boundary condition vectors.

	DM        DA_CEN;                // central points array
	DM        DA_X, DA_Y, DA_Z;      // face points arrays
	DOFIndex  dof;                   // indexing vectors
	Vec       bcvx, bcvy, bcvz, bcp; // restricted boundary condition vectors
	Mat       R, P;                  // restriction & prolongation operators (not set on finest grid)

	// ******** fine level ************
	//     |                   ^
	//     R-matrix            |
	//     |                   P-matrix
	//     v                   |
	// ******** this level ************

} MGLevel;

//---------------------------------------------------------------------------

PetscErrorCode MGLevelCreate(MGLevel *lvl, MGLevel *fine, FDSTAG *fs, BCCtx *bc);

PetscErrorCode MGLevelDestroy(MGLevel *lvl);

PetscErrorCode MGLevelRestrictBC(MGLevel *lvl, MGLevel *fine);

PetscErrorCode MGLevelSetupRestrict(MGLevel *lvl, MGLevel *fine);

PetscErrorCode MGLevelSetupProlong(MGLevel *lvl, MGLevel *fine);

PetscErrorCode MGLevelAllocRestrict(MGLevel *lvl, MGLevel *fine);

PetscErrorCode MGLevelAllocProlong(MGLevel *lvl, MGLevel *fine);

//---------------------------------------------------------------------------

// setup row of restriction matrix
void getRowRestrict(PetscScalar parent, PetscInt n, PetscInt idx[], PetscScalar bc[], PetscScalar v[], PetscScalar vs[]);

// setup row of prolongation matrix
void getRowProlong(PetscInt parent, PetscScalar pbc, PetscInt n, PetscScalar bc[], PetscScalar v[], PetscScalar vs[]);

//---------------------------------------------------------------------------

typedef struct
{
	// PETSc level numbering (inverse w.r.t. coarsening sequence):
	// 0   - coarse grid
	// n-1 - fine grid
	// R & P matrices connect with coarser level (i.e. not set on coarsest grid).
	// Coarsening step yields coarse grid operator. Own operator is prescribed.

	// LaMEM level numbering (natural w.r.t. coarsening sequence):
	// 0   - fine grid
	// n-1 - coarse grid
	// R & P matrices connect with finer level (i.e. not set on finest grid).
	// Coarsening step yields own operator. Fine level operator is prescribed.

	PetscInt  nlvl; // number of levels
	MGLevel  *lvls; // multigrid levles

	PC        pc;   // internal preconditioner context

	FDSTAG   *fs;   // finest level grid
	BCCtx    *bc;   // finest level boundary conditions

} MG;

//---------------------------------------------------------------------------

PetscErrorCode MGCreate(MG *mg, FDSTAG *fs, BCCtx *bc);

PetscErrorCode MGDestroy(MG *mg);

PetscErrorCode MGSetup(MG *mg, Mat A);

PetscErrorCode MGApply(PC pc, Vec x, Vec y);

PetscErrorCode MGDumpMat(MG *mg);

PetscErrorCode MGGetNumLevels(MG *mg);

//---------------------------------------------------------------------------
#endif
