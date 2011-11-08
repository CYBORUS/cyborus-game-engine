#ifndef ENTITY_H
#define ENTITY_H

#include "Actor.h"
#include "Vectors.h"
#include <lua.hpp>
#include <vector>

namespace CGE
{
    class Entity : public SceneGraphNode
    {
        public:
            Entity();
            virtual ~Entity();

            void update();
            void impact(const vec3d& inMomentum);
            bool isInRangeOf(Entity* inEntity);

            inline double getMass() const           { return mMass;         }
            inline double getRadius() const         { return mRadius;       }
            inline const vec3d& getPosition() const { return mPosition;     }
            inline const vec3d& getVelocity() const { return mVelocity;     }
            inline double getMaxSpeed() const       { return mMaxSpeed;     }
            inline double getCurrentSpeed() const   { return mCurrentSpeed; }
            inline int getCollisionCR() const       { return mCollisionCR;}

            inline void setMaxSpeed(double inSpeed) { mMaxSpeed = inSpeed;  }
            inline void setRadius(double inRadius)  { mRadius = inRadius;   }
            inline void setMass(double inMass)      { mMass = inMass;       }
            inline void setCollisionCR(int inCollisionCR)      { mCollisionCR = inCollisionCR;       }

            inline void setVelocity(const vec3d& inVelocity)
            {
                mVelocity = inVelocity;
            }

            inline void setVelocity(double inX, double inY, double inZ)
            {
                mVelocity[0] = inX;
                mVelocity[1] = inY;
                mVelocity[2] = inZ;
            }

            inline const vec3d& getMomentum()
            {
                mMomentum[0] = mVelocity[0] * mMass;
                mMomentum[1] = mVelocity[1] * mMass;
                mMomentum[2] = mVelocity[2] * mMass;

                return mMomentum;
            }

            inline void setDefaultRotation(const vec3d& inDefault)
            {
                mDefaultRotation = inDefault;
            }

            inline void setCurrentSpeed(double inSpeed)
            {
                mCurrentSpeed = inSpeed;
            }

            inline void setPosition(const vec3d& inPosition)
            {
                mPosition = inPosition;
            }

            inline void addActor(Actor* inActor)
            {
                mActors.push_back(inActor);
                addChildNode(inActor);
            }

        protected:
            vec3d mVelocity;
            vec3d mMomentum;
            vec3d mPosition;
            vec3d mRotation;
            vec3d mDefaultRotation;

            double mMass;
            double mRadius;
            double mMaxSpeed;
            double mCurrentSpeed;

            std::vector<Actor*> mActors;

             ;
    };
}
#endif
