/*******************************************************************************
*
*  model_Ar_Morse_c
*
*  Morse pair potential model for argon 
*  (Parameters taken from: G. E. Jelinek, Phys. Rev. B, Vol 5, no 8, 1972, pp3210--3217)
*  modified to have smooth cutoff
*
*  Author: Ryan S. Elliott, Ellad B. Tadmor
*
*  Copyright 2011 Ellad B. Tadmor, Ryan S. Elliott, and James P. Sethna
*  All rights reserved.
*
*******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "KIMserviceC.h"

#define DIM 3
#define Ar  1

/* Define prototypes for model init */
void model_ar_morse_c_init_(void* km);

/* Define prototypes for model reinit, compute, and destroy */
static void reinit(void* km);
static void destroy(void* km);
static void compute(void* km, int* ier);


static void pair(double* epsilon, double* C, double* Rzero,
                 double* A1, double* A2, double* A3,
                 double R, double* phi, double* dphi, double* d2phi)
{
   /* local variables */
   double rsq;
   double ep;
   double ep2;

   ep  = exp(-(*C)*(R-*Rzero));
   ep2 = ep*ep;

   *phi   = (*epsilon)*( -ep2 + 2.0*ep ) + (*A1)*(R*R) + (*A2)*R + (*A3);
   *dphi  = 2.0*(*epsilon)*(*C)*( -ep + ep2 ) + 2.0*(*A1)*R + (*A2);
   *d2phi = 2.0*(*epsilon)*(*C)*(*C)*( -2.0*ep2 + ep ) + 2.0*(*A1);

   return;
}

/* compute function */
static void compute(void* km, int* ier)
{
   /* local variables */
   intptr_t* pkim = *((intptr_t**) km);
   double R;
   double Rsqij;
   double phi;
   double dphi;
   double d2phi;
   double dx[DIM];
   int i;
   int j;
   int jj;
   int k;
   int numOfAtomNeigh;
   int requestedAtom;
   int currentAtom;
   int comp_force;
   int comp_energyPerAtom;
   int comp_virial;
   int IterOrLoca;
   int NBC;
   char* NBCstr;

   intptr_t* nAtoms;
   int* nAtomTypes;
   int* atomTypes;
   double* cutoff;
   double* epsilon;
   double* C;
   double* Rzero;
   double* A1;
   double* A2;
   double* A3;
   double* cutsq;
   double* Rij;
   double* coords;
   double* energy;
   double* force;
   double* energyPerAtom;
   double* virial;
   int* neighListOfCurrentAtom;
   double* boxlength;
   
   
   /* determine NBC */
   /* NBC = even -> Full, NBC = odd -> Half */
   NBCstr = KIM_API_get_NBC_method(pkim, ier); if (1 > *ier) return;
   if (!strcmp("CLUSTER",NBCstr))
      NBC = 0;
   else if (!strcmp("MI-OPBC-H",NBCstr))
      NBC = 1;
   else if (!strcmp("MI-OPBC-F",NBCstr))
      NBC = 2;
   else if (!strcmp("NEIGH-PURE-H",NBCstr))
      NBC = 3;
   else if (!strcmp("NEIGH-PURE-F",NBCstr))
      NBC = 4;
   else if (!strcmp("NEIGH-RVEC-F",NBCstr))
      NBC = 6;
   else
   {
      printf("* ERROR: unknown NBC type.\n");
      *ier = 0;
      return;
   }
   free(NBCstr);

   /* determine mode */
   if (NBC > 0)
   {
      IterOrLoca = 0; /* Iterator mode */
      /* use Iterator mode until a new KIM_API call is available to provide the correct value */
   }


   /* unpack data from KIM object */
   nAtoms     = (intptr_t*) KIM_API_get_data(pkim, "numberOfAtoms",       ier); if (1 > *ier) return;
   nAtomTypes = (int*)      KIM_API_get_data(pkim, "numberAtomTypes",     ier); if (1 > *ier) return;
   atomTypes  = (int*)      KIM_API_get_data(pkim, "atomTypes",           ier); if (1 > *ier) return;
   cutoff     = (double*)   KIM_API_get_data(pkim, "cutoff",              ier); if (1 > *ier) return;
   epsilon    = (double*)   KIM_API_get_data(pkim, "PARAM_FREE_epsilon",  ier); if (1 > *ier) return;
   C          = (double*)   KIM_API_get_data(pkim, "PARAM_FREE_C",        ier); if (1 > *ier) return;
   Rzero      = (double*)   KIM_API_get_data(pkim, "PARAM_FREE_Rzero",    ier); if (1 > *ier) return;
   A1         = (double*)   KIM_API_get_data(pkim, "PARAM_FIXED_A1",      ier); if (1 > *ier) return;
   A2         = (double*)   KIM_API_get_data(pkim, "PARAM_FIXED_A2",      ier); if (1 > *ier) return;
   A3         = (double*)   KIM_API_get_data(pkim, "PARAM_FIXED_A3",      ier); if (1 > *ier) return;
   cutsq      = (double*)   KIM_API_get_data(pkim, "PARAM_FIXED_cutsq",   ier); if (1 > *ier) return;
   energy     = (double*)   KIM_API_get_data(pkim, "energy",              ier); if (1 > *ier) return;
   coords     = (double*)   KIM_API_get_data(pkim, "coordinates",         ier); if (1 > *ier) return;
   if ((NBC == 1) || (NBC == 2))
   {
      boxlength = (double*) KIM_API_get_data(pkim, "boxlength",           ier); if (1 > *ier) return;
   }

   /* check to see if we have been asked to compute the forces, energyPerAtom, and virial */
   comp_force         = KIM_API_isit_compute(pkim, "forces",        ier); if (1 > *ier) return;
   comp_energyPerAtom = KIM_API_isit_compute(pkim, "energyPerAtom", ier); if (1 > *ier) return;
   comp_virial        = KIM_API_isit_compute(pkim, "virial",        ier); if (1 > *ier) return;

   
   /* check to be sure the atom types are correct */
   *ier = 0; /* assume an error */
   for (i = 0; i < *nAtoms; ++i)
   {
      if (Ar != atomTypes[i]) return;
   }
   *ier = 1; /* everything is ok */

   /* initialize potential energies, forces, and virial term */
   if (comp_energyPerAtom)
   {
      energyPerAtom = (double*) KIM_API_get_data(pkim, "energyPerAtom", ier); if (1 > *ier) return;
      for (i = 0; i < *nAtoms; ++i)
      {
         energyPerAtom[i] = 0.0;
      }
   }
   else
   {
      *energy = 0.0;
   }

   if (comp_force)
   {
      force = (double*) KIM_API_get_data(pkim, "forces", ier); if (1 > *ier) return;
      for (i = 0; i < *nAtoms; ++i)
      {
         for (k = 0; k < DIM; ++k)
         {
            force[i*DIM + k] = 0.0;
         }
      }
   }

   if (comp_virial)
   {
      virial = (double*) KIM_API_get_data(pkim, "virial", ier); if (1 > *ier) return;
      *virial = 0.0;
   }

   /* Ready to setup for and perform the computation */

   /* branch based on NBC */
   if (0 == NBC) /* CLUSTER */
   {
      /* Loop over all pairs of atoms */
      for (i = 0; i < *nAtoms; ++i)
      {
         for (j = i+1; j < *nAtoms; ++j) /* use a half-neighbor approach */
         {
            Rsqij = 0.0;
            for (k = 0; k < DIM; ++k)
            {
               Rsqij += (coords[i*DIM + k] - coords[j*DIM + k])*(coords[i*DIM + k] - coords[j*DIM + k]);
            }

            if (Rsqij < *cutsq)
            {
               R = sqrt(Rsqij);
               /* compute pair potential */
               pair(epsilon, C, Rzero, A1, A2, A3, R, &phi, &dphi, &d2phi);

               /* accumlate energy */
               if (comp_energyPerAtom)
               {
                  energyPerAtom[i] += 0.5*phi;
                  energyPerAtom[j] += 0.5*phi;
               }
               else
               {
                  *energy += phi;
               }

               /* accumulate virial */
               if (comp_virial)
               {
                  *virial += R*dphi;
               }

               /* accumulate force */
               if (comp_force)
               {
                  for (k = 0; k < DIM; ++k)
                  {
                     force[i*DIM + k] -= dphi*(coords[i*DIM + k] - coords[j*DIM + k])/R;
                     force[j*DIM + k] += dphi*(coords[i*DIM + k] - coords[j*DIM + k])/R;
                  }
               }
            }
         }
      }
   }
   else /* everything else */
   {
      if (0 == IterOrLoca) /* Iterator mode */
      {
         /* reset neighbor iterator */
         if (0 == (NBC%2)) /* full list */
         {
            *ier = KIM_API_get_full_neigh(pkim, 0, 0, &currentAtom, &numOfAtomNeigh, &neighListOfCurrentAtom, &Rij);
         }
         else /* half list */
         {
            *ier = KIM_API_get_half_neigh(pkim, 0, 0, &currentAtom, &numOfAtomNeigh, &neighListOfCurrentAtom, &Rij);
         }
         if (2 != *ier)
         {
            *ier = 0;
            return;
         }

         if (0 == (NBC%2)) /* full list */
         {
            *ier = KIM_API_get_full_neigh(pkim, 0, 1, &currentAtom, &numOfAtomNeigh, &neighListOfCurrentAtom, &Rij);
         }
         else /* half list */
         {
            *ier = KIM_API_get_half_neigh(pkim, 0, 1, &currentAtom, &numOfAtomNeigh, &neighListOfCurrentAtom, &Rij);
         }
         while (1 == *ier)
         {
            /* loop over the neighbors of currentAtom */
            for (jj = 0; jj < numOfAtomNeigh; ++ jj)
            {
               /* compute square distance */
               Rsqij = 0.0;
               for (k = 0; k < DIM; ++k)
               {
                  if (NBC < 5) /* MI-OPBC-H/F & NEIGH-PURE-H/F */
                  {
                     dx[k] = coords[currentAtom*DIM + k] - coords[neighListOfCurrentAtom[jj]*DIM + k];
                     
                     if ((NBC < 3) && (fabs(dx[k]) > 0.5*boxlength[k])) /* MI-OPBC-H/F */
                     {
                        dx[k] -= (dx[k]/fabs(dx[k]))*boxlength[k];
                     }
                  }
                  else /* NEIGH-RVEC-F */
                  {
                     dx[k] = Rij[jj*DIM + k];
                  }
                  
                  Rsqij += dx[k]*dx[k];
               }
               
               /* particles are interacting ? */
               if (Rsqij < *cutsq)
               {
                  R = sqrt(Rsqij);
                  /* compute pair potential */
                  pair(epsilon, C, Rzero, A1, A2, A3, R, &phi, &dphi, &d2phi);
                  
                  /* accumulate energy */
                  if (comp_energyPerAtom)
                  {
                     energyPerAtom[currentAtom] += 0.5*phi;
                     if (1 == (NBC%2)) energyPerAtom[neighListOfCurrentAtom[jj]] += 0.5*phi;
                  }
                  else
                  {
                     *energy += ( (0 == (NBC%2)) ? (0.5*phi) : (phi) );
                  }
                  
                  /* accumulate virial */
                  if (comp_virial)
                  {
                     *virial += ( (0 == (NBC%2)) ? 0.5 : 1.0 )*R*dphi;
                  }
                  
                  /* accumulate force */
                  if (comp_force)
                  {
                     for (k = 0; k < DIM; ++k)
                     {
                        force[currentAtom*DIM + k] -= dphi*dx[k]/R;
                        if (1 == (NBC%2)) force[neighListOfCurrentAtom[jj]*DIM + k] += dphi*dx[k]/R;
                     }
                  }
               }
            }
            
            /* increment iterator */
            if (0 == (NBC%2)) /* full list */
            {
               *ier = KIM_API_get_full_neigh(pkim, 0, 1, &currentAtom, &numOfAtomNeigh, &neighListOfCurrentAtom, &Rij);
            }
            else /* half list */
            {
               *ier = KIM_API_get_half_neigh(pkim, 0, 1, &currentAtom, &numOfAtomNeigh, &neighListOfCurrentAtom, &Rij);
            }
         }
      }
      else /* Locator mode */
      {
         /* loop over atoms */
         for (i = 0; i < *nAtoms; ++i)
         {
            /* get neighbor list for atom i */
            if (0 == (NBC%2)) /* full list */
            {
               *ier = KIM_API_get_full_neigh(pkim, 1, i, &currentAtom, &numOfAtomNeigh, &neighListOfCurrentAtom, &Rij);
            }
            else /* half list */
            {
               *ier = KIM_API_get_half_neigh(pkim, 1, i, &currentAtom, &numOfAtomNeigh, &neighListOfCurrentAtom, &Rij);
            }
            
            /* loop over the neighbors of currentAtom */
            for (jj = 0; jj < numOfAtomNeigh; ++ jj)
            {
               /* compute square distance */
               Rsqij = 0.0;
               for (k = 0; k < DIM; ++k)
               {
                  if (NBC < 5) /* MI-OPBC-H/F & NEIGH-PURE-H/F */
                  {
                     dx[k] = coords[currentAtom*DIM + k] - coords[neighListOfCurrentAtom[jj]*DIM + k];
                     
                     if ((NBC < 3) && (fabs(dx[k]) > 0.5*boxlength[k])) /* MI-OPBC-H/F */
                     {
                        dx[k] -= sign(boxlength[k])*dx[k];
                     }
                  }
                  else /* NEIGH-RVEC-F */
                  {
                     dx[k] = Rij[jj*DIM + k];
                  }
                  
                  Rsqij += dx[k]*dx[k];
               }
               
               /* particles are interacting ? */
               if (Rsqij < *cutsq)
               {
                  R = sqrt(Rsqij);
                  /* compute pair potential */
                  pair(epsilon, C, Rzero, A1, A2, A3, R, &phi, &dphi, &d2phi);
                  
                  /* accumulate energy */
                  if (comp_energyPerAtom)
                  {
                     energyPerAtom[currentAtom] += 0.5*phi;
                     if (1 == (NBC%2)) energyPerAtom[neighListOfCurrentAtom[jj]] += 0.5*phi;
                  }
                  else
                  {
                     *energy += ( (0 == (NBC%2)) ? (0.5*phi) : (phi) );
                  }
                  
                  /* accumulate virial */
                  if (comp_virial)
                  {
                     *virial += ( (0 == (NBC%2)) ? 0.5 : 1.0 )*R*dphi;
                  }
                  
                  /* accumulate force */
                  if (comp_force)
                  {
                     for (k = 0; k < DIM; ++k)
                     {
                        force[currentAtom*DIM + k] -= dphi*dx[k]/R;
                        if (1 == (NBC%2)) force[neighListOfCurrentAtom[jj]*DIM + k] += dphi*dx[k]/R;
                     }
                  }
               }
            }

         }
      }
   }


   /* perform final tasks */
   
   if (comp_virial)
   {
      *virial = -*virial/( (double) DIM); /* definition of virial term */
   }

   if (comp_energyPerAtom)
   {
      *energy = 0.0;
      for (i = 0; i < *nAtoms; ++i)
      {
         *energy += energyPerAtom[i];
      }
   }

   /* everything is great */
   *ier = 1;
   return;
}


/* Reinitialization function */
static void reinit(void *km)
{
   /* Local variables */
   intptr_t* pkim = *((intptr_t**) km);
   double* model_cutoff;
   double* model_epsilon;
   double* model_C;
   double* model_Rzero;
   double* model_Pcutoff;
   double* model_A1;
   double* model_A2;
   double* model_A3;
   double* model_cutsq;
   int ier;
   double ep;
   double ep2;

   /* get (changed) parameters from KIM object */

   /* get parameter cutoff from KIM object */
   model_Pcutoff = (double*) KIM_API_get_data(pkim, "PARAM_FREE_cutoff", &ier);
   if (1 > ier)
   {
      printf("* ERROR: PARAM_FREE_cutoff not found in KIM object.\n");
   }

   /* get epsilon from KIM object */
   model_epsilon = (double*) KIM_API_get_data(pkim, "PARAM_FREE_epsilon", &ier);
   if (1 > ier)
   {
      printf("* ERROR: PARAM_FREE_epsilon not found in KIM object.\n");
   }

   /* get C from KIM object */
   model_C = (double*) KIM_API_get_data(pkim, "PARAM_FREE_C", &ier);
   if (1 > ier)
   {
      printf("* ERROR: PARAM_FREE_C not found in KIM object.\n");
   }

   /* get Rzero from KIM object */
   model_Rzero = (double*) KIM_API_get_data(pkim, "PARAM_FREE_Rzero", &ier);
   if (1 > ier)
   {
      printf("* ERROR: PARAM_FREE_Rzero not found in KIM object.\n");
   }

   /* set new values in KIM object */
   

   /* store model cutoff in KIM object */
   model_cutoff = (double*) KIM_API_get_data(pkim, "cutoff", &ier);
   if (1 > ier)
   {
      printf("* ERROR: cutoff not found in KIM object.\n");
   }
   *model_cutoff = *model_Pcutoff;

   ep  = exp(-(*model_C)*((*model_cutoff) - (*model_Rzero)));
   ep2 = ep*ep;
   
   /* store model_A1 in KIM object */
   model_A1 = (double*) KIM_API_get_data(pkim, "PARAM_FIXED_A1", &ier);
   if (1 > ier)
   {
      printf("* ERROR: PARAM_FIXED_A1 not found in KIM object.\n");
   }
   /* set value of parameter A1 */
   *model_A1 = -((*model_epsilon)*(*model_C)*(*model_C)*( -2.0*ep2 + ep ));

   /* store model_A2 in KIM object */
   model_A2 = (double*) KIM_API_get_data(pkim, "PARAM_FIXED_A2", &ier);
   if (1 > ier)
   {
      printf("* ERROR: PARAM_FIXED_A2 not found in KIM object.\n");
   }
   /* set value of parameter A2 */
   *model_A2 = -( 2.0*(*model_epsilon)*(*model_C)*( -ep + ep2 )
                 +2.0*(*model_A1)*(*model_cutoff) );

   /* store model_A3 in KIM object */
   model_A3 = (double*) KIM_API_get_data(pkim, "PARAM_FIXED_A3", &ier);
   if (1 > ier)
   {
      printf("* ERROR: PARAM_FIXED_A3 not found in KIM object.\n");
   }
   /* set value of parameter A3 */
   *model_A3 = -( (*model_epsilon)*( -ep2 + 2.0*ep )
                 +(*model_A1)*(*model_cutoff)*(*model_cutoff)
                 +(*model_A2)*(*model_cutoff) );

   /* store model_cutsq in KIM object */
   model_cutsq = KIM_API_get_data(pkim, "PARAM_FIXED_cutsq", &ier);
   if (1 > ier)
   {
      printf("* ERROR: PARAM_FIXED_cutsq not found in KIM object.\n");
   }
   /* set value of parameter cutsq */
   *model_cutsq = (*model_cutoff)*(*model_cutoff);

   return;
}

/* destroy function */
static void destroy(void *km)
{
   /* Local variables */
   intptr_t* pkim = *((intptr_t**) km);
   double* model_cutoff;
   double* model_epsilon;
   double* model_C;
   double* model_Rzero;
   double* model_Pcutoff;
   double* model_A1;
   double* model_A2;
   double* model_A3;
   double* model_cutsq;
   int ier;

   /* get and free parameter cutoff from KIM object */
   model_Pcutoff = (double*) KIM_API_get_data(pkim, "PARAM_FREE_cutoff", &ier);
   if (1 > ier)
   {
      printf("* ERROR: PARAM_FREE_cutoff not found in KIM object.\n");
   }
   free(model_Pcutoff);

   /* get and free epsilon from KIM object */
   model_epsilon = (double*) KIM_API_get_data(pkim, "PARAM_FREE_epsilon", &ier);
   if (1 > ier)
   {
      printf("* ERROR: PARAM_FREE_epsilon not found in KIM object.\n");
   }
   free(model_epsilon);

   /* get and free C from KIM object */
   model_C = (double*) KIM_API_get_data(pkim, "PARAM_FREE_C", &ier);
   if (1 > ier)
   {
      printf("* ERROR: PARAM_FREE_C not found in KIM object.\n");
   }
   free(model_C);

   /* get and free model_A1 in KIM object */
   model_A1 = (double*) KIM_API_get_data(pkim, "PARAM_FIXED_A1", &ier);
   if (1 > ier)
   {
      printf("* ERROR: PARAM_FIXED_A1 not found in KIM object.\n");
   }
   free(model_A1);

   /* get and free model_A2 in KIM object */
   model_A2 = (double*) KIM_API_get_data(pkim, "PARAM_FIXED_A2", &ier);
   if (1 > ier)
   {
      printf("* ERROR: PARAM_FIXED_A2 not found in KIM object.\n");
   }
   free(model_A2);

   /* get and free model_A3 in KIM object */
   model_A3 = (double*) KIM_API_get_data(pkim, "PARAM_FIXED_A3", &ier);
   if (1 > ier)
   {
      printf("* ERROR: PARAM_FIXED_A3 not found in KIM object.\n");
   }
   free(model_A3);
   
   /* get and free model_cutsq in KIM object */
   model_cutsq = KIM_API_get_data(pkim, "PARAM_FIXED_cutsq", &ier);
   if (1 > ier)
   {
      printf("* ERROR: PARAM_FIXED_cutsq not found in KIM object.\n");
   }
   free(model_cutsq);

   return;
}


/* Initialization function */
void model_ar_morse_c_init_(void *km)
{
   /* Local variables */
   intptr_t* pkim = *((intptr_t**) km);
   double* model_cutoff;
   double* model_epsilon;
   double* model_C;
   double* model_Rzero;
   double* model_Rsq;
   double* model_Pcutoff;
   double* model_A1;
   double* model_A2;
   double* model_A3;
   double* model_cutsq;
   int ier;
   double ep;
   double ep2;

   /* store pointer to compute function in KIM object */
   if (! KIM_API_set_data(pkim, "compute", 1, (void*) &compute))
   {
      printf("* ERROR: compute not found in KIM object.\n");
      exit(1);
   }

   /* store pointer to reinit function in KIM object */
   if (! KIM_API_set_data(pkim, "reinit", 1, (void*) &reinit))
   {
      printf("* ERROR: reinit not found in KIM object.\n");
      exit(1);
   }

   /* store pointer to destroy function in KIM object */
   if (! KIM_API_set_data(pkim, "destroy", 1, (void*) &destroy))
   {
      printf("* ERROR: destroy not found in KIM object.\n");
      exit(1);
   }

   /* store model cutoff in KIM object */
   model_cutoff = (double*) KIM_API_get_data(pkim, "cutoff", &ier);
   if (1 > ier)
   {
      printf("* ERROR: cutoff not found in KIM object.\n");
   }
   *model_cutoff = 8.15; /* cutoff distance in angstroms */

   /* allocate memory for parameter cutoff and store value */
   model_Pcutoff = (double*) malloc(1*sizeof(double));
   if (NULL == model_Pcutoff)
   {
      printf("* ERROR: malloc of model_Pcutoff returned with error.\n");
      exit(1);
   }
   /* store model_Pcutoff in KIM object */
   if (! KIM_API_set_data(pkim, "PARAM_FREE_cutoff", 1, (void*) model_Pcutoff))
   {
      printf("* ERROR: PARAM_FREE_cutoff not found in KIM object.\n");
   }
   /* set value of parameter cutoff */
   *model_Pcutoff = *model_cutoff;

   /* allocate memory for epsilon and store value */
   model_epsilon = (double*) malloc(1*sizeof(double));
   if (NULL == model_epsilon)
   {
      printf("* ERROR: malloc of model_epsilon returned with error.\n");
      exit(1);
   }
   /* store model_epsilon in KIM object */
   if (! KIM_API_set_data(pkim, "PARAM_FREE_epsilon", 1, (void*) model_epsilon))
   {
      printf("* ERROR: PARAM_FREE_epsilon not found in KIM object.\n");
   }
   /* set value of epsilon */
   *model_epsilon = - (156.41/(1.160451911e4)); /* Morse epsilon in eV */

   /* allocate memory for C and store value */
   model_C = (double*) malloc(1*sizeof(double));
   if (NULL == model_C)
   {
      printf("* ERROR: malloc of model_C returned with error.\n");
      exit(1);
   }
   /* store model_C in KIM object */
   if (! KIM_API_set_data(pkim, "PARAM_FREE_C", 1, (void*) model_C))
   {
      printf("* ERROR: PARAM_FREE_C not found in KIM object.\n");
   }
   /* set value of C */
   *model_C = 1.545; /* Morse C in 1/Angstroms */

   /* allocate memory for Rzero and store value */
   model_Rzero = (double*) malloc(1*sizeof(double));
   if (NULL == model_Rzero)
   {
      printf("* ERROR: malloc of model_Rzero returned with error.\n");
      exit(1);
   }
   /* store model_Rzero in KIM object */
   if (! KIM_API_set_data(pkim, "PARAM_FREE_Rzero", 1, (void*) model_Rzero))
   {
      printf("* ERROR: PARAM_FREE_Rzero not found in KIM object.\n");
   }
   /* set value of Rzero */
   *model_Rzero = 3.786; /* Morse Rzero in Angstroms */

   ep  = exp(-(*model_C)*((*model_cutoff) - (*model_Rzero)));
   ep2 = ep*ep;

   /* allocate memory for parameter A1 and store value */
   model_A1 = (double*) malloc(1*sizeof(double));
   if (NULL == model_A1)
   {
      printf("* ERROR: malloc of model_A1 returned with error.\n");
      exit(1);
   }
   /* store model_A1 in KIM object */
   if (! KIM_API_set_data(pkim, "PARAM_FIXED_A1", 1, (void*) model_A1))
   {
      printf("* ERROR: PARAM_FIXED_A1 not found in KIM object.\n");
   }
   /* set value of parameter A1 */
   *model_A1 = -((*model_epsilon)*(*model_C)*(*model_C)*( -2.0*ep2 + ep ));

   /* allocate memory for parameter A2 and store value */
   model_A2 = (double*) malloc(1*sizeof(double));
   if (NULL == model_A2)
   {
      printf("* ERROR: malloc of model_A2 returned with error.\n");
      exit(1);
   }
   /* store model_A2 in KIM object */
   if (! KIM_API_set_data(pkim, "PARAM_FIXED_A2", 1, (void*) model_A2))
   {
      printf("* ERROR: PARAM_FIXED_A2 not found in KIM object.\n");
   }
   /* set value of parameter A2 */
   *model_A2 = -( 2.0*(*model_epsilon)*(*model_C)*( -ep + ep2 )
                 +2.0*(*model_A1)*(*model_cutoff) );

   /* allocate memory for parameter A3 and store value */
   model_A3 = (double*) malloc(1*sizeof(double));
   if (NULL == model_A3)
   {
      printf("* ERROR: malloc of model_A3 returned with error.\n");
      exit(1);
   }
   /* store model_A3 in KIM object */
   if (! KIM_API_set_data(pkim, "PARAM_FIXED_A3", 1, (void*) model_A3))
   {
      printf("* ERROR: PARAM_FIXED_A3 not found in KIM object.\n");
   }
   /* set value of parameter A3 */
   *model_A3 = -( (*model_epsilon)*( -ep2 + 2.0*ep )
                 +(*model_A1)*(*model_cutoff)*(*model_cutoff)
                 +(*model_A2)*(*model_cutoff) );

   /* allocate memory for parameter cutsq and store value */
   model_cutsq = (double*) malloc(1*sizeof(double));
   if (NULL == model_cutsq)
   {
      printf("* ERROR: malloc of model_cutsq returned with error.\n");
      exit(1);
   }
   /* store model_cutsq in KIM object */
   if (! KIM_API_set_data(pkim, "PARAM_FIXED_cutsq", 1, (void*) model_cutsq))
   {
      printf("* ERROR: PARAM_FIXED_cutsq not found in KIM object.\n");
   }
   /* set value of parameter cutsq */
   *model_cutsq = (*model_cutoff)*(*model_cutoff);

   return;
}
