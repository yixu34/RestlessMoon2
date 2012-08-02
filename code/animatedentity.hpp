#ifndef ANIMATEDENTITY_HPP
#define ANIMATEDENTITY_HPP

#include "entity.hpp"
#include "skinnedmesh.hpp"
#include "util.hpp"
#include <boost/shared_ptr.hpp>

struct ID3DXAnimationController;

class AnimatedEntity;
typedef boost::shared_ptr<AnimatedEntity> AnimatedEntityPtr;

class AnimatedEntity: 
	public Entity
{
	public:
		AnimatedEntity(NewEntityStruct* newEntityParams);
		~AnimatedEntity();

		void walkForward();
		void walkBackward();
		void turnRight();
		void turnLeft();
		void turnTo(float angle);
		bool turnTo(D3DXVECTOR3 tar);
		void strafeLeft();
		void strafeRight();
		void jump();
        void dash();

        void throwGrenade();
		void shoot();
		void reload();
		bool isReloading() const;
		bool canReload() const;
		void stop();
		void nextWeapon();
		void previousWeapon();
        void setWeapon(int weaponNum);

		void killEntity();
		void renderInstance(int subset);
		void timepass();
		void getCameraOffsets(float &offsetY, float &offsetZ) const;

		struct AnimationInterval
		{
			AnimationInterval(
				float startTime = 0.0f, 
				float endTime = 0.0f, 
				float speed = 0.0f)
				: _startTime(startTime), _endTime(endTime), _speed(speed)
			{}

			float _startTime;
			float _endTime;
			float _speed;
		};

		typedef std::vector<AnimationInterval> AnimationSequence;

		static void addAnimationSequence(const std::string &name, const AnimationSequence &sequence);
		static bool animationsExist(const std::string &name);

	private:

		enum animation_set
		{
			animation_idle = 1,  // Stand around and do nothing
			animation_run_shoot, // Run/shoot
			animation_reload,
			animation_death,
			animation_none,
		};

		class AnimationSetLess
		{
		public:
			bool operator()(animation_set anim1, animation_set anim2) const
			{
				return AnimatedEntity::getAnimationSetPriority(anim1) < 
					AnimatedEntity::getAnimationSetPriority(anim2);
			}
		};

		void initializeAnimations();
		void selectBestAnimationSet();
		void addAnimation(animation_set anim);
		bool hasStoppedAnimations() const;
		void stopFutureAnimations();


		// 
		// Retrieves the [index]th animation sequence associated with entities of type [name].
		// [index] starts at 1, not 0.
		//
		static AnimationInterval getInterval(const std::string &name, animation_set index);

		static unsigned animationSetToIndex(animation_set anim);

		// Returns a number that represents how important [anim] is
		// compared to the other animation sets.  
		// We prefer to use the animation set with the lowest priority number.
		static int getAnimationSetPriority(animation_set anim);

		typedef std::map<std::string, AnimationSequence> AnimationPool;
		static AnimationPool animations;

		typedef std::vector<animation_set> AnimationList;
		AnimationList _pendingAnimations;

		ID3DXAnimationController *_animCtrl;
		SkinnedMeshPtr _mesh;
		AnimationInterval _animationInterval;
		animation_set _currAnimationSet;

		bool _futureAnimationsStopped;
		double _currAnimationTime;

		float _cameraOffsetY;
		float _cameraOffsetZ;
};

#endif