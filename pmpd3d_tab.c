void pmpd3d_massesPosT(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i, j, vecsize;
    t_garray *a;
    t_word *vec;
    
    if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {
            int taille_max = x->nb_mass;
            taille_max = min(taille_max, (vecsize-2)/3);
            for (i=0; i < taille_max ; i++)
            {
                vec[3*i  ].w_float = x->mass[i].posX;
                vec[3*i+1].w_float = x->mass[i].posY;
                vec[3*i+2].w_float = x->mass[i].posZ;
            }
            garray_redraw(a);
        }
    }
    else 
    if ( (argc==2) && (argv[0].a_type == A_SYMBOL) && (argv[1].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {    
            i = 0;
            j = 0;
            while ((i < vecsize-2) && (j < x->nb_mass))
            {
                if (atom_getsymbolarg(1,argc,argv) == x->mass[j].Id)
                {
                    vec[i].w_float = x->mass[j].posX;
                    i++;
                    vec[i].w_float = x->mass[j].posY;
                    i++;
                    vec[i].w_float = x->mass[j].posZ;
                    i++;
                }
                j++;
            }
            garray_redraw(a);
        }
    }
}

void pmpd3d_massesSpeedsT(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i, j, vecsize;
    t_garray *a;
    t_word *vec;
    
    if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {
            int taille_max = x->nb_mass;
            taille_max = min(taille_max, (vecsize-2)/3);
            for (i=0; i < taille_max ; i++)
            {
                vec[3*i  ].w_float = x->mass[i].speedX;
                vec[3*i+1].w_float = x->mass[i].speedY;
                vec[3*i+2].w_float = x->mass[i].speedZ;
            }
            garray_redraw(a);
        }
    }
    else 
    if ( (argc==2) && (argv[0].a_type == A_SYMBOL) && (argv[1].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {    
            i = 0;
            j = 0;
            while ((i < vecsize-2) && (j < x->nb_mass))
            {
                if (atom_getsymbolarg(1,argc,argv) == x->mass[j].Id)
                {
                    vec[i].w_float = x->mass[j].speedX;
                    i++;
                    vec[i].w_float = x->mass[j].speedY;
                    i++;
                    vec[i].w_float = x->mass[j].speedZ;
                    i++;
                }
                j++;
            }
            garray_redraw(a);
        }
    }
}

void pmpd3d_massesForcesT(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i, j, vecsize;
    t_garray *a;
    t_word *vec;
    
    if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {
            int taille_max = x->nb_mass;
            taille_max = min(taille_max, (vecsize-2)/3);
            for (i=0; i < taille_max ; i++)
            {
                vec[3*i  ].w_float = x->mass[i].forceX;
                vec[3*i+1].w_float = x->mass[i].forceY;
                vec[3*i+2].w_float = x->mass[i].forceZ;
            }
            garray_redraw(a);
        }
    }
    else 
    if ( (argc==2) && (argv[0].a_type == A_SYMBOL) && (argv[1].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {    
            i = 0;
            j = 0;
            while ((i < vecsize-2) && (j < x->nb_mass))
            {
                if (atom_getsymbolarg(1,argc,argv) == x->mass[j].Id)
                {
                    vec[i].w_float = x->mass[j].forceX;
                    i++;
                    vec[i].w_float = x->mass[j].forceY;
                    i++;
                    vec[i].w_float = x->mass[j].forceZ;
                    i++;
                }
                j++;
            }
            garray_redraw(a);
        }
    }
}

void pmpd3d_massesPosXT(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i, j, vecsize;
    t_garray *a;
    t_word *vec;
    
    if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {
            int taille_max = x->nb_mass;
            taille_max = min(taille_max, vecsize);
            for (i=0; i < taille_max ; i++)
            {
                vec[i].w_float = x->mass[i].posX;
            }
            garray_redraw(a);
        }
    }
    else 
    if ( (argc==2) && (argv[0].a_type == A_SYMBOL) && (argv[1].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {    
            i = 0;
            j = 0;
            while ((i < vecsize) && (j < x->nb_mass))
            {
                if (atom_getsymbolarg(1,argc,argv) == x->mass[j].Id)

                {
                    vec[i].w_float = x->mass[j].posX;
                    i++;
                }
                j++;
            }
            garray_redraw(a);
        }
    }
}

void pmpd3d_massesSpeedsXT(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i, j, vecsize;
    t_garray *a;
    t_word *vec;
    
    if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
    {        
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {
            int taille_max = x->nb_mass;
            taille_max = min(taille_max, vecsize);
            for (i=0; i < taille_max ; i++)
            {
                vec[i].w_float = x->mass[i].speedX;
            }
            garray_redraw(a);
        }
    }
    else 
    if ( (argc==2) && (argv[0].a_type == A_SYMBOL) && (argv[1].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {    
            i = 0;
            j = 0;
            while ((i < vecsize) && (j < x->nb_mass))
            {
                if (atom_getsymbolarg(1,argc,argv) == x->mass[j].Id)
                {
                    vec[i].w_float = x->mass[j].speedX;
                    i++;
                }
                j++;
            }
            garray_redraw(a);
        }
    }
}

void pmpd3d_massesForcesXT(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i, j, vecsize;
    t_garray *a;
    t_word *vec;
    
    if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {
            int taille_max = x->nb_mass;
            taille_max = min(taille_max, vecsize);
            for (i=0; i < taille_max ; i++)
            {
                vec[i].w_float = x->mass[i].forceX;
            }
            garray_redraw(a);
        }
    }
    else 
    if ( (argc==2) && (argv[0].a_type == A_SYMBOL) && (argv[1].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {    
            i = 0;
            j = 0;
            while ((i < vecsize) && (j < x->nb_mass))
            {
                if (atom_getsymbolarg(1,argc,argv) == x->mass[j].Id)
                {
                    vec[i].w_float = x->mass[j].forceX;
                    i++;
                }
                j++;
            }
            garray_redraw(a);
        }
    }
}

void pmpd3d_massesPosYT(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i, j, vecsize;
    t_garray *a;
    t_word *vec;
    
    if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {
            int taille_max = x->nb_mass;
            taille_max = min(taille_max, vecsize);
            for (i=0; i < taille_max ; i++)
            {
                vec[i].w_float = x->mass[i].posY;
            }
            garray_redraw(a);
        }
    }
    else 
    if ( (argc==2) && (argv[0].a_type == A_SYMBOL) && (argv[1].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {    
            i = 0;
            j = 0;
            while ((i < vecsize) && (j < x->nb_mass))
            {
                if (atom_getsymbolarg(1,argc,argv) == x->mass[j].Id)
                {
                    vec[i].w_float = x->mass[j].posY;
                    i++;
                }
                j++;
            }
            garray_redraw(a);
        }
    }
}

void pmpd3d_massesSpeedsYT(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i, j, vecsize;
    t_garray *a;
    t_word *vec;
    
    if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {
            int taille_max = x->nb_mass;
            taille_max = min(taille_max, vecsize);
            for (i=0; i < taille_max ; i++)
            {
                vec[i].w_float = x->mass[i].speedY;
            }
            garray_redraw(a);
        }
    }
    else 
    if ( (argc==2) && (argv[0].a_type == A_SYMBOL) && (argv[1].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {    
            i = 0;
            j = 0;
            while ((i < vecsize) && (j < x->nb_mass))
            {
                if (atom_getsymbolarg(1,argc,argv) == x->mass[j].Id)
                {
                    vec[i].w_float = x->mass[j].speedY;
                    i++;
                }
                j++;
            }
            garray_redraw(a);
        }
    }
}

void pmpd3d_massesForcesYT(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i, j, vecsize;
    t_garray *a;
    t_word *vec;
    
    if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {
            int taille_max = x->nb_mass;
            taille_max = min(taille_max, vecsize);
            for (i=0; i < taille_max ; i++)
            {
                vec[i].w_float = x->mass[i].forceY;
            }
            garray_redraw(a);
        }
    }
    else 
    if ( (argc==2) && (argv[0].a_type == A_SYMBOL) && (argv[1].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {    
            i = 0;
            j = 0;
            while ((i < vecsize) && (j < x->nb_mass))
            {
                if (atom_getsymbolarg(1,argc,argv) == x->mass[j].Id)
                {
                    vec[i].w_float = x->mass[j].forceY;
                    i++;
                }
                j++;
            }
            garray_redraw(a);
        }
    }
}

void pmpd3d_massesPosZT(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i, j, vecsize;
    t_garray *a;
    t_word *vec;
    
    if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {
            int taille_max = x->nb_mass;
            taille_max = min(taille_max, vecsize);
            for (i=0; i < taille_max ; i++)
            {
                vec[i].w_float = x->mass[i].posZ;
            }
            garray_redraw(a);
        }
    }
    else 
    if ( (argc==2) && (argv[0].a_type == A_SYMBOL) && (argv[1].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {    
            i = 0;
            j = 0;
            while ((i < vecsize) && (j < x->nb_mass))
            {
                if (atom_getsymbolarg(1,argc,argv) == x->mass[j].Id)
                {
                    vec[i].w_float = x->mass[j].posZ;
                    i++;
                }
                j++;
            }
            garray_redraw(a);
        }
    }
}

void pmpd3d_massesSpeedsZT(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i, j, vecsize;
    t_garray *a;
    t_word *vec;
    
    if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {
            int taille_max = x->nb_mass;
            taille_max = min(taille_max, vecsize);
            for (i=0; i < taille_max ; i++)
            {
                vec[i].w_float = x->mass[i].speedZ;
            }
            garray_redraw(a);
        }
    }
    else 
    if ( (argc==2) && (argv[0].a_type == A_SYMBOL) && (argv[1].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {    
            i = 0;
            j = 0;
            while ((i < vecsize) && (j < x->nb_mass))
            {
                if (atom_getsymbolarg(1,argc,argv) == x->mass[j].Id)
                {
                    vec[i].w_float = x->mass[j].speedZ;
                    i++;
                }
                j++;
            }
            garray_redraw(a);
        }
    }
}

void pmpd3d_massesForcesZT(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i, j, vecsize;
    t_garray *a;
    t_word *vec;
    
    if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {
            int taille_max = x->nb_mass;
            taille_max = min(taille_max, vecsize);
            for (i=0; i < taille_max ; i++)
            {
                vec[i].w_float = x->mass[i].forceZ;
            }
            garray_redraw(a);
        }
    }
    else 
    if ( (argc==2) && (argv[0].a_type == A_SYMBOL) && (argv[1].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {    
            i = 0;
            j = 0;
            while ((i < vecsize) && (j < x->nb_mass))
            {
                if (atom_getsymbolarg(1,argc,argv) == x->mass[j].Id)
                {
                    vec[i].w_float = x->mass[j].forceZ;
                    i++;
                }
                j++;
            }
            garray_redraw(a);
        }
    }
}

// ---------------------------------------------------------------------

void pmpd3d_massesPosNormT(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i, j, vecsize;
    t_garray *a;
    t_word *vec;
    
    if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {
            int taille_max = x->nb_mass;
            taille_max = min(taille_max, vecsize);
            for (i=0; i < taille_max ; i++)
            {
                vec[i].w_float = sqrt(sqr(x->mass[i].posX)+sqr(x->mass[i].posY)+sqr(x->mass[i].posZ));
            }
            garray_redraw(a);
        }
    }
    else 
    if ( (argc==2) && (argv[0].a_type == A_SYMBOL) && (argv[1].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {    
            i = 0;
            j = 0;
            while ((i < vecsize) && (j < x->nb_mass))
            {
                if (atom_getsymbolarg(1,argc,argv) == x->mass[j].Id)
                {
                    vec[i].w_float = sqrt(sqr(x->mass[j].posX)+sqr(x->mass[j].posY)+sqr(x->mass[i].posZ));
                    i++;
                }
                j++;
            }
            garray_redraw(a);
        }
    }
}

void pmpd3d_massesSpeedsNormT(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i, j, vecsize;
    t_garray *a;
    t_word *vec;
    
    if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {
            int taille_max = x->nb_mass;
            taille_max = min(taille_max, vecsize);
            for (i=0; i < taille_max ; i++)
            {
                vec[i].w_float = sqrt(sqr(x->mass[i].speedX)+sqr(x->mass[i].speedY)+sqr(x->mass[i].speedZ));
            }
            garray_redraw(a);
        }
    }
    else 
    if ( (argc==2) && (argv[0].a_type == A_SYMBOL) && (argv[1].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {    
            i = 0;
            j = 0;
            while ((i < vecsize) && (j < x->nb_mass))
            {
                if (atom_getsymbolarg(1,argc,argv) == x->mass[j].Id)
                {
                    vec[i].w_float = sqrt(sqr(x->mass[j].speedX)+sqr(x->mass[j].speedY)+sqr(x->mass[i].speedZ));
                    i++;
                }
                j++;
            }
            garray_redraw(a);
        }
    }
}

void pmpd3d_massesForcesNormT(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i, j, vecsize;
    t_garray *a;
    t_word *vec;
    
    if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {
            int taille_max = x->nb_mass;
            taille_max = min(taille_max, vecsize);
            for (i=0; i < taille_max ; i++)
            {
                vec[i].w_float = sqrt(sqr(x->mass[i].forceX)+sqr(x->mass[i].forceY)+sqr(x->mass[i].forceZ));
            }
            garray_redraw(a);
        }
    }
    else 
    if ( (argc==2) && (argv[0].a_type == A_SYMBOL) && (argv[1].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {    
            i = 0;
            j = 0;
            while ((i < vecsize) && (j < x->nb_mass))
            {
                if (atom_getsymbolarg(1,argc,argv) == x->mass[j].Id)
                {
                    vec[i].w_float = sqrt(sqr(x->mass[j].forceX)+sqr(x->mass[j].forceY)+sqr(x->mass[i].forceZ));
                    i++;
                }
                j++;
            }
            garray_redraw(a);
        }
    }
}

// ---------------------------------------------------------------------

void pmpd3d_linksPosT(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i, j, vecsize;
    t_garray *a;
    t_word *vec;
    
    if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {
            int taille_max = x->nb_link;
            taille_max = min(taille_max, (vecsize-2)/3);
            for (i=0; i < taille_max ; i++)
            {
                vec[3*i  ].w_float = (x->link[i].mass2->posX + x->link[i].mass1->posX)/2;
                vec[3*i+1].w_float = (x->link[i].mass2->posY + x->link[i].mass1->posY)/2;
                vec[3*i+2].w_float = (x->link[i].mass2->posZ + x->link[i].mass1->posZ)/2;
            }
            garray_redraw(a);
        }
    }
    else 
    if ( (argc==2) && (argv[0].a_type == A_SYMBOL) && (argv[1].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {    
            i = 0;
            j = 0;
            while ((i < vecsize-2) && (j < x->nb_link))
            {
                if (atom_getsymbolarg(1,argc,argv) == x->link[j].Id)
                {
                    vec[i].w_float = (x->link[j].mass2->posX + x->link[j].mass1->posX)/2;
                    i++;
                    vec[i].w_float = (x->link[j].mass2->posY + x->link[j].mass1->posY)/2;
                    i++;
                    vec[i].w_float = (x->link[j].mass2->posZ + x->link[j].mass1->posZ)/2;
                    i++;
                }
                j++;
            }
            garray_redraw(a);
        }
    }
}

void pmpd3d_linksLengthT(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i, j, vecsize;
    t_garray *a;
    t_word *vec;
    
    if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {
            int taille_max = x->nb_link;
            taille_max = min(taille_max, (vecsize-2)/3);
            for (i=0; i < taille_max ; i++)
            {
                vec[3*i  ].w_float = x->link[i].mass2->posX - x->link[i].mass1->posX;
                vec[3*i+1].w_float = x->link[i].mass2->posY - x->link[i].mass1->posY;
                vec[3*i+2].w_float = x->link[i].mass2->posZ - x->link[i].mass1->posZ;
            }
            garray_redraw(a);
        }
    }
    else 
    if ( (argc==2) && (argv[0].a_type == A_SYMBOL) && (argv[1].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {    
            i = 0;
            j = 0;
            while ((i < vecsize-2) && (j < x->nb_link))
            {
                if (atom_getsymbolarg(1,argc,argv) == x->link[j].Id)
                {
                    vec[i].w_float = x->link[j].mass2->posX + x->link[j].mass1->posX;
                    i++;
                    vec[i].w_float = x->link[j].mass2->posY + x->link[j].mass1->posY;
                    i++;
                    vec[i].w_float = x->link[j].mass2->posZ + x->link[j].mass1->posZ;
                    i++;
                }
                j++;
            }
            garray_redraw(a);
        }
    }
}

void pmpd3d_linksPosSpeedT(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i, j, vecsize;
    t_garray *a;
    t_word *vec;
    
    if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {
            int taille_max = x->nb_link;
            taille_max = min(taille_max, (vecsize-2)/3);
            for (i=0; i < taille_max ; i++)
            {
                vec[3*i  ].w_float = (x->link[i].mass2->speedX + x->link[i].mass1->speedX)/2;
                vec[3*i+1].w_float = (x->link[i].mass2->speedY + x->link[i].mass1->speedY)/2;
                vec[3*i+2].w_float = (x->link[i].mass2->speedZ + x->link[i].mass1->speedZ)/2;
            }
            garray_redraw(a);
        }
    }
    else 
    if ( (argc==2) && (argv[0].a_type == A_SYMBOL) && (argv[1].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {    
            i = 0;
            j = 0;
            while ((i < vecsize-2) && (j < x->nb_link))
            {
                if (atom_getsymbolarg(1,argc,argv) == x->link[j].Id)
                {
                    vec[i].w_float = (x->link[j].mass2->speedX + x->link[j].mass1->speedX)/2;
                    i++;
                    vec[i].w_float = (x->link[j].mass2->speedY + x->link[j].mass1->speedY)/2;
                    i++;
                    vec[i].w_float = (x->link[j].mass2->speedZ + x->link[j].mass1->speedZ)/2;
                    i++;
                }
                j++;
            }
            garray_redraw(a);
        }
    }
}

void pmpd3d_linksLengthSpeedT(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i, j, vecsize;
    t_garray *a;
    t_word *vec;
    
    if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {
            int taille_max = x->nb_link;
            taille_max = min(taille_max, (vecsize-2)/3);
            for (i=0; i < taille_max ; i++)
            {
                vec[3*i  ].w_float = x->link[i].mass2->speedX - x->link[i].mass1->speedX;
                vec[3*i+1].w_float = x->link[i].mass2->speedY - x->link[i].mass1->speedY;
                vec[3*i+2].w_float = x->link[i].mass2->speedZ - x->link[i].mass1->speedZ;
            }
            garray_redraw(a);
        }
    }
    else 
    if ( (argc==2) && (argv[0].a_type == A_SYMBOL) && (argv[1].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {    
            i = 0;
            j = 0;
            while ((i < vecsize-2) && (j < x->nb_link))
            {
                if (atom_getsymbolarg(1,argc,argv) == x->link[j].Id)
                {
                    vec[i].w_float = x->link[j].mass2->speedX + x->link[j].mass1->speedX;
                    i++;
                    vec[i].w_float = x->link[j].mass2->speedY + x->link[j].mass1->speedY;
                    i++;
                    vec[i].w_float = x->link[j].mass2->speedZ + x->link[j].mass1->speedZ;
                    i++;
                }
                j++;
            }
            garray_redraw(a);
        }
    }
}

void pmpd3d_linksPosXT(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i, j, vecsize;
    t_garray *a;
    t_word *vec;
    
    if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {
            int taille_max = x->nb_link;
            taille_max = min(taille_max, vecsize);
            for (i=0; i < taille_max ; i++)
            {
                vec[i].w_float = (x->link[i].mass1->posX + x->link[i].mass2->posX)/2;
            }
            garray_redraw(a);
        }
    }
    else 
    if ( (argc==2) && (argv[0].a_type == A_SYMBOL) && (argv[1].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {    
            i = 0;
            j = 0;
            while ((i < vecsize) && (j < x->nb_link))
            {
                if (atom_getsymbolarg(1,argc,argv) == x->link[j].Id)
                {
                    vec[i].w_float = (x->link[j].mass1->posX + x->link[j].mass2->posX)/2;
                    i++;
                }
                j++;
            }
            garray_redraw(a);
        }
    }
}

void pmpd3d_linksLengthXT(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i, j, vecsize;
    t_garray *a;
    t_word *vec;
    
    if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {
            int taille_max = x->nb_link;
            taille_max = min(taille_max, vecsize);
            for (i=0; i < taille_max ; i++)
            {
                vec[i].w_float = x->link[i].mass2->posX - x->link[i].mass1->posX;
            }
            garray_redraw(a);
        }
    }
    else 
    if ( (argc==2) && (argv[0].a_type == A_SYMBOL) && (argv[1].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {    
            i = 0;
            j = 0;
            while ((i < vecsize) && (j < x->nb_link))
            {
                if (atom_getsymbolarg(1,argc,argv) == x->link[j].Id)
                {
                    vec[i].w_float = x->link[j].mass2->posX - x->link[j].mass1->posX;
                    i++;
                }
                j++;
            }
            garray_redraw(a);
        }
    }
}

void pmpd3d_linksPosSpeedXT(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i, j, vecsize;
    t_garray *a;
    t_word *vec;
    
    if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {
            int taille_max = x->nb_link;
            taille_max = min(taille_max, vecsize);
            for (i=0; i < taille_max ; i++)
            {
                vec[i].w_float = (x->link[i].mass1->speedX + x->link[i].mass2->speedX)/2;
            }
            garray_redraw(a);
        }
    }
    else 
    if ( (argc==2) && (argv[0].a_type == A_SYMBOL) && (argv[1].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {    
            i = 0;
            j = 0;
            while ((i < vecsize) && (j < x->nb_link))
            {
                if (atom_getsymbolarg(1,argc,argv) == x->link[j].Id)
                {
                    vec[i].w_float = (x->link[j].mass1->speedX + x->link[j].mass2->speedX)/2;
                    i++;
                }
                j++;
            }
            garray_redraw(a);
        }
    }
}

void pmpd3d_linksLengthSpeedXT(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i, j, vecsize;
    t_garray *a;
    t_word *vec;
    
    if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {
            int taille_max = x->nb_link;
            taille_max = min(taille_max, vecsize);
            for (i=0; i < taille_max ; i++)
            {
                vec[i].w_float = x->link[i].mass2->speedX - x->link[i].mass1->speedX;
            }
            garray_redraw(a);
        }
    }
    else 
    if ( (argc==2) && (argv[0].a_type == A_SYMBOL) && (argv[1].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {    
            i = 0;
            j = 0;
            while ((i < vecsize) && (j < x->nb_link))
            {
                if (atom_getsymbolarg(1,argc,argv) == x->link[j].Id)
                {
                    vec[i].w_float = x->link[j].mass2->speedX - x->link[j].mass1->speedX;
                    i++;
                }
                j++;
            }
            garray_redraw(a);
        }
    }
}

void pmpd3d_linksPosYT(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i, j, vecsize;
    t_garray *a;
    t_word *vec;
    
    if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {
            int taille_max = x->nb_link;
            taille_max = min(taille_max, vecsize);
            for (i=0; i < taille_max ; i++)
            {
                vec[i].w_float = (x->link[i].mass1->posY + x->link[i].mass2->posY)/2;
            }
            garray_redraw(a);
        }
    }
    else 
    if ( (argc==2) && (argv[0].a_type == A_SYMBOL) && (argv[1].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {    
            i = 0;
            j = 0;
            while ((i < vecsize) && (j < x->nb_link))
            {
                if (atom_getsymbolarg(1,argc,argv) == x->link[j].Id)
                {
                    vec[i].w_float = (x->link[j].mass1->posY + x->link[j].mass2->posY)/2;
                    i++;
                }
                j++;
            }
            garray_redraw(a);
        }
    }
}

void pmpd3d_linksLengthYT(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i, j, vecsize;
    t_garray *a;
    t_word *vec;
    
    if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {
            int taille_max = x->nb_link;
            taille_max = min(taille_max, vecsize);
            for (i=0; i < taille_max ; i++)
            {
                vec[i].w_float = x->link[i].mass2->posY - x->link[i].mass1->posY;
            }
            garray_redraw(a);
        }
    }
    else 
    if ( (argc==2) && (argv[0].a_type == A_SYMBOL) && (argv[1].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {    
            i = 0;
            j = 0;
            while ((i < vecsize) && (j < x->nb_link))
            {
                if (atom_getsymbolarg(1,argc,argv) == x->link[j].Id)
                {
                    vec[i].w_float = x->link[j].mass2->posY - x->link[j].mass1->posY;
                    i++;
                }
                j++;
            }
            garray_redraw(a);
        }
    }
}

void pmpd3d_linksPosSpeedYT(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i, j, vecsize;
    t_garray *a;
    t_word *vec;
    
    if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {
            int taille_max = x->nb_link;
            taille_max = min(taille_max, vecsize);
            for (i=0; i < taille_max ; i++)
            {
                vec[i].w_float = (x->link[i].mass1->speedY + x->link[i].mass2->speedY)/2;
            }
            garray_redraw(a);
        }
    }
    else 
    if ( (argc==2) && (argv[0].a_type == A_SYMBOL) && (argv[1].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {    
            i = 0;
            j = 0;
            while ((i < vecsize) && (j < x->nb_link))
            {
                if (atom_getsymbolarg(1,argc,argv) == x->link[j].Id)
                {
                    vec[i].w_float = (x->link[j].mass1->speedY + x->link[j].mass2->speedY)/2;
                    i++;
                }
                j++;
            }
            garray_redraw(a);
        }
    }
}

void pmpd3d_linksLengthSpeedYT(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i, j, vecsize;
    t_garray *a;
    t_word *vec;
    
    if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {
            int taille_max = x->nb_link;
            taille_max = min(taille_max, vecsize);
            for (i=0; i < taille_max ; i++)
            {
                vec[i].w_float = x->link[i].mass2->speedY - x->link[i].mass1->speedY;
            }
            garray_redraw(a);
        }
    }
    else 
    if ( (argc==2) && (argv[0].a_type == A_SYMBOL) && (argv[1].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {    
            i = 0;
            j = 0;
            while ((i < vecsize) && (j < x->nb_link))
            {
                if (atom_getsymbolarg(1,argc,argv) == x->link[j].Id)
                {
                    vec[i].w_float = x->link[j].mass2->speedY - x->link[j].mass1->speedY;
                    i++;
                }
                j++;
            }
            garray_redraw(a);
        }
    }
}

void pmpd3d_linksPosZT(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i, j, vecsize;
    t_garray *a;
    t_word *vec;
    
    if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {
            int taille_max = x->nb_link;
            taille_max = min(taille_max, vecsize);
            for (i=0; i < taille_max ; i++)
            {
                vec[i].w_float = (x->link[i].mass1->posZ + x->link[i].mass2->posZ)/2;
            }
            garray_redraw(a);
        }
    }
    else 
    if ( (argc==2) && (argv[0].a_type == A_SYMBOL) && (argv[1].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {    
            i = 0;
            j = 0;
            while ((i < vecsize) && (j < x->nb_link))
            {
                if (atom_getsymbolarg(1,argc,argv) == x->link[j].Id)
                {
                    vec[i].w_float = (x->link[j].mass1->posZ + x->link[j].mass2->posZ)/2;
                    i++;
                }
                j++;
            }
            garray_redraw(a);
        }
    }
}

void pmpd3d_linksLengthZT(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i, j, vecsize;
    t_garray *a;
    t_word *vec;
    
    if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {
            int taille_max = x->nb_link;
            taille_max = min(taille_max, vecsize);
            for (i=0; i < taille_max ; i++)
            {
                vec[i].w_float = x->link[i].mass2->posZ - x->link[i].mass1->posZ;
            }
            garray_redraw(a);
        }
    }
    else 
    if ( (argc==2) && (argv[0].a_type == A_SYMBOL) && (argv[1].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {    
            i = 0;
            j = 0;
            while ((i < vecsize) && (j < x->nb_link))
            {
                if (atom_getsymbolarg(1,argc,argv) == x->link[j].Id)
                {
                    vec[i].w_float = x->link[j].mass2->posZ - x->link[j].mass1->posZ;
                    i++;
                }
                j++;
            }
            garray_redraw(a);
        }
    }
}

void pmpd3d_linksPosSpeedZT(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i, j, vecsize;
    t_garray *a;
    t_word *vec;
    
    if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {
            int taille_max = x->nb_link;
            taille_max = min(taille_max, vecsize);
            for (i=0; i < taille_max ; i++)
            {
                vec[i].w_float = (x->link[i].mass1->speedZ + x->link[i].mass2->speedZ)/2;
            }
            garray_redraw(a);
        }
    }
    else 
    if ( (argc==2) && (argv[0].a_type == A_SYMBOL) && (argv[1].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {    
            i = 0;
            j = 0;
            while ((i < vecsize) && (j < x->nb_link))
            {
                if (atom_getsymbolarg(1,argc,argv) == x->link[j].Id)
                {
                    vec[i].w_float = (x->link[j].mass1->speedZ + x->link[j].mass2->speedZ)/2;
                    i++;
                }
                j++;
            }
            garray_redraw(a);
        }
    }
}

void pmpd3d_linksLengthSpeedZT(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i, j, vecsize;
    t_garray *a;
    t_word *vec;
    
    if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {
            int taille_max = x->nb_link;
            taille_max = min(taille_max, vecsize);
            for (i=0; i < taille_max ; i++)
            {
                vec[i].w_float = x->link[i].mass2->speedZ - x->link[i].mass1->speedZ;
            }
            garray_redraw(a);
        }
    }
    else 
    if ( (argc==2) && (argv[0].a_type == A_SYMBOL) && (argv[1].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {    
            i = 0;
            j = 0;
            while ((i < vecsize) && (j < x->nb_link))
            {
                if (atom_getsymbolarg(1,argc,argv) == x->link[j].Id)
                {
                    vec[i].w_float = x->link[j].mass2->speedZ - x->link[j].mass1->speedZ;
                    i++;
                }
                j++;
            }
            garray_redraw(a);
        }
    }
}

// ---------------------------------------------------------------------

void pmpd3d_linksPosNormT(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i, j, vecsize;
    t_garray *a;
    t_word *vec;
    
    if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {
            int taille_max = x->nb_link;
            taille_max = min(taille_max, vecsize);
            for (i=0; i < taille_max ; i++)
            {
                vec[i].w_float = sqrt( \
                            sqr((x->link[i].mass1->posX + x->link[i].mass2->posX)/2) + \
                            sqr((x->link[i].mass1->posY + x->link[i].mass2->posY)/2) + \
                            sqr((x->link[i].mass1->posZ + x->link[i].mass2->posZ)/2) );
            }
            garray_redraw(a);
        }
    }
    else 
    if ( (argc==2) && (argv[0].a_type == A_SYMBOL) && (argv[1].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {    
            i = 0;
            j = 0;
            while ((i < vecsize) && (j < x->nb_link))
            {
                if (atom_getsymbolarg(1,argc,argv) == x->link[j].Id)
                {
                    vec[j].w_float = sqrt( \
                            sqr((x->link[j].mass1->posX + x->link[j].mass2->posX)/2) + \
                            sqr((x->link[j].mass1->posY + x->link[j].mass2->posY)/2) + \
                            sqr((x->link[j].mass1->posZ + x->link[j].mass2->posZ)/2) );
                    i++;
                }
                j++;
            }
            garray_redraw(a);
        }
    }
}

void pmpd3d_linksLengthNormT(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i, j, vecsize;
    t_garray *a;
    t_word *vec;
    
    if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {
            int taille_max = x->nb_link;
            taille_max = min(taille_max, vecsize);
            for (i=0; i < taille_max ; i++)
            {
                vec[i].w_float = sqrt( \
                            sqr(x->link[i].mass2->posX - x->link[i].mass1->posX) + \
                            sqr(x->link[i].mass2->posY - x->link[i].mass1->posY) + \
                            sqr(x->link[i].mass2->posZ - x->link[i].mass1->posZ) );
            }
            garray_redraw(a);
        }
    }
    else 
    if ( (argc==2) && (argv[0].a_type == A_SYMBOL) && (argv[1].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {    
            i = 0;
            j = 0;
            while ((i < vecsize) && (j < x->nb_link))
            {
                if (atom_getsymbolarg(1,argc,argv) == x->link[j].Id)
                {
                    vec[i].w_float = sqrt( \
                            sqr(x->link[j].mass2->posX - x->link[j].mass1->posX) + \
                            sqr(x->link[j].mass2->posY - x->link[j].mass1->posY) + \
                            sqr(x->link[j].mass2->posZ - x->link[j].mass1->posZ) );
                    i++;
                }
                j++;
            }
            garray_redraw(a);
        }
    }
}

void pmpd3d_linksPosSpeedNormT(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i, j, vecsize;
    t_garray *a;
    t_word *vec;
    
    if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {
            int taille_max = x->nb_link;
            taille_max = min(taille_max, vecsize);
            for (i=0; i < taille_max ; i++)
            {
                vec[i].w_float = sqrt( \
                            sqr((x->link[i].mass1->speedX + x->link[i].mass2->speedX)/2) + \
                            sqr((x->link[i].mass1->speedY + x->link[i].mass2->speedY)/2) + \
                            sqr((x->link[i].mass1->speedZ + x->link[i].mass2->speedZ)/2) );
            }
            garray_redraw(a);
        }
    }
    else 
    if ( (argc==2) && (argv[0].a_type == A_SYMBOL) && (argv[1].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {    
            i = 0;
            j = 0;
            while ((i < vecsize) && (j < x->nb_link))
            {
                if (atom_getsymbolarg(1,argc,argv) == x->link[j].Id)
                {
                    vec[i].w_float = sqrt( \
                            sqr((x->link[j].mass1->speedX + x->link[j].mass2->speedX)/2) + \
                            sqr((x->link[j].mass1->speedY + x->link[j].mass2->speedY)/2) + \
                            sqr((x->link[j].mass1->speedZ + x->link[j].mass2->speedZ)/2) );
                    i++;
                }
                j++;
            }
            garray_redraw(a);
        }
    }
}

void pmpd3d_linksLengthSpeedNormT(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i, j, vecsize;
    t_garray *a;
    t_word *vec;
    
    if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {
            int taille_max = x->nb_link;
            taille_max = min(taille_max, vecsize);
            for (i=0; i < taille_max ; i++)
            {
                vec[i].w_float = sqrt( \
                            sqr(x->link[i].mass2->speedX - x->link[i].mass1->speedX) + \
                            sqr(x->link[i].mass2->speedY - x->link[i].mass1->speedY) + \
                            sqr(x->link[i].mass2->speedZ - x->link[i].mass1->speedZ) );
            }
            garray_redraw(a);
        }
    }
    else 
    if ( (argc==2) && (argv[0].a_type == A_SYMBOL) && (argv[1].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {    
            i = 0;
            j = 0;
            while ((i < vecsize) && (j < x->nb_link))
            {
                if (atom_getsymbolarg(1,argc,argv) == x->link[j].Id)
                {
                    vec[i].w_float = sqrt( \
                            sqr(x->link[j].mass2->speedX - x->link[j].mass1->speedX) + \
                            sqr(x->link[j].mass2->speedY - x->link[j].mass1->speedY) + \
                            sqr(x->link[j].mass2->speedZ - x->link[j].mass1->speedZ) );
                    i++;
                }
                j++;
            }
            garray_redraw(a);
        }
    }
}

// ---------------------------------------------------------------------

void pmpd3d_linksEndT(t_pmpd3d *x, t_symbol *s, int argc, t_atom *argv)
{
    int i, j, vecsize;
    t_garray *a;
    t_word *vec;
    
    if ( (argc==1) && (argv[0].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {
            int taille_max = x->nb_link;
            taille_max = min(taille_max, (vecsize-5)/6);
            for (i=0; i < taille_max ; i++)
            {
                vec[6*i  ].w_float = x->link[i].mass1->posX;
                vec[6*i+1].w_float = x->link[i].mass1->posY;
                vec[6*i+2].w_float = x->link[i].mass1->posZ;
                vec[6*i+3].w_float = x->link[i].mass2->posX;
                vec[6*i+4].w_float = x->link[i].mass2->posY;
                vec[6*i+5].w_float = x->link[i].mass2->posZ;
                
            }
            garray_redraw(a);
        }
    }
    else 
    if ( (argc==2) && (argv[0].a_type == A_SYMBOL) && (argv[1].a_type == A_SYMBOL) )
    {
        t_symbol *tab_name = atom_getsymbolarg(0, argc, argv);
        if (!(a = (t_garray *)pd_findbyclass(tab_name, garray_class)))
            pd_error(x, "%s: no such array", tab_name->s_name);
        else if (!garray_getfloatwords(a, &vecsize, &vec))
            pd_error(x, "%s: bad template for tabwrite", tab_name->s_name);
        else
        {    
            i = 0;
            j = 0;
            while ((i < vecsize-6) && (j < x->nb_link))
            {
                if (atom_getsymbolarg(1,argc,argv) == x->link[j].Id)
                {
                    vec[i].w_float = x->link[j].mass1->posX;
                    i++;
                    vec[i].w_float = x->link[j].mass1->posY;
                    i++;
                    vec[i].w_float = x->link[j].mass1->posZ;
                    i++;
                    vec[i].w_float = x->link[j].mass2->posX;
                    i++;
                    vec[i].w_float = x->link[j].mass2->posY;
                    i++;
                    vec[i].w_float = x->link[j].mass2->posZ;
                    i++;
                }
                j++;
            }
            garray_redraw(a);
        }
    }
}
