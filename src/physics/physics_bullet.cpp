#include "physics.h"
#include <btBulletDynamicsCommon.h>
#include <stdio.h>
#include <stdlib.h>

// C++ wrapper for Bullet Physics to be used from C
// It's implementation sucks don't mind on this

extern "C" {

physics_world_t physics_world_create(void) {
    physics_world_t world = {0};
    
    // Create collision configuration
    world.collision_config = new btDefaultCollisionConfiguration();
    
    // Create collision dispatcher
    world.dispatcher = new btCollisionDispatcher(
        static_cast<btDefaultCollisionConfiguration*>(world.collision_config)
    );
    
    // Create broadphase
    world.broadphase = new btDbvtBroadphase();
    
    // Create constraint solver
    world.solver = new btSequentialImpulseConstraintSolver();
    
    // Create dynamics world
    world.dynamics_world = new btDiscreteDynamicsWorld(
        static_cast<btCollisionDispatcher*>(world.dispatcher),
        static_cast<btBroadphaseInterface*>(world.broadphase),
        static_cast<btSequentialImpulseConstraintSolver*>(world.solver),
        static_cast<btDefaultCollisionConfiguration*>(world.collision_config)
    );
    
    // Set gravity
    static_cast<btDiscreteDynamicsWorld*>(world.dynamics_world)->setGravity(btVector3(0, -9.81f, 0));
    
    printf("Bullet Physics world created successfully\n");
    printf("Gravity: (0, -9.81, 0)\n");
    
    return world;
}

btRigidBody* physics_add_box(physics_world_t* world, vec3 pos, vec3 size, float mass) {
    if (!world || !world->dynamics_world) {
        printf("Error: Invalid physics world\n");
        return nullptr;
    }
    
    // Create box shape
    btCollisionShape* box_shape = new btBoxShape(btVector3(size[0]/2, size[1]/2, size[2]/2));
    
    // Create motion state
    btTransform start_transform;
    start_transform.setIdentity();
    start_transform.setOrigin(btVector3(pos[0], pos[1], pos[2]));
    btDefaultMotionState* motion_state = new btDefaultMotionState(start_transform);
    
    // Calculate local inertia
    btVector3 local_inertia(0, 0, 0);
    if (mass != 0.0f) {
        box_shape->calculateLocalInertia(mass, local_inertia);
    }
    
    // Create rigid body
    btRigidBody::btRigidBodyConstructionInfo rb_info(mass, motion_state, box_shape, local_inertia);
    btRigidBody* body = new btRigidBody(rb_info);
    
    // Add to world
    static_cast<btDiscreteDynamicsWorld*>(world->dynamics_world)->addRigidBody(body);
    
    printf("Added physics box at (%.2f, %.2f, %.2f) with mass %.2f\n", 
           pos[0], pos[1], pos[2], mass);
    
    return body;
}

void physics_step_simulation(physics_world_t* world, float delta_time) {
    if (!world || !world->dynamics_world) {
        return;
    }
    
    static_cast<btDiscreteDynamicsWorld*>(world->dynamics_world)->stepSimulation(
        delta_time, 10, 1.0f/60.0f
    );
}

void physics_get_transform(btRigidBody* body, vec3 pos, mat4 rotation) {
    if (!body) {
        // Return identity transform
        pos[0] = pos[1] = pos[2] = 0.0f;
        glm_mat4_identity(rotation);
        return;
    }
    
    btTransform trans;
    body->getMotionState()->getWorldTransform(trans);
    
    // Extract position
    btVector3 origin = trans.getOrigin();
    pos[0] = origin.getX();
    pos[1] = origin.getY();
    pos[2] = origin.getZ();
    
    // Extract rotation matrix
    btMatrix3x3 basis = trans.getBasis();
    rotation[0][0] = basis[0][0]; rotation[0][1] = basis[0][1]; rotation[0][2] = basis[0][2]; rotation[0][3] = 0.0f;
    rotation[1][0] = basis[1][0]; rotation[1][1] = basis[1][1]; rotation[1][2] = basis[1][2]; rotation[1][3] = 0.0f;
    rotation[2][0] = basis[2][0]; rotation[2][1] = basis[2][1]; rotation[2][2] = basis[2][2]; rotation[2][3] = 0.0f;
    rotation[3][0] = 0.0f;        rotation[3][1] = 0.0f;        rotation[3][2] = 0.0f;        rotation[3][3] = 1.0f;
}

void physics_world_destroy(physics_world_t* world) {
    if (!world || !world->dynamics_world) {
        return;
    }
    
    btDiscreteDynamicsWorld* dynamics_world = static_cast<btDiscreteDynamicsWorld*>(world->dynamics_world);
    
    // Remove all rigid bodies
    for (int i = dynamics_world->getNumCollisionObjects() - 1; i >= 0; i--) {
        btCollisionObject* obj = dynamics_world->getCollisionObjectArray()[i];
        btRigidBody* body = btRigidBody::upcast(obj);
        if (body && body->getMotionState()) {
            delete body->getMotionState();
        }
        dynamics_world->removeCollisionObject(obj);
        delete obj;
    }
    
    // Delete collision shapes
    for (int i = 0; i < dynamics_world->getNumCollisionObjects(); i++) {
        btCollisionObject* obj = dynamics_world->getCollisionObjectArray()[i];
        btCollisionShape* shape = obj->getCollisionShape();
        delete shape;
    }
    
    // Delete dynamics world
    delete static_cast<btDiscreteDynamicsWorld*>(world->dynamics_world);
    delete static_cast<btSequentialImpulseConstraintSolver*>(world->solver);
    delete static_cast<btBroadphaseInterface*>(world->broadphase);
    delete static_cast<btCollisionDispatcher*>(world->dispatcher);
    delete static_cast<btDefaultCollisionConfiguration*>(world->collision_config);
    
    printf("Bullet Physics world destroyed\n");
}

} // extern "C"
