#include "d2q9_bgk.h"


/* The main processes in one step */
int collision(const t_param params, t_speed* cells, t_speed* tmp_cells, int* obstacles);
int streaming(const t_param params, t_speed* cells, t_speed* tmp_cells);
int obstacle(const t_param params, t_speed* cells, t_speed* tmp_cells, int* obstacles);
int boundary(const t_param params, t_speed* cells, t_speed* tmp_cells, float* inlets);

/*
** The main calculation methods.
** timestep calls, in order, the functions:
** collision(), obstacle(), streaming() & boundary()
*/
int timestep(const t_param params, t_speed* cells, t_speed* tmp_cells, float* inlets, int* obstacles)
{
  /* The main time overhead, you should mainly optimize these processes. */
  collision(params, cells, tmp_cells, obstacles);
 // obstacle(params, cells, tmp_cells, obstacles);
  streaming(params, cells, tmp_cells);
  boundary(params, cells, tmp_cells, inlets);
  return EXIT_SUCCESS;
}

/*
** The collision of fluids in the cell is calculated using 
** the local equilibrium distribution and relaxation process
*/
int collision(const t_param params, t_speed* cells, t_speed* tmp_cells, int* obstacles) {
  const float c_sq = 1.f / 3.f; /* square of speed of sound */
  const float w0 = 4.f / 9.f;   /* weighting factor */
  const float w1 = 1.f / 9.f;   /* weighting factor */
  const float w2 = 1.f / 36.f;  /* weighting factor */

  /* loop over the cells in the grid
  ** the collision step is called before
  ** the streaming step and so values of interest
  ** are in the scratch-space grid */
#pragma omp parallel for //num_threads(NUM_THREADS)
for (int idx=0;idx<params.ny*params.nx;idx++)
{
  //for (int jj = 0; jj < params.ny; jj++)
 // {
   // int index = jj*params.nx;
   /// for (int ii = 0; ii < params.nx; ii++)
   // {
      float* cell_index = cells[idx].speeds;
      float* tmp_cell_index = tmp_cells[idx].speeds;
      float local_density = 0.f;
      if (!obstacles[idx]){
        /* compute local density total */

/*
        for (int kk = 0; kk <9; kk++){
            local_density += cells[ii + jj*params.nx].speeds[kk];
        }*/
        
        local_density += cell_index[0];
        local_density += cell_index[1];
        local_density += cell_index[2];
        local_density += cell_index[3];
        local_density += cell_index[4];
        local_density += cell_index[5];
        local_density += cell_index[6];
        local_density += cell_index[7];
        local_density += cell_index[8];

        /* compute x velocity component */
        float u_x = (cell_index[1]- cell_index[3]
                      + cell_index[5]- cell_index[6]
                      - cell_index[7]+ cell_index[8])
                     / local_density;
        /* compute y velocity component */
        float u_y = (cell_index[2]- cell_index[4]
                      + cell_index[5]
                      + cell_index[6]- cell_index[7]
                      - cell_index[8])
                     / local_density;

        /* velocity squared */
        float u_sq = u_x * u_x + u_y * u_y;

        /* directional velocity components */
        float u[NSPEEDS];
        u[0] = 0;            /* zero */
        u[1] =   u_x;        /* east */
        u[2] =         u_y;  /* north */
        u[3] = - u_x;        /* west */
        u[4] =       - u_y;  /* south */
        u[5] =   u_x + u_y;  /* north-east */
        u[6] = - u_x + u_y;  /* north-west */
        u[7] = - u_x - u_y;  /* south-west */
        u[8] =   u_x - u_y;  /* south-east */

        /* equilibrium densities */
        //float d_equ[NSPEEDS];
        __m256 v_d_equ_1 =_mm256_setzero_ps();
       // __m256 v_d_equ_2 =_mm256_setzero_ps();
        /* zero velocity density: weight w0 */        
        /*
        __m128 v_d_equ_3 =_mm_setzero_ps();
        __m128 v_d_equ_4 =_mm_setzero_ps();*/
        float c_sq_2 = 2.f * c_sq * c_sq;
        float u_sq_divide = u_sq / (2.f * c_sq);
        float w1_mul = w1 * local_density;
        float w2_mul = w2 * local_density;
        float d_equ = w0 * local_density * (1.f-u_sq_divide);
        cell_index[0] = cell_index[0]+ params.omega* (d_equ - cell_index[0]);
        _mm256_storeu_ps(&tmp_cells[idx].speeds[1],_mm256_add_ps(_mm256_loadu_ps(&cells[idx].speeds[1]),_mm256_mul_ps(_mm256_set1_ps(params.omega),_mm256_sub_ps(_mm256_add_ps(v_d_equ_1,_mm256_mul_ps(_mm256_set_m128(_mm_set1_ps(w2_mul),_mm_set1_ps(w1_mul)),_mm256_add_ps(_mm256_set1_ps(1.f),_mm256_add_ps(_mm256_div_ps(_mm256_load_ps(&u[1]) ,_mm256_set1_ps(c_sq)),_mm256_sub_ps(_mm256_div_ps(_mm256_mul_ps(_mm256_load_ps(&u[1]),_mm256_load_ps(&u[1])), _mm256_set1_ps(c_sq_2)),_mm256_set1_ps(u_sq_divide)))))),_mm256_loadu_ps(&cells[idx].speeds[1])))));
        //_mm256_storeu_ps(&tmp_cells[idx].speeds[5],_mm256_add_ps(_mm256_loadu_ps(&cells[idx].speeds[5]),_mm256_mul_ps(_mm256_set1_ps(params.omega),_mm256_sub_ps(_mm256_add_ps(v_d_equ_2,_mm256_mul_ps(_mm256_set1_ps(w2_mul),_mm256_add_ps(_mm256_set1_ps(1.f),_mm256_add_ps(_mm256_div_ps(_mm256_load_ps(&u[5]) ,_mm256_set1_ps(c_sq)),_mm_sub_ps(_mm_div_ps(_mm_mul_ps(_mm_load_ps(&u[5]),_mm_load_ps(&u[5])), _mm_set1_ps(c_sq_2)),_mm_set1_ps(u_sq_divide)))))),_mm_loadu_ps(&cells[idx].speeds[5])))));

      }
      else{
          tmp_cell_index[1] = cell_index[3];
          tmp_cell_index[2] = cell_index[4];
          tmp_cell_index[3] = cell_index[1];
          tmp_cell_index[4] = cell_index[2];
          tmp_cell_index[5] = cell_index[7];
          tmp_cell_index[6] = cell_index[8];
          tmp_cell_index[7] = cell_index[5];
          tmp_cell_index[8] = cell_index[6];
      }
   // }
  }
  return EXIT_SUCCESS;
}

/*
** For obstacles, mirror their speed.
*/
int obstacle(const t_param params, t_speed* cells, t_speed* tmp_cells, int* obstacles) {

  /* loop over the cells in the grid */
#pragma omp parallel for //num_threads(NUM_THREADS)
  for (int jj = 0; jj < params.ny; jj++)
  {
    int index = jj*params.nx;
    for (int ii = 0; ii < params.nx; ii++)
    {
      
      /* if the cell contains an obstacle */
      if (obstacles[index + ii])
      {
        //float* cell_index = cells[ii + index].speeds;
       // float* tmp_cell_index = tmp_cells[ii + index].speeds;
        /* called after collision, so taking values from scratch space
        ** mirroring, and writing into main grid */

      }
    }
  }
  return EXIT_SUCCESS;
}

/*
** Particles flow to the corresponding cell according to their speed direaction.
*/
int streaming(const t_param params, t_speed* cells, t_speed* tmp_cells) {
  /* loop over _all_ cells */
  int j_block = 128;
  int i_block = 64;
#pragma omp parallel for //num_threads(NUM_THREADS)
  for (int jj = 0; jj < params.ny; jj+=j_block)
  {
    for(int j = jj; j < jj + j_block && j < params.ny; j++) {          
      int y_n = (j + 1) % params.ny;
      int y_s = (j == 0) ? (params.ny - 1) : (j - 1);
      for (int ii = 0; ii < params.nx; ii+=i_block)
      {
        for(int i = ii; i < ii + i_block && i < params.nx; i++) {
          register int x_e = (i + 1) % params.nx;
          register int x_w = (i == 0) ? (params.nx - 1) : (i - 1);
          //float* tmp_cell_index = tmp_cells[ii + jj*params.nx].speeds;
          register int index = i + j*params.nx;
          /* determine indices of axis-direction neighbours
          ** respecting periodic boundary conditions (wrap around) */
          
          /* propagate densities from neighbouring cells, following
          ** appropriate directions of travel and writing into
          ** scratch space grid */ /* central cell, no movement */
          cells[index].speeds[1] = tmp_cells[x_w + j *params.nx].speeds[1]; /* west */ 
          cells[index].speeds[2] = tmp_cells[i + y_s*params.nx].speeds[2]; /* south */
          cells[index].speeds[3] = tmp_cells[x_e + j *params.nx].speeds[3]; /* east */
          cells[index].speeds[4] = tmp_cells[i + y_n*params.nx].speeds[4]; /* north */
          cells[index].speeds[5] = tmp_cells[x_w + y_s*params.nx].speeds[5]; /* south-west */
          cells[index].speeds[6] = tmp_cells[x_e + y_s*params.nx].speeds[6]; /* south-east */
          cells[index].speeds[7] = tmp_cells[x_e + y_n*params.nx].speeds[7]; /* north-east */
          cells[index].speeds[8] = tmp_cells[x_w + y_n*params.nx].speeds[8]; /* north-west */
        }
      }
    }
  }

  return EXIT_SUCCESS;
}

/*
** Work with boundary conditions. The upper and lower boundaries use the rebound plane, 
** the left border is the inlet of fixed speed, and 
** the right border is the open outlet of the first-order approximation.
*/
int boundary(const t_param params, t_speed* cells,  t_speed* tmp_cells, float* inlets) {
  /* Set the constant coefficient */
  const float cst1 = 2.0/3.0;
  const float cst2 = 1.0/6.0;
  const float cst3 = 1.0/2.0; 
  int ii, jj;
  int block_size = 64; // set the block size to a power of 2 that is a multiple of the cache line size
  int jj_1 = (params.ny -1)*params.nx;
  int ii_2 = params.nx-1;
#pragma omp parallel for //num_threads(NUM_THREADS)

  for(ii = 0; ii < params.nx; ii += block_size){
    for(int i = ii; i < ii + block_size && i < params.nx; i++) {
      // top wall (bounce)
      cells[i + jj_1].speeds[4] = tmp_cells[i + jj_1].speeds[2];
      cells[i + jj_1].speeds[7] = tmp_cells[i + jj_1].speeds[5];
      cells[i + jj_1].speeds[8] = tmp_cells[i + jj_1].speeds[6];

      // bottom wall (bounce)
      //jj_2 = 0;
      cells[i].speeds[2] = tmp_cells[i].speeds[4];
      cells[i].speeds[5] = tmp_cells[i].speeds[7];
      cells[i].speeds[6] = tmp_cells[i].speeds[8];
    }
    
  }
#pragma omp parallel for //num_threads(NUM_THREADS)
  for(jj = 0; jj < params.ny; jj+=block_size){
    for(int j = jj; j < jj + block_size && j < params.ny; j++){
      // left wall (inlet)
      //ii_1 = 0;
      float local_density = ( cells[j*params.nx].speeds[0]
                        + cells[j*params.nx].speeds[2]
                        + cells[j*params.nx].speeds[4]
                        + 2.0 * cells[j*params.nx].speeds[3]
                        + 2.0 * cells[j*params.nx].speeds[6]
                        + 2.0 * cells[j*params.nx].speeds[7]
                        )/(1.0 - inlets[j]);

      cells[j*params.nx].speeds[1] = cells[j*params.nx].speeds[3] + cst1*local_density*inlets[j];

      cells[j*params.nx].speeds[5] = cells[j*params.nx].speeds[7] - cst3*(cells[j*params.nx].speeds[2]-cells[j*params.nx].speeds[4])
                                          + cst2*local_density*inlets[j];
      cells[j*params.nx].speeds[8] = cells[j*params.nx].speeds[6] + cst3*(cells[j*params.nx].speeds[2]-cells[j*params.nx].speeds[4])
                                          + cst2*local_density*inlets[j];
      // right wall (outlet)
      
      cells[ii_2 + j*params.nx].speeds[0] = cells[ii_2-1 + j*params.nx].speeds[0];
      _mm256_storeu_ps(&cells[ii_2 + j*params.nx].speeds[1],_mm256_loadu_ps(&cells[ii_2-1 + j*params.nx].speeds[1]));
    } 
  }
  return EXIT_SUCCESS;
}
