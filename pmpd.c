/*
--------------------------  pmpd  ---------------------------------------- 
     
	  
 pmpd = physical modeling for pure data                                     
 Written by Cyrille Henry (ch@chnry.net)
 
 Get sources on pd svn on sourceforge.

 This program is free software; you can redistribute it and/or                
 modify it under the terms of the GNU General Public License                  
 as published by the Free Software Foundation; either version 2               
 of the License, or (at your option) any later version.                       
                                                                             
 This program is distributed in the hope that it will be useful,              
 but WITHOUT ANY WARRANTY; without even the implied warranty of               
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                
 GNU General Public License for more details.                           
                                                                              
 You should have received a copy of the GNU General Public License           
 along with this program; if not, write to the Free Software                  
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.  
                                                                              
 Based on PureData by Miller Puckette and others.                             
                                                                             

----------------------------------------------------------------------------
*/

#ifndef VERSION
#define VERSION "0.10"
#endif

#include "m_pd.h"
#include "stdio.h"

#ifndef __DATE__ 
#define __DATE__ ""
#endif

#define nb_max_link   1000000
#define nb_max_mass   100000

#define max(a,b) ( ((a) > (b)) ? (a) : (b) ) 
#define min(a,b) ( ((a) < (b)) ? (a) : (b) ) 

t_float sign_ch(t_float v)
{
    return v > 0 ? 1 : -1;
}

t_float abs_ch(t_float v)
{
    return v > 0 ? v : -v;
}

t_float pow_ch(t_float a, t_float x)
{
    return a > 0 ? pow(a,x) : -pow(-a,x);
}

static t_class *pmpd_class;

typedef struct _mass {
    t_symbol *Id;
    int mobile;
	t_float invM;
	t_float speedX;
	t_float posX;
	t_float forceX;
    int num;
} foo;

typedef struct _link {
    t_symbol *Id;
	struct _mass *mass1;
	struct _mass *mass2;
	t_float K;
    t_float D;
    t_float L;
    t_float Pow;
    t_float Lmin;
    t_float Lmax;
    t_float distance;
} foo1 ;

typedef struct _pmpd 
{
 	t_object  x_obj;
	struct _link link[nb_max_link];
	struct _mass mass[nb_max_mass];
	t_outlet *main_outlet;
    t_outlet *info_outlet;
    int nb_link;
	int nb_mass;
    t_float minX, maxX;
} t_pmpd;

void pmpd_bang(t_pmpd *x)
{
// this part is doing all the PM
	t_float F, L, absL;
	t_int i;
    post("bang");

	for (i=1; i<x->nb_mass; i++)
	// compute new masses position
		if (x->mass[i].mobile > 0) // only if mobile
		{
			x->mass[i].speedX += x->mass[i].forceX * x->mass[i].invM;
			x->mass[i].forceX = 0;
			x->mass[i].posX += x->mass[i].speedX ;
            x->mass[i].posX = min(x->maxX,max(x->minX,x->mass[i].posX));
		}

	for (i=0; i<x->nb_link; i++)
	// comput link forces
	{
        L = x->link[i].mass1->posX - x->link[i].mass2->posX;
        absL = abs_ch(L);
        if ( (absL >= x->link[i].Lmin) & (absL < x->link[i].Lmax)  & (L!=0))
        {
   	    	F = x->link[i].D * (L - x->link[i].distance) ;
            x->link[i].distance=L;
                
            L =  sign_ch(L) * pow_ch( absL - x->link[i].L, x->link[i].Pow);

    		F  += x->link[i].K *  L;
		    x->link[i].mass1->forceX -= F;
		    x->link[i].mass2->forceX += F;
        }
	}
}

void pmpd_forceX(t_pmpd *x, t_symbol *s, int argc, t_atom *argv)
{
// add a force to a specific mass
    int tmp, i;

    if ( ( argv[0].a_type == A_FLOAT ) & ( argv[1].a_type == A_FLOAT ) )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_mass-1, tmp));
	    x->mass[tmp].forceX += atom_getfloatarg(1, argc, argv);
    }
    if ( ( argv[0].a_type == A_SYMBOL ) & ( argv[1].a_type == A_FLOAT ) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            if ( atom_getsymbolarg(0,argc,argv) == x->mass[i].Id)
            {
	            x->mass[i].forceX += atom_getfloatarg(1, argc, argv);
            }
        }
    }
}

void pmpd_posX(t_pmpd *x, t_symbol *s, int argc, t_atom *argv)
{
// displace a mass to a certain position
    int tmp, i;

    if ( ( argv[0].a_type == A_FLOAT ) & ( argv[1].a_type == A_FLOAT ) )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_mass-1, tmp));
	    x->mass[tmp].posX = atom_getfloatarg(1, argc, argv);
   	    x->mass[tmp].speedX = 0; // ??? TODO : esce la bonne chose a faire?
        x->mass[tmp].forceX = 0; // ??? TODO : esce la bonne chose a faire?
        
    }
    if ( ( argv[0].a_type == A_SYMBOL ) & ( argv[1].a_type == A_FLOAT ) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            if ( atom_getsymbolarg(0,argc,argv) == x->mass[i].Id)
            {
	            x->mass[i].posX = atom_getfloatarg(1, argc, argv);
        	    x->mass[i].speedX = 0; // ??? TODO : esce la bonne chose a faire?
                x->mass[i].forceX = 0; // ??? TODO : esce la bonne chose a faire?

            }
        }
    }
}

void pmpd_mass(t_pmpd *x, t_symbol *Id, t_float mobile, t_float M, t_float posX )
// add a mass
// Id, invM speedX posX forceX
{
	if (M<=0) M=1;
	x->mass[x->nb_mass].Id = Id;
	x->mass[x->nb_mass].mobile = (int)mobile;
	x->mass[x->nb_mass].invM = 1/M;
	x->mass[x->nb_mass].speedX = 0;
	x->mass[x->nb_mass].posX = posX;
	x->mass[x->nb_mass].forceX = 0;
	x->mass[x->nb_mass].num = x->nb_mass;

	x->nb_mass++ ;
	x->nb_mass = min ( nb_max_mass -1, x->nb_mass );
}

void pmpd_create_link(t_pmpd *x, t_symbol *Id, int mass1, int mass2, t_float K, t_float D, t_float Pow, t_float Lmin, t_float Lmax)
{ // create a link based on mass number

    if ((x->nb_mass>0) & (mass1 != mass2) & (mass1 >= 0) & (mass2 >= 0) & (mass1 < x->nb_mass) & (mass2 < x->nb_mass) )
    {
	    x->link[x->nb_link].Id = Id;
	    x->link[x->nb_link].mass1 = &x->mass[mass1]; 
	    x->link[x->nb_link].mass2 = &x->mass[mass2];
	    x->link[x->nb_link].K = K;
	    x->link[x->nb_link].D = D;
	    x->link[x->nb_link].L = x->mass[mass1].posX - x->mass[mass2].posX;
	    x->link[x->nb_link].Pow = Pow;
	    x->link[x->nb_link].Lmin = Lmin;
	    x->link[x->nb_link].Lmax = Lmax;
	    x->link[x->nb_link].distance = 0;

	    x->nb_link++ ;
	    x->nb_link = min ( nb_max_link -1, x->nb_link );
    }
}

void pmpd_link(t_pmpd *x, t_symbol *s, int argc, t_atom *argv)
// add a link
// Id, *mass1, *mass2, K, D, Pow, Lmin, Lmax;
{
    int i, j;

    t_symbol *Id = atom_getsymbolarg(0,argc,argv);
    int mass1 = atom_getfloatarg(1, argc, argv);
    int mass2 = atom_getfloatarg(2, argc, argv);
    t_float K = atom_getfloatarg(3, argc, argv);
    t_float D = atom_getfloatarg(4, argc, argv);
    t_float Pow = 1; 
    if (argc > 5) Pow = atom_getfloatarg(5, argc, argv);
    t_float Lmin = -1000000;
    if (argc > 6) Lmin = atom_getfloatarg(6, argc, argv);
    t_float Lmax =  1000000;
    if (argc > 7) Lmax = atom_getfloatarg(7, argc, argv);
//    post("%d,%d, %f,%f", mass1, mass2, K, D);

    if ( ( argv[1].a_type == A_FLOAT ) & ( argv[2].a_type == A_FLOAT ) )
    {
        pmpd_create_link(x, Id, mass1, mass2, K, D, Pow, Lmin, Lmax);
    }
    else
    if ( ( argv[1].a_type == A_SYMBOL ) & ( argv[2].a_type == A_FLOAT ) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            if ( atom_getsymbolarg(1,argc,argv) == x->mass[i].Id)
            {
                pmpd_create_link(x, Id, i, mass2, K, D, Pow, Lmin, Lmax);
            }
        }
    }
    else
    if ( ( argv[1].a_type == A_FLOAT ) & ( argv[2].a_type == A_SYMBOL ) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            if ( atom_getsymbolarg(2,argc,argv) == x->mass[i].Id)
            {
                pmpd_create_link(x, Id, mass1, i, K, D, Pow, Lmin, Lmax);
            }
        }
    }
    else
    if ( ( argv[1].a_type == A_SYMBOL ) & ( argv[2].a_type == A_SYMBOL ) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            for (j=0; j< x->nb_mass; j++)
            {
                if ( (atom_getsymbolarg(1,argc,argv) == x->mass[i].Id)&(atom_getsymbolarg(2,argc,argv) == x->mass[j].Id))
                {
                    pmpd_create_link(x, Id, i, j, K, D, Pow, Lmin, Lmax);
                }
            }   
        }
    }
}

void pmpd_reset(t_pmpd *x)
{
	x->nb_link = 0;
	x->nb_mass = 0;
    x->minX = -1000000;
    x->maxX = 1000000;
}

void pmpd_infosL(t_pmpd *x)
{
    int i;
    post("list of mass");
    post("number, Id, mobile, mass, position, speed, forces");
	for(i=0; i < x->nb_mass; i++)
    {
        post("masse %i: %s, %d, %f, %f, %f, %f",i, x->mass[i].Id->s_name, x->mass[i].mobile, 1/x->mass[i].invM, x->mass[i].posX, x->mass[i].speedX, x->mass[i].forceX );
    }

    post("list of link");
    post("number, Id, mass1, mass2, K, D, Pow, L, Lmin, Lmax");
	for(i=0; i < x->nb_link; i++)
    {
        post("link %i: %s, %i, %i, %f, %f, %f, %f, %f, %f", i, x->link[i].Id->s_name, x->link[i].mass1->num, x->link[i].mass2->num, x->link[i].K, x->link[i].D, x->link[i].Pow, x->link[i].L, x->link[i].Lmin, x->link[i].Lmax);
    }
}

void pmpd_minX(t_pmpd *x, t_float min)
{
	x->minX = min;
}

void pmpd_maxX(t_pmpd *x, t_float max)
{
	x->maxX = max;
}

void pmpd_setFixed(t_pmpd *x, t_symbol *s, int argc, t_atom *argv)
{
    int tmp, i;

    if ( argv[0].a_type == A_FLOAT ) 
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_mass-1, tmp));
	    x->mass[tmp].mobile = 0;
    }
    if ( argv[0].a_type == A_SYMBOL )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            if ( atom_getsymbolarg(0,argc,argv) == x->mass[i].Id)
            {
	            x->mass[i].mobile = 0;
            }
        }
    }
}

void pmpd_setMobile(t_pmpd *x, t_symbol *s, int argc, t_atom *argv)
{
    int tmp, i;

    if ( argv[0].a_type == A_FLOAT )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_mass-1, tmp));
	    x->mass[tmp].mobile = 1;
    }
    if ( argv[0].a_type == A_SYMBOL ) 
    {
        for (i=0; i< x->nb_mass; i++)
        {
            if ( atom_getsymbolarg(0,argc,argv) == x->mass[i].Id)
            {
	            x->mass[i].mobile = 1;
            }
        }
    }
}

void pmpd_setK(t_pmpd *x, t_symbol *s, int argc, t_atom *argv)
{
    int tmp, i;

    if ( ( argv[0].a_type == A_FLOAT ) & ( argv[1].a_type == A_FLOAT ) )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_link-1, tmp));
	    x->link[tmp].K = atom_getfloatarg(1, argc, argv);
    }
    if ( ( argv[0].a_type == A_SYMBOL ) & ( argv[1].a_type == A_FLOAT ) )
    {
        for (i=0; i< x->nb_link; i++)
        {
            if ( atom_getsymbolarg(0,argc,argv) == x->link[i].Id)
            {
	            x->link[i].K = atom_getfloatarg(1, argc, argv);
            }
        }
    }
}

void pmpd_setD(t_pmpd *x, t_symbol *s, int argc, t_atom *argv)
{
    int tmp, i;

    if ( ( argv[0].a_type == A_FLOAT ) & ( argv[1].a_type == A_FLOAT ) )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_link-1, tmp));
	    x->link[tmp].D = atom_getfloatarg(1, argc, argv);
    }
    if ( ( argv[0].a_type == A_SYMBOL ) & ( argv[1].a_type == A_FLOAT ) )
    {
        for (i=0; i< x->nb_link; i++)
        {
            if ( atom_getsymbolarg(0,argc,argv) == x->link[i].Id)
            {
	            x->link[i].D = atom_getfloatarg(1, argc, argv);
            }
        }
    }
}

void pmpd_setL(t_pmpd *x, t_symbol *s, int argc, t_atom *argv)
{
    int tmp, i;

    if ( ( argv[0].a_type == A_FLOAT ) & ( argv[1].a_type == A_FLOAT ) )
    {
        tmp = atom_getfloatarg(0, argc, argv);
        tmp = max(0, min( x->nb_link-1, tmp));
	    x->link[tmp].L = atom_getfloatarg(1, argc, argv);
    }
    if ( ( argv[0].a_type == A_SYMBOL ) & ( argv[1].a_type == A_FLOAT ) )
    {
        for (i=0; i< x->nb_link; i++)
        {
            if ( atom_getsymbolarg(0,argc,argv) == x->link[i].Id)
            {
	            x->link[i].L = atom_getfloatarg(1, argc, argv);
            }
        }
    }
}

void pmpd_get(t_pmpd *x, t_symbol *s, int argc, t_atom *argv)
{
    int tmp, i;
    t_float X;
    t_symbol *toget; 
    t_atom  toout[2];
    toget = atom_getsymbolarg(0, argc, argv);

    if ( (toget == gensym("massesPos")) & (argv[1].a_type == A_FLOAT) )
    {
        i = atom_getfloatarg(1, argc, argv);
        if ( (i>=0) & (i<x->nb_mass) )
        {
            SETFLOAT(&(toout[0]), i);
            SETFLOAT(&(toout[1]), x->mass[i].posX);
            outlet_anything(x->main_outlet, gensym("massesPosNo"), 2, toout);
        }  
    }
    else
    if ( (toget == gensym("massesPos")) & (argv[1].a_type == A_SYMBOL) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            if ( atom_getsymbolarg(1,argc,argv) == x->mass[i].Id)
            {
                SETFLOAT(&(toout[0]), i);
                SETFLOAT(&(toout[1]), x->mass[i].posX);
                outlet_anything(x->main_outlet, gensym("massesPosId"), 2, toout);
            }
        } 
    }
    else
    if ( (toget == gensym("massesPos")) & (argc == 1) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            SETFLOAT(&(toout[0]), i);
            SETFLOAT(&(toout[1]), x->mass[i].posX);
            outlet_anything(x->main_outlet, gensym("massesPos"), 2, toout);
        } 
    }
    else
    if ( (toget == gensym("massesPosName")) & (argv[1].a_type == A_FLOAT) )
    {
        i = atom_getfloatarg(1, argc, argv);
        if ( (i>=0) & (i<x->nb_mass) )
        {
            SETSYMBOL(&(toout[0]), x->mass[i].Id);
            SETFLOAT(&(toout[1]), x->mass[i].posX);
            outlet_anything(x->main_outlet, gensym("massesPosNameNo"), 2, toout);
        }  
    }
    else
    if ( (toget == gensym("massesPosName")) & (argv[1].a_type == A_SYMBOL) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            if ( atom_getsymbolarg(1,argc,argv) == x->mass[i].Id)
            {
                SETSYMBOL(&(toout[0]), x->mass[i].Id);
                SETFLOAT(&(toout[1]), x->mass[i].posX);
                outlet_anything(x->main_outlet, gensym("massesPosNameId"), 2, toout);
            }
        } 
    }
    else
    if ( (toget == gensym("massesPosName")) & (argc == 1) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            SETSYMBOL(&(toout[0]), x->mass[i].Id);
            SETFLOAT(&(toout[1]), x->mass[i].posX);
            outlet_anything(x->main_outlet, gensym("massesPosName"), 2, toout);
        } 
    }
    else
    if ( (toget == gensym("massesSpeeds")) & (argv[1].a_type == A_FLOAT) )
    {
        i = atom_getfloatarg(1, argc, argv);
        if ( (i>=0) & (i<x->nb_mass) )
        {
            SETFLOAT(&(toout[0]), i);
            SETFLOAT(&(toout[1]), x->mass[i].speedX);
            outlet_anything(x->main_outlet, gensym("massesSpeedsNo"), 2, toout);
        }  
    }
    else
    if ( (toget == gensym("massesSpeeds")) & (argv[1].a_type == A_SYMBOL) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            if ( atom_getsymbolarg(1,argc,argv) == x->mass[i].Id)
            {
                SETFLOAT(&(toout[0]), i);
                SETFLOAT(&(toout[1]), x->mass[i].speedX);
                outlet_anything(x->main_outlet, gensym("massesSpeedsId"), 2, toout);
            }
        } 
    }
    else
    if ( (toget == gensym("massesSpeeds")) & (argc == 1) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            SETFLOAT(&(toout[0]), i);
            SETFLOAT(&(toout[1]), x->mass[i].speedX);
            outlet_anything(x->main_outlet, gensym("massesSpeeds"), 2, toout);
        } 
    }
    else
    if ( (toget == gensym("massesSpeedsName")) & (argv[1].a_type == A_FLOAT) )
    {
        i = atom_getfloatarg(1, argc, argv);
        if ( (i>=0) & (i<x->nb_mass) )
        {
            SETSYMBOL(&(toout[0]), x->mass[i].Id);
            SETFLOAT(&(toout[1]), x->mass[i].speedX);
            outlet_anything(x->main_outlet, gensym("massesSpeedsNameNo"), 2, toout);
        }  
    }
    else
    if ( (toget == gensym("massesSpeedsName")) & (argv[1].a_type == A_SYMBOL) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            if ( atom_getsymbolarg(1,argc,argv) == x->mass[i].Id)
            {
                SETSYMBOL(&(toout[0]), x->mass[i].Id);
                SETFLOAT(&(toout[1]), x->mass[i].speedX);
                outlet_anything(x->main_outlet, gensym("massesSpeedsNameId"), 2, toout);
            }
        } 
    }
    else
    if ( (toget == gensym("massesSpeedsName")) & (argc == 1) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            SETSYMBOL(&(toout[0]), x->mass[i].Id);
            SETFLOAT(&(toout[1]), x->mass[i].speedX);
            outlet_anything(x->main_outlet, gensym("massesSpeedsName"), 2, toout);
        } 
    }
    else
    if ( (toget == gensym("massesForces")) & (argv[1].a_type == A_FLOAT) )
    {
        i = atom_getfloatarg(1, argc, argv);
        if ( (i>=0) & (i<x->nb_mass) )
        {
            SETFLOAT(&(toout[0]), i);
            SETFLOAT(&(toout[1]), x->mass[i].forceX);
            outlet_anything(x->main_outlet, gensym("massesForcesNo"), 2, toout);
        }  
    }
    else
    if ( (toget == gensym("massesForces")) & (argv[1].a_type == A_SYMBOL) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            if ( atom_getsymbolarg(1,argc,argv) == x->mass[i].Id)
            {
                SETFLOAT(&(toout[0]), i);
                SETFLOAT(&(toout[1]), x->mass[i].forceX);
                outlet_anything(x->main_outlet, gensym("massesForcesId"), 2, toout);
            }
        } 
    }
    else
    if ( (toget == gensym("massesForces")) & (argc == 1) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            SETFLOAT(&(toout[0]), i);
            SETFLOAT(&(toout[1]), x->mass[i].forceX);
            outlet_anything(x->main_outlet, gensym("massesForces"), 2, toout);
        } 
    }
    else
    if ( (toget == gensym("massesForcesName")) & (argv[1].a_type == A_FLOAT) )
    {
        i = atom_getfloatarg(1, argc, argv);
        if ( (i>=0) & (i<x->nb_mass) )
        {
            SETSYMBOL(&(toout[0]), x->mass[i].Id);
            SETFLOAT(&(toout[1]), x->mass[i].forceX);
            outlet_anything(x->main_outlet, gensym("massesForcesNameNo"), 2, toout);
        }
    }
    else
    if ( (toget == gensym("massesForcesName")) & (argv[1].a_type == A_SYMBOL) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            if ( atom_getsymbolarg(1,argc,argv) == x->mass[i].Id)
            {
                SETSYMBOL(&(toout[0]), x->mass[i].Id);
                SETFLOAT(&(toout[1]), x->mass[i].forceX);
                outlet_anything(x->main_outlet, gensym("massesForcesNameId"), 2, toout);
            }
        } 
    }
    else
    if ( (toget == gensym("massesForcesName")) & (argc == 1) )
    {
        for (i=0; i< x->nb_mass; i++)
        {
            SETSYMBOL(&(toout[0]), x->mass[i].Id);
            SETFLOAT(&(toout[1]), x->mass[i].forceX);
            outlet_anything(x->main_outlet, gensym("massesForcesName"), 2, toout);
        } 
    }
    else
    if ( (toget == gensym("linksPos")) & (argv[1].a_type == A_FLOAT) )
    {
        i = atom_getfloatarg(1, argc, argv);
        if ( (i>=0) & (i<x->nb_mass) )
        {
            SETFLOAT(&(toout[0]), i);
            SETFLOAT(&(toout[1]), x->link[i].mass1->posX);
            SETFLOAT(&(toout[2]), x->link[i].mass2->posX);
            outlet_anything(x->main_outlet, gensym("linksPosNo"), 3, toout);
        }
    }
    else
    if ( (toget == gensym("linksPos")) & (argv[1].a_type == A_SYMBOL) )
    {
        for (i=0; i< x->nb_link; i++)
        {
            if ( atom_getsymbolarg(1,argc,argv) == x->link[i].Id)
            {
                SETFLOAT(&(toout[0]), i);
                SETFLOAT(&(toout[1]), x->link[i].mass1->posX);
                SETFLOAT(&(toout[2]), x->link[i].mass2->posX);
                outlet_anything(x->main_outlet, gensym("linksPosId"), 3, toout);
            }
        } 
    }
    else
    if ( (toget == gensym("linksPos")) & (argc == 1) )
    {
        for (i=0; i< x->nb_link; i++)
        {
            SETFLOAT(&(toout[0]), i);
            SETFLOAT(&(toout[1]), x->link[i].mass1->posX);
            SETFLOAT(&(toout[2]), x->link[i].mass2->posX);
            outlet_anything(x->main_outlet, gensym("linksPos"), 3, toout);
        } 
    }
    else
    if ( (toget == gensym("linksPosName")) & (argv[1].a_type == A_FLOAT) )
    {
        i = atom_getfloatarg(1, argc, argv);
        if ( (i>=0) & (i<x->nb_mass) )
        {
            SETSYMBOL(&(toout[0]), x->link[i].Id);
            SETFLOAT(&(toout[1]), x->link[i].mass1->posX);
            SETFLOAT(&(toout[2]), x->link[i].mass2->posX);
            outlet_anything(x->main_outlet, gensym("linksPosNameNo"), 3, toout);
        }
    }
    else
    if ( (toget == gensym("linksPosName")) & (argv[1].a_type == A_SYMBOL) )
    {
        for (i=0; i< x->nb_link; i++)
        {
            if ( atom_getsymbolarg(1,argc,argv) == x->link[i].Id)
            {
                SETSYMBOL(&(toout[0]), x->link[i].Id);
                SETFLOAT(&(toout[1]), x->link[i].mass1->posX);
                SETFLOAT(&(toout[2]), x->link[i].mass2->posX);
                outlet_anything(x->main_outlet, gensym("linksPosNameId"), 3, toout);
            }
        } 
    }
    else
    if ( (toget == gensym("linksPosName")) & (argc == 1) )
    {
        for (i=0; i< x->nb_link; i++)
        {
            SETSYMBOL(&(toout[0]), x->link[i].Id);
            SETFLOAT(&(toout[1]), x->link[i].mass1->posX);
            SETFLOAT(&(toout[2]), x->link[i].mass2->posX);
            outlet_anything(x->main_outlet, gensym("linksPosName"), 3, toout);
        } 
    }
    else
        error("not get attribute");
}

void pmpd_massesPosL(t_pmpd *x)
{
    int i;
    t_atom pos_list[nb_max_link];

    for (i=0; i< x->nb_mass; i++)
    {
        SETFLOAT(&(pos_list[i]),x->mass[i].posX);
    }
    outlet_anything(x->main_outlet, gensym("massesPosL"),x->nb_mass , pos_list);
}

void pmpd_massesForcesL(t_pmpd *x)
{
    int i;
    t_atom pos_list[nb_max_link];

    for (i=0; i< x->nb_mass; i++)
    {
        SETFLOAT(&(pos_list[i]),x->mass[i].forceX);
    }
    outlet_anything(x->main_outlet, gensym("massesForcesL"),x->nb_mass , pos_list);
}

void *pmpd_new()
{
	t_pmpd *x = (t_pmpd *)pd_new(pmpd_class);

	pmpd_reset(x);
	
	x->main_outlet=outlet_new(&x->x_obj, 0);
	x->info_outlet=outlet_new(&x->x_obj, 0); // TODO

	return (void *)x;
}

void pmpd_setup(void) 
{
 pmpd_class = class_new(gensym("pmpd"),
        (t_newmethod)pmpd_new,
        0, sizeof(t_pmpd),CLASS_DEFAULT, 0);

	class_addbang(pmpd_class, pmpd_bang);
	class_addmethod(pmpd_class, (t_method)pmpd_mass,            gensym("mass"), A_DEFSYMBOL, A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, 0);
	class_addmethod(pmpd_class, (t_method)pmpd_link,            gensym("link"), A_GIMME, 0);
	class_addmethod(pmpd_class, (t_method)pmpd_posX,            gensym("posX"), A_GIMME, 0);
	class_addmethod(pmpd_class, (t_method)pmpd_forceX,          gensym("forceX"), A_GIMME, 0);
	class_addmethod(pmpd_class, (t_method)pmpd_reset,           gensym("reset"), 0);
	class_addmethod(pmpd_class, (t_method)pmpd_infosL,          gensym("infosL"), 0);
	class_addmethod(pmpd_class, (t_method)pmpd_minX,            gensym("Xmin"), A_DEFFLOAT, 0);
	class_addmethod(pmpd_class, (t_method)pmpd_maxX,            gensym("Xmax"), A_DEFFLOAT, 0);
	class_addmethod(pmpd_class, (t_method)pmpd_setFixed,        gensym("setFixed"), A_GIMME, 0);
	class_addmethod(pmpd_class, (t_method)pmpd_setMobile,       gensym("setMobile"), A_GIMME, 0);
	class_addmethod(pmpd_class, (t_method)pmpd_setK,            gensym("setK"), A_GIMME, 0);
	class_addmethod(pmpd_class, (t_method)pmpd_setD,            gensym("setD"), A_GIMME, 0);
	class_addmethod(pmpd_class, (t_method)pmpd_setL,            gensym("setL"), A_GIMME, 0);
	class_addmethod(pmpd_class, (t_method)pmpd_get,             gensym("get"), A_GIMME, 0);
	class_addmethod(pmpd_class, (t_method)pmpd_massesPosL,      gensym("massesPosL"), 0);
	class_addmethod(pmpd_class, (t_method)pmpd_massesForcesL,   gensym("massesForcesL"), 0);
}

