/*
 *  pmpd3d_core.c
 */


t_float sign_ch(t_float v)
{
    return v > 0 ? 1 : -1;
}

t_float sqr(t_float x)
{
    return x*x ;
}

t_float pow_ch(t_float x, t_float y)
{
    return x > 0 ? pow(x,y) : -pow(-x,y);
}

t_float mix(t_float X, t_float Y, t_float mix)
{
    return (1-mix)*X + mix*Y ;
}

t_float tabread2(t_pmpd3d *x, t_float pos, t_symbol *array)
{
    t_garray *a;
    int npoints;
    t_word *vec;
    t_float posx;
    
    if (!(a = (t_garray *)pd_findbyclass(array, garray_class)))
        pd_error(x, "%s: no such array", array->s_name);
    else if (!garray_getfloatwords(a, &npoints, &vec))
        pd_error(x, "%s: bad template for tabLink", array->s_name);
    else
    {
        posx = fabs(pos)*npoints;
        int n=posx;
        if (n >= npoints - 1) 
            return (sign_ch(pos)*vec[npoints-1].w_float);
        float fract = posx-n;
        return (sign_ch(pos) * ( fract*vec[n+1].w_float+(1-fract)*vec[n].w_float));
    }
    return( pos); // si il y a un pb sur le tableau, on renvoie l'identité
}

void pmpd3d_reset(t_pmpd3d *x)
{
    x->nb_link = 0;
    x->nb_mass = 0;
    x->minX = -1000000;
    x->maxX = 1000000;
    x->minY = -1000000;
    x->maxY = 1000000;
    x->minZ = -1000000;
    x->maxZ = 1000000;
    x->grab = 0;
}

void *pmpd3d_new()
{
    t_pmpd3d *x = (t_pmpd3d *)pd_new(pmpd3d_class);
	
    pmpd3d_reset(x);
    
    x->main_outlet=outlet_new(&x->x_obj, 0);
    // x->info_outlet=outlet_new(&x->x_obj, 0); // TODO
	
    return (void *)x;
}

void pmpd3d_bang(t_pmpd3d *x)
{
	// this part is doing all the PM
    t_float F, L, Lx,Ly, Lz, Fx, Fy, Fz, tmpX, tmpY, tmpZ, speed;
    t_int i;
    // post("bang");
	
    for (i=0; i<x->nb_mass; i++)
		// compute new masses position
        if (x->mass[i].mobile > 0) // only if mobile
        {
            x->mass[i].speedX += x->mass[i].forceX * x->mass[i].invM;
            x->mass[i].speedY += x->mass[i].forceY * x->mass[i].invM;
            x->mass[i].speedZ += x->mass[i].forceZ * x->mass[i].invM;
            // x->mass[i].forceX = 0;
            // x->mass[i].forceY = 0;        
            // x->mass[i].forceZ = 0;        
            x->mass[i].posX += x->mass[i].speedX ;
            x->mass[i].posY += x->mass[i].speedY ;
            x->mass[i].posZ += x->mass[i].speedZ ;
            if ( (x->mass[i].posX < x->minX) || (x->mass[i].posX > x->maxX) || (x->mass[i].posY < x->minY) 
                || (x->mass[i].posY > x->maxY) || (x->mass[i].posZ < x->minZ) || (x->mass[i].posZ > x->maxZ) ) 
            {
                tmpX = min(x->maxX,max(x->minX,x->mass[i].posX));
                tmpY = min(x->maxY,max(x->minY,x->mass[i].posY));
                tmpZ = min(x->maxZ,max(x->minZ,x->mass[i].posZ));
                x->mass[i].speedX -= x->mass[i].posX - tmpX;
                x->mass[i].speedY -= x->mass[i].posY - tmpY;
                x->mass[i].speedZ -= x->mass[i].posZ - tmpZ;
                x->mass[i].posX = tmpX;
                x->mass[i].posY = tmpY;
                x->mass[i].posZ = tmpZ;
            }
            x->mass[i].forceX = -x->mass[i].D2 * x->mass[i].speedX;
            x->mass[i].forceY = -x->mass[i].D2 * x->mass[i].speedY;
            x->mass[i].forceZ = -x->mass[i].D2 * x->mass[i].speedZ;
            speed = sqrt(x->mass[i].speedX * x->mass[i].speedX + x->mass[i].speedY * x->mass[i].speedY + x->mass[i].speedZ * x->mass[i].speedZ);
            if (speed != 0) {
                x->mass[i].forceX += x->mass[i].D2offset * (x->mass[i].speedX/speed);
                x->mass[i].forceY += x->mass[i].D2offset * (x->mass[i].speedY/speed);
                x->mass[i].forceZ += x->mass[i].D2offset * (x->mass[i].speedZ/speed);
            }
        }
	
    for (i=0; i<x->nb_link; i++)
		// compute link forces
    {
		if (x->link[i].active == 1)
        {
			Lx = x->link[i].mass1->posX - x->link[i].mass2->posX;
			Ly = x->link[i].mass1->posY - x->link[i].mass2->posY;
			Lz = x->link[i].mass1->posZ - x->link[i].mass2->posZ;
			L = sqrt( sqr(Lx) + sqr(Ly) + sqr(Lz) );
			
			if ( (L >= x->link[i].Lmin) && (L < x->link[i].Lmax)  && (L != 0))
			{
				if (x->link[i].lType == 2)
				{ // K et D viennent d'une table
					F  = x->link[i].D * tabread2(x, (L - x->link[i].distance) / x->link[i].D_L, x->link[i].arrayD);
					F += x->link[i].K * tabread2(x, L / x->link[i].K_L, x->link[i].arrayK);
				}
				else
				{            
					F  = x->link[i].D * (L - x->link[i].distance) ;
					F += x->link[i].K *  pow_ch( L - x->link[i].L, x->link[i].Pow);
				}
				
				Fx = F * Lx/L;
				Fy = F * Ly/L;    
				Fz = F * Lz/L;    
                
				if (x->link[i].lType == 1)
				{ // on projette selon 1 axe
					Fx = Fx*x->link[i].VX; // V est unitaire, dc on projete sans pb
					Fy = Fy*x->link[i].VY;                
					Fz = Fz*x->link[i].VZ;                
				}
				
				x->link[i].mass1->forceX -= Fx;
				x->link[i].mass1->forceY -= Fy;
				x->link[i].mass1->forceZ -= Fz;
				x->link[i].mass2->forceX += Fx;
				x->link[i].mass2->forceY += Fy;
				x->link[i].mass2->forceZ += Fz;
				x->link[i].forceX = Fx; // save for latter use
				x->link[i].forceY = Fy;
				x->link[i].forceZ = Fz;
			}
			x->link[i].distance=L;
		}
	}
}

void pmpd3d_mass(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
//, t_symbol *Id, t_float mobile, t_float M, t_float posX, t_float posY, t_float posZ )
{ // add a mass : Id, invM speedX posX
    if(argv[0].a_type == A_SYMBOL)
    {
        x->mass[x->nb_mass].Id = atom_getsymbolarg(0,argc,argv);
        x->mass[x->nb_mass].mobile = (int) atom_getfloatarg(1, argc, argv);
        t_float M = atom_getfloatarg(2, argc, argv);
        if (M<=0) M=1;
        x->mass[x->nb_mass].invM = 1/M;
        x->mass[x->nb_mass].speedX = 0;
        x->mass[x->nb_mass].speedY = 0;
        x->mass[x->nb_mass].speedZ = 0;
        x->mass[x->nb_mass].posX = atom_getfloatarg(3, argc, argv);
        x->mass[x->nb_mass].posY = atom_getfloatarg(4, argc, argv);
        x->mass[x->nb_mass].posZ = atom_getfloatarg(5, argc, argv);
        x->mass[x->nb_mass].forceX = 0;
        x->mass[x->nb_mass].forceY = 0;
        x->mass[x->nb_mass].forceZ = 0;
        x->mass[x->nb_mass].num = x->nb_mass;
        x->mass[x->nb_mass].D2 = 0;
        x->mass[x->nb_mass].D2offset = 0;
		
        x->nb_mass++ ;
        x->nb_mass = min ( nb_max_mass -1, x->nb_mass );
    }
}

void pmpd3d_create_link(t_pmpd3d *x, t_symbol *Id, int mass1, int mass2, t_float K, t_float D, t_float Pow, t_float Lmin, t_float Lmax, t_int type)
{ // create a link based on mass number
	
    if ((x->nb_mass>1) && (mass1 != mass2) && (mass1 >= 0) && (mass2 >= 0) && (mass1 < x->nb_mass) && (mass2 < x->nb_mass) )
    {
        x->link[x->nb_link].lType = type;
        x->link[x->nb_link].Id = Id;
        x->link[x->nb_link].active = 1;
        x->link[x->nb_link].mass1 = &x->mass[mass1]; 
        x->link[x->nb_link].mass2 = &x->mass[mass2];
        x->link[x->nb_link].K = K;
        x->link[x->nb_link].D = D;
        x->link[x->nb_link].L = sqrt(sqr(x->mass[mass1].posX - x->mass[mass2].posX) + 
									 sqr(x->mass[mass1].posY - x->mass[mass2].posY) + sqr(x->mass[mass1].posZ - x->mass[mass2].posZ));
        x->link[x->nb_link].Pow = Pow;
        x->link[x->nb_link].Lmin = Lmin;
        x->link[x->nb_link].Lmax = Lmax;
        x->link[x->nb_link].distance = x->link[x->nb_link].L ;
		
        x->nb_link++ ;
        x->nb_link = min ( nb_max_link -1, x->nb_link );
    }
}

void pmpd3d_link(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{ // add a link : Id, *mass1, *mass2, K, D, Pow, Lmin, Lmax;
	
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
	
    if ( ( argv[1].a_type == A_FLOAT ) && ( argv[2].a_type == A_FLOAT ) )
    {
        pmpd3d_create_link(x, Id, mass1, mass2, K, D, Pow, Lmin, Lmax, 0);
    }
    else
		if ( ( argv[1].a_type == A_SYMBOL ) && ( argv[2].a_type == A_FLOAT ) )
		{
			for (i=0; i< x->nb_mass; i++)
			{
				if ( atom_getsymbolarg(1,argc,argv) == x->mass[i].Id)
				{
					pmpd3d_create_link(x, Id, i, mass2, K, D, Pow, Lmin, Lmax, 0);
				}
			}
		}
		else
			if ( ( argv[1].a_type == A_FLOAT ) && ( argv[2].a_type == A_SYMBOL ) )
			{
				for (i=0; i< x->nb_mass; i++)
				{
					if ( atom_getsymbolarg(2,argc,argv) == x->mass[i].Id)
					{
						pmpd3d_create_link(x, Id, mass1, i, K, D, Pow, Lmin, Lmax, 0);
					}
				}
			}
			else
				if ( ( argv[1].a_type == A_SYMBOL ) && ( argv[2].a_type == A_SYMBOL ) )
				{
					for (i=0; i< x->nb_mass; i++)
					{
						for (j=0; j< x->nb_mass; j++)
						{
							if ( (atom_getsymbolarg(1,argc,argv) == x->mass[i].Id)&(atom_getsymbolarg(2,argc,argv) == x->mass[j].Id))
							{
								if (!( (x->mass[i].Id == x->mass[j].Id) && (i>j) )) 
									// si lien entre 2 serie de masses identique entres elle, alors on ne creer qu'un lien sur 2, pour evider les redondances
									pmpd3d_create_link(x, Id, i, j, K, D, Pow, Lmin, Lmax, 0);
							}
						}   
					}
				}
}

void pmpd3d_tLink(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{ // add a link : Id, *mass1, *mass2, K, D, Vx, Vy, Pow, Lmin, Lmax;
	
    int i, j;
    
    t_symbol *Id = atom_getsymbolarg(0,argc,argv);
    int mass1 = atom_getfloatarg(1, argc, argv);
    int mass2 = atom_getfloatarg(2, argc, argv);
    t_float K = atom_getfloatarg(3, argc, argv);
    t_float D = atom_getfloatarg(4, argc, argv);
    t_float vecteurX = atom_getfloatarg(5, argc, argv);
    t_float vecteurY = atom_getfloatarg(6, argc, argv);
    t_float vecteurZ = atom_getfloatarg(7, argc, argv);
    t_float vecteur = sqrt( sqr(vecteurX) + sqr(vecteurY) + sqr(vecteurZ) );
    vecteurX /= vecteur;
    vecteurY /= vecteur;
    vecteurZ /= vecteur;
    t_float Pow = 1; 
    if (argc > 8) Pow = atom_getfloatarg(8, argc, argv);
    t_float Lmin = 0;
    if (argc > 9) Lmin = atom_getfloatarg(9, argc, argv);
    t_float Lmax =  1000000;
    if (argc > 10) Lmax = atom_getfloatarg(10, argc, argv);
	
    if ( ( argv[1].a_type == A_FLOAT ) && ( argv[2].a_type == A_FLOAT ) )
    {
        pmpd3d_create_link(x, Id, mass1, mass2, K, D, Pow, Lmin, Lmax, 1);
        x->link[x->nb_link-1].VX = vecteurX;
        x->link[x->nb_link-1].VY = vecteurY;
        x->link[x->nb_link-1].VZ = vecteurZ;
    }
    else if ( ( argv[1].a_type == A_SYMBOL ) && ( argv[2].a_type == A_FLOAT ) )
	{
		for (i=0; i< x->nb_mass; i++)
		{
			if ( atom_getsymbolarg(1,argc,argv) == x->mass[i].Id)
			{
				pmpd3d_create_link(x, Id, i, mass2, K, D, Pow, Lmin, Lmax, 1);
				x->link[x->nb_link-1].VX = vecteurX;
				x->link[x->nb_link-1].VY = vecteurY;
				x->link[x->nb_link-1].VZ = vecteurZ;
			}
		}
	}
	else if ( ( argv[1].a_type == A_FLOAT ) && ( argv[2].a_type == A_SYMBOL ) )
	{
		for (i=0; i< x->nb_mass; i++)
		{
			if ( atom_getsymbolarg(2,argc,argv) == x->mass[i].Id)
			{
				pmpd3d_create_link(x, Id, mass1, i, K, D, Pow, Lmin, Lmax, 1);
				x->link[x->nb_link-1].VX = vecteurX;
				x->link[x->nb_link-1].VY = vecteurY;
				x->link[x->nb_link-1].VZ = vecteurZ;
			}
		}
	}
	else if ( ( argv[1].a_type == A_SYMBOL ) && ( argv[2].a_type == A_SYMBOL ) )
	{
		for (i=0; i< x->nb_mass; i++)
		{
			for (j=0; j< x->nb_mass; j++)
			{
				if ( (atom_getsymbolarg(1,argc,argv) == x->mass[i].Id)&(atom_getsymbolarg(2,argc,argv) == x->mass[j].Id))
				{
					if (!( (x->mass[i].Id == x->mass[j].Id) && (i>j) )) 
						// si lien entre 2 serie de masses identique entres elle, alors on ne creer qu'un lien sur 2, pour evider les redondances
					{
						pmpd3d_create_link(x, Id, i, j, K, D, Pow, Lmin, Lmax, 1);
						x->link[x->nb_link-1].VX = vecteurX;
						x->link[x->nb_link-1].VY = vecteurY;
						x->link[x->nb_link-1].VZ = vecteurZ;
					}
				}
			}   
		}
	}
}

void pmpd3d_tabLink(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i, j;
    
    t_symbol *Id = atom_getsymbolarg(0,argc,argv);
    int mass1 = atom_getfloatarg(1, argc, argv);
    int mass2 = atom_getfloatarg(2, argc, argv);
    t_symbol *arrayK = atom_getsymbolarg(3,argc,argv);
    t_float Kl = atom_getfloatarg(4, argc, argv);
    if (Kl <= 0) Kl = 1;
    t_symbol *arrayD = atom_getsymbolarg(5,argc,argv);    
    t_float Dl = atom_getfloatarg(6, argc, argv);
    if (Dl <= 0) Dl = 1;
	
    if ( ( argv[1].a_type == A_FLOAT ) && ( argv[2].a_type == A_FLOAT ) )
    {
        pmpd3d_create_link(x, Id, mass1, mass2, 1, 1, 1, 0, 1000000, 2);
        x->link[x->nb_link-1].arrayK = arrayK;
        x->link[x->nb_link-1].arrayD = arrayD;
        x->link[x->nb_link-1].K_L = Kl;
        x->link[x->nb_link-1].D_L = Dl;        
    }
    else if ( ( argv[1].a_type == A_SYMBOL ) && ( argv[2].a_type == A_FLOAT ) )
	{
		for (i=0; i< x->nb_mass; i++)
		{
			if ( atom_getsymbolarg(1,argc,argv) == x->mass[i].Id)
			{
				pmpd3d_create_link(x, Id, i, mass2, 1, 1, 1, 0, 1000000, 2);
				x->link[x->nb_link-1].arrayK = arrayK;
				x->link[x->nb_link-1].arrayD = arrayD;
				x->link[x->nb_link-1].K_L = Kl;
				x->link[x->nb_link-1].D_L = Dl;    
			}
		}
	}
	else if ( ( argv[1].a_type == A_FLOAT ) && ( argv[2].a_type == A_SYMBOL ) )
	{
		for (i=0; i< x->nb_mass; i++)
		{
			if ( atom_getsymbolarg(2,argc,argv) == x->mass[i].Id)
			{
				pmpd3d_create_link(x, Id, mass1, i, 1, 1, 1, 0, 1000000, 2);
				x->link[x->nb_link-1].arrayK = arrayK;
				x->link[x->nb_link-1].arrayD = arrayD;
				x->link[x->nb_link-1].K_L = Kl;
				x->link[x->nb_link-1].D_L = Dl;    
			}
		}
	}
	else if ( ( argv[1].a_type == A_SYMBOL ) && ( argv[2].a_type == A_SYMBOL ) )
	{
		for (i=0; i< x->nb_mass; i++)
		{
			for (j=0; j< x->nb_mass; j++)
			{
				if ( (atom_getsymbolarg(1,argc,argv) == x->mass[i].Id)&(atom_getsymbolarg(2,argc,argv) == x->mass[j].Id))
				{
					if (!( (x->mass[i].Id == x->mass[j].Id) && (i>j) )) 
						// si lien entre 2 serie de masses identique entres elle, alors on ne creer qu'un lien sur 2, pour evider les redondances
					{
						pmpd3d_create_link(x, Id, i, j, 1, 1, 1, 0, 1000000, 2);
						x->link[x->nb_link-1].arrayK = arrayK;
						x->link[x->nb_link-1].arrayD = arrayD;
						x->link[x->nb_link-1].K_L = Kl;
						x->link[x->nb_link-1].D_L = Dl;    
					}
				}
			}   
		}
	}
}
