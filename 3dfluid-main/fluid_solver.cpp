#include "fluid_solver.h"
#include <cmath>
#include <string.h>

#define IX(i, j, k) ((i) + (M + 2) * (j) + (M + 2) * (N + 2) * (k))
#define SWAP(x0, x)                                                            \
  {                                                                            \
    float *tmp = x0;                                                           \
    x0 = x;                                                                    \
    x = tmp;                                                                   \
  }
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define LINEARSOLVERTIMES 20

// Add sources (density or velocity)
void add_source(int M, int N, int O, float *x, float *s, float dt) {
  int size = (M + 2) * (N + 2) * (O + 2);
  for (int i = 0; i < size; i++) {
    x[i] += dt * s[i];
  }
}

// Set boundary conditions
void set_bnd(int M, int N, int O, int b, float *x) {
  int i, j;

  // Set boundary on faces
  for (i = 1; i <= M; i++) {
    for (j = 1; j <= N; j++) {
      x[IX(i, j, 0)] = b == 3 ? -x[IX(i, j, 1)] : x[IX(i, j, 1)];
      x[IX(i, j, O + 1)] = b == 3 ? -x[IX(i, j, O)] : x[IX(i, j, O)];
    }
  }
  for (i = 1; i <= N; i++) {
    for (j = 1; j <= O; j++) {
      x[IX(0, i, j)] = b == 1 ? -x[IX(1, i, j)] : x[IX(1, i, j)];
      x[IX(M + 1, i, j)] = b == 1 ? -x[IX(M, i, j)] : x[IX(M, i, j)];
    }
  }
  for (i = 1; i <= M; i++) {
    for (j = 1; j <= O; j++) {
      x[IX(i, 0, j)] = b == 2 ? -x[IX(i, 1, j)] : x[IX(i, 1, j)];
      x[IX(i, N + 1, j)] = b == 2 ? -x[IX(i, N, j)] : x[IX(i, N, j)];
    }
  }

  // Set corners
  x[IX(0, 0, 0)] = 0.33f * (x[IX(1, 0, 0)] + x[IX(0, 1, 0)] + x[IX(0, 0, 1)]);
  x[IX(M + 1, 0, 0)] =
      0.33f * (x[IX(M, 0, 0)] + x[IX(M + 1, 1, 0)] + x[IX(M + 1, 0, 1)]);
  x[IX(0, N + 1, 0)] =
      0.33f * (x[IX(1, N + 1, 0)] + x[IX(0, N, 0)] + x[IX(0, N + 1, 1)]);
  x[IX(M + 1, N + 1, 0)] = 0.33f * (x[IX(M, N + 1, 0)] + x[IX(M + 1, N, 0)] +
                                    x[IX(M + 1, N + 1, 1)]);
}

// Linear solve for implicit methods (diffusion)
void lin_solve(int M, int N, int O, int b, float *x, const float *x0, float a, float c) {
    int M2 = M + 2;
    int N2 = N + 2;
    int M2xN2 = M2 * N2;
    float inv_c = 1.0f / c;
    float inv2= a*inv_c;
    int size=IX(M,N,O);
    float x_new[size];
  
    
    
    for (int l = 0; l < LINEARSOLVERTIMES; l++) {
      memcpy(x_new,x,size);
      for (int k = 1; k <= O; k++) {
            int k_offset = k * M2xN2;
            for (int j = 1; j <= N; j++) {
                int jk_offset = k_offset + j * M2;
                for (int i = 1; i <= M; i++) {
                    int idx = jk_offset + i;
                    
                    float x_right = x[idx + 1];
                    float x_up = x[idx + M2];
                    float x_front = x[idx + M2xN2];
                    
                    // Atualização de x[idx]
                    x_new[idx] = (x0[idx]+  a* ( x_right + x_up +  x_front)) * inv_c;
                }
            }
        }

        for (int k = 1; k <= O; k++) {
            int k_offset = k * M2xN2;
            for (int j = 1; j <= N; j++) {
                int jk_offset = k_offset + j * M2;
                for (int i = 1; i <= M; i++) {
                    int idx = jk_offset + i;
                    
                    // Acesso aos valores adjacentes
                    float x_left = x[idx - 1];
                    float x_down = x[idx - M2];
                    float x_back = x[idx - M2xN2];
                    
                    // Atualização de x[idx]
                    x[idx] = x_new[idx] + ((x_left + x_down + x_back)* inv2);
                }
            }
        }
        set_bnd(M, N, O, b, x);
    }
}


// Diffusion step (uses implicit method)
void diffuse(int M, int N, int O, int b, float *x, float *x0, float diff,
             float dt) {
  int max = MAX(MAX(M, N), O);
  float a = dt * diff * max * max;
  lin_solve(M, N, O, b, x, x0, a, 1 + 6 * a);
}

// Advection step (uses velocity field to move quantities)
void advect(int M, int N, int O, int b, float *d, float *d0, float *u, float *v,
            float *w, float dt) {
  float dtX = dt * M;
  float dtY = dt * N;
  float dtZ = dt * O;


  for (int k = 1; k <= O; k++) {
    for (int j = 1; j <= N; j++) {
      for (int i = 1; i <= M; i++) {
        int idx = IX(i, j, k);
        float u_val = u[idx];
        float v_val = v[idx];
        float w_val = w[idx];

        // Cálculo das posições retroativas
        float x = i - dtX * u_val;
        float y = j - dtY * v_val;
        float z = k - dtZ * w_val;

        // Clamp to grid boundaries
        if (x < 0.5f)
          x = 0.5f;
        if (x > M + 0.5f)
          x = M + 0.5f;
        if (y < 0.5f)
          y = 0.5f;
        if (y > N + 0.5f)
          y = N + 0.5f;
        if (z < 0.5f)
          z = 0.5f;
        if (z > O + 0.5f)
          z = O + 0.5f;

        int i0 = (int)x, i1 = i0 + 1;
        int j0 = (int)y, j1 = j0 + 1;
        int k0 = (int)z, k1 = k0 + 1;

        float s1 = x - i0, s0 = 1 - s1;
        float t1 = y - j0, t0 = 1 - t1;
        float u1 = z - k0, u0 = 1 - u1;

        d[IX(i, j, k)] =
            s0 * (t0 * (u0 * d0[IX(i0, j0, k0)] + u1 * d0[IX(i0, j0, k1)]) +
                  t1 * (u0 * d0[IX(i0, j1, k0)] + u1 * d0[IX(i0, j1, k1)])) +
            s1 * (t0 * (u0 * d0[IX(i1, j0, k0)] + u1 * d0[IX(i1, j0, k1)]) +
                  t1 * (u0 * d0[IX(i1, j1, k0)] + u1 * d0[IX(i1, j1, k1)]));
      }
    }
  }
  set_bnd(M, N, O, b, d);
}

// Projection step to ensure incompressibility (make the velocity field
// divergence-free)
void project(int M, int N, int O, float *u, float *v, float *w, float *p,
             float *div) {
  
  const float max= -0.5f/MAX(M, MAX(N, O));
  for (int k = 1; k <= O; k++) {
    for (int j = 1; j <= N; j++) {
      int idx_base = IX(0, j, k);
      for (int i = 1; i <= M; i++) {
        int idx = idx_base + i;

        float du=u[idx + 1] - u[idx - 1];
        float dv= v[idx + (M + 2)] - v[idx - (M + 2)];
        float dw= w[idx + (M + 2) * (N + 2)] - w[idx - (M + 2) * (N + 2)];

        div[idx]= max * (du + dv + dw);
        p[idx] = 0;
      }
    }
  }

  set_bnd(M, N, O, 0, div);
  set_bnd(M, N, O, 0, p);
  lin_solve(M, N, O, 0, p, div, 1, 6);

  for (int k = 1; k <= O; k++) {
    for (int j = 1; j <= N; j++) {
      int idx_base = IX(0, j, k);
      for (int i = 1; i <= M; i++) {
        int idx = idx_base + i;
        u[idx] -= 0.5f * (p[idx + 1] - p[idx - 1]);
        v[idx] -= 0.5f * (p[idx + (M + 2)] - p[idx - (M + 2)]);
        w[idx] -= 0.5f * (p[idx + (M + 2) * (N + 2)] - p[idx - (M + 2) * (N + 2)]);
      }
    }
  }
  set_bnd(M, N, O, 1, u);
  set_bnd(M, N, O, 2, v);
  set_bnd(M, N, O, 3, w);
}

// Step function for density
void dens_step(int M, int N, int O, float *x, float *x0, float *u, float *v,
               float *w, float diff, float dt) {
  add_source(M, N, O, x, x0, dt);
  SWAP(x0, x);
  diffuse(M, N, O, 0, x, x0, diff, dt);
  SWAP(x0, x);
  advect(M, N, O, 0, x, x0, u, v, w, dt);
}

// Step function for velocity
void vel_step(int M, int N, int O, float *u, float *v, float *w, float *u0,
              float *v0, float *w0, float visc, float dt) {
  add_source(M, N, O, u, u0, dt);
  add_source(M, N, O, v, v0, dt);
  add_source(M, N, O, w, w0, dt);
  SWAP(u0, u);
  diffuse(M, N, O, 1, u, u0, visc, dt);
  SWAP(v0, v);
  diffuse(M, N, O, 2, v, v0, visc, dt);
  SWAP(w0, w);
  diffuse(M, N, O, 3, w, w0, visc, dt);
  project(M, N, O, u, v, w, u0, v0);
  SWAP(u0, u);
  SWAP(v0, v);
  SWAP(w0, w);
  advect(M, N, O, 1, u, u0, u0, v0, w0, dt);
  advect(M, N, O, 2, v, v0, u0, v0, w0, dt);
  advect(M, N, O, 3, w, w0, u0, v0, w0, dt);
  project(M, N, O, u, v, w, u0, v0);
}
