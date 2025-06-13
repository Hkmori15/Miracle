#ifndef PHYSICS_H
#define PHYSICS_H

#include <cglm/cglm.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct btDiscreteDynamicsWorld btDiscreteDynamicsWorld;
typedef struct btBroadphaseInterface btBroadphaseInterface;
typedef struct btDefaultCollisionConfiguration btDefaultCollisionConfiguration;
typedef struct btCollisionDispatcher btCollisionDispatcher;
typedef struct btSequentialImpulseConstraintSolver btSequentialImpulseConstrainSolver;
typedef struct btRigidBody btRigidBody;
typedef struct btCollisionShape btCollisionShape;

typedef struct {
    btDiscreteDynamicsWorld* dynamics_world;
    btBroadphaseInterface* broadphase;
    btDefaultCollisionConfiguration* collision_config;
    btCollisionDispatcher* dispatcher;
    btSequentialImpulseConstrainSolver* solver;
} physics_world_t;

/**
    * Create and init a physics world
    * @return initialized physics world
**/

physics_world_t physics_world_create(void);

/**
    * Add a rigid body box to the physics world
    * @param world Physics world
    * @param pos Initial position
    * @param size Box dimensions
    * @param mass Mass: 0 for static objects
    * @return Rigid body pointer
**/

btRigidBody* physics_add_box(physics_world_t* world, vec3 pos, vec3 size, float mass);

/**
    * Step the physics simulation
    * @param world Physics world
    * @param delta_time Time step in sec
**/

void physics_step_simulation(physics_world_t* world, float delta_time);

/**
    * Get rigid body transform
    * @param body Rigid body
    * @param pos Output pos
    * @param rotation Output rotation matrix
**/

void physics_get_transform(btRigidBody* body, vec3 pos, mat4 rotation);

/**
    * Destroy physics world and cleanup resources
    * @param world Physics world to destroy
**/

void physics_world_destroy(physics_world_t* world);

#ifdef __cplusplus
}
#endif

#endif // PHYSICS_H
