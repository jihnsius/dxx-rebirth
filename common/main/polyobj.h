/*
THE COMPUTER CODE CONTAINED HEREIN IS THE SOLE PROPERTY OF PARALLAX
SOFTWARE CORPORATION ("PARALLAX").  PARALLAX, IN DISTRIBUTING THE CODE TO
END-USERS, AND SUBJECT TO ALL OF THE TERMS AND CONDITIONS HEREIN, GRANTS A
ROYALTY-FREE, PERPETUAL LICENSE TO SUCH END-USERS FOR USE BY SUCH END-USERS
IN USING, DISPLAYING,  AND CREATING DERIVATIVE WORKS THEREOF, SO LONG AS
SUCH USE, DISPLAY OR CREATION IS FOR NON-COMMERCIAL, ROYALTY OR REVENUE
FREE PURPOSES.  IN NO EVENT SHALL THE END-USER USE THE COMPUTER CODE
CONTAINED HEREIN FOR REVENUE-BEARING PURPOSES.  THE END-USER UNDERSTANDS
AND AGREES TO THE TERMS HEREIN AND ACCEPTS THE SAME BY USE OF THIS FILE.
COPYRIGHT 1993-1999 PARALLAX SOFTWARE CORPORATION.  ALL RIGHTS RESERVED.
*/

/*
 *
 * Header for polyobj.c, the polygon object code
 *
 */


#ifndef _POLYOBJ_H
#define _POLYOBJ_H

#include "vecmat.h"
#include "gr.h"
#include "3d.h"

#include "robot.h"
#include "piggy.h"

#if defined(DXX_BUILD_DESCENT_I) || defined(DXX_BUILD_DESCENT_II)
#if defined(DXX_BUILD_DESCENT_I)
#define MAX_POLYGON_MODELS 85
#elif defined(DXX_BUILD_DESCENT_II)
#define MAX_POLYGON_MODELS 200
#endif

// array of names of currently-loaded models
extern char Pof_names[MAX_POLYGON_MODELS][13];

//for each model, a model number for dying & dead variants, or -1 if none
extern int Dying_modelnums[MAX_POLYGON_MODELS];
extern int Dead_modelnums[MAX_POLYGON_MODELS];
#endif
#define MAX_SUBMODELS 10

//used to describe a polygon model
typedef struct polymodel {
	int     n_models;
	int     model_data_size;
	ubyte   *model_data;
	int     submodel_ptrs[MAX_SUBMODELS];
	vms_vector submodel_offsets[MAX_SUBMODELS];
	vms_vector submodel_norms[MAX_SUBMODELS];   // norm for sep plane
	vms_vector submodel_pnts[MAX_SUBMODELS];    // point on sep plane
	fix     submodel_rads[MAX_SUBMODELS];       // radius for each submodel
	ubyte   submodel_parents[MAX_SUBMODELS];    // what is parent for each submodel
	vms_vector submodel_mins[MAX_SUBMODELS];
	vms_vector submodel_maxs[MAX_SUBMODELS];
	vms_vector mins,maxs;                       // min,max for whole model
	fix     rad;
	ubyte   n_textures;
	ushort  first_texture;
	ubyte   simpler_model;                      // alternate model with less detail (0 if none, model_num+1 else)
	//vms_vector min,max;
} __pack__ polymodel;

#if defined(DXX_BUILD_DESCENT_I) || defined(DXX_BUILD_DESCENT_II)
// array of pointers to polygon objects
extern polymodel Polygon_models[MAX_POLYGON_MODELS];
#endif

// how many polygon objects there are
extern int N_polygon_models;

void free_polygon_models();
void init_polygon_models();

int load_polygon_model(const char *filename,int n_textures,int first_texture,robot_info *r);

// draw a polygon model
void draw_polygon_model(vms_vector *pos,vms_matrix *orient,vms_angvec *anim_angles,int model_num,int flags,g3s_lrgb lrgb,fix *glow_values,bitmap_index alt_textures[]);

// fills in arrays gun_points & gun_dirs, returns the number of guns read
int read_model_guns(const char *filename,vms_vector *gun_points, vms_vector *gun_dirs, int *gun_submodels);

// draws the given model in the current canvas.  The distance is set to
// more-or-less fill the canvas.  Note that this routine actually renders
// into an off-screen canvas that it creates, then copies to the current
// canvas.
void draw_model_picture(int mn,vms_angvec *orient_angles);

#if defined(DXX_BUILD_DESCENT_I) || defined(DXX_BUILD_DESCENT_II)
#if defined(DXX_BUILD_DESCENT_I)
#define MAX_POLYOBJ_TEXTURES 50
#elif defined(DXX_BUILD_DESCENT_II)
// free up a model, getting rid of all its memory
void free_model(polymodel *po);

#define MAX_POLYOBJ_TEXTURES 100
#endif
extern grs_bitmap *texture_list[MAX_POLYOBJ_TEXTURES];
extern bitmap_index texture_list_index[MAX_POLYOBJ_TEXTURES];
#endif
#define MAX_POLYGON_VECS 1000
extern g3s_point robot_points[MAX_POLYGON_VECS];

#if defined(DXX_BUILD_DESCENT_I)
#ifdef WORDS_NEED_ALIGNMENT
/*
 * A chunk struct (as used for alignment) contains all relevant data
 * concerning a piece of data that may need to be aligned.
 * To align it, we need to copy it to an aligned position,
 * and update all pointers  to it.
 * (Those pointers are actually offsets
 * relative to start of model_data) to it.
 */
typedef struct chunk {
	ubyte *old_base; // where the offset sets off from (relative to beginning of model_data)
	ubyte *new_base; // where the base is in the aligned structure
	short offset; // how much to add to base to get the address of the offset
	short correction; // how much the value of the offset must be shifted for alignment
} chunk;
#define MAX_CHUNKS 100 // increase if insufficent
/*
 * finds what chunks the data points to, adds them to the chunk_list, 
 * and returns the length of the current chunk
 */
int get_chunks(ubyte *data, ubyte *new_data, chunk *list, int *no);
#endif //def WORDS_NEED_ALIGNMENT
#elif defined(DXX_BUILD_DESCENT_II)
/*
 * reads a polymodel structure from a PHYSFS_file
 */
extern void polymodel_read(polymodel *pm, PHYSFS_file *fp);
#endif

/*
 * reads n polymodel structs from a PHYSFS_file
 */
extern int polymodel_read_n(polymodel *pm, int n, PHYSFS_file *fp);

/*
 * routine which allocates, reads, and inits a polymodel's model_data
 */
void polygon_model_data_read(polymodel *pm, PHYSFS_file *fp);

#endif /* _POLYOBJ_H */
