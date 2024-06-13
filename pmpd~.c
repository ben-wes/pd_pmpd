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

#include "m_pd.h"
#include <stdio.h>
#include <math.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif
#include "pmpd_export.h"
#include "pmpd_version.h"

#define max(a,b) ( ((a) > (b)) ? (a) : (b) )

#define NB_MAX_LINK_DEFAULT 10000
#define NB_MAX_MASS_DEFAULT 10000
#define NB_MAX_IN_DEFAULT    1000
#define NB_MAX_OUT_DEFAULT   1000
#define REF_SAMPLERATE      44100

typedef void (*t_signal_setmultiout)(t_signal **, int); 
static t_signal_setmultiout g_signal_setmultiout;
static t_class *pmpd_tilde_class;

struct _mass {
    t_float invM;
    t_float speedX;
    t_float posX;
    t_float forceX;
    t_int Id;
};

struct _link {
    struct _mass *mass1;
    struct _mass *mass2;
    t_float K, D;
};

struct _NLlink {
    struct _mass *mass1;
    struct _mass *mass2;
    t_float K, D, L, L0, Lmin, Lmax, Pow;
};

struct _input {
    t_int nbr_inlet;
    struct _mass *mass;
    t_float influence;
};

struct _output {
    t_int nbr_outlet;
    struct _mass *mass;
    t_float influence;
};

typedef struct _pmpd_tilde {
    t_object  x_obj;

    struct _mass *mass;
    struct _link *link;
    struct _NLlink *NLlink;

    struct _input *inPos;
    struct _input *inForce;
    struct _output *outPos;
    struct _output *outSpeed;

    t_float *outlet;
    t_sample **inlet_vector;
    t_sample **outlet_vector;
    t_int multichannel;
    t_int nb_max_link, nb_max_mass;
    t_int nb_link, nb_NLlink, nb_mass;
    t_int nb_inlet, nb_outlet, nb_max_in, nb_max_out;
    t_int nb_inPos, nb_inForce, nb_outPos, nb_outSpeed;
    t_sample f; // used for signal inlet
    t_int nb_loop; // to be able not to compute everything a each iteration
    t_float sr_mass_factor; // factor to compensate for different samplerates
} t_pmpd_tilde;

t_int *pmpd_tilde_perform(t_int *w)
///////////////////////////////////////////////////////////////////////////////////
{
    t_pmpd_tilde *x = (t_pmpd_tilde *)(w[1]);
    t_int n = w[2]; // sample count from sp[0]->s_n

    t_float F,L;
    t_int i, si, loop;

    for (si=0; si<n; si++)
    {
        for (loop=0; loop<x->nb_loop; loop++)
        {
            for (i=0; i<x->nb_inPos; i++)
                // get inlet value and make it a position to the specified mass
                x->inPos[i].mass->posX = x->inPos[i].influence * x->inlet_vector[x->inPos[i].nbr_inlet][si];
            for (i=0; i<x->nb_inForce; i++)
                // get inlet value and make it a force to the specified mass
                x->inForce[i].mass->forceX += x->inForce[i].influence * x->inlet_vector[x->inForce[i].nbr_inlet][si];

            for (i=0; i<x->nb_link; i++)
            // compute forces generated by links (spring / dashpot)
            {
                F  = x->link[i].K * ( x->link[i].mass1->posX - x->link[i].mass2->posX);
                // spring

                F += x->link[i].D * ( x->link[i].mass1->speedX - x->link[i].mass2->speedX);
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
                    F = x->NLlink[i].K * pow((fabs(L)), x->NLlink[i].Pow);
                    F = (L < 0) ? -F : F;
                    // spring

                    F += x->NLlink[i].D * ( x->NLlink[i].mass1->speedX - x->NLlink[i].mass2->speedX);
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
                x->mass[i].posX += x->mass[i].speedX;
            }
        }

        // compute output vector value
        for (i=0; i<x->nb_outPos; i++) x->outlet[x->outPos[i].nbr_outlet] += x->outPos[i].mass->posX * x->outPos[i].influence;
        for (i=0; i<x->nb_outSpeed; i++) x->outlet[x->outSpeed[i].nbr_outlet] += x->outSpeed[i].mass->speedX * x->outSpeed[i].influence;

        // send vector value to the vector pointer and reset for next iteration
        for (i=0; i<x->nb_outlet; i++) {
            x->outlet_vector[i][si] = x->outlet[i];
            x->outlet[i] = 0;
        }
    }
    return(w+3);
}

void pmpd_tilde_dsp(t_pmpd_tilde *x, t_signal **sp)
{
    int i;
    t_int nsamples = (t_int)sp[0]->s_n;
    t_int nchans = (t_int)sp[0]->s_nchans;
    if (x->multichannel) {
        // set inlet vectors and wrap around input channels
        // if expected count > given count
        for (i=0; i<x->nb_inlet; i++)
            x->inlet_vector[i] = sp[0]->s_vec + nsamples * (i % nchans);
        // set outlet multichannel count
        g_signal_setmultiout(&sp[1], x->nb_outlet);
        for (i=0; i<x->nb_outlet; i++)
            x->outlet_vector[i] = sp[1]->s_vec + nsamples * i;
    } else {
        for (i=0; i<x->nb_inlet; i++)
            x->inlet_vector[i] = sp[i]->s_vec;
        for (i=0; i<x->nb_outlet; i++) {
            // set outlets to single channel if multichannel mode not active
            if (g_signal_setmultiout)
                g_signal_setmultiout(&sp[i+x->nb_inlet], 1);
            x->outlet_vector[i] = sp[i+x->nb_inlet]->s_vec;
        }
    }
    dsp_add(pmpd_tilde_perform, 2, x, nsamples);
}

void pmpd_tilde_bang(t_pmpd_tilde *x)
{
    t_int i;
    for (i=0; i<x->nb_mass; i++)     logpost(x, 2, "mass:%ld, M:%f, pos:%f", i, x->mass[i].invM > 0 ? 1/x->mass[i].invM/x->sr_mass_factor : 0, x->mass[i].posX);
    for (i=0; i<x->nb_link; i++)     logpost(x, 2, "link:%ld, mass1:%ld, mass2:%ld, K:%f, D:%f", i, x->link[i].mass1->Id, x->link[i].mass2->Id, x->link[i].K, x->link[i].D);
    for (i=0; i<x->nb_NLlink; i++)   logpost(x, 2, "NLlink:%ld, mass1:%ld, mass2:%ld, K:%f, D:%f, L0:%f, Lmin:%f, Lmax:%f, Pow:%f", i, x->NLlink[i].mass1->Id, x->NLlink[i].mass2->Id, x->NLlink[i].K, x->NLlink[i].D, x->NLlink[i].L0, x->NLlink[i].Lmin, x->NLlink[i].Lmax, x->NLlink[i].Pow);
    for (i=0; i<x->nb_inPos; i++)    logpost(x, 2, "inPos:%ld, Inlet:%ld, Mass:%ld, Amplitude:%f", i, x->inPos[i].nbr_inlet, x->inPos[i].mass->Id, x->inPos[i].influence);
    for (i=0; i<x->nb_inForce; i++)  logpost(x, 2, "inForce:%ld, Inlet:%ld, Mass:%ld, Amplitude:%f", i, x->inForce[i].nbr_inlet, x->inForce[i].mass->Id, x->inForce[i].influence);
    for (i=0; i<x->nb_outPos; i++)   logpost(x, 2, "outPos:%ld, Outlet:%ld, Mass:%ld, Amplitude:%f", i, x->outPos[i].nbr_outlet, x->outPos[i].mass->Id, x->outPos[i].influence);
    for (i=0; i<x->nb_outSpeed; i++) logpost(x, 2, "outSpeed:%ld, Outlet:%ld, Mass:%ld, Amplitude:%f", i, x->outSpeed[i].nbr_outlet, x->outSpeed[i].mass->Id, x->outSpeed[i].influence);
}

inline int validate_index(t_pmpd_tilde *x, int idx, t_int count, const char* type)
{
    if (idx < 0 || idx+1 > count) {
        pd_error(x, "pmpd~: no %s at index %i", type, idx);
        return 0;
    }
    return 1;
}

void pmpd_tilde_float(t_pmpd_tilde *x, t_float force)
{
// add a force to all masses
    int i;
    for (i=0;i < x->nb_mass; i++)
        x->mass[i].forceX += force;
}

void pmpd_tilde_forceX(t_pmpd_tilde *x, t_float idx_mass, t_float force)
{
    if (!validate_index(x, (int)idx_mass, x->nb_mass, "mass")) return;
    x->mass[(int)idx_mass].forceX += force;
}

void pmpd_tilde_posX(t_pmpd_tilde *x, t_float idx_mass, t_float posX)
{
    if (!validate_index(x, (int)idx_mass, x->nb_mass, "mass")) return;
    x->mass[(int)idx_mass].posX = posX;
}

void pmpd_tilde_setM(t_pmpd_tilde *x, t_float idx_mass, t_float M)
{
    if (!validate_index(x, (int)idx_mass, x->nb_mass, "mass")) return;
    x->mass[(int)idx_mass].invM = M>0 ? 1/M/x->sr_mass_factor : 0;
}

void pmpd_tilde_setK(t_pmpd_tilde *x, t_float idx_link, t_float K)
{
    if (!validate_index(x, (int)idx_link, x->nb_link, "link")) return;
    x->link[(int)idx_link].K = K;
}

void pmpd_tilde_setD(t_pmpd_tilde *x, t_float idx_link, t_float D)
{
    if (!validate_index(x, (int)idx_link, x->nb_link, "link")) return;
    x->link[(int)idx_link].D = D;
}

void pmpd_tilde_setNLD(t_pmpd_tilde *x, t_float idx_NLlink, t_float D)
{
    if (!validate_index(x, (int)idx_NLlink, x->nb_NLlink, "NLlink")) return;
    x->NLlink[(int)idx_NLlink].D = D;
}

void pmpd_tilde_setNLK(t_pmpd_tilde *x, t_symbol *s, int argc, t_atom *argv)
{
    int idx_NLlink;
    if (argc < 2)
    {
        pd_error(x, "pmpd~: 'setNLK' requires link index and rigidity arguments");
        return;
    }
    idx_NLlink = atom_getintarg(0, argc, argv);
    if (!validate_index(x, idx_NLlink, x->nb_NLlink, "NLlink")) return;
    x->NLlink[idx_NLlink].K = atom_getfloatarg(1, argc, argv);
    if (argc > 2)
        x->NLlink[idx_NLlink].Pow = atom_getfloatarg(2, argc, argv);
}

void pmpd_tilde_setNLKPow(t_pmpd_tilde *x, t_float idx_NLlink, t_float Pow)
{
    if (!validate_index(x, (int)idx_NLlink, x->nb_NLlink, "NLlink")) return;
    x->NLlink[(int)idx_NLlink].Pow = Pow;
}

void pmpd_tilde_setNLL(t_pmpd_tilde *x, t_float idx_NLlink, t_float L)
{
    if (!validate_index(x, (int)idx_NLlink, x->nb_NLlink, "NLlink")) return;
    x->NLlink[(int)idx_NLlink].L0 = L;
}

void pmpd_tilde_setNLLMin(t_pmpd_tilde *x, t_float idx_NLlink, t_float Lmin)
{
    if (!validate_index(x, (int)idx_NLlink, x->nb_NLlink, "NLlink")) return;
    x->NLlink[(int)idx_NLlink].Lmin = Lmin;
}

void pmpd_tilde_setNLLMax(t_pmpd_tilde *x, t_float idx_NLlink, t_float Lmax)
{
    if (!validate_index(x, (int)idx_NLlink, x->nb_NLlink, "NLlink")) return;
    x->NLlink[(int)idx_NLlink].Lmax = Lmax;
}

void pmpd_tilde_setNLLCurrent(t_pmpd_tilde *x, t_symbol *s, int argc, t_atom *argv)
{
    int idx_NLlink;
    t_float percent;
    if (argc < 1)
    {
        pd_error(x, "pmpd~: 'setNLLCurrent' requires NLlink index argument");
        return;
    }
    idx_NLlink = atom_getintarg(0, argc, argv);
    if (!validate_index(x, idx_NLlink, x->nb_NLlink, "NLlink")) return;
    if (argc > 1)
        percent = atom_getfloatarg(1, argc, argv);
    else
        percent = 1;
    x->NLlink[idx_NLlink].L0 += percent * (x->NLlink[idx_NLlink].L - x->NLlink[idx_NLlink].L0);
}

inline int validate_count(t_pmpd_tilde *x, t_int count, t_int count_max, const char* type)
{
    if (count == count_max)
    {
        pd_error(x, "pmpd~: too many %s (increase limit with creation argument)", type);
        return 0;
    }
    return 1;
}

void pmpd_tilde_mass(t_pmpd_tilde *x, t_float M, t_float posX)
// add a mass
// invM speedX posX force
{
    if (!validate_count(x, x->nb_mass, x->nb_max_mass, "masses")) return;
    x->mass[x->nb_mass].invM = M>0 ? 1/M/x->sr_mass_factor : 0;
    x->mass[x->nb_mass].speedX = 0;
    x->mass[x->nb_mass].posX = posX;
    x->mass[x->nb_mass].forceX = 0;
    x->mass[x->nb_mass].Id = x->nb_mass;
    x->nb_mass++ ;
}

void pmpd_tilde_link(t_pmpd_tilde *x, t_float mass_1, t_float mass_2, t_float K, t_float D)
// add a link
// *mass1, *mass2, K, D;
{
    if (!validate_count(x, x->nb_link, x->nb_max_link, "links") ||
        !validate_index(x, (int)mass_1, x->nb_mass, "mass1") ||
        !validate_index(x, (int)mass_2, x->nb_mass, "mass2")) return;
    x->link[x->nb_link].mass1 = &x->mass[(int)mass_1];
    x->link[x->nb_link].mass2 = &x->mass[(int)mass_2];
    x->link[x->nb_link].K = K;
    x->link[x->nb_link].D = D;
    x->nb_link++ ;
}

void pmpd_tilde_NLlink(t_pmpd_tilde *x, t_symbol *s, int argc, t_atom *argv)
// add a NLlink
// t_float mass_1, t_float mass_2, t_float K, t_float D, t_float Pow, t_float L0, t_float Lmin, t_float Lmax
{
    int mass_1, mass_2;
    if (argc < 2)
    {
        pd_error(x, "pmpd~: 'NLlink' requires 2 mass index arguments");
        return;
    } 
    mass_1 = atom_getintarg(0, argc, argv);
    mass_2 = atom_getintarg(1, argc, argv);
    if (!validate_count(x, x->nb_NLlink, x->nb_max_link, "NLlinks") ||
        !validate_index(x, mass_1, x->nb_mass, "mass1") ||
        !validate_index(x, mass_2, x->nb_mass, "mass2")) return;
    x->NLlink[x->nb_NLlink].mass1 = &x->mass[mass_1];
    x->NLlink[x->nb_NLlink].mass2 = &x->mass[mass_2];
    x->NLlink[x->nb_NLlink].K = (argc >= 3) ? atom_getfloatarg(2, argc, argv) : 0;
    x->NLlink[x->nb_NLlink].D = (argc >= 4) ? atom_getfloatarg(3, argc, argv) : 0;
    x->NLlink[x->nb_NLlink].Pow = (argc >= 5) ? atom_getfloatarg(4, argc, argv) : 1;
    x->NLlink[x->nb_NLlink].L0 = (argc >= 6) ? atom_getfloatarg(5, argc, argv) : 0;
    x->NLlink[x->nb_NLlink].Lmin = (argc >= 7) ? atom_getfloatarg(6, argc, argv) : -1000000;
    x->NLlink[x->nb_NLlink].Lmax = (argc >= 8) ? atom_getfloatarg(7, argc, argv) : 1000000;
    x->nb_NLlink++ ;
}

void pmpd_tilde_inPos(t_pmpd_tilde *x, t_float idx_inlet, t_float idx_mass, t_float influence)
//add an input point
// nbr_inlet, *mass, influence;
{
    if (!validate_count(x, x->nb_inPos, x->nb_max_in, "inPos inlet assignments") ||
        !validate_index(x, idx_inlet, x->nb_inlet, "inlet") ||
        !validate_index(x, idx_mass, x->nb_mass, "mass")) return;
    x->inPos[x->nb_inPos].nbr_inlet = (int)idx_inlet;
    x->inPos[x->nb_inPos].mass = &x->mass[(int)idx_mass];
    x->inPos[x->nb_inPos].influence = influence;
    x->nb_inPos++;
}

void pmpd_tilde_inForce(t_pmpd_tilde *x, t_float idx_inlet, t_float idx_mass, t_float influence)
{
    if (!validate_count(x, x->nb_inForce, x->nb_max_in, "inForce inlet assignments") ||
        !validate_index(x, idx_inlet, x->nb_inlet, "inlet") ||
        !validate_index(x, idx_mass, x->nb_mass, "mass")) return;
    x->inForce[x->nb_inForce].nbr_inlet = (int)idx_inlet;
    x->inForce[x->nb_inForce].mass = &x->mass[(int)idx_mass];
    x->inForce[x->nb_inForce].influence = influence;
    x->nb_inForce++;
}

void pmpd_tilde_outPos(t_pmpd_tilde *x, t_float idx_outlet, t_float idx_mass, t_float influence)
{
    if (!validate_count(x, x->nb_outPos, x->nb_max_out, "outPos outlet assignments") ||
        !validate_index(x, idx_outlet, x->nb_outlet, "outlet") ||
        !validate_index(x, idx_mass, x->nb_mass, "mass")) return;
    x->outPos[x->nb_outPos].nbr_outlet = (int)idx_outlet;
    x->outPos[x->nb_outPos].mass = &x->mass[(int)idx_mass];
    x->outPos[x->nb_outPos].influence = influence;
    x->nb_outPos++;
}

void pmpd_tilde_outSpeed(t_pmpd_tilde *x, t_float idx_outlet, t_float idx_mass, t_float influence)
{
    if (!validate_count(x, x->nb_outSpeed, x->nb_max_out, "outSpeed outlet assignments") ||
        !validate_index(x, idx_outlet, x->nb_outlet, "outlet") ||
        !validate_index(x, idx_mass, x->nb_mass, "mass")) return;
    x->outSpeed[x->nb_outSpeed].nbr_outlet = (int)idx_outlet;
    x->outSpeed[x->nb_outSpeed].mass = &x->mass[(int)idx_mass];
    x->outSpeed[x->nb_outSpeed].influence = influence;
    x->nb_outSpeed++;
}

void pmpd_tilde_reset(t_pmpd_tilde *x)
{
    t_float sr = sys_getsr();
    x->sr_mass_factor = (sr / REF_SAMPLERATE) * (sr / REF_SAMPLERATE);
    x->nb_link     = 0;
    x->nb_NLlink   = 0;
    x->nb_mass     = 0;
    x->nb_inPos    = 0;
    x->nb_inForce  = 0;
    x->nb_outSpeed = 0;
    x->nb_outPos   = 0;
}

void pmpd_tilde_free(t_pmpd_tilde *x) {
    if (x->outlet) freebytes(x->outlet, x->nb_outlet * sizeof(t_float));

    if (x->inlet_vector) freebytes(x->inlet_vector, x->nb_inlet * sizeof(t_sample *));
    if (x->outlet_vector) freebytes(x->outlet_vector, x->nb_outlet * sizeof(t_sample *));

    if (x->mass) freebytes(x->mass, x->nb_mass * sizeof(struct _mass));
    if (x->link) freebytes(x->link, x->nb_link * sizeof(struct _link));
    if (x->NLlink) freebytes(x->NLlink, x->nb_NLlink * sizeof(struct _NLlink));

    if (x->inPos) freebytes(x->inPos, x->nb_inPos * sizeof(struct _input));
    if (x->inForce) freebytes(x->inForce, x->nb_inForce * sizeof(struct _input));
    if (x->outPos) freebytes(x->outPos, x->nb_outPos * sizeof(struct _output));
    if (x->outSpeed) freebytes(x->outSpeed, x->nb_outSpeed * sizeof(struct _output));
}

void *pmpd_tilde_new(t_symbol *s, int argc, t_atom *argv)
{
    int i, arg;
    t_pmpd_tilde *x = (t_pmpd_tilde *)pd_new(pmpd_tilde_class);
    int maj = 0, min = 0, bug = 0;
    sys_getversion(&maj, &min, &bug);
    x->multichannel = 0;

    pmpd_tilde_reset(x);

    // check for flags (currently need to be positioned first)
    while (argc && argv->a_type == A_SYMBOL) {
        if (atom_getsymbol(argv) == gensym("-m")) {
            if(g_signal_setmultiout)
                x->multichannel = 1;
            else
                pd_error(x, "[pmpd~]: no multichannel support in Pd %i.%i-%i, ignoring '-m' flag", maj, min, bug);
        } else
            pd_error(x, "[pmpd~]: invalid argument");
        argc--, argv++;
    }

    x->nb_inlet = max(1, atom_getintarg(0, argc, argv));
    x->nb_outlet = max(1, atom_getintarg(1, argc, argv));
    x->nb_loop = max(1, atom_getintarg(2, argc, argv));
    x->nb_max_mass = (arg = atom_getintarg(3, argc, argv)) > 0 ? arg : NB_MAX_MASS_DEFAULT;
    x->nb_max_link = (arg = atom_getintarg(4, argc, argv)) > 0 ? arg : NB_MAX_LINK_DEFAULT;
    x->nb_max_in   = (arg = atom_getintarg(5, argc, argv)) > 0 ? arg : NB_MAX_IN_DEFAULT;
    x->nb_max_out  = (arg = atom_getintarg(6, argc, argv)) > 0 ? arg : NB_MAX_OUT_DEFAULT;

    x->outlet = (t_float *)getbytes(x->nb_outlet * sizeof(t_float));

    x->inlet_vector = (t_sample **)getbytes(x->nb_inlet * sizeof(t_sample *));
    x->outlet_vector = (t_sample **)getbytes(x->nb_outlet * sizeof(t_sample *));

    x->mass     = (struct _mass *)getbytes(x->nb_max_mass * sizeof(struct _link));
    x->link     = (struct _link *)getbytes(x->nb_max_link * sizeof(struct _link));
    x->NLlink   = (struct _NLlink *)getbytes(x->nb_max_link * sizeof(struct _link));

    x->inPos    = (struct _input *)getbytes(x->nb_max_in * sizeof(struct _input));
    x->inForce  = (struct _input *)getbytes(x->nb_max_in * sizeof(struct _input));
    x->outPos   = (struct _output *)getbytes(x->nb_max_out * sizeof(struct _output));
    x->outSpeed = (struct _output *)getbytes(x->nb_max_out * sizeof(struct _output));

    outlet_new(&x->x_obj, &s_signal);
    // add more channels if multichannel not set
    if (!x->multichannel) {
        for(i=0; i<x->nb_inlet-1; i++)
            inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_signal, &s_signal);
        for(i=0; i<x->nb_outlet-1; i++)
            outlet_new(&x->x_obj, &s_signal);
    }
    return (void *)x;
}

PMPD_EXPORT void pmpd_tilde_setup(void)
{
// multichannel handling copied from https://github.com/Spacechild1/vstplugin/blob/3f0ed8a800ea238bf204a2ead940b2d1324ac909/pd/src/vstplugin~.cpp#L4122-L4136
#ifdef _WIN32
    // get a handle to the module containing the Pd API functions.
    // NB: GetModuleHandle("pd.dll") does not cover all cases.
    HMODULE module;
    if (GetModuleHandleEx(
            GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            (LPCSTR)&pd_typedmess, &module)) {
        g_signal_setmultiout = (t_signal_setmultiout)(void *)GetProcAddress(
            module, "signal_setmultiout");
    }
#else
    // search recursively, starting from the main program
    g_signal_setmultiout = (t_signal_setmultiout)dlsym(
        dlopen(NULL, RTLD_NOW), "signal_setmultiout");
#endif

    pmpd_tilde_class = class_new(
        gensym("pmpd~"),
        (t_newmethod)pmpd_tilde_new,
        (t_method)pmpd_tilde_free,
        sizeof(t_pmpd_tilde), CLASS_MULTICHANNEL, A_GIMME, 0);

    if(!pmpd_tilde_class)
        return;

    //verbose(4, "pmpd~ version %s (%s)", pmpd_tag(), pmpd_sha());

    CLASS_MAINSIGNALIN(pmpd_tilde_class, t_pmpd_tilde, f);

    class_addbang(pmpd_tilde_class, pmpd_tilde_bang);
    class_addmethod(pmpd_tilde_class, (t_method)pmpd_tilde_bang, gensym("print"), 0);
    class_addfloat(pmpd_tilde_class, (t_method)pmpd_tilde_float);
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
    class_addmethod(pmpd_tilde_class, (t_method)pmpd_tilde_dsp, gensym("dsp"), A_CANT, 0);
}
