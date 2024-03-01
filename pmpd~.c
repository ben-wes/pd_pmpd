// --------------------------------------------------------------------------
// This file is part of the pmpd software.
//
//    pmpd software is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    pmpd firmware is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with pmpd software. If not, see <http://www.gnu.org/licenses/>.
// --------------------------------------------------------------------------
//
// pmpd = physical modeling for pure data
// ch@chnry.net

#include <stdio.h>
#include <math.h>

#include "pmpd_export.h"
#include "pmpd_version.h"

#include "m_pd.h"

#define max(a,b) ( ((a) > (b)) ? (a) : (b) )
#define min(a,b) ( ((a) < (b)) ? (a) : (b) )

static t_class *pmpd_tilde_class;

struct _mass {
	t_float invM;
	t_float speedX;
	t_float posX;
	t_float forceX;
	t_int Id;
} ;

struct _link {
	struct _mass *mass1;
	struct _mass *mass2;
	t_float K1, D1; 
} ;

struct _NLlink {
	struct _mass *mass1;
	struct _mass *mass2;
	t_float K1, D1, L, L0, Lmin, Lmax, Pow; 
} ;

struct _inPos {
	// in en position
	t_int nbr_inlet;
	struct _mass *mass1;
	t_float influence;
} ;

struct _inForce {
	// in en force
	t_int nbr_inlet;
	struct _mass *mass1;
	t_float influence;
} ;

struct _outPos {
	// out en position
	t_int nbr_outlet;
	struct _mass *mass1;
	t_float influence;
} ;

struct _outSpeed {
	// out en vitesse
	t_int nbr_outlet;
	struct _mass *mass1;
	t_float influence;
} ;

typedef struct _pmpd_tilde {
	t_object  x_obj;

    struct _mass *mass;
    struct _link *link;
    struct _NLlink *NLlink;

    struct _inPos *inPos;
    struct _inForce *inForce;
    struct _outPos *outPos;
    struct _outSpeed *outSpeed;

	t_float *outlet;
    t_sample **inlet_vector;
    t_sample **outlet_vector;


	int nb_link, nb_NLlink, nb_mass, nb_inlet, nb_outlet, nb_inPos, nb_inForce, nb_outPos, nb_outSpeed;
	int nb_max_mass, nb_max_link, nb_max_in, nb_max_out;

	t_sample f; // used for signal inlet
	t_int loop, nb_loop; // to be able not to compute everything a each iteration
	unsigned int x_state; // random
    t_float x_f; // random
} t_pmpd_tilde;

/*
static int makeseed_pmpd_tilde(void)
{
    static unsigned int random_nextseed = 1489853723;
    random_nextseed = random_nextseed * 435898247 + 938284287;
    return (random_nextseed & 0x7fffffff);
}

static t_float random_bang_pmpd_tilde(t_pmpd_tilde *x)
{
    int nval;
    int range = 2000000;
	t_float rnd;
	unsigned int randval = x->x_state;
	x->x_state = randval = randval * 472940017 + 832416023;
    nval = ((double)range) * ((double)randval)
    	* (1./4294967296.);
    if (nval >= range) nval = range-1;
	rnd=nval;
	rnd-=1000000;
	rnd=rnd/1000000.;	//pour mettre entre -1 et 1;
    return (rnd);
}
*/
t_int *pmpd_tilde_perform(t_int *w)
///////////////////////////////////////////////////////////////////////////////////
{
	t_pmpd_tilde *x = (t_pmpd_tilde *)(w[1]);
	int n = (int)(w[2]);

	t_float F,L;
	t_int i;

	t_sample *out[x->nb_outlet];
	t_sample *in[x->nb_inlet];

	for (i=0; i<x->nb_inlet; i++)
		in[i]= x->inlet_vector[i];

	for (i=0; i<x->nb_outlet; i++)
		out[i]= x->outlet_vector[i];

	while (n--)
	{
		x->loop = 0;
		while (x->loop++ < x->nb_loop)
		{
			for (i=0; i<x->nb_inPos; i++)
				// get inlet value and make it a position to the specified mass
				x->inPos[i].mass1->posX = x->inPos[i].influence * *in[x->inPos[i].nbr_inlet];
			for (i=0; i<x->nb_inForce; i++)
				// get inlet value and make it a force to the specified mass
				x->inForce[i].mass1->forceX += x->inForce[i].influence * *in[x->inForce[i].nbr_inlet];

			for (i=0; i<x->nb_link; i++)
			// compute forces generated by links (spring / dashpot)
			{
				F  = x->link[i].K1 * ( x->link[i].mass1->posX   - x->link[i].mass2->posX  ) ;
				// spring

				F += x->link[i].D1 * ( x->link[i].mass1->speedX - x->link[i].mass2->speedX) ;
				// dashpot

				x->link[i].mass1->forceX -= F;
				x->link[i].mass2->forceX += F;
			}

			for (i=0; i<x->nb_NLlink; i++)
			// compute forces generated by NLlinks (spring / dashpot)
			{
				L=x->NLlink[i].mass1->posX - x->NLlink[i].mass2->posX - x->NLlink[i].L0;
				x->NLlink[i].L = L;
				if ((L < x->NLlink[i].Lmax) && (L > x->NLlink[i].Lmin))
				{
					F  = x->NLlink[i].K1 * pow(fabs(L) ,x->NLlink[i].Pow) ;
					if (L < 0) F *= -1;
					// spring

					F += x->NLlink[i].D1 * ( x->NLlink[i].mass1->speedX - x->NLlink[i].mass2->speedX) ;
					// dashpot

					x->NLlink[i].mass1->forceX -= F;
					x->NLlink[i].mass2->forceX += F;
				}
			}
			for (i=0; i<x->nb_mass; i++)
			{
			// compute new masses position
			// a mass does not move if M=0 (i.e : invM = 0)
				x->mass[i].speedX += x->mass[i].forceX * x->mass[i].invM;
				x->mass[i].forceX = 0; //random_bang_pmpd_tilde(x) * 1e-25;
					// only used for denormal problem
					// -ffast-math -O6 does not solve the problem
				x->mass[i].posX += x->mass[i].speedX ;
			}
		}

		for (i=0; i<x->nb_inlet; i++)
		// increase pointer to inlet vectors value
			in[i]++;
		for (i=0; i<x->nb_outlet; i++) x->outlet[i] = 0;
		for (i=0; i<x->nb_outPos; i++)
			x->outlet[x->outPos[i].nbr_outlet] += x->outPos[i].mass1->posX * x->outPos[i].influence;
			// compute output vector value
		for (i=0; i<x->nb_outSpeed; i++)
			x->outlet[x->outSpeed[i].nbr_outlet] += x->outSpeed[i].mass1->speedX * x->outSpeed[i].influence;
			// compute output vector value

		for (i=0; i<x->nb_outlet; i++)
		// send vector value to the vector pointer
		{
			*out[i]++ = x->outlet[i];
		}
	}
	return(w+3);
}

void pmpd_tilde_dsp(t_pmpd_tilde *x, t_signal **sp)
{
	int i;
	for (i=0; i<x->nb_inlet; i++)
		x->inlet_vector[i] = sp[i]->s_vec;

	for (i=0; i<x->nb_outlet; i++)
		x->outlet_vector[i] = sp[i+x->nb_inlet]->s_vec;

	dsp_add(pmpd_tilde_perform, 2, x, sp[0]->s_n);
}

void pmpd_tilde_bang(t_pmpd_tilde *x)
{
	t_int i;

	for (i=0; i<x->nb_mass; i++) logpost(x, 2, "mass:%ld, M:%f, pos:%f",i, x->mass[i].invM<=0.?0:1/x->mass[i].invM, x->mass[i].posX);
	for (i=0; i<x->nb_link; i++) logpost(x, 2, "link:%ld, mass1:%ld, mass2:%ld, K:%f, D:%f", i, x->link[i].mass1->Id, x->link[i].mass2->Id, x->link[i].K1, x->link[i].D1);
	for (i=0; i<x->nb_NLlink; i++) logpost(x, 2, "NLlink:%ld, mass1:%ld, mass2:%ld, K:%f, D:%f, L0:%f, Lmin:%f, Lmax:%f, Pow:%f", i, x->NLlink[i].mass1->Id, x->NLlink[i].mass2->Id, x->NLlink[i].K1, x->NLlink[i].D1, x->NLlink[i].L0, x->NLlink[i].Lmin, x->NLlink[i].Lmax, x->NLlink[i].Pow);
	for (i=0; i<x->nb_inPos; i++) logpost(x, 2, "In_pos:%ld, Inlet:%ld, Mass:%ld, Amplitude:%f", i, x->inPos[i].nbr_inlet, x->inPos[i].mass1->Id, x->inPos[i].influence);
	for (i=0; i<x->nb_inForce; i++) logpost(x, 2, "In_force:%ld, Inlet:%ld, Mass:%ld, Amplitude:%f", i, x->inForce[i].nbr_inlet, x->inForce[i].mass1->Id, x->inForce[i].influence);
	for (i=0; i<x->nb_outPos; i++) logpost(x, 2, "Out_pos:%ld, Outlet:%ld, Mass:%ld, Amplitude:%f", i, x->outPos[i].nbr_outlet, x->outPos[i].mass1->Id, x->outPos[i].influence);
	for (i=0; i<x->nb_outSpeed; i++) logpost(x, 2, "Out_speed:%ld, Outlet:%ld, Mass:%ld, Amplitude:%f", i, x->outSpeed[i].nbr_outlet, x->outSpeed[i].mass1->Id, x->outSpeed[i].influence);
}

void pmpd_tilde_float(t_pmpd_tilde *x, t_float force)
{
// add a force to all masses
	int i;
	for (i=0;i < x->nb_mass; i++)
		x->mass[i].forceX += force;
}

void pmpd_tilde_forceX(t_pmpd_tilde *x, t_float nbr_mass, t_float force)
{
// add a force to a specific mass
	if( (nbr_mass >= 0) && (nbr_mass < x->nb_mass) ) x->mass[(int)nbr_mass].forceX += force;
}

void pmpd_tilde_posX(t_pmpd_tilde *x, t_float nbr_mass, t_float posX)
{
// move a mass to a certain position
	if( (nbr_mass >= 0) && (nbr_mass < x->nb_mass) )  x->mass[(int)nbr_mass].posX = posX;
}

void pmpd_tilde_setM(t_pmpd_tilde *x, t_float nbr_mass, t_float M)
{
	if( (nbr_mass >= 0) && (nbr_mass < x->nb_mass) && (M>0)) x->mass[(int)nbr_mass].invM = 1./M;
}

void pmpd_tilde_setK(t_pmpd_tilde *x, t_float nb_link, t_float K)
{
	if( (nb_link >= 0) && (nb_link < x->nb_link) ) x->link[(int)nb_link].K1 = K;
}

void pmpd_tilde_setD(t_pmpd_tilde *x, t_float nb_link, t_float D)
{
	if( (nb_link >= 0) && (nb_link < x->nb_link) ) x->link[(int)nb_link].D1 = D;
}

void pmpd_tilde_setNLD(t_pmpd_tilde *x, t_float nb_NLlink, t_float D)
{
	if( (nb_NLlink >= 0) && (nb_NLlink < x->nb_NLlink) )  x->NLlink[(int)nb_NLlink].D1 = D;
}

void pmpd_tilde_setNLK(t_pmpd_tilde *x, t_symbol *s, int argc, t_atom *argv)
{
	int nbr_NLlink;
	if ( (argc == 2) && (argv[0].a_type == A_FLOAT) && (argv[1].a_type == A_FLOAT) ) {
		nbr_NLlink = atom_getfloatarg(0,argc,argv);
		if( (nbr_NLlink >= 0) && (nbr_NLlink < x->nb_NLlink) )
			x->NLlink[(int)nbr_NLlink].K1 = atom_getfloatarg(1,argc,argv);;
	} else
	if ( (argc == 3) && (argv[0].a_type == A_FLOAT) && (argv[1].a_type == A_FLOAT) && (argv[2].a_type == A_FLOAT) ) {
		nbr_NLlink = atom_getfloatarg(0,argc,argv);
		if( (nbr_NLlink >= 0) && (nbr_NLlink < x->nb_NLlink) )  {
			x->NLlink[(int)nbr_NLlink].K1 = atom_getfloatarg(1,argc,argv);
			x->NLlink[(int)nbr_NLlink].Pow = atom_getfloatarg(2,argc,argv);
		}
	}
}

void pmpd_tilde_setNLKPow(t_pmpd_tilde *x, t_float nbr_NLlink, t_float Pow)
{
	if( (nbr_NLlink >= 0) && (nbr_NLlink < x->nb_NLlink) )  x->NLlink[(int)nbr_NLlink].Pow = Pow;
}

void pmpd_tilde_setNLL(t_pmpd_tilde *x, t_float nb_NLlink, t_float L)
{
	if( (nb_NLlink >= 0) && (nb_NLlink < x->nb_NLlink) )  x->NLlink[(int)nb_NLlink].L0 = L;
}

void pmpd_tilde_setNLLMin(t_pmpd_tilde *x, t_float nb_NLlink, t_float M)
{
	if( (nb_NLlink >= 0) && (nb_NLlink < x->nb_NLlink) )  x->NLlink[(int)nb_NLlink].Lmin = M;
}

void pmpd_tilde_setNLLMax(t_pmpd_tilde *x, t_float nb_NLlink, t_float M)
{
	if( (nb_NLlink >= 0) && (nb_NLlink < x->nb_NLlink) )  x->NLlink[(int)nb_NLlink].Lmax = M;
}

void pmpd_tilde_setNLLCurrent(t_pmpd_tilde *x, t_symbol *s, int argc, t_atom *argv) {
	int nbr_NLlink;
	t_float percent;
	if ( (argc >= 1) && (argv[0].a_type == A_FLOAT) ) {
		nbr_NLlink = (int)atom_getfloatarg(0,argc,argv);
		if ( (argc >= 2) && (argv[1].a_type == A_FLOAT) )
			percent = atom_getfloatarg(1,argc,argv);
		else
			percent = 1.;
		if( (nbr_NLlink >= 0) && (nbr_NLlink < x->nb_NLlink) )
			x->NLlink[nbr_NLlink].L0 += percent * (x->NLlink[nbr_NLlink].L - x->NLlink[nbr_NLlink].L0);
	}
}

void pmpd_tilde_mass(t_pmpd_tilde *x, t_float M, t_float posX)
// add a mass
//invM speedX posX force
{
	if (x->nb_mass == x->nb_max_mass) {
		pd_error(x, "too many mass");
		return;
	}

	if (M<=0)
	{
		M = 0;
		x->mass[x->nb_mass].invM = 0;
	}
	else
		x->mass[x->nb_mass].invM = 1/M;

	x->mass[x->nb_mass].speedX = 0;
	x->mass[x->nb_mass].posX = posX;
	x->mass[x->nb_mass].forceX = 0;
	x->mass[x->nb_mass].Id = x->nb_mass;
	
	x->nb_mass++ ;
}

void pmpd_tilde_link(t_pmpd_tilde *x, t_float mass_1, t_float mass_2, t_float K1, t_float D1)
// add a link
// *mass1, *mass2, K1, D1;
{
	if (x->nb_link == x->nb_max_link) {
		pd_error(x, "too many link");
		return;
	}
		
	x->link[x->nb_link].mass1 = &x->mass[max(0, min ( x->nb_mass, (int)mass_1))];
	x->link[x->nb_link].mass2 = &x->mass[max(0, min ( x->nb_mass, (int)mass_2))];
	x->link[x->nb_link].K1 = K1;
	x->link[x->nb_link].D1 = D1;

	x->nb_link++ ;
}

void pmpd_tilde_NLlink(t_pmpd_tilde *x, t_symbol *s, int argc, t_atom *argv)
// t_float mass_1, t_float mass_2, t_float K1, t_float D1, t_float Pow, t_float L0, t_float Lmin, t_float Lmax
// add a NLlink
{
	if (x->nb_NLlink == x->nb_max_link) {
		pd_error(x, "too many NLlinks (increase limit with creation argument)");
		return;
	}
		
	if  (argc == 8)
	{
		x->NLlink[x->nb_NLlink].mass1 = &x->mass[max(0, min ( x->nb_mass, (int)atom_getfloatarg(0, argc, argv)))];
		x->NLlink[x->nb_NLlink].mass2 = &x->mass[max(0, min ( x->nb_mass, (int)atom_getfloatarg(1, argc, argv)))];
		x->NLlink[x->nb_NLlink].K1 = atom_getfloatarg(2, argc, argv);
		x->NLlink[x->nb_NLlink].D1 = atom_getfloatarg(3, argc, argv);
		x->NLlink[x->nb_NLlink].Pow = atom_getfloatarg(4, argc, argv);
		x->NLlink[x->nb_NLlink].L0 = atom_getfloatarg(5, argc, argv);
		x->NLlink[x->nb_NLlink].Lmin = atom_getfloatarg(6, argc, argv);
		x->NLlink[x->nb_NLlink].Lmax = atom_getfloatarg(7, argc, argv);
	}
	else
	pd_error(x, "wrong argument count for NLlink");
	
	x->nb_NLlink++ ;
}

void pmpd_tilde_inPos(t_pmpd_tilde *x, t_float nb_inlet, t_float mass_1, t_float influence)
//add an input point
// nbr_inlet, *mass1, influence;
{
	if (x->nb_inPos == x->nb_max_in) {
		pd_error(x, "too many inPos (increase limit with creation argument)");
		return;
	}
		
	x->inPos[x->nb_inPos].nbr_inlet = max(0, min( x->nb_inlet,(int)nb_inlet));
	x->inPos[x->nb_inPos].mass1 = &x->mass[max(0, min ( x->nb_mass, (int)mass_1))];
	x->inPos[x->nb_inPos].influence = influence;

	x->nb_inPos++;
}
void pmpd_tilde_inForce(t_pmpd_tilde *x, t_float nb_inlet, t_float mass_1, t_float influence)
//add an input point
// nbr_inlet, *mass1, influence;
{
	if (x->nb_inForce == x->nb_max_in) {
		pd_error(x, "too many inForce (increase limit with creation argument)");
		return;
	}
		
	x->inForce[x->nb_inForce].nbr_inlet = max(0, min( x->nb_inlet,(int)nb_inlet));
	x->inForce[x->nb_inForce].mass1 = &x->mass[max(0, min ( x->nb_mass, (int)mass_1))];
	x->inForce[x->nb_inForce].influence = influence;

	x->nb_inForce++;
}

void pmpd_tilde_outPos(t_pmpd_tilde *x, t_float nb_outlet, t_float mass_1, t_float influence)
// add an output point
// nbr_outlet, *mass1, influence;
{
	if (x->nb_outPos == x->nb_max_out) {
		pd_error(x, "too many outPos (increase limit with creation argument)");
		return;
	}
		
	x->outPos[x->nb_outPos].nbr_outlet = max(0, min( x->nb_outlet,(int)nb_outlet));
	x->outPos[x->nb_outPos].mass1 = &x->mass[max(0, min ( x->nb_mass, (int)mass_1))];
	x->outPos[x->nb_outPos].influence = influence;

	x->nb_outPos++ ;
}

void pmpd_tilde_outSpeed(t_pmpd_tilde *x, t_float nb_outlet, t_float mass_1, t_float influence)
// add an output point
// nbr_outlet, *mass1, influence;
{
	if (x->nb_outSpeed == x->nb_max_out) {
		pd_error(x, "too many outSpeed (increase limit with creation argument)");
		return;
	}
		
	x->outSpeed[x->nb_outSpeed].nbr_outlet = max(0, min( x->nb_outlet,(int)nb_outlet));
	x->outSpeed[x->nb_outSpeed].mass1 = &x->mass[max(0, min ( x->nb_mass, (int)mass_1))];
	x->outSpeed[x->nb_outSpeed].influence = influence;

	x->nb_outSpeed++ ;
}

void pmpd_tilde_reset(t_pmpd_tilde *x)
{
	x->nb_link = 0;
	x->nb_NLlink = 0;
	x->nb_mass = 0;
	x->nb_inPos = 0;
	x->nb_inForce = 0;
	x->nb_outSpeed = 0;
	x->nb_outPos = 0;
}

void *pmpd_tilde_new(t_symbol *s, int argc, t_atom *argv)
{
	int i;

	t_pmpd_tilde *x = (t_pmpd_tilde *)pd_new(pmpd_tilde_class);

	pmpd_tilde_reset(x);
	//makeseed_pmpd_tilde();

	x->nb_inlet= max(1, (int)atom_getfloatarg(0, argc, argv));

    	x->inlet_vector = (t_sample **)getbytes(x->nb_inlet * sizeof(t_sample *));
	for(i=0; i<x->nb_inlet-1; i++)
		inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_signal, &s_signal);

	x->nb_outlet= max(1, (int)atom_getfloatarg(1, argc, argv));

	x->outlet = (t_float *)getbytes(x->nb_outlet * sizeof(t_float));
    	x->outlet_vector = (t_sample **)getbytes(x->nb_outlet * sizeof(t_sample *));
	for(i=0; i<x->nb_outlet; i++)
		outlet_new(&x->x_obj, &s_signal);

	x->nb_loop = max(1, (int)atom_getfloatarg(2, argc, argv));

	x->nb_max_mass = 10000;
	if ((argc >= 4) && (argv[3].a_type == A_FLOAT) && (int)atom_getfloatarg(3, argc, argv) > 0)
		x->nb_max_mass = (int)atom_getfloatarg(3, argc, argv);
	x->nb_max_link = 10000;
	if ((argc >= 5) && (argv[4].a_type == A_FLOAT) && (int)atom_getfloatarg(4, argc, argv) > 0)
		x->nb_max_link = (int)atom_getfloatarg(4, argc, argv);
	x->nb_max_in = x->nb_inlet;
	if ((argc >= 6) && (argv[5].a_type == A_FLOAT) && (int)atom_getfloatarg(5, argc, argv) > 0)
		x->nb_max_in = (int)atom_getfloatarg(5, argc, argv);
	x->nb_max_out = x->nb_outlet;
	if ((argc >= 7) && (argv[6].a_type == A_FLOAT) && (int)atom_getfloatarg(6, argc, argv) > 0)
		x->nb_max_out = (int)atom_getfloatarg(6, argc, argv);
		

    x->mass   = (struct _mass *  )getbytes(x->nb_max_mass * sizeof(struct _link));
    x->link   = (struct _link *  )getbytes(x->nb_max_link * sizeof(struct _link));
    x->NLlink = (struct _NLlink *)getbytes(x->nb_max_link * sizeof(struct _link));

    x->inPos    = (struct _inPos *   )getbytes(x->nb_max_in  * sizeof(struct _inPos));
    x->inForce  = (struct _inForce * )getbytes(x->nb_max_in  * sizeof(struct _inForce));
    x->outPos   = (struct _outPos *  )getbytes(x->nb_max_out * sizeof(struct _outPos));
    x->outSpeed = (struct _outSpeed *)getbytes(x->nb_max_out * sizeof(struct _outSpeed));

	return (void *)x;
}

void pmpd_tilde_free(t_pmpd_tilde *x) {

    if (x->inlet_vector)
        freebytes(x->inlet_vector, x->nb_inlet * sizeof(t_sample *));
    if (x->outlet_vector)
        freebytes(x->outlet_vector, x->nb_outlet * sizeof(t_sample *));
    if (x->link)
        freebytes(x->link, x->nb_link * sizeof(struct _link));
    if (x->NLlink)
        freebytes(x->NLlink, x->nb_NLlink * sizeof(struct _NLlink));
    if (x->mass)
        freebytes(x->mass, x->nb_mass * sizeof(struct _mass));
    if (x->inPos)
        freebytes(x->inPos, x->nb_inPos * sizeof(struct _inPos));
    if (x->inForce)
        freebytes(x->inForce, x->nb_inForce * sizeof(struct _inForce));
    if (x->outPos)
        freebytes(x->outPos, x->nb_outPos * sizeof(struct _outPos));
    if (x->outSpeed)
        freebytes(x->outSpeed, x->nb_outSpeed * sizeof(struct _outSpeed));
}

PMPD_EXPORT void pmpd_tilde_setup(void) {
	pmpd_tilde_class = class_new(
		gensym("pmpd~"),
		(t_newmethod)pmpd_tilde_new,
		(t_method)pmpd_tilde_free,
		sizeof(t_pmpd_tilde), CLASS_DEFAULT, A_GIMME, 0);

    if(!pmpd_tilde_class)
        return;

    verbose(4, "pmpd~ version %s (%s)", pmpd_tag(), pmpd_sha());

	CLASS_MAINSIGNALIN(pmpd_tilde_class, t_pmpd_tilde, f);

	class_addbang(pmpd_tilde_class, pmpd_tilde_bang);
	class_addfloat(pmpd_tilde_class,  (t_method)pmpd_tilde_float);
	class_addmethod(pmpd_tilde_class, (t_method)pmpd_tilde_mass, gensym("mass"), A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, 0);
	class_addmethod(pmpd_tilde_class, (t_method)pmpd_tilde_link, gensym("link"), A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, 0);
	class_addmethod(pmpd_tilde_class, (t_method)pmpd_tilde_NLlink, gensym("NLlink"), A_GIMME, 0);
	class_addmethod(pmpd_tilde_class, (t_method)pmpd_tilde_inPos, gensym("inPos"), A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, 0);
	class_addmethod(pmpd_tilde_class, (t_method)pmpd_tilde_inPos, gensym("inPosX"), A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, 0);
	class_addmethod(pmpd_tilde_class, (t_method)pmpd_tilde_inForce, gensym("inForce"), A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, 0);
	class_addmethod(pmpd_tilde_class, (t_method)pmpd_tilde_inForce, gensym("inForceX"), A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, 0);
	class_addmethod(pmpd_tilde_class, (t_method)pmpd_tilde_outPos, gensym("outPos"), A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, 0);
	class_addmethod(pmpd_tilde_class, (t_method)pmpd_tilde_outPos, gensym("outPosX"), A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, 0);
	class_addmethod(pmpd_tilde_class, (t_method)pmpd_tilde_outSpeed, gensym("outSpeed"), A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, 0);
	class_addmethod(pmpd_tilde_class, (t_method)pmpd_tilde_outSpeed, gensym("outSpeedX"), A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, 0);
	class_addmethod(pmpd_tilde_class, (t_method)pmpd_tilde_posX, gensym("pos"), A_DEFFLOAT, A_DEFFLOAT, 0);
	class_addmethod(pmpd_tilde_class, (t_method)pmpd_tilde_posX, gensym("posX"), A_DEFFLOAT, A_DEFFLOAT, 0);
	class_addmethod(pmpd_tilde_class, (t_method)pmpd_tilde_forceX, gensym("force"), A_DEFFLOAT, A_DEFFLOAT, 0);
	class_addmethod(pmpd_tilde_class, (t_method)pmpd_tilde_forceX, gensym("forceX"), A_DEFFLOAT, A_DEFFLOAT, 0);
	class_addmethod(pmpd_tilde_class, (t_method)pmpd_tilde_setK, gensym("setK"), A_DEFFLOAT, A_DEFFLOAT, 0);
	class_addmethod(pmpd_tilde_class, (t_method)pmpd_tilde_setD, gensym("setD"), A_DEFFLOAT, A_DEFFLOAT, 0);
	class_addmethod(pmpd_tilde_class, (t_method)pmpd_tilde_setM, gensym("setM"), A_DEFFLOAT, A_DEFFLOAT, 0);
	class_addmethod(pmpd_tilde_class, (t_method)pmpd_tilde_setNLK, gensym("setNLK"), A_GIMME, 0);
	class_addmethod(pmpd_tilde_class, (t_method)pmpd_tilde_setNLKPow, gensym("setNLKPow"), A_DEFFLOAT, A_DEFFLOAT, 0);
	class_addmethod(pmpd_tilde_class, (t_method)pmpd_tilde_setNLD, gensym("setNLD"), A_DEFFLOAT, A_DEFFLOAT, 0);
	class_addmethod(pmpd_tilde_class, (t_method)pmpd_tilde_setNLL, gensym("setNLL"), A_DEFFLOAT, A_DEFFLOAT, 0);
	class_addmethod(pmpd_tilde_class, (t_method)pmpd_tilde_setNLLMin, gensym("setNLLMin"), A_DEFFLOAT, A_DEFFLOAT, 0);
	class_addmethod(pmpd_tilde_class, (t_method)pmpd_tilde_setNLLMax, gensym("setNLLMax"), A_DEFFLOAT, A_DEFFLOAT, 0);
	class_addmethod(pmpd_tilde_class, (t_method)pmpd_tilde_setNLLCurrent, gensym("setNLLCurrent"), A_GIMME, 0);

	class_addmethod(pmpd_tilde_class, (t_method)pmpd_tilde_reset, gensym("reset"), 0);
	class_addmethod(pmpd_tilde_class, (t_method)pmpd_tilde_dsp, gensym("dsp"),  A_CANT, 0);
}
